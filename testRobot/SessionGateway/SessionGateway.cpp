#include "SessionGateway.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Fish.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "WfCommonPackets.h"

using namespace rpc;
NS_WF_BEGIN
#define GameFishId	1
SessionGateway::SessionGateway()
	: ConnectionSession(&App::GetInstance(), ESessionType_Client)
{
	m_loginSession = "";
	m_loginState = emLoginState_unLogin;
	m_bBattle = false;
	m_BattleTime = 0;
}
SessionGateway::~SessionGateway()
{

} 

int SessionGateway::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == EInnerPacketCommand_Ping) {
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong)
	{
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}
	else if (packet->m_Command == rpc::EM_SC_LoginRst)
	{		
		rpc::SC_LoginRst msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
		if (msg.m_rst() == 0) {
			m_loginState = emLoginState_Login;
			m_loginSession = msg.m_sessionid();
		}
		else {
			m_loginState = emLoginState_unLogin;
		}
		log_dbg("rcv: EM_SC_LoginRst.rst(%d),sessionid(%s)",msg.m_rst(),msg.m_sessionid().c_str());
	}
	else if (packet->m_Command == rpc::EM_SC_LoginGateWayRst)
	{
		log_dbg("rcv: EM_SC_LoginGateWayRst");
	}
	else if (packet->m_Command == rpc::em_SC_PlayerInfo){
		log_dbg("rcv: em_SC_PlayerInfo");
		rpc::CS_EnterGame req;
		req.set_m_gameid(1);
		SendPacket(rpc::em_CS_EnterGame, &req);
		log_dbg("send: em_CS_EnterGame");
	}
	else if (packet->m_Command == rpc::em_SC_EnterGameRst)
	{
		rpc::SC_EnterGameRst msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
		if (msg.m_rst() != 0)
		{
			log_waring("enter game(%d) failed(%d).", msg.m_gameid(), msg.m_rst());
		}else {
			log_dbg("rcv: em_SC_EnterGameRst success");
			SendPacket(rpc::em_CS_FishGameData, GameFishId);
			log_dbg("send: em_CS_FishGameData");
		}
	}
	else if (packet->m_Command == rpc::em_SC_FishGameData)
	{
		rpc::SC_FishGameData msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
			
		int tableSize = msg.m_tables_size();
		for (int i = 0; i < tableSize; i++)
		{
			const rpc::t_fish_table& table = msg.m_tables(i);
			int seatnum = table.m_seats_size();
			log_dbg("table(%d) seatNum(%d):", table.m_tableid(), seatnum);
			for (int j = 0; j < seatnum; j++)
			{
				const rpc::t_fish_seat& obj = table.m_seats(j);
				log_dbg("seatid(%d) playerId(%d)",obj.m_seatid(),obj.m_playerid());
			}
		}
		//½øÈë×À×Ó
		rpc::CS_FishEnterTable req;
		req.set_m_tableid(m_tableId);
		req.set_m_seatid(m_seatId);
		SendPacket(rpc::em_CS_FishEnterTable, &req, GameFishId);
		log_dbg("send: em_CS_FishEnterTable tableid(%d) seatid(%d)", m_tableId, m_seatId);
	}
	else if (packet->m_Command == rpc::em_SC_FishEnterTableRst)
	{
		rpc::SC_FishEnterTableRst msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

		if (msg.m_rst() != 0) {
			log_waring("enter table failed.player(%s) table(%d) seat(%d", m_account.c_str(), m_tableId, m_seatId);
		}else {
			log_dbg("enter table success.player(%s) table(%d) seat(%d", m_account.c_str(), m_tableId, m_seatId);
			m_bBattle = true;
			m_BattleTime = 0;
		}
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyAddPlayerMySelf)
	{
		log_dbg("rcv: em_SC_FishNotifyAddPlayerMySelf");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyAddPlayer)
	{
		log_dbg("rcv: em_SC_FishNotifyAddPlayer");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyAddBullet)
	{
		SC_FishNotifyAddBullet msg;
		msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

		log_dbg("rcv: em_SC_FishNotifyAddBullet");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyAddFish)
	{
		log_dbg("rcv: em_SC_FishNotifyAddFish");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyRmvBullet)
	{
		log_dbg("rcv: em_SC_FishNotifyRmvBullet");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyRmvFish)
	{
		log_dbg("rcv: em_SC_FishNotifyRmvFish");
	}
	else if (packet->m_Command == rpc::em_SC_FishNotifyRmvPlayer)
	{
		log_dbg("rcv: em_SC_FishNotifyRmvPlayer");
	}
	else if (packet->m_Command == rpc::em_SC_FriendList)
	{
		log_dbg("rcv: em_SC_FriendList");
	}
	else if (packet->m_Command == rpc::em_SC_FriendReqList)
	{
		log_dbg("rcv: em_SC_FriendReqList");
	}
	else if (packet->m_Command == rpc::em_SC_FriendFindRst)
	{
		log_dbg("rcv: em_SC_FriendFindRst");
		
	}	
	else if (packet->m_Command == rpc::em_SC_NotifyReqAddFriend)
	{
		log_dbg("rcv: em_SC_NotifyReqAddFriend");
		
	}
	else if (packet->m_Command == rpc::em_SC_RankData)
	{
		log_dbg("rcv: em_SC_RankData");
	}
	else if (packet->m_Command == rpc::em_SC_Announcement)
	{
		log_dbg("rcv: em_SC_Announcement");
	}
	else
	{
		log_dbg("not set cmd(%d)", packet->m_Command);
	}
	return 0;
}
void SessionGateway::Update(uint32 delta)
{
	if (m_bBattle) {
		m_BattleTime += delta;
		while (m_BattleTime >= 1000) {
			m_BattleTime -= 1000;

			rpc::CS_FishSendBullet req;
			rpc::t_Vector2* dir = req.mutable_m_dir();
			dir->set_x(10.6f);
			dir->set_y(100.33f);
			req.set_m_multiple(1);
			SendPacket(rpc::em_CS_FishSendBullet, &req, GameFishId);
		}
	}
}
void SessionGateway::OnConnected()
{
	m_bBattle = false;

	if (m_loginState == emLoginState_unLogin)
	{
		CS_Login req;
		req.set_m_type(0);
		req.set_m_account(m_account);
		req.set_m_pwd("123456");
		SendPacket(EM_CS_Login, &req);
	}
	else if (m_loginState == emLoginState_GateWay) {
		CS_LoginGateWay req;
		req.set_m_sessionid(m_loginSession);
		SendPacket(EM_CS_LoginGateWay, &req);
	}
}
void SessionGateway::OnDisconnected()
{
	Connection* pConnection = GetConnection();
	if (!pConnection)
	{
		Assert(false);
		return;
	}

	if (m_loginState == emLoginState_unLogin)
	{
		pConnection->reSetPort(25550);
		ConnectionSession::OnDisconnected();
	}else if(m_loginState == emLoginState_Login){
		m_loginState = emLoginState_GateWay;

		pConnection->reSetPort(24000);
		ConnectionSession::OnDisconnected();
	}
	else if (m_loginState == emLoginState_GateWay)
	{
		m_loginState = emLoginState_unLogin;
		pConnection->reSetPort(25550);
		ConnectionSession::OnDisconnected();
	}
	m_bBattle = false;
}
NS_WF_END
