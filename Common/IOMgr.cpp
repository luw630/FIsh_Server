#include "IOMgr.h"
#include "IOCPModel.h"
#include <json/json.h>
#include "WfSysUtil.h"
#include "IOCPModel.h"

USING_NS_WF;
uint ConCfg::m_MaxSendPacketSize = 8192;	// 发送包的最大size
uint ConCfg::m_MaxRecvPacketSize = 8192;	// 接收包的最大size
uint ConCfg::m_SendBufferSize = 1024;		// 发送包的缓存大小
uint ConCfg::m_RecvBufferSize = 1024;		// 接收包的缓存大小
absFactoryLock	IOMgr::m_PacketFactory;
IOModel* IOMgr::m_IoModel=nullptr;
IOSession::IOSession()
{
	m_SendList = new LockFreeList<Wf::abs*>();
	m_RecvBuffer.Init(MAX_RCVBUFFER_LEN);
	m_SendBuffer.Init(MAX_SENDBUFFER_LEN);
}
IOSession::~IOSession()
{
	WF_SAFE_DELETE(m_SendList);
}
void IOSession::init(uint64 sessionId, SOCKET s, SOCKADDR_IN* clientAddr)
{
	m_sessionId = sessionId;
	m_Socket = s;
	memcpy(&m_ClientAddr, clientAddr, sizeof(SOCKADDR_IN));
	m_OverlappedRead.m_OpType = RECV_POSTED;
	m_OverlappedRead.m_sockAccept = m_Socket;
	m_OverlappedRead.m_pSession = this;
	m_OverlappedWrite.m_OpType = SEND_POSTED;
	m_OverlappedWrite.m_sockAccept = m_Socket;
	m_OverlappedWrite.m_pSession = this;

	m_CurrentPacket = nullptr;
	m_CurrentPacketOffset = 0;
	m_IsReading = false;
	m_IsSending.Set(false);
	m_IsClosed = false;

	m_LastPingTime = time(NULL);
	m_LastOnPingTime = time(NULL);
	m_timeout = 5 * 60;//secs
	OnInit();
}
void IOSession::release()
{
	if (m_CurrentPacket) {
		m_CurrentPacket->release();
		m_CurrentPacket = nullptr;
		m_CurrentPacketOffset = 0;
	}
	while (m_SendList->GetSize()>0)
	{
		Wf::abs** pPacket = m_SendList->ReadLock();
		if (pPacket) {
			Wf::abs* packet = *pPacket;
			packet->release();
		}
	}
}
void IOSession::SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet)
{
	Wf::abs* buffer = IOMgr::createBuffer(0);
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	int32 nSpace = buffer->getCapacity() - sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);

	int32 size = packet->ByteSize();
	if (size > nSpace) {
		log_err("space is mall");
		return;
	}
	else {
		bool rst = packet->SerializeToArray(header + 1, size);
		if (!rst) {
			log_err("unknow error");
			return;
		}
		else {
			stream.SetPointer(size);
		}
	}
	header->m_Length = sizeof(PacketHeader) + stream.GetOffset();

	m_SendListLock.lock();
	Wf::abs** pNewPacket = m_SendList->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = buffer;
		m_SendList->WriteUnlock();
	}
	m_SendListLock.unlock();
	PostWrite();
}
void IOSession::SendPacket(PacketCommand cmd, ISerializePacket* packet)
{
	Wf::abs* buffer = IOMgr::createBuffer(0);
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	header->m_Command = cmd;
	header->m_PacketParam = 0;
	header->m_Length = sizeof(PacketHeader);

	int32 nSpace = buffer->getCapacity() - sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);
	if (packet) {
		packet->Serialize(stream);
		header->m_Length += sizeof(PacketHeader) + stream.GetOffset();
	}
	
	m_SendListLock.lock();
	Wf::abs** pNewPacket = m_SendList->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = buffer;
		m_SendList->WriteUnlock();
	}
	m_SendListLock.unlock();
	PostWrite();
}
void IOSession::SendPacket(PacketHeader* packet)
{
	Wf::abs* buffer = IOMgr::createBuffer(0);
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	memcpy(header, packet, packet->m_Length);

	m_SendListLock.lock();
	Wf::abs** pNewPacket = m_SendList->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = buffer;
		m_SendList->WriteUnlock();
	}
	m_SendListLock.unlock();
	PostWrite();
}
bool IOSession::ParsePacketBuffer()
{
	bool continueParse = true;
	while (continueParse)
	{
		Wf::abs* newPacket = nullptr;
		if (!ParseAPacketFromBuffer(m_RecvBuffer, continueParse, newPacket)){
			return false;
		}

		// 解析出一个新的数据包,投递给逻辑线程
		if (newPacket != nullptr){
			newPacket->m_sessionId = m_sessionId;
			PacketHeader* packet = (PacketHeader*)newPacket->getData();
			OnPacket(newPacket, packet, Stream((uint8*)(packet + 1), packet->m_Length - sizeof(PacketHeader)));
		}
	}
	return true;
}
bool IOSession::ParseAPacketFromBuffer(RWBuffer& buffer, bool& continueParse, Wf::abs*& newPacket)
{
	size_t bufferSize = 0;
	// 消息头还没解析,就先解析消息头
	if (m_CurrentPacket == nullptr)
	{
		bufferSize = buffer.GetUnreadSize();
		if (bufferSize < sizeof(PacketHeader))
		{
			continueParse = false;
			return true;
		}


		// 解析出消息头
		PacketHeader curheader;
		buffer.BytesDeserialize(&curheader, sizeof(PacketHeader));
		if (buffer.GetError() == EStreamError_BufferFull) //缓冲区已满
		{
			continueParse = false;
			return false;
		}

		// 长度异常
		if (curheader.m_Length < sizeof(PacketHeader))
		{
			continueParse = false;
			return false;
		}
		// 长度异常
		if (curheader.m_Length > ConCfg::m_MaxRecvPacketSize)
		{
			continueParse = false;

			log_waring("ParseAPacketFromBuffer size too big! con id=%llu, pack type:%d, len:%d!\n", m_sessionId, curheader.m_Command, curheader.m_Length);
			return false;
		}

		m_CurrentPacket = IOMgr::createBuffer(curheader.m_Length);
		if (m_CurrentPacket == nullptr)
		{
			continueParse = false;
			log_err("space is mall");
			return false;
		}
		::memcpy(m_CurrentPacket->getData(), &curheader, sizeof(PacketHeader));
		m_CurrentPacketOffset = sizeof(PacketHeader);
	}


	bufferSize = buffer.GetUnreadSize();
	// 包数据还没收完,该包收到了多少,就先读取多少
	if (bufferSize + m_CurrentPacketOffset < ((PacketHeader*)m_CurrentPacket->getData())->m_Length)
	{
		if (bufferSize > 0)
		{
			buffer.BytesDeserialize(((uint8*)m_CurrentPacket->getData()) + m_CurrentPacketOffset, bufferSize);
			if (buffer.GetError() == EStreamError_BufferFull) //缓冲区已满
			{
				continueParse = false;
				return false;
			}
			m_CurrentPacketOffset += bufferSize;
		}
		continueParse = false;
		return true;
	}

	// 完整包都收到了
	buffer.BytesDeserialize(((uint8*)m_CurrentPacket->getData()) + m_CurrentPacketOffset, ((PacketHeader*)m_CurrentPacket->getData())->m_Length - m_CurrentPacketOffset);

	if (buffer.GetError() == EStreamError_BufferFull) //缓冲区已满
	{
		continueParse = false;
		return false;
	}

	newPacket = m_CurrentPacket;
	m_CurrentPacket = NULL;
	m_CurrentPacketOffset = 0;
	return true;
}

bool IOSession::PostRead()
{
	if (m_IsClosed)
	{
		m_IsSending.Set(false);
		return false;
	}
	return IOMgr::m_IoModel->PostRead(this);
}
bool IOSession::PostWrite()
{
	if (m_IsSending.Get() == 0)
	{
		size_t writeSize = 0;
		int8* writeData = (int8*)m_SendBuffer.GetTailUnreadData(writeSize);
		// 异步发送数据
		if (writeData != NULL && writeSize > 0)
		{
			if (m_IsSending.Set(true)) // 原子锁操作,保证线程同步
			{
				IOMgr::m_IoModel->PostSend(this);
			}
		}else if(m_SendList->GetSize() >0){
			if (m_IsSending.Set(true)) // 原子锁操作,保证线程同步
			{
				while(m_SendBuffer.GetSpaceSize() >abs_Size && m_SendList->GetSize()>0)
				{
					Wf::abs** pPacket = m_SendList->ReadLock();
					if (pPacket) {
						Wf::abs* packet = *pPacket;
						m_SendList->ReadUnlock();
						char* pData = packet->getData();
						PacketHeader* header = (PacketHeader*)pData;
						if (!m_SendBuffer.Write((uint8*)packet->getData(), header->m_Length)) {
							log_waring("error");
							packet->release();
							return false;
						}
						packet->release();						
					}
				}	
				IOMgr::m_IoModel->PostSend(this);
			}
		}
		else if (m_IsClosed == true)
		{
			if (m_IsSending.Set(true)) // 原子锁操作,保证线程同步
			{
				IOMgr::m_IoModel->PostSend(this);
			}
		}
	}
	return true;
}
void IOSession::OnPing(uint last)
{
	m_LastOnPingTime = time(NULL);

	PacketPong pong;
	pong.m_Tick = last;
	SendPacket(&pong);
}
void IOSession::OnPong(uint time)
{
	m_LastOnPingTime = time;
}

