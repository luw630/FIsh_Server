#include "DispatcherTable.h"
#include "Packets/Packet_Fish.pb.h"
#include "Game_Fish/TableMgr.h"
#include "GameFish.h"
#include "GamePacket.h"
#include "WfRandom.h"
NS_WF_BEGIN
DispatcherTable::DispatcherTable(Table* handle)
{
	m_pTable = handle;
}
bool DispatcherTable::Init()
{
	m_pTable->Register(emTableFishPacket_t_EnterTable, &DispatcherTable::OnHandle_EnterTable);
	m_pTable->Register(emTableFishPacket_t_ReEnterTable, &DispatcherTable::OnHandle_ReEnterTable);
	m_pTable->Register(emTableFishPacket_t_PlayerOffLine, &DispatcherTable::OnHandle_PlayerOffLine);
	m_pTable->Register(rpc::em_CS_FishChangeFireCost, &DispatcherTable::OnHandle_CS_FishChangeFireCost);	
	m_pTable->Register(rpc::em_CS_FishReqSwitchCannon, &DispatcherTable::OnHandle_CS_FishReqSwitchCannon);	
	m_pTable->Register(rpc::em_CS_FishSendBullet, &DispatcherTable::OnHandle_CS_FishSendBullet);
	m_pTable->Register(rpc::em_CS_FishNotifyHitted, &DispatcherTable::OnHandle_CS_FishNotifyHitted);
	return true;
}
void DispatcherTable::OnHandle_EnterTable(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	t_EnterTable* msg = (t_EnterTable*)header;

	Seat* pSeat = m_pTable->getSeat(msg->m_seatId);
	if (!pSeat){
		log_err("seat(%d) id error", msg->m_seatId);
		return;
	}
	Player* pPlayer = msg->m_player;

	abs* buffer = m_pTable->m_GameService->newBuffer_Send();
	t_EnterTableRst* packet = (t_EnterTableRst*)buffer->getData();
	packet->m_Command = emTableFishPacket_t_EnterTableRst;
	packet->m_PacketParam = 0;
	packet->m_Length = sizeof(t_EnterTableRst);

	packet->m_rst = 0;
	packet->m_tableid = m_pTable->getId();
	packet->m_seatId = msg->m_seatId;
	packet->m_player = msg->m_player;


	if (pSeat->havePlayer() && pSeat->getPlayer() != pPlayer){
		packet->m_rst = 1;	//位置上有人了
		m_pTable->sendPacket(buffer);

		rpc::SC_FishEnterTableRst rsp;
		rsp.set_m_rst(1);
		rsp.set_m_tableid(m_pTable->getId());
		rsp.set_m_seatid(msg->m_seatId);
		m_pTable->dupPlayers(rsp.mutable_m_players());
		m_pTable->sendPacket(pPlayer->getId(),rpc::em_SC_FishEnterTableRst,&rsp);
		return;
	}
	else {
		pSeat->bindPlayer(pPlayer);
		packet->m_rst = 0;
		m_pTable->sendPacket(buffer);

		rpc::SC_FishEnterTableRst rsp;
		rsp.set_m_rst(0);
		rsp.set_m_tableid(m_pTable->getId());
		rsp.set_m_seatid(msg->m_seatId);
		rsp.set_m_bomb(pPlayer->getBombType());
		m_pTable->sendPacket(pPlayer->getId(), rpc::em_SC_FishEnterTableRst, &rsp);
		m_pTable->onAddPlayer(pPlayer);
	}
}
void DispatcherTable::OnHandle_ReEnterTable(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	t_ReEnterTable* msg = (t_ReEnterTable*)header;

	Player* pPlayer = msg->m_player;

	Seat* pSeat = m_pTable->getSeat(pPlayer->getSeatId());
	Assert(pSeat);
	
	if (pSeat->getPlayer() == pPlayer) {
		rpc::SC_FishEnterTableRst rsp;
		rsp.set_m_rst(0);
		rsp.set_m_tableid(m_pTable->getId());
		rsp.set_m_seatid(pSeat->getId());
		rsp.set_m_bomb(pPlayer->getBombType());
		m_pTable->dupPlayers(rsp.mutable_m_players());
		m_pTable->sendPacket(pPlayer->getId(), rpc::em_SC_FishEnterTableRst, &rsp);
		m_pTable->onPlayerReEnter(pPlayer, pSeat);
		return;
	}
	else {
		rpc::SC_FishEnterTableRst rsp;
		rsp.set_m_rst(2);
		rsp.set_m_tableid(m_pTable->getId());
		rsp.set_m_seatid(pSeat->getId());
		m_pTable->sendPacket(pPlayer->getId(), rpc::em_SC_FishEnterTableRst, &rsp);
		log_err("player reEnterTable unknow error,playerid(%d),tableid(%d),seatid(%d)",pPlayer->getId(), m_pTable->getId(),pSeat->getId());
	}
}
void DispatcherTable::OnHandle_PlayerOffLine(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	t_PlayerOffLine* msg = (t_PlayerOffLine*)header;
	Seat* pSeat = m_pTable->getSeat(msg->m_seatId);

	Player* pPlayer = pSeat->getPlayer();
	if (pPlayer && pPlayer->getId()==msg->m_playerId) {
		m_pTable->rmvPlayer(pSeat);
	}
	
	abs* buffer = m_pTable->m_GameService->newBuffer_Send();
	t_PlayerLeaveTable* packet = (t_PlayerLeaveTable*)buffer->getData();
	packet->m_Command = emTableFishPacket_t_PlayerLeaveTable;
	packet->m_PacketParam = 0;
	packet->m_Length = sizeof(t_PlayerLeaveTable);

	packet->m_playerId = msg->m_playerId;
	packet->m_type = emPlayerLeaveTableType_leaveFishSessionIdInvalid;
	packet->m_sessionId = msg->m_sessionId;
	packet->m_tableid = m_pTable->getId();
	packet->m_seatId = pSeat->getId();

	m_pTable->sendPacket(buffer);
}


