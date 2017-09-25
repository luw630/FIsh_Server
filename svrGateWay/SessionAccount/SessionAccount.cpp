#include "SessionAccount.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Lobby.pb.h"
NS_WF_BEGIN

SessionAccount::SessionAccount()
	: ConnectionSession(&App::GetInstance(), ESessionType_Client)
{

}
SessionAccount::~SessionAccount()
{

}

int SessionAccount::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == rpc::EM_AG_LoginRst)
	{
		OnHandle_AG_LoginRst(packet, stream);
	}
	else if(packet->m_Command >rpc::EM_sys_End){
		Player* pPlayer = App::GetInstance().getListenerClient()->getPlayer(packet->m_PacketParam);
		if (pPlayer)
		{
			if (packet->m_Command == rpc::em_SC_EnterGameRst)
			{
				rpc::SC_EnterGameRst msg;
				msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
				if (msg.m_rst() == 0) {
					pPlayer->m_GameSvrId = msg.m_gameid();
				}
			}
			App::GetInstance().getListenerClient()->SendPacket(pPlayer->sessionId, packet);
		}
		else {
			log_waring("not finde player(%d) cmd(%d)", packet->m_PacketParam, packet->m_Command);
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
	else if (packet->m_Command == rpc::EM_Sys_SvrNodeLoginRst){
		log_dbg("rcv EM_Sys_SvrNodeLoginRst from Account");
	}
	return 0;
}
void SessionAccount::OnConnected()
{
	rpc::Sys_SvrNodeLogin req;
	req.set_m_nodeid(App::GetInstance().GetAppId());
	req.set_m_key(NodeKey);
	SendPacket(rpc::EM_Sys_SvrNodeLogin, &req);
	log_dbg("send Sys_SvrNodeLogin to Account");
}
void SessionAccount::OnDisconnected()
{
	ConnectionSession::OnDisconnected();
}
void SessionAccount::OnHandle_AG_LoginRst(PacketHeader* packet, Stream& stream)
{
	rpc::AG_LoginRst msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	t_sessionId sessionId = msg.m_param();
	bool rst = App::GetInstance().getListenerClient()->bindPlayer(sessionId,msg.m_playerid(),(uint32)GetSessionId());
	if (rst)
	{
		rpc::SC_LoginGateWayRst msg;
		msg.set_m_rst(0);
		App::GetInstance().getListenerClient()->SendPacket(sessionId, rpc::EM_SC_LoginGateWayRst, &msg);
	}  
}
NS_WF_END
