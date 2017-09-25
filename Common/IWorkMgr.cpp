#include "IWorkMgr.h"
#include <json/json.h>
#include "WfSysUtil.h"
#include "WfIService.h"
#include "WfProxyHead.h"
#include "WfCommonPackets.h"

NS_WF_BEGIN


TaskHandle::TaskHandle()
{
	m_LastUpdateTick = 0;
	m_pingMysqlDelt = 0;
	m_workMgr = nullptr;
	m_SendPackets = new LockFreeList<abs*>();
	m_RcvPackets = new LockFreeList<abs*>();
}

TaskHandle::~TaskHandle()
{
	WF_SAFE_DELETE(m_SendPackets);
	WF_SAFE_DELETE(m_RcvPackets);
}
void TaskHandle::init(IWorkMgr* workMgr)
{
	m_LastUpdateTick = SysUtil::GetMSTime();
	m_pingMysqlDelt = 0;
	m_workMgr = workMgr;
}
void TaskHandle::sendBuffer(abs* pBuffer)
{
	abs** pNewPacket = m_SendPackets->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = pBuffer;
		m_SendPackets->WriteUnlock();
	}
}
void TaskHandle::addBuffer(abs* pBuffer)
{
	abs** pNewPacket = m_RcvPackets->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = pBuffer;
		m_RcvPackets->WriteUnlock();
	}
}
void TaskHandle::pingMysql(uint32 delta)
{
	m_pingMysqlDelt += delta;
	if (m_connect.IsConnected())
	{
		if (m_pingMysqlDelt > 5000) {
			m_pingMysqlDelt -= 5000;
			if (!m_connect.Ping()) {
				m_connect.Disconnect();
				m_connect.Connect(m_workMgr->m_conOpt);
			}
			else {
				SysUtil::Sleep(1);
			}
		}
	}else {
		if (m_pingMysqlDelt > 50){
			m_pingMysqlDelt -= 50;
			if (!m_connect.Ping()){
				m_connect.Disconnect();
				m_connect.Connect(m_workMgr->m_conOpt);
			}else {
				SysUtil::Sleep(1);
			}
		}
	}
}
void TaskHandle::update()
{
	int32 max_Process_num = 50;
	while ( (--max_Process_num) >= 0)
	{
		abs** pPacket = m_SendPackets->ReadLock();
		if (pPacket) {			
			abs* packet = *pPacket;
			m_SendPackets->ReadUnlock();
			char* pData = packet->getData();
			PacketHeader* header = (PacketHeader*)pData;
			m_workMgr->OnPacket(packet->m_sessionId, header, Stream(((uint8*)header + sizeof(PacketHeader)), header->m_Length - sizeof(PacketHeader)));
			packet->release();
		}else {
			break;
		}
	}
}
void TaskHandle::SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet, t_sessionId connectId)
{
	abs* buffer = m_sendFactory.newBuffer();
	buffer->m_sessionId = connectId;
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	int32 nSpace = buffer->getCapacity()- sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);

	int32 size = packet->ByteSize();
	if (size > nSpace) {
		log_err("space is mall");
		return;
	}else {
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

	sendBuffer(buffer);
}
void TaskHandle::SendPacket(PacketCommand cmd, ISerializePacket* packet, t_sessionId connectId)
{
	abs* buffer = m_sendFactory.newBuffer();
	buffer->m_sessionId = connectId;
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	int32 nSpace = buffer->getCapacity() - sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);

	if (packet) 
	{
		packet->Serialize(stream);
	}
	header->m_Length = sizeof(PacketHeader) + stream.GetOffset();

	sendBuffer(buffer);
}
void TaskHandle::SendPacket(PacketHeader* pHeader, t_sessionId connectId)
{
	abs* buffer = m_sendFactory.newBuffer();
	buffer->m_sessionId = connectId;
	memcpy(buffer->getData(), pHeader, pHeader->m_Length);
	sendBuffer(buffer);
}
void TaskHandle::SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet, int32 msgParamLen, uint8* msgParam, t_ObjId playerId, t_sessionId connectId)
{
	abs* buffer = m_sendFactory.newBuffer();
	memcpy(buffer->getData(), &connectId, sizeof(t_sessionId));
	PacketHeader* header = (PacketHeader*)(buffer->getData() + sizeof(t_sessionId));
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	CProxyHead proxyHead;
	proxyHead.m_srcType = emSvrType_Db;
	proxyHead.m_srcId = 0;
	proxyHead.m_desType = emSvrType_Account;
	proxyHead.m_transferType = trans_p2p;
	proxyHead.m_stTransferInfo.nDstID = playerId % AccountSvr_Num + 1;

	int32 nSpace = buffer->getCapacity() - sizeof(t_sessionId) - sizeof(PacketHeader);
	Stream stream((uint8*)(header + 1), nSpace);
	proxyHead.Serialize(stream);
	stream << msgParamLen;
	if (msgParamLen > 0) stream.BytesSerialize(msgParam, msgParamLen);
	nSpace -= stream.GetOffset();
	if (packet) {
		int32 size = packet->ByteSize();
		if (size > nSpace) {
			log_err("space is mall");
			return;
		}
		else {
			bool rst = packet->SerializeToArray(stream.GetOffsetPointer(), size);
			if (!rst) {
				log_err("unknow error");
				return;
			}
			else {
				stream.SetPointer(size);
			}
		}

		header->m_Length = sizeof(PacketHeader) + stream.GetOffset();
	}
	else {
		header->m_Length = sizeof(PacketHeader);
	}

	sendBuffer(buffer);
}
void TaskHandle::OnHandlePacket(uint64 connectId, PacketHeader*pHeader, Stream& stream)
{
	m_workMgr->OnHandlePacket(this, connectId, pHeader, stream);
}


