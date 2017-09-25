#include "FishLauncher.h"
#include "FishResMgr.h"
#include "WfRandom.h"
#include "Fish.h"
#include "Table.h"
#include "WfLog.h"
NS_WF_BEGIN
void FishTableLevel::init(FishLauncher* Launcher)
{
	m_Launcher = Launcher;
	m_CfgFishBattle = FishResMgr::GetInstance().getCfgFishBattle();
	m_isEnd = false;
	m_LevelTime = 0;
	m_CurLevel = 1;
	m_LevelState = emLevelState_Run;


	m_ProduceGroupNum = 0;
	m_ProduceGroupTime = Random::RandomInt(0, m_CfgFishBattle->t_generalLauncherTime * 1 / 3);
}
void FishTableLevel::reset(uint32 level)
{
	m_isEnd = false;
	m_LevelTime = 0;
	m_CurLevel = level;
	m_LevelState = emLevelState_Run;

	m_ProduceGroupNum = 0;
	m_ProduceGroupTime = Random::RandomInt(0, m_CfgFishBattle->t_generalLauncherTime * 1 / 3);
}
uint32 FishTableLevel::getNextLevel()
{
	uint32 level = m_CurLevel + 1;
	if (level > 4) level = 1;
	return level;
}
void FishTableLevel::update(uint32 delta)
{
	if (m_isEnd) return;
	if (m_Launcher->m_pTable->m_Screen.getFishNum() >= m_CfgFishBattle->t_screenFishNum) return;
	
	m_LevelTime += delta;
	switch (m_LevelState)
	{
	case emLevelState_Run: {
		if (m_LevelTime >= m_CfgFishBattle->t_generalLauncherTime && m_Launcher->_LaunchByGroupIsEnd())
		{
			m_LevelState = emLevelState_Stop;
			m_LevelTime = 0;
			m_Launcher->m_pTable->broadCastPacket(rpc::em_SC_FishAllFishLeave,nullptr);
		}
		else {
			//产生鱼群(鱼群最多产生次数)
#define FishGroupNum 4
			if (m_ProduceGroupNum < FishGroupNum && m_LevelTime >= m_ProduceGroupTime&& m_Launcher->_LaunchByGroupIsEnd())
			{
				t_GroupData* cfg = FishResMgr::GetInstance().randomGroupData();
				if (!cfg) return;
				LauncherGroupEvent* obj = new LauncherGroupEvent(cfg);
				m_Launcher->addGroup(obj);
				++m_ProduceGroupNum;
				uint32 endTime = m_CfgFishBattle->t_generalLauncherTime * 3 / 4;
				if (m_LevelTime < endTime){
					m_ProduceGroupTime = Random::RandomInt(m_LevelTime, endTime);
				}
			}
			//鱼群更新
			if (!m_Launcher->_LaunchByGroupIsEnd())
			{
				m_Launcher->_LaunchByGroup(delta);
			}
			else {
				//产生鱼
				m_Launcher->LaunchByPower(delta);
			}
		}
	}break;
	case emLevelState_Stop: {
		if (m_LevelTime >= m_CfgFishBattle->t_changeBossTime)
		{
			m_LevelState = emLevelState_Boss;
			m_LevelTime = 0;
			m_LevelBossDead = false;
		}
	}break;
	case emLevelState_Boss: {
		if (m_LevelBossDead || m_LevelTime >= m_CfgFishBattle->t_bossLifeTime)
		{
			m_LevelState = emLevelState_BossEnd;
			m_LevelTime = 0;
			m_Launcher->m_pTable->broadCastPacket(rpc::em_SC_FishAllFishLeave, nullptr);
		}
		else {
			m_Launcher->LaunchByPower(delta);
		}
	}break;
	case emLevelState_BossEnd: {
		if (m_LevelTime >= m_CfgFishBattle->t_changeLevelTime)
		{
			m_isEnd = true;
		}
	}break;
	default:
		break;
	}
}
bool FishTableLevel::isEnd()
{
	return m_isEnd;
}

