#include "WfLog.h"
#include "SessionGateWay.h"
#include "ListenerGateWay.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_DB.pb.h"
#include "App/App.h"
#include "Player/PlayerMgr.h"
#include "Dispatcher/Dispatcher_Player.h"
#include "WfCommonPackets.h"
using namespace rpc;

NS_WF_BEGIN

SessionGateWay::SessionGateWay()
: ConnectionSession(&App::GetInstance(),ESessionType_Client)
{
	m_bCheck = false;
	m_NodeId = -1;
}
SessionGateWay::~SessionGateWay()
{

}


int SessionGateWay::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command > rpc::EM_sys_End && m_bCheck) {
		PlayerMgr* playerMgr = App::GetInstance().getPlayerMgr();
		Player* pPlayer = playerMgr->getOnlinePlayer(packet->m_PacketParam);
		if (pPlayer)
		{
			Dispatcher_Player& dispatcher = App::GetInstance().getDispatcherPackets()->m_DispatcherPlayer;
			auto handler = dispatcher.getHandle(packet->m_Command);
			if (handler != nullptr) {
				(dispatcher.*handler)(pPlayer, packet, stream);
			}
			else {
				log_waring("not find cmd(%d)", packet->m_Command);
			}
		}
		else {
			log_waring("not find player(%d)", packet->m_PacketParam);
		}
	}
	else if (packet->m_Command == EM_GA_Login && m_bCheck){
		OnHandle_GA_Login(packet, stream);
	}
	else if (packet->m_Command == EInnerPacketCommand_Ping){
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong){
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}
	else if (packet->m_Command == rpc::EM_Sys_SvrNodeLogin) {
		OnHandle_SvrNodeLogin(packet, stream);
	}else if (packet->m_Command == rpc::EM_GA_Offline) {
		OnHandle_GA_Offline(packet, stream);
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
void SessionGateWay::OnHandle_GA_Login(PacketHeader* packet, Stream& stream)
{
	log_dbg("rcv EM_GA_Login");
	GA_Login msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
	
	PlayerMgr* pPlayerMgr = App::GetInstance().getPlayerMgr();
	Player* pPlayer = pPlayerMgr->getPlayerByAccountId(msg.m_account());
	if (pPlayer){
		pPlayerMgr->LoginOk(pPlayer, m_GateWayId,msg.m_param());
	}else {
		SD_LoadData req;
		req.set_m_accountid(msg.m_account());
		req.set_m_param(msg.m_param());
		req.set_m_svrnodeid(m_GateWayId);
		App::GetInstance().getSeesionDb()->addBuffer(em_SD_LoadData,&req, GetSessionId(), getIndex(msg.m_account()));

		//App::GetInstance().m_SessionProxyMgr->m_session->PostMsg(em_SD_LoadData,
		//	&req,
		//	0,
		//	nullptr,
		//	emSvrType_Db,
		//	trans_p2p,
		//	getIndex(msg.m_account()));
	}
}
void SessionGateWay::OnHandle_GA_Offline(PacketHeader* packet, Stream& stream)
{
	rpc::GA_Offline msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	PlayerMgr* pPlayerMgr = App::GetInstance().getPlayerMgr();
	Player* pPlayer = pPlayerMgr->getPlayer(msg.m_playerid());
	if (pPlayer) {
		if (pPlayer->m_state == Player::emPlayerState_online)
		{
			pPlayerMgr->setOffline(pPlayer); 
		}
	}
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
	}else {
		App::GetInstance().getListenerGateWay()->bindGateway(this, msg.m_nodeid());
	}
	SendPacket(rpc::EM_Sys_SvrNodeLoginRst, &rsp);
}
NS_WF_END
