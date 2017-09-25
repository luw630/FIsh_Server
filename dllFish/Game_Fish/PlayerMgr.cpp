#include "PlayerMgr.h"
#include "WfLog.h"
#include "GameFish.h"
NS_WF_BEGIN
PlayerMgr::PlayerMgr(GameFish* handle)
{
	m_gameHandle = handle;
	m_PlayerNum = 0;
}
PlayerMgr::~PlayerMgr()
{

}
Player* PlayerMgr::getObjectById(const t_ObjId& objectId)
{
	auto t = m_Players.find(objectId);
	if (t != m_Players.end())
	{
		return t->second;
	}
	return nullptr;
}
bool PlayerMgr::addObject(Player* obj)
{
	if (!obj) return false;
	if (obj->getId() == -1) return false;
	auto t = m_Players.find(obj->getId());
	if (t == m_Players.end())
	{
		m_Players.insert(make_pair(obj->getId(), obj));
		++m_PlayerNum;
	}
	return true;
}
Player* PlayerMgr::rmvObject(t_ObjId id)
{
	Player* pPlayer = nullptr;
	auto t = m_Players.find(id);
	if (t != m_Players.end())
	{
		pPlayer = t->second;
		m_Players.erase(t);
		--m_PlayerNum;
	}
	return pPlayer;
}
NS_WF_END

