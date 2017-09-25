#include "WfLog.h"
#include "SessionClient.h"
#include "ListenerClient.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"

#include "Packets/Packet_Base.pb.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "App/App.h"
#include "WfCommonPackets.h"

using namespace rpc;

NS_WF_BEGIN

SessionClient::SessionClient()
: ConnectionSession(&App::GetInstance(),ESessionType_Client)
{

}
SessionClient::~SessionClient()
{

}


int SessionClient::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == EInnerPacketCommand_Ping) {
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong) {
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}else if(packet->m_Command == rpc::EM_CS_LoginGateWayOld) {
		onHandleLoginGateWayOld(packet, stream);
	}else if (packet->m_Command == rpc::EM_CS_LoginGateWay) {
		onHandleLoginGateWay(packet, stream);
	}else{
		Player* pPlayer = App::GetInstance().getListenerClient()->getPlayerBySessionId(GetSessionId());
		if (!pPlayer) {
			//关闭会话
			//...
			return 0;
		}
		if (packet->m_Command>rpc::emPacket_Lobby_Begin && packet->m_Command<rpc::emPacket_Lobby_End) {
		//大厅
			ConnectorMgr<SessionAccount>* connectorMgr = App::GetInstance().getSessionAccount();
			packet->m_PacketParam = pPlayer->playerId;
			connectorMgr->SendPacket(pPlayer->m_accountSvrId, packet);
		}else if (packet->m_Command > rpc::emPacket_Lobby_End) {
		//游戏
			int32 gameId = packet->m_PacketParam;
			ConnectorMgr<SessionGameSvr>* connectorMgr = App::GetInstance().getSessionGameSvr();
			packet->m_PacketParam = pPlayer->playerId;
			connectorMgr->SendPacket(gameId, packet);
		}else {
			GetConnection()->Close();
			log_waring("error cmd(%d)", packet->m_Command);
		}
	}
	return 0;
}
void SessionClient::OnConnected()
{
	App::GetInstance().getListenerClient()->m_tmpSession.insert(GetSessionId());
	//log_dbg("new gameSession sessionId(%llu)", GetSessionId());
}
void SessionClient::OnDisconnected()
{
	Player* pPlayer = App::GetInstance().getListenerClient()->getPlayerBySessionId(GetSessionId());
	if (pPlayer) {
		if (pPlayer->m_GameSvrId != -1) {
			rpc::GGame_Offline req;
			req.set_m_playerid(pPlayer->playerId);
			req.set_m_sessionid(GetSessionId());
			App::GetInstance().getSessionGameSvr()->SendPacket(pPlayer->m_GameSvrId, rpc::EM_GGame_Offline, &req);
		}
		else {
			rpc::GA_Offline req;
			req.set_m_playerid(pPlayer->playerId);
			App::GetInstance().getSessionAccount()->SendPacket(1, rpc::EM_GA_Offline, &req);
		}
		App::GetInstance().getListenerClient()->delPlayer(pPlayer);
	}
	else {
		App::GetInstance().getListenerClient()->m_tmpSession.erase(GetSessionId());
	}
}
void SessionClient::onHandleLoginGateWayOld(PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv EM_CS_LoginGateWayOld");
	CS_LoginGateWayOld msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());


	if(msg.m_account().empty()){
		//账号为空
		rpc::SC_LoginGateWayRst msg;
		msg.set_m_rst(1);	
		SendPacket(rpc::EM_SC_LoginGateWayRst, &msg);
		log_waring("send SC_LoginGateWayRst,login failed,account error");
		shutDownSession();
		return;
	}
	//通知Account，玩家登录
	GA_Login req;
	req.set_m_account(msg.m_account());
	req.set_m_param(GetSessionId());
	log_dbg("send GA_Login (%llu)", GetSessionId());
	App::GetInstance().getSessionAccount()->SendPacket(1, rpc::EM_GA_Login, &req);
}
void SessionClient::onHandleLoginGateWay(PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv EM_CS_LoginGateWay");
	CS_LoginGateWay msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	MemoryDbPacket_OutData getData;
	getData.m_key = msg.m_sessionid();
	getData.m_callParam = TypeConvert::ToString(GetSessionId());

	App::GetInstance().getSessionMemoryDb()->SendPacket(1, emMemoryDbPacket_OutData, &getData);
}
NS_WF_END
