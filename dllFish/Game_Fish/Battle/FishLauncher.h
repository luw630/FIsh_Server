/********************************************************************
created:	2012-08-20
author:		lilin
summary:	��ķ�����
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
	int32 m_id;		//��Ⱥid
	int32 m_fishId;	//���id
	int32 m_intervalTime;	//ÿ���������ʱ����
	int32 m_perLineFishNum;	//ÿ�����߲�����ĸ���
	int32 m_PathType;		//��������

	int32 m_produceNum;		//�Ѿ����������
	int32 m_produceTime;	//�������ʱ��
};

class FishLauncher;
//����ؿ�
class FishTableLevel
{
public:
	//�㷢����״̬
	enum emFishLauncherState
	{
		emLevelState_Run = 0,
		emLevelState_Stop = 1,
		emLevelState_Boss = 2,
		emLevelState_BossEnd = 3
	};

	uint32	m_CurLevel;					//��ǰ�ؿ�(1,2,3,4)
	emFishLauncherState  m_LevelState;	//�ؿ�״̬				
	uint32	m_LevelTime;				//ÿ������ʱ��
	FishLauncher* m_Launcher;			//��ķ�����
	const t_CfgFishBattle* m_CfgFishBattle;
										//��Ⱥ
	uint32	m_ProduceGroupTime;		//�ؿ�������Ⱥ��ʱ��
	uint32	m_ProduceGroupNum;			//�����������

										//boss����
	uint32 m_LevelBossNum;				//boss��������
	bool m_LevelBossDead;				//boss�Ƿ�����

	bool m_isEnd;						//�Ƿ����

	//��ʼ���ؿ�
	void init(FishLauncher* Launcher);
	//���ùؿ�
	void reset(uint32 level);
	//��ȡ��һ�ؿ�
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
	//��ʼ��
	bool init();
	void update(uint32 delta);
	void LaunchByGroup(uint32 delta);
	void LaunchByPower(uint32 delta);
	void LaunchByGroupNew(uint32 delta);
	//�����㣬
	//��������Ⱥ���Ƿ�ǿ�Ʒ���
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
	float	m_LaunchByPowerInterval_1;	//ÿ���Ӳ���һ���㣬��λ����

	float   m_LauncherTime;
	float	LauncherInterval;			//��λ����


	FishTableLevel m_FishLevel;		//����ؿ�

	bool m_LevelGroupEnd;				//��Ⱥ�Ƿ��������
	std::list<LauncherGroupEvent*> m_LauncherGroupEvent;
};
NS_WF_END

