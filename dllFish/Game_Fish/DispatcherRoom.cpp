#include "DispatcherRoom.h"
#include "Packets/Packet_Fish.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "Game_Fish/TableMgr.h"
#include "GameFish.h"
#include "GamePacket.h"

NS_WF_BEGIN
DispatcherRoom::DispatcherRoom(GameFish* handle)
{
	m_gameHandle = handle;
}
bool DispatcherRoom::Init()
{
	Register(rpc::em_CS_FishGameData, &DispatcherRoom::OnHandle_CS_FishGameData);
	Register(rpc::em_CS_FishEnterTable, &DispatcherRoom::OnHandle_CS_FishEnterTable);
	return true;
}
void DispatcherRoom::EnterTable(Player* pPlayer, int32 tableId, int32 seatId)
{
	if (pPlayer->getStatus() == emPlayerStatus_PreGame ||
		pPlayer->getStatus() == emPlayerStatus_Game)
	{
		//重新进入原来的桌子
		log_dbg("reEnter old table(%d) seatId(%d)", pPlayer->getTableId(), pPlayer->getSeatId());
		Table* pTable = m_gameHandle->getTableMgr()->getTable(pPlayer->getTableId());
		if (!pTable) {
			Assert(pTable);
			return;
		}
		abs* buffer = pTable->newBuffer_Rcv();
		*(t_ObjId*)(buffer->getData()) = Invalid_ObjId;
		t_ReEnterTable* data = (t_ReEnterTable*)(buffer->getData() + sizeof(t_ObjId));
		data->m_PacketParam = 0;
		data->m_Length = sizeof(t_ReEnterTable);
		data->m_Command = emTableFishPacket_t_ReEnterTable;
		data->m_player = pPlayer;
		pTable->pushMsg(buffer);
		return;
	}
	else {
		Table* pTable = m_gameHandle->getTableMgr()->getTable(tableId);
		if (!pTable) {
			log_waring("not find table(%d)", tableId);
			return;
		}
		pPlayer->setStatus(emPlayerStatus_PreGame);
		pPlayer->setTableId(tableId);
		pPlayer->setSeatId(seatId);

		abs* buffer = pTable->newBuffer_Rcv();
		*(t_ObjId*)(buffer->getData()) = Invalid_ObjId;
		t_EnterTable* data = (t_EnterTable*)(buffer->getData() + sizeof(t_ObjId));

		data->m_PacketParam = 0;
		data->m_Length = sizeof(t_EnterTable);
		data->m_Command = emTableFishPacket_t_EnterTable;
		data->m_player = pPlayer;
		data->m_seatId = seatId;

		pTable->pushMsg(buffer);
	}
}
void DispatcherRoom::EnterGameOk(Player* pPlayer, int32 gateWaySvrId, int32 gameId)
{
	rpc::GameA_EnterGameRst rsp;
	rsp.set_m_rst(0);
	rsp.set_m_playerid(pPlayer->getId());
	rsp.set_m_gameid(gameId);
	m_gameHandle->m_AppProxy->SendPacketToAccountSvr(rpc::EM_GameA_EnterGameRst, &rsp);
	pPlayer->setGateWayId(gateWaySvrId);
	pPlayer->OnEventEnterGame();

	if (pPlayer->getStatus() == emPlayerStatus_PreGame||
		pPlayer->getStatus() == emPlayerStatus_Game)
	{
		EnterTable(pPlayer, pPlayer->getTableId(), pPlayer->getSeatId());
	}
}
void DispatcherRoom::OnHandle_AGame_EnterGame(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam)
{
	rpc::AGame_EnterGame msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());


	t_ObjId playerId = msg.m_playerid();
	Player* pPlayer = (Player*)m_gameHandle->getPlayerMgr()->getObjectById(playerId);
	if (pPlayer)
	{
		EnterGameOk(pPlayer, msg.m_gatewayid(), msg.m_gameid());
		return;
	}

	int32 rst = -100;
	do {
		pPlayer = new Player(m_gameHandle);
		if (!pPlayer->Init(playerId, msg.m_playerdata()))
		{
			log_waring("init player(%d) error", playerId);
			delete pPlayer;
			rst = 1;
			break;
		}
		if (!m_gameHandle->getPlayerMgr()->addObject(pPlayer)) {
			log_waring("playerMgr add player(%d) error", playerId);
			delete pPlayer;
			rst = 2;
			break;
		}

		EnterGameOk(pPlayer, msg.m_gatewayid(), msg.m_gameid());
		rst = 0;
	} while (false);
	
	if (rst != 0) {
		rpc::GameA_EnterGameRst rsp;
		rsp.set_m_rst(rst);
		rsp.set_m_playerid(msg.m_playerid());
		rsp.set_m_gameid(msg.m_gameid());
		m_gameHandle->m_AppProxy->SendPacketToAccountSvr(rpc::EM_GameA_EnterGameRst, &rsp);
	}
}
void DispatcherRoom::OnHandle_AGame_LeaveGame(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam)
{
	rpc::AGame_LeaveGame msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	PlayerMgr* pPlayerMgr = m_gameHandle->getPlayerMgr();
	Player* pPlayer = (Player*)pPlayerMgr->getObjectById(msg.m_playerid());
	if (!pPlayer)
	{
		rpc::GameA_LeaveGameRst rsp;
		rsp.set_m_rst(0);
		rsp.set_m_playerid(msg.m_playerid());

		m_gameHandle->m_AppProxy->SendPacketToAccountSvr(rpc::EM_GameA_LeaveGameRst, &rsp);
	}
	//移除对象
	//...

}
void DispatcherRoom::OnHandle_CS_FishGameData(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv:em_CS_FishGameData");
	rpc::SC_FishGameData rsp;
	rsp.set_m_playernum(m_gameHandle->getPlayerMgr()->getPlayerNum());
	
	int32 tableNum = m_gameHandle->getTableViewMgr()->getTableNum();
	for (int i = 0; i < tableNum; i++)
	{
		t_TableFishData* table = m_gameHandle->getTableViewMgr()->getTableData(i);
		rpc::t_fish_table* data = rsp.add_m_tables();
		table->dup(data);
	}
	pPlayer->SendPacket(rpc::em_SC_FishGameData, &rsp);
}
void DispatcherRoom::OnHandle_CS_FishEnterTable(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv:em_CS_FishEnterTable");
	rpc::CS_FishEnterTable msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	EnterTable(pPlayer, msg.m_tableid(), msg.m_seatid());
}

NS_WF_END