bool IOMgr::Init()
{
	

	Json::Value root;
	Json::Reader jsonReader;
	bool ok = jsonReader.parse(SysUtil::ReadFileCPlus("Config/Network.cfg"), root);

	std::string sIp = "";
	int32 nPort = 24000;
	if (ok)
	{
		sIp = root.get("IP", "127.0.0.1").asString();
		nPort = root.get("Port", 24000).asInt();
		ConCfg::m_MaxSendPacketSize = root.get("MaxSendPacketSize", 8192).asInt();
		ConCfg::m_MaxRecvPacketSize = root.get("MaxRecvPacketSize", 8192).asInt();
		ConCfg::m_SendBufferSize = root.get("SendBufferSize", 1024).asInt();
		ConCfg::m_RecvBufferSize = root.get("RecvBufferSize", 1024).asInt();
	}
	else {
		log_waring("Network.cfg error");
		return false;
	}
	m_IoModel = new CIOCPModel();
	if (!m_IoModel->Init(sIp, nPort, this)){
		WF_SAFE_DELETE(m_IoModel);
		return false;
	}
	if (!OnInit()) {
		return false;
	}
	return m_IoModel->Start();
}
void IOMgr::accept(IOSession* pSession)
{
	m_sessionLock.lock();
	m_sessions[pSession->getSessionId()] = pSession;
	OnNewSession(pSession);
	m_sessionLock.unlock();
}
uint64 IOMgr::newSessionId()
{
	static uint64 sessionId = -1;
	m_sessionLock.lock();
	++sessionId;
	do {
		auto t = m_sessions.find(sessionId);
		if (t == m_sessions.end())
		{
			break;
		}
		else {
			++sessionId;
		}
	} while (false);
	m_sessionLock.unlock();		
	return sessionId;
}
void IOMgr::rmvSession(uint64 sessionId)
{
	m_sessionLock.lock();
	auto t = m_sessions.find(sessionId);
	if (t != m_sessions.end())
	{
		IOSession* pSession = t->second;
		OnRmvSession(pSession);	
		shutdown(pSession->m_Socket, SD_BOTH);
		m_sessionShutdown.push_back(pSession);
		m_sessions.erase(t);
	}
	m_sessionLock.unlock();
}
void IOMgr::clearAllConnect()
{
	m_sessionLock.lock();
	for (auto&t : m_sessions) {
		delSession(t.second);
	}
	m_sessions.clear();
	m_sessionLock.unlock();
}

void IOMgr::SendPacket(uint64 sessionId, PacketCommand cmd, ::google::protobuf::Message* packet)
{
	m_sessionLock.lock();
	do {
		auto t = m_sessions.find(sessionId);
		if (t == m_sessions.end()){
			break;
		}
		IOSession* pSession = t->second;
		pSession->SendPacket(cmd, packet);
	} while (false);	
	m_sessionLock.unlock();
}
void IOMgr::SendPacket(uint64 sessionId, PacketCommand cmd, ISerializePacket* packet)
{
	m_sessionLock.lock();
	do {
		auto t = m_sessions.find(sessionId);
		if (t == m_sessions.end()) {
			break;
		}
		IOSession* pSession = t->second;
		pSession->SendPacket(cmd, packet);
	} while (false);
	m_sessionLock.unlock();
}
void IOMgr::SendPacket(uint64 sessionId, PacketHeader* packet)
{
	m_sessionLock.lock();
	do {
		auto t = m_sessions.find(sessionId);
		if (t == m_sessions.end()) {
			break;
		}
		IOSession* pSession = t->second;
		pSession->SendPacket(packet);
	} while (false);
	m_sessionLock.unlock();
}
Wf::abs* IOMgr::createBuffer(uint32 size)
{
	return m_PacketFactory.newBuffer();
}
void IOMgr::update(uint32 delta)
{
	if (m_sessionShutdown.size() > 0)
	{
		m_sessionLock.lock();
		std::vector<IOSession*> destroy;
		m_sessionShutdown.swap(destroy);
		m_sessionLock.unlock();
		for (auto& t : destroy)
		{
			m_sessionLock.lock();
			do {
				closesocket(t->m_Socket);
				delSession(t);
			} while (false);
			m_sessionLock.unlock();
		}
	}	
}
void IOMgr::close(uint64 sessionId)
{
	IOSession* pSession = nullptr;
	m_sessionLock.lock();
	auto t = m_sessions.find(sessionId);
	if (t != m_sessions.end()) {
		pSession = t->second;
	}
	if (pSession && pSession->m_IsClosed==false) {
		pSession->m_IsClosed = true;
		pSession->PostWrite();
	}
	m_sessionLock.unlock();
}
