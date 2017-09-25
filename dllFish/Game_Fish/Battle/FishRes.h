/********************************************************************
created:	2012-08-20
author:		lilin
summary:	鱼资源
*********************************************************************/
#pragma once

#include "WfConfig.h"
#include "D3DHeader.h"
#include "WfStringUtil.h"
#include <boost/lexical_cast.hpp>
#include "Packets/Packet_Fish.pb.h"

enum PathEventType
{
	EVENT_NONE = 0,
	EVENT_STAY,     //停留
	EVENT_LAUGH,    //嘲讽
	EVENT_ATTACK,   //受击
};

enum FishClipType
{
	CLIP_YOUYONG = 0,
	CLIP_SIWANG,
	CLIP_CHAOFENG,
	CLIP_GONGJI,
	CLIP_MAX
};


enum BulletProperty
{
	BulletProperty_rebound = 1,			//反弹	
	BulletProperty_penetration = 2,		//穿透性
	BulletProperty_endBomb = 4,			//结束爆炸
	BulletProperty_ele = 8,				//电磁
};

#define FISH_TYPE_MAX 256
#define FISH_GROUP_MAX 256
#define SEAT_NUM_MAX 8	//座子位置最大个数
//结构定义
struct Ray
{
	Vector3 Pos;
	Vector3 Dir;
};

struct GroupData
{
	struct GroupItem
	{
		GroupItem()
		{
			pFishPosList = NULL;
			FishNum = 0;
			FishIndex = -1;
		}

		USHORT   FishNum;		//鱼数量
		BYTE	 FishIndex;		//鱼的索引
		Vector3  *pFishPosList;	//鱼所在位置
	};
	GroupData()
	{
		pGroupDataArray = nullptr;
		//pPathList = nullptr;
	}

	//USHORT	PathCount;
	//USHORT *pPathList;

	USHORT	GroupDataCount;		//组的个数
	GroupItem *pGroupDataArray;
};
struct FlowGroupData
{
	int		GroupID;
	int		Probability;		//出现的概率0 - 1000, 越大出现机率越大
	int		MaxCount;			//最多出现次数,-1无限

};
struct FlowTimeData
{
	int      GroupID;
	float    StartTime;          //0 - 1
	float    EndTime;            //0 - 1
	int      Probability;        //出现的概率0 - 100, 100必出现
	int      MaxCount;           //最多出现次数
	float    LaunchInterval;     //发射间隔，毫秒
	bool     ClearAll;           //清空场景
};
struct FlowData
{
	FlowData()
	{
		pFlowGroupList = NULL;
		pFlowTimeList = NULL;
	}
	int		MaxTime;                //秒
	int		LoopCount;
	float	LauncherInterval;		//秒
	int		GroupCount;
	int		FlowTimeCount;
	FlowGroupData *pFlowGroupList;
	FlowTimeData *pFlowTimeList;

	int		GetMaxTimeSeconds()
	{
		return MaxTime;
	}
};

struct t_CfgFishBattle {
	uint32 t_id;			//
	uint32 t_screenFishNum;	//屏幕中鱼的最大个数
	uint32 t_generalLauncherTime;	//每关普通产生鱼时间(秒)
	uint32 t_changeBossTime;	//切换boss的停止产生鱼的时间(秒)
	uint32 t_bossLifeTime;		//boss出现的最长时间(秒)
	uint32 t_changeLevelTime;	//切换下关的时间(秒)
};

struct t_GroupData {
	uint32 m_id;		//鱼群id
	uint32 m_fishId;	//鱼的id
	uint32 m_intervalTime;	//每条鱼产生的时间间隔
	uint32 m_perLineFishNum;	//每条鱼线产生鱼的个数
	uint32 m_PathType;		//鱼线类型
};
class CRSpline
{
public:
	Vector3* pts;
	int ptsLen;
	CRSpline(Vector3* pts, int ptsLen) {
		this->pts = new Vector3[ptsLen];
		this->ptsLen = ptsLen;
		memcpy(this->pts, pts, sizeof(Vector3)*ptsLen);
	}
	~CRSpline() { delete[] pts; }
	Vector3 Interp(float t)
	{
		int numSections = ptsLen - 3;
		int currPt = min(FloorToInt(t * (float)numSections), numSections - 1);
		float u = t * (float)numSections - (float)currPt;
		Vector3 a = pts[currPt];
		Vector3 b = pts[currPt + 1];
		Vector3 c = pts[currPt + 2];
		Vector3 d = pts[currPt + 3];
		return .5f*((-a + 3.0f*b - 3.0f*c + d)*(u*u*u) + (2.0f*a - 5.0f*b + 4.0f*c - d)*(u*u) + (-a + c)*u + 2.0f*b);
	}
};

