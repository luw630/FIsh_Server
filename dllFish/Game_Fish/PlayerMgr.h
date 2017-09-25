#pragma once
#include "Player.h"

NS_WF_BEGIN
class GameFish;
class PlayerMgr
{
private:
	GameFish* m_gameHandle;
public:
	PlayerMgr(GameFish* handle);
	virtual ~PlayerMgr();
public:
	Player* getObjectById(const t_ObjId& objectId);
	bool addObject(Player* obj);
	int32 getPlayerNum(){ return m_PlayerNum; }
	Player* rmvObject(t_ObjId id);
private:
	hash_map<t_ObjId, Player*> m_Players;
	int32 m_PlayerNum;						//在线玩家数量
};

NS_WF_END