void DispatcherTable::OnHandle_CS_FishChangeFireCost(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	Seat* pSeat = m_pTable->getSeat(seatId);
	if (!pSeat) return;
	Player* pPlayer = pSeat->getPlayer();
	if (!pPlayer) return;

	if (pPlayer->getId() != header->m_PacketParam) return;
	rpc::CS_FishChangeFireCost msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	if (msg.m_icost() >0 )
	{
		rpc::SC_FishChangeFireCost notify;
		notify.set_m_iseatid(seatId);
		notify.set_m_icost(msg.m_icost());
		m_pTable->broadCastPacket(rpc::em_SC_FishChangeFireCost, &notify);

		pPlayer->setFireCost(msg.m_icost());
	}
}
void DispatcherTable::OnHandle_CS_FishReqSwitchCannon(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	Seat* pSeat = m_pTable->getSeat(seatId);
	if (!pSeat) return;
	Player* pPlayer = pSeat->getPlayer();
	if (!pPlayer) return;

	if (pPlayer->getId() != header->m_PacketParam) return;
	rpc::CS_FishReqSwitchCannon msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	if (msg.m_icannontype() >0  &&
		msg.m_icannontype() < rpc::BulletType_max )
	{
		rpc::SC_FishNotifySwitchCannon notify;
		notify.set_m_iplayerid(pPlayer->getId());
		notify.set_m_iseatid(seatId);
		notify.set_m_icannontype(msg.m_icannontype());
		m_pTable->broadCastPacket(rpc::em_SC_FishNotifySwitchCannon, &notify);

		pPlayer->setBombType(msg.m_icannontype());
	}
}
void DispatcherTable::OnHandle_CS_FishSendBullet(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	Seat* pSeat = m_pTable->getSeat(seatId);
	if (!pSeat) return;
	Player* pPlayer = pSeat->getPlayer();
	if (!pPlayer) return;

	if (pPlayer->getId() != header->m_PacketParam) return;
	rpc::CS_FishSendBullet msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	switch (pPlayer->getBombType())
	{
	case rpc::BulletType_fishingnet: 
	case rpc::BulletType_normal:
	case rpc::BulletType_drill: {
		Bullet* pBullet = m_pTable->m_ObjectFactory.newBullet(pPlayer->getBombType(), Vector3(msg.m_dir().x(), msg.m_dir().y(), 0), pSeat->getId());
		if (!pBullet) {
			log_waring("send bullet error.playerid(%u),seatid(%d),bullettype(%d),dir(%f,%f)",
				pPlayer->getId(),
				pSeat->getId(),
				pPlayer->getBombType(),
				msg.m_dir().x(),
				msg.m_dir().y());
			return;
		}
		m_pTable->m_BulletMgr.addObj(pBullet);
	}break;
	case rpc::BulletType_laser: {

	}break;
	default:
		break;
	}
}
void DispatcherTable::OnHandle_CS_FishNotifyHitted(t_ObjId seatId, PacketHeader* header, Stream& stream)
{
	Seat* pSeat = m_pTable->getSeat(seatId);
	if (!pSeat) return;
	Player* pPlayer = pSeat->getPlayer();
	if (!pPlayer) return;

	if (pPlayer->getId() != header->m_PacketParam) return;
	rpc::CS_FishNotifyHitted msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	Fish* fish = m_pTable->m_FishMgr.getFish(msg.m_fishid());
	if (fish) {
		int32 point = 0;
		fish->t_fishRadio = Random::RandomInt(fish->t_minFishRadio, fish->t_maxFishRadio);
		if (m_pTable->isFishDead(
			100,
			fish->t_maxFishRadio,
			fish->t_minFishRadio,
			point))
		{
			fish->m_KillSeatId = pSeat->getId();
			fish->m_KillPlayerId = pSeat->getPlayerId();
			rpc::SC_FishNotifyHitFish notify;
			notify.set_m_bulletid(0);
			notify.set_m_fishid(msg.m_fishid());
			notify.set_m_seatid(seatId);
			notify.set_m_playerid(pPlayer->getId());
			notify.set_m_money(point);
			m_pTable->broadCastPacket(rpc::em_SC_FishNotifyHitFish, &notify);
			log_dbg("............hit fish(%d)", fish->getId());
			fish->setUnActive();
		}

	}
	else {
		log_waring("not find fish(%d)", msg.m_fishid());
	}
	
}
NS_WF_END