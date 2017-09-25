#pragma once
#include "WfDataType.h"
#include "Player.h"

NS_WF_BEGIN

class PlayerMgr 
{
public:
	PlayerMgr();
	~PlayerMgr();
public:
	Player* getOnlinePlayer(t_PlayerId playerId);
	Player* getPlayer(t_PlayerId playerId);
	Player* getPlayerByAccountId(t_AccountId accountId);
	int32 getOnlinePlayerNum() { return m_offlinePlayer.size(); }
public:
	void update(uint delta);
	void kickPlayer(Player* pPlayer, bool notifyClient = false, const string& info = "");
	void LoginOk(Player* pPlayer, int32 gateWayId, t_sessionId sessionId);
	void setOffline(Player* pPlayer);
private:
	hash_map <t_PlayerId, Player*> m_Players;
	hash_map <t_AccountId, t_PlayerId> m_PlayerByAccount;			

	hash_map <t_PlayerId, Player*> m_offlinePlayer;
	hash_map <t_PlayerId, Player*> m_onlinePlayer;
};
NS_WF_END