/********************************************************************
created:	2012-08-20
author:		lilin
summary:	����Դ
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
	EVENT_STAY,     //ͣ��
	EVENT_LAUGH,    //����
	EVENT_ATTACK,   //�ܻ�
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
	BulletProperty_rebound = 1,			//����	
	BulletProperty_penetration = 2,		//��͸��
	BulletProperty_endBomb = 4,			//������ը
	BulletProperty_ele = 8,				//���
};

#define FISH_TYPE_MAX 256
#define FISH_GROUP_MAX 256
#define SEAT_NUM_MAX 8	//����λ��������
//�ṹ����
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

		USHORT   FishNum;		//������
		BYTE	 FishIndex;		//�������
		Vector3  *pFishPosList;	//������λ��
	};
	GroupData()
	{
		pGroupDataArray = nullptr;
		//pPathList = nullptr;
	}

	//USHORT	PathCount;
	//USHORT *pPathList;

	USHORT	GroupDataCount;		//��ĸ���
	GroupItem *pGroupDataArray;
};
struct FlowGroupData
{
	int		GroupID;
	int		Probability;		//���ֵĸ���0 - 1000, Խ����ֻ���Խ��
	int		MaxCount;			//�����ִ���,-1����

};
struct FlowTimeData
{
	int      GroupID;
	float    StartTime;          //0 - 1
	float    EndTime;            //0 - 1
	int      Probability;        //���ֵĸ���0 - 100, 100�س���
	int      MaxCount;           //�����ִ���
	float    LaunchInterval;     //������������
	bool     ClearAll;           //��ճ���
};
struct FlowData
{
	FlowData()
	{
		pFlowGroupList = NULL;
		pFlowTimeList = NULL;
	}
	int		MaxTime;                //��
	int		LoopCount;
	float	LauncherInterval;		//��
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
	uint32 t_screenFishNum;	//��Ļ�����������
	uint32 t_generalLauncherTime;	//ÿ����ͨ������ʱ��(��)
	uint32 t_changeBossTime;	//�л�boss��ֹͣ�������ʱ��(��)
	uint32 t_bossLifeTime;		//boss���ֵ��ʱ��(��)
	uint32 t_changeLevelTime;	//�л��¹ص�ʱ��(��)
};

struct t_GroupData {
	uint32 m_id;		//��Ⱥid
	uint32 m_fishId;	//���id
	uint32 m_intervalTime;	//ÿ���������ʱ����
	uint32 m_perLineFishNum;	//ÿ�����߲�����ĸ���
	uint32 m_PathType;		//��������
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
	int32 m_NodeNum;		//�ڵ����
	Vector3* m_NodeList;	//�ڵ�
	CRSpline* path;
	float m_Len;			//���߳���
};


struct DeadStayTime
{
	float TianZai_DouDong_Time;           //���ֶ���ʱ��

	float TianZai_Stay_Time1;             //���ֵȴ���ʯ����ͣ�����ʱ��
	float TianZai_Stay_Time2;             //���ֵȴ���ʯ����ͣ���ʱ��

	float TianZai_Dead_Time1;             //��������ͣ�����ʱ��
	float TianZai_Dead_Time2;             //��������ͣ���ʱ��

	float Bullet_BingDong_Dead_Time1;     //�����ڻ�������ͣ�����ʱ��
	float Bullet_BingDong_Dead_Time2;     //�����ڻ�������ͣ������ʱ��

	float BingDong_Dead_Time1;            //���������������ʱ��
	float BingDong_Dead_Time2;            //�������������ʱ��

	float ShanDian_Dead_Time1;            //���缼���������ʱ��
	float ShanDian_Dead_Time2;            //���缼�������ʱ��

	float LongJuanFeng_Dead_Time1;        //����缼���������ʱ��
	float LongJuanFeng_Dead_Time2;        //����缼�������ʱ��

	float JiGuang_Stay_Time;
	float JiGuang_Dead_Time1;             //��������������ʱ��
	float JiGuang_Dead_Time2;             //������������ʱ��

	float ShandDian_Speed;
	float JiGuang_Speed;
	float JiGuang_BingDong_Speed;
	float TianZai_Speed;
	float BingDong_Speed;
};

struct t_fishData
{
	int32 t_id;
	int32 t_minFishRadio;	//��С����
	int32 t_maxFishRadio;	//�����
	int32 t_Stypes;			//���ͣ�0��ʾ��ͨ�㣬1��ʾ����з��2��ʾBOSS��,3��ʾ�޴��㣬4��ʾ�����㣬5��ʾ�����㣬6������
	int32 t_Rate;			//����Ȩ�أ�0��ʾ�����ù���
	int32 t_Scenes;			//����ֵĳ�����0����з��1��ҹ���ޣ�2���צ�㣬3ʷǰ������4�ƽ��
	int32 t_FishPath;		//����ߵ�·����� 0��ʾ�޴��#��ʾǰ��Χ����
	float t_Life;			//�룬�������Ĵ��ʱ�䣬0��ʾ�޴���	
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
	int32 t_times;	//��ײ����
};
struct t_MapData {
	int32 t_id;
	Vector3 t_LeftTop;		//�������Ͻ�����
	Vector3 t_RightBottom;	//�������½�����
	int32 t_SeatNum;		//���Ӹ���
	Vector3 t_Seat[SEAT_NUM_MAX];
	float t_batteryLen;		//��̨����
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