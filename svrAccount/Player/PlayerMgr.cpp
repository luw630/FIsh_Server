#include "PlayerMgr.h"
#include "WfLog.h"
#include "WfLog.h"
#include "Packets/Packet_Server.pb.h"
#include "App/App.h"
#include "Packets/Packet_Login.pb.h"

NS_WF_BEGIN
PlayerMgr::PlayerMgr()
{
	
}


PlayerMgr::~PlayerMgr()
{
}
Player* PlayerMgr::getOnlinePlayer(t_PlayerId playerId)
{
	auto iter = m_onlinePlayer.find(playerId);
	if (iter != m_onlinePlayer.end())
	{
		return iter->second;
	}
	return nullptr;
}
Player* PlayerMgr::getPlayer(t_PlayerId playerId)
{
	auto iter = m_Players.find(playerId);
	if (iter != m_Players.end())
	{
		return iter->second;
	}
	return nullptr;
}
Player* PlayerMgr::getPlayerByAccountId(t_AccountId accountId)
{
	auto iter = m_PlayerByAccount.find(accountId);
	if (iter != m_PlayerByAccount.end())
	{
		return getPlayer(iter->second);
	}
	return nullptr;
}
void PlayerMgr::update(uint delta)
{

}
void PlayerMgr::kickPlayer(Player* pPlayer, bool notifyClient, const string& info)
{
	if (!pPlayer) return;
	if (pPlayer->m_state != Player::emPlayerState_online) return;

	if (notifyClient) {
		rpc::SC_PlayerOtherLogin msg;
		msg.set_m_info(info);
		pPlayer->SendPacket(rpc::EM_SC_PlayerOtherLogin, &msg);
	}
	setOffline(pPlayer);

	rpc::AG_KickPlayer rsp;
	rsp.set_m_playerid(pPlayer->getPlayerId());
	SessionGateWay* pSessionGateWay = App::GetInstance().getListenerGateWay()->getSessionGateWayByNodeId(pPlayer->getGateWayId());
	if (pSessionGateWay)
	{
		pSessionGateWay->SendPacket(rpc::EM_AG_KickPlayer, &rsp);
	}else {
		log_waring("not find SessionGateWay by nodeid(%d)", pPlayer->getGateWayId());
	}


}
void PlayerMgr::LoginOk(Player* pPlayer, int32 gateWayId, t_sessionId sessionId)
{
	if (!pPlayer) return;
	App::GetInstance().m_DbLog->Log_PlayerLogin(pPlayer->getPlayerId(), time(0));

	if (pPlayer->m_state == Player::emPlayerState_new)
	{
		m_Players[pPlayer->getPlayerId()] = pPlayer;
		m_PlayerByAccount[pPlayer->getAccountId()] = pPlayer->getPlayerId();
	}
	else if (pPlayer->m_state == Player::emPlayerState_online){
		kickPlayer(pPlayer,true,"");
		m_offlinePlayer.erase(pPlayer->getPlayerId());
	}
	else {
		m_offlinePlayer.erase(pPlayer->getPlayerId());
	}

	pPlayer->m_state = Player::emPlayerState_online;
	pPlayer->setGateWayId(gateWayId);
	pPlayer->setSessionId(sessionId);
	m_onlinePlayer[pPlayer->getPlayerId()] = pPlayer;

	rpc::AG_LoginRst rsp;
	rsp.set_m_playerid(pPlayer->getPlayerId());
	rsp.set_m_param(sessionId);

	SessionGateWay* pSessionGateWay = App::GetInstance().getListenerGateWay()->getSessionGateWayByNodeId(gateWayId);
	if (pSessionGateWay)
	{
		pSessionGateWay->SendPacket(rpc::EM_AG_LoginRst, &rsp);
		log_dbg("send:EM_AG_LoginRst");

		pPlayer->OnEventEnterLobby();
	}else {
		log_waring("not find SessionGateWay by nodeid(%d)", gateWayId);
	}
}
void PlayerMgr::setOffline(Player* pPlayer)
{
	Assert(pPlayer);
	if (pPlayer->m_state != Player::emPlayerState_online){
		Assert(false);
		return;
	}

	m_onlinePlayer.erase(pPlayer->getPlayerId());
	pPlayer->m_state = Player::emPlayerState_offline;
	pPlayer->setGateWayId(-1);
	pPlayer->setSessionId(INVALID_SESSIONID);
	m_offlinePlayer[pPlayer->getPlayerId()] = pPlayer;
}
NS_WF_END