struct PathData
{
	PathData(uint32 type,uint32 index)
	{
		m_type = type;
		m_index = index;
		m_NodeNum = 0;
		m_NodeList = nullptr;
		path = nullptr;
		m_Len = 0.0f;
	}
	uint32 m_index;
	uint32 m_type;
	int32 m_NodeNum;		//节点个数
	Vector3* m_NodeList;	//节点
	CRSpline* path;
	float m_Len;			//曲线长度
};


struct DeadStayTime
{
	float TianZai_DouDong_Time;           //天灾抖动时间

	float TianZai_Stay_Time1;             //天灾等待陨石降落停留最短时间
	float TianZai_Stay_Time2;             //天灾等待陨石降落停留最长时间

	float TianZai_Dead_Time1;             //天灾死亡停留最短时间
	float TianZai_Dead_Time2;             //天灾死亡停留最长时间

	float Bullet_BingDong_Dead_Time1;     //冰冻炮击中死亡停留最短时间
	float Bullet_BingDong_Dead_Time2;     //冰冻炮击中死亡停留长短时间

	float BingDong_Dead_Time1;            //冰冻技能死亡最短时间
	float BingDong_Dead_Time2;            //冰冻技能死亡最长时间

	float ShanDian_Dead_Time1;            //闪电技能死亡最短时间
	float ShanDian_Dead_Time2;            //闪电技能死亡最长时间

	float LongJuanFeng_Dead_Time1;        //龙卷风技能死亡最短时间
	float LongJuanFeng_Dead_Time2;        //龙卷风技能死亡最长时间

	float JiGuang_Stay_Time;
	float JiGuang_Dead_Time1;             //激光大招死亡最短时间
	float JiGuang_Dead_Time2;             //激光大招死亡最长时间

	float ShandDian_Speed;
	float JiGuang_Speed;
	float JiGuang_BingDong_Speed;
	float TianZai_Speed;
	float BingDong_Speed;
};

struct t_fishData
{
	int32 t_id;
	int32 t_minFishRadio;	//最小倍率
	int32 t_maxFishRadio;	//最大倍率
	int32 t_Stypes;			//类型，0表示普通鱼，1表示特殊蟹，2表示BOSS鱼,3表示巨大化鱼，4表示旋风鱼，5表示闪电鱼，6特殊鱼
	int32 t_Rate;			//出鱼权重，0表示不适用规则
	int32 t_Scenes;			//鱼出现的场景，0霸王蟹，1暗夜炬兽，2深海半爪鱼，3史前巨鳄，4黄金城
	int32 t_FishPath;		//鱼会走的路径编号 0表示无此项，#表示前后范围区间
	float t_Life;			//秒，鱼出生后的存活时间，0表示无此项	
	float t_radius;
};
struct t_PathType
{
	int32 t_id;
	int32 t_num;
	vector<PathData*>	m_PathList;
};
struct t_bulletData {
	int32 t_id;
	float t_range;
	float t_width;
	float t_radius;
	float t_speed;
	int32 t_times;	//碰撞次数
};
struct t_MapData {
	int32 t_id;
	Vector3 t_LeftTop;		//桌子左上角坐标
	Vector3 t_RightBottom;	//座子右下角坐标
	int32 t_SeatNum;		//椅子个数
	Vector3 t_Seat[SEAT_NUM_MAX];
	float t_batteryLen;		//炮台长度
};

inline bool ParseVector3(const string& st, Vector3& value)
{
	string str = st;
	Wf::StringUtil::Replace(str, "{", "");
	Wf::StringUtil::Replace(str, "}", "");

	std::vector<std::string> tmp = Wf::StringUtil::Split(str, ",");
	if (tmp.size() != 3)
	{
		return false;
	}
	value.x = boost::lexical_cast<float>(tmp[0]);
	value.y = boost::lexical_cast<float>(tmp[1]);
	value.z = boost::lexical_cast<float>(tmp[2]);
	return true;
}