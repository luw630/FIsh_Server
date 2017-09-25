#include "Dispatcher_Games.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Lobby.pb.h"
#include "App/App.h"
NS_WF_BEGIN

bool Dispatcher_Games::Init()
{
	Register(rpc::EM_GameA_EnterGameRst, &Dispatcher_Games::OnHandle_GameA_EnterGameRst);
	return true;
}


void Dispatcher_Games::OnHandle_GameA_EnterGameRst(PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam)
{
	log_dbg("rcv:GameA_EnterGameRst");
	rpc::GameA_EnterGameRst msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	PlayerMgr* pPlayerMgr = App::GetInstance().getPlayerMgr();
	Player* pPlayer = pPlayerMgr->getOnlinePlayer(msg.m_playerid());
	if (!pPlayer)
	{
		log_waring("not find player(%d)", msg.m_playerid());
		//need to do
		//通知GameSvr玩家离线了
		//...
		return;
	}else {
		pPlayer->m_playerPos = Player::emPlayerPos_Game;
	}

	rpc::SC_EnterGameRst rsp;
	rsp.set_m_rst(msg.m_rst());
	rsp.set_m_gameid(msg.m_gameid());
	pPlayer->SendPacket(rpc::em_SC_EnterGameRst, &rsp);
	pPlayer->OnEventEnterGame(msg.m_rst(), msg.m_gameid());
}
NS_WF_END