FishLauncher::FishLauncher(Table* pTable)
{
	m_pTable = pTable;
	m_LauncherTime = 0;
	m_FishFlow = nullptr;
	m_bActive = false;
	LauncherInterval = 5000;

	m_LaunchByPowerTime_1 = 0;
	m_LaunchByPowerInterval_1 = 500;
}
bool FishLauncher::init()
{
	m_FishLevel.init(this);
	return true;
}
void FishLauncher::LaunchByGroup(uint32 delta)
{
	m_LauncherTime += delta;

	if (m_LauncherTime >= LauncherInterval)
	{
		LaunchFish(0, false);
		m_LauncherTime -= LauncherInterval;
	}
}
void FishLauncher::LaunchByPower(uint32 delta)
{
	m_LaunchByPowerTime_1 += delta;

	if (m_LaunchByPowerTime_1 >= m_LaunchByPowerInterval_1)
	{
		m_LaunchByPowerTime_1 -= m_LaunchByPowerInterval_1;

		t_fishData* pFishData = FishResMgr::GetInstance().randomFish();
		Assert(pFishData);
		PathData* path = nullptr;
		if (pFishData->t_FishPath < 0) {
			path = getPathByPathType(0);
			log_waring("Special path need to do");
		}
		else {
			path = getPathByPathType(pFishData->t_FishPath);
		}
		Assert(path);
		if (!path)return;

		Fish *pFish = m_pTable->m_ObjectFactory.newFish(pFishData->t_id, path);
		if (!pFish) {
			log_err("create fish failed");
			return;
		}
		m_pTable->m_FishMgr.addFish(pFish);
	}
}
void FishLauncher::LaunchByGroupNew(uint32 delta)
{
	
}
void FishLauncher::_LaunchByGroup(uint32 delta)
{
	std::list<LauncherGroupEvent*>::iterator iter = m_LauncherGroupEvent.begin();
	while (iter != m_LauncherGroupEvent.end())
	{
		LauncherGroupEvent* obj = *iter;
		if (obj->m_produceNum >= obj->m_perLineFishNum)
		{
			//删除对象
			iter = m_LauncherGroupEvent.erase(iter);
			delete obj;
		}
		else {
			obj->m_produceTime += delta;
			if (obj->m_produceTime >= obj->m_intervalTime) {
				obj->m_produceTime -= obj->m_intervalTime;
				obj->m_produceNum += 1;
				t_PathType* pathType = FishResMgr::GetInstance().getPathType(obj->m_PathType);
				if (pathType) {
					for (int i = 0; i < pathType->t_num; i++)
					{
						PathData* path = pathType->m_PathList[i];
						Fish *pFish = m_pTable->m_ObjectFactory.newFish(obj->m_fishId, path);
						if (!pFish) {
							log_err("create fish failed");
							return;
						}
						m_pTable->m_FishMgr.addFish(pFish);
					}
				}
				else {
					log_err("not find pathtype(%d)", obj->m_PathType);
					return;
				}
			}
			iter++;
		}
	}
	if (m_LauncherGroupEvent.empty())
	{
		m_LevelGroupEnd = true;
	}
}
bool  FishLauncher::_LaunchByGroupIsEnd()
{
	return m_LevelGroupEnd;
}
void FishLauncher::addGroup(LauncherGroupEvent* msg)
{
	m_LauncherGroupEvent.push_back(msg);
	m_LevelGroupEnd = false;
}
void FishLauncher::update(uint32 delta)
{
	//{
	//	if (m_pTable->getId() == 1)return;
	//	static uint32 tmp = 0;
	//	tmp += delta;
	//	if (tmp >= 10 * 1000)
	//	{
	//		tmp -= 10 * 1000;
	//		PathData* path = FishResMgr::GetInstance().getPathType(0)->m_PathList[0];
	//		Fish *pFish = m_pTable->m_ObjectFactory.newFish(1, path);
	//		m_pTable->m_FishMgr.addFish(pFish);
	//		log_dbg("create Fish(%d) pos(%f,%f)", pFish->getId(), pFish->m_Transform.position.x, pFish->m_Transform.position.y);
	//	}
	//	return;
	//}
	m_FishLevel.update(delta);
	if (m_FishLevel.isEnd())
	{
		uint32 level = m_FishLevel.getNextLevel();
		m_FishLevel.reset(level);
		rpc::SC_FishEnterNextLvl msg;
		msg.set_m_lvl(level);
		m_pTable->broadCastPacket(rpc::em_SC_FishEnterNextLvl, &msg);
	}
}
bool FishLauncher::LaunchFish(int32 fishGroup, bool bforceLaunch)
{
	GroupData *pGroup = FishResMgr::GetInstance().getGroupData(fishGroup);
	PathData* path = getGroupPath(pGroup);

	if (!path) {
		log_waring("not find path");
		return false;
	}
	bool bOK = true;
	for (int n = 0; n < pGroup->GroupDataCount; ++n)
	{
		GroupData::GroupItem &gd = pGroup->pGroupDataArray[n];
		for (int i = 0; i < gd.FishNum; ++i)
		{
			Fish *pFish = m_pTable->m_ObjectFactory.newFish(1, path);
			if (!pFish){
				log_err("create fish failed");
				continue;
			}
			m_pTable->m_FishMgr.addFish(pFish);
		}
	}
	return bOK;
}
int32 FishLauncher::GetPathIndex(GroupData *pGroup)
{
	//int32 value = Random::RandomInt(0, pGroup->PathCount-1);
	//return pGroup->pPathList[value];
	return -1;
}
PathData* FishLauncher::getGroupPath(GroupData *pGroup)
{
	t_PathType* pathType = FishResMgr::GetInstance().getPathType(1);
	if (pathType) {
		int32 value = Random::RandomInt(0, pathType->t_num - 1);
		return pathType->m_PathList[value];
	}
	return nullptr;
}
PathData* FishLauncher::getPathByPathType(int32 type)
{
	t_PathType* pathType = FishResMgr::GetInstance().getPathType(type);
	if (pathType) {
		int32 value = Random::RandomInt(0, pathType->t_num - 1);
		return pathType->m_PathList[value];
	}
	return nullptr;
}
NS_WF_END

