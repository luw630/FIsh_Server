/********************************************************************
created:	2012-08-20
author:		lilin
summary:	鱼资源管理
*********************************************************************/
#pragma once
#include "WfConfig.h"
#include "FishRes.h"


class FishResMgr
{
public:
	SINGLETON_MODE(FishResMgr);
public:
	FishResMgr();
	bool load();

	bool loadFishData(const string& pcDir);
	bool loadPath();
	bool loadBullet();
	bool loadMap();
	bool loadGroup();
	bool loadFishBattle();
	//导入鱼群
	//参数：资源所在目录
	bool loadGroup(const string& pcDir);

	GroupData* getGroupData(int32 index);
	PathData* getPathData(int32 index);
	t_MapData* getMapData(int32 mapId);
	t_fishData* getFishData(int32 index);
	t_bulletData* getBulletData(int32 index);
	t_PathType* getPathType(int32 id);
	t_fishData* randomFish();
	t_GroupData* getGroupDataNew(int32 index);
	t_GroupData* randomGroupData();
	const t_CfgFishBattle* getCfgFishBattle();
private:
	t_CfgFishBattle m_CfgFishBattle;
	vector<PathData*>	m_PathList;
	vector<GroupData*>	m_GroupList;
	map<int32, t_MapData*> m_MapDatas;
	map<int32, t_PathType*> m_PathType;
	t_fishData*	m_FishDataList[FISH_TYPE_MAX];
	t_GroupData* m_GroupListNew[FISH_GROUP_MAX];
	vector<int32> m_GroupListNewIndex;
	uint8*  m_fishPowerData;
	int32 m_fishPowerNum;
	t_bulletData*	m_BulletList[rpc::BulletType_max];
};


