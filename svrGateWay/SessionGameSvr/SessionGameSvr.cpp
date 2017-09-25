#include "SessionGameSvr.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Server.pb.h"

NS_WF_BEGIN

SessionGameSvr::SessionGameSvr()
	: ConnectionSession(&App::GetInstance(), ESessionType_Client)
{

}
SessionGameSvr::~SessionGameSvr()
{

}

int SessionGameSvr::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command >rpc::EM_sys_End) {
		Player* pPlayer = App::GetInstance().getListenerClient()->getPlayer(packet->m_PacketParam);
		if (pPlayer)
		{
			App::GetInstance().getListenerClient()->SendPacket(pPlayer->sessionId,packet);
			log_dbg("send cmd(%d)", packet->m_Command);
		}
		else {
			log_waring("not finde player(%d)", packet->m_PacketParam);
		}
	}
	else if (packet->m_Command == EInnerPacketCommand_Ping) {
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong) {
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}
	else if (packet->m_Command == rpc::EM_Sys_SvrNodeLoginRst) {
		log_dbg("rcv EM_Sys_SvrNodeLoginRst from GameSvr");
	}
	return 0;
}
void SessionGameSvr::OnConnected()
{
	rpc::Sys_SvrNodeLogin req;
	req.set_m_nodeid(App::GetInstance().GetAppId());
	req.set_m_key(NodeKey);
	SendPacket(rpc::EM_Sys_SvrNodeLogin, &req);
	log_dbg("send Sys_SvrNodeLogin to GameSvr");
}
void SessionGameSvr::OnDisconnected()
{
	ConnectionSession::OnDisconnected();
}
NS_WF_END
