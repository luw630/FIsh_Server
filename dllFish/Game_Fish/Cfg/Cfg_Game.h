#pragma once

#include "WfConfig.h"
#include <json/json.h>
#include "WfSysUtil.h"
#include "FishRes.h"

NS_WF_BEGIN

class Cfg_Game
{
public:
	Cfg_Game();
	bool load();
	int32 getTableNum() { return m_TableNum; }
private:
	bool loadScoreCfg(int32 index);
public:
	int32 m_TableNum;		//桌子个数
	int32 m_MaxPlayer;		//最大玩家数量
	int32 m_MapId;			//地图id

	int32 t_step;		//难度等级（最容易，容易，难，最难，死难）
	int32 t_placeType;	//场地类型（小,中,大）
	int32 t_radio;		//抽水比例(正数为抽水比例,负数为防水比例)
	int32 t_minScore;	//最小压分
	int32 t_maxScore;	//最大压分
	int32 t_coinPoint;	//投币比例
	uint32 t_totalGunPoints;	//总发炮分数初值
	uint32 t_totalFishPoints;	//总击中鱼得分初值

	t_MapData* m_pMapData;
};

NS_WF_END