DbConnectionOpt IWorkMgr::m_conOpt;
IWorkMgr::IWorkMgr(IService* svr)
{
	m_svr = svr;
	m_threadNum = 5;
}


IWorkMgr::~IWorkMgr()
{
}
bool  IWorkMgr::Intial()
{
	Json::Value root;
	Json::Reader jsonReader;
	bool ok = jsonReader.parse(SysUtil::ReadFileCPlus("Config/db.cfg"), root);

	if (ok)
	{
		m_conOpt.m_IP = root.get("IP", "127.0.0.1").asString();
		m_conOpt.m_Port = root.get("Port", 3306).asInt();
		m_conOpt.m_UserName = root.get("UserName", "root").asString();
		m_conOpt.m_Pwd = root.get("Pwd", "123456").asString();
		m_conOpt.m_DbName = root.get("DbName", "db_game").asString();

		m_threadNum = root.get("WorkThread", 5).asInt();
	}
	else{
		log_err("DbConnectionPool db.cfg error");
		return false;
	}

	for (int i = 0; i < m_threadNum; i++) {
		TaskHandle* handle = new TaskHandle();
		m_workHandle.push_back(handle);
	}
	for (int i = 0; i < m_threadNum; i++) {
		TaskHandle* pHandle = m_workHandle[i];
		pHandle->init(this);
		if (!pHandle->getConnection()->Connect(m_conOpt)){
 			return false;
		}
		m_threads.push_back(std::thread(task, pHandle));
	}
	return OnIntial();
}
void IWorkMgr::update()
{
	for (int i = 0; i < m_threadNum; i++)
	{
		m_workHandle[i]->update();
	}
}

void IWorkMgr::addBuffer(PacketCommand cmd, ::google::protobuf::Message* packet, t_sessionId sessionId, uint32 accountId/* = 0*/)
{
	TaskHandle* pHandleTask = m_workHandle[accountId%m_threadNum];
	abs* buffer = pHandleTask->m_recvFactory.newBuffer();
	buffer->m_sessionId = sessionId;
	PacketHeader* header = (PacketHeader*)(buffer->getData());
	header->m_Command = cmd;
	header->m_PacketParam = 0;

	int32 nSpace = buffer->getCapacity()- sizeof(PacketHeader);
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

	pHandleTask->addBuffer(buffer);
}
void IWorkMgr::addBuffer(PacketHeader* pHeader, t_sessionId sessionId , uint32 accountId /*= 0*/)
{
	TaskHandle* pHandleTask = m_workHandle[accountId%m_threadNum];
	abs* buffer = pHandleTask->m_recvFactory.newBuffer();
	buffer->m_sessionId = sessionId;
	memcpy(buffer->getData(), pHeader, pHeader->m_Length);
	pHandleTask->addBuffer(buffer);
}
void IWorkMgr::addBuffer(Wf::abs* newPacket, uint32 accountId)
{
	TaskHandle* pHandleTask = m_workHandle[accountId%m_threadNum];
	pHandleTask->addBuffer(newPacket);
}
void  IWorkMgr::task(TaskHandle* pHandle)
{
	while (true){
		uint nowTick = SysUtil::GetMSTime();
		uint delt = TIMEPOINTDIFF(nowTick, pHandle->m_LastUpdateTick);

		uint max_Process_num = 10;		
		while ((--max_Process_num) >= 0)
		{
			abs** pPacket = pHandle->m_RcvPackets->ReadLock();
			if (pPacket)
			{
				abs* packet = *pPacket;
				pHandle->m_RcvPackets->ReadUnlock();

				PacketHeader* header = (PacketHeader*)(packet->getData());
				pHandle->OnHandlePacket(packet->m_sessionId, header, Stream((uint8*)(header + 1), header->m_Length - sizeof(PacketHeader)));
				packet->release();
			}
			else {
				break;
			}
		}
		pHandle->pingMysql(delt);
	}
}

NS_WF_END