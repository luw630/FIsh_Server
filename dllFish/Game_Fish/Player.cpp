#include "Player.h"
#include "WfLog.h"
#include "Packets/Packet_Fish.pb.h"
#include "GameFish.h"
#include "Table.h"
NS_WF_BEGIN
Player::Player(GameFish* handle): IPlayer(handle)
{
	m_gameHandle = handle;
}
Player::~Player()
{

}
void Player::OnEventEnterGame()
{

}

bool Player::Init(t_ObjId playerId, const rpc::t_EnterGamePlayerData& playerData)
{
	m_PlayerStatus = emPlayerStatus_Lobby;
	m_tableId = Invalid_ObjId;
	m_seatId = Invalid_ObjId;
	m_ObjectId = playerId;
	m_headId = playerData.m_headid();
	m_money = playerData.m_money();
	m_BombType = rpc::BulletType_normal;
	m_BombSendTime = 0;
	m_lockFishid = Invalid_ObjId;
	return true;
}

bool Player::SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet)
{
	//主线程中调用
	static uint8 s_sendBuffer[8192];
	PacketHeader* header = (PacketHeader*)s_sendBuffer;
	header->m_Command = cmd;
	header->m_PacketParam = m_ObjectId;
	if (packet)
	{
		int32 size = packet->ByteSize();
		int32 nSpace = 8192 - sizeof(PacketHeader);
		if (size > nSpace) {
			log_err("space is mall");
			return false;
		}
		else {
			bool rst = packet->SerializeToArray(header + 1, size);
			if (!rst) {
				log_err("unknow error");
				return false;
			}
		}

		header->m_Length = sizeof(PacketHeader) + size;
	}
	else {
		header->m_Length = sizeof(PacketHeader);
	}

	return  SendMsg(header);
}
void Player::LockFish(Table* pTable, t_ObjId fishid)
{
	m_lockFishid = fishid;
	if (m_lockFishid != Invalid_ObjId)
	{
		rpc::SC_FishLockFish notify;
		notify.set_m_fishid(fishid);
		notify.set_m_seatid(m_seatId);
		pTable->broadCastPacket(rpc::em_SC_FishLockFish,&notify);
	}
}
NS_WF_END

