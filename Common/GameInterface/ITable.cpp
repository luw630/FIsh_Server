#include "ITable.h"
#include "IGameService.h"
#include "IGame.h"
#include "WfLog.h"


NS_WF_BEGIN
ITable::ITable()
{
	m_RecvPackets = nullptr;
	m_GameService = nullptr;
	m_RecvPackets = new LockFreeList<abs*>();
}
ITable::~ITable()
{
	WF_SAFE_DELETE(m_RecvPackets);
}
bool ITable::Init(t_ObjId id, IGameService* service, IGame* game)
{
	setId(id);
	m_GameService = service;
	m_game = game;

	return onInit();
}
void ITable::pushMsg(abs* msg)
{
	abs** pNewPacket = m_RecvPackets->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = msg;
		m_RecvPackets->WriteUnlock();
	}
}
void ITable::sendPacket(abs* msg)
{
	m_GameService->sendPacket(msg);
}
void ITable::sendPacket(t_ObjId playerId, PacketCommand cmd, ::google::protobuf::Message* packet)
{
	abs* buffer = m_GameService->newBuffer_Send();
	PacketHeader* header = (PacketHeader*)buffer->getData();
	header->m_Command = cmd;
	header->m_PacketParam = playerId;
	if (packet){
		int32 size = packet->ByteSize();
		int32 nSpace = abs_Size - sizeof(PacketHeader);
		if (size > nSpace) {
			log_err("space is mall");
			return;
		}else {
			bool rst = packet->SerializeToArray(header + 1, size);
			if (!rst) {
				log_err("unknow error");
				return;
			}
		}
		header->m_Length = sizeof(PacketHeader) + size;
	}else {
		header->m_Length = sizeof(PacketHeader);
	}
	sendPacket(buffer);
}
void ITable::Update(uint32 delta)
{
	uint max_Process_num = 100;	
	while ((--max_Process_num) >= 0)
	{
		abs** pPacket = m_RecvPackets->ReadLock();
		if (pPacket) {
			abs* packet = *pPacket;
			m_RecvPackets->ReadUnlock();

			onDispatcherMsg(packet);
			packet->release();
		}else {
			break;
		}
		
	}
	onUpdate(delta);
}

IGameService* ITable::getGameService() 
{ 
	return m_GameService; 
}
IGame* ITable::getGame()
{
	return m_game;
}
abs* ITable::newBuffer_Rcv() 
{ 
	return m_GameService->newBuffer_Rcv(); 
}
abs* ITable::newBuffer_Send() 
{ 
	return m_GameService->newBuffer_Send(); 
}
NS_WF_END

