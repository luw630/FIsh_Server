#include "WfLog.h"
#include "SessionGateWay.h"
#include "ListenerGateWay.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Fish.pb.h"
#include "App.h"
using namespace rpc;

NS_WF_BEGIN

SessionGateWay::SessionGateWay()
: ConnectionSession(&App::GetInstance(),ESessionType_Client)
{
	m_bCheck = false;
	m_GatewayId = Invalid_ObjId;
}
SessionGateWay::~SessionGateWay()
{

}


int SessionGateWay::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == rpc::EM_Sys_SvrNodeLogin) {
		OnHandle_SvrNodeLogin(packet, stream);
	}
	else if (packet->m_Command == EInnerPacketCommand_Ping) {
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong) {
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}
	else if (packet->m_Command == rpc::EM_GGame_Offline && m_bCheck) {
		GGame_Offline msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
		App::GetInstance().getGame()->OnPlayerOffLine(msg.m_playerid(), getGatewayId(), msg.m_sessionid());
	}
	else if (m_bCheck) {
		App::GetInstance().getGame()->OnGateWayPacket(this, packet, stream);
	}
	else {
		log_waring("error packet(%d) check(%s)", packet->m_Command, m_bCheck == true ? "true" : "false");
	}
	return 0;
}
void SessionGateWay::OnConnected()
{
	log_dbg("new gameSession sessionId(%llu)", GetSessionId());
}
void SessionGateWay::OnDisconnected()
{

}
void SessionGateWay::OnHandle_SvrNodeLogin(PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv Sys_SvrNodeLogin");
	Sys_SvrNodeLogin msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::Sys_SvrNodeLoginRst rsp;
	rsp.set_m_rst(0);
	if (msg.m_key().compare(NodeKey) != 0)
	{
		log_waring("error node connected");
		rsp.set_m_rst(1);	//key´íÎó
		SendPacket(rpc::EM_Sys_SvrNodeLoginRst, &rsp);
		GetConnection()->Close();
	}
	else {
		App::GetInstance().getListenerGateWay()->bindNodeId(this, msg.m_nodeid());
	}
	SendPacket(rpc::EM_Sys_SvrNodeLoginRst, &rsp);
}

NS_WF_END
