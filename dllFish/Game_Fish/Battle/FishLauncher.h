/********************************************************************
created:	2012-08-20
author:		lilin
summary:	鱼的发射器
*********************************************************************/
#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
#include "FishRes.h"

NS_WF_BEGIN
struct LauncherGroupEvent{
	LauncherGroupEvent(t_GroupData* cfg)
	{
		m_id = cfg->m_id;
		m_fishId = cfg->m_fishId;
		m_intervalTime = cfg->m_intervalTime;
		m_perLineFishNum = cfg->m_perLineFishNum;
		m_PathType = cfg->m_PathType;

		m_produceNum = 0;
		m_produceTime = 0;
	}
	int32 m_id;		//鱼群id
	int32 m_fishId;	//鱼的id
	int32 m_intervalTime;	//每条鱼产生的时间间隔
	int32 m_perLineFishNum;	//每条鱼线产生鱼的个数
	int32 m_PathType;		//鱼线类型

	int32 m_produceNum;		//已经产生鱼个数
	int32 m_produceTime;	//产生鱼的时间
};

class FishLauncher;
//捕鱼关卡
class FishTableLevel
{
public:
	//鱼发射器状态
	enum emFishLauncherState
	{
		emLevelState_Run = 0,
		emLevelState_Stop = 1,
		emLevelState_Boss = 2,
		emLevelState_BossEnd = 3
	};

	uint32	m_CurLevel;					//当前关卡(1,2,3,4)
	emFishLauncherState  m_LevelState;	//关卡状态				
	uint32	m_LevelTime;				//每关运行时间
	FishLauncher* m_Launcher;			//鱼的发射器
	const t_CfgFishBattle* m_CfgFishBattle;
										//鱼群
	uint32	m_ProduceGroupTime;		//关卡产生鱼群的时间
	uint32	m_ProduceGroupNum;			//产生鱼阵次数

										//boss产生
	uint32 m_LevelBossNum;				//boss产生次数
	bool m_LevelBossDead;				//boss是否死亡

	bool m_isEnd;						//是否结束

	//初始化关卡
	void init(FishLauncher* Launcher);
	//重置关卡
	void reset(uint32 level);
	//获取下一关卡
	uint32 getNextLevel();
	void update(uint32 delta);
	bool isEnd();
};
class Table;
class FishLauncher
{
	friend class FishTableLevel;
public:
	Table* m_pTable;
public:
	FishLauncher(Table* pTable);
	//初始化
	bool init();
	void update(uint32 delta);
	void LaunchByGroup(uint32 delta);
	void LaunchByPower(uint32 delta);
	void LaunchByGroupNew(uint32 delta);
	//发射鱼，
	//参数：鱼群，是否强制发射
	bool LaunchFish(int32 fishGroup, bool bforceLaunch);
private:
	void _LaunchByGroup(uint32 delta);
	bool  _LaunchByGroupIsEnd();
	void addGroup(LauncherGroupEvent* msg);

	int32 GetPathIndex(GroupData *pGroup);
	PathData* getGroupPath(GroupData *pGroup);
	PathData* getPathByPathType(int32 type);
	
	FlowData* m_FishFlow;
	bool    m_bActive;

	float	m_LaunchByPowerTime_1;
	float	m_LaunchByPowerInterval_1;	//每秒钟产生一条鱼，单位毫秒

	float   m_LauncherTime;
	float	LauncherInterval;			//单位毫秒


	FishTableLevel m_FishLevel;		//捕鱼关卡

	bool m_LevelGroupEnd;				//鱼群是否产生结束
	std::list<LauncherGroupEvent*> m_LauncherGroupEvent;
};
NS_WF_END

