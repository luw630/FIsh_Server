#pragma once
#include "WfConfig.h"
#include "WfCObjMgr.h"
#include "Object/Fish.h"
#include "FishUtility.h"

NS_WF_BEGIN

class Table;

//激光区域
struct t_LaserZone 
{
	uint8 m_seatId;	//位置id
	bool m_Active;	//是否活动
	int32 m_money;	//激光价钱
	Vector3 m_dir;
	Vector3 m_StartA;	
	Vector3 m_StartB;
	Vector3 m_EndA;
	Vector3 m_EndB;
	//是否在激光区域内
	bool isInzone(Fish* pFish)
	{
		if (!m_Active) return false;
		if (!pFish->isActive()) return false;

		bool inZone = false;
		if (PointInParallellines(m_StartA, m_StartB, m_dir, pFish->m_Transform.position))
		{
			inZone = true;
		}
		else  if(PointToSegDist(
			pFish->m_Transform.position.x, pFish->m_Transform.position.y,
			m_StartA.x, m_StartA.y,
			m_EndA.x, m_EndA.y) < pFish->t_radius){
			inZone = true;
		}
		else if (PointToSegDist(
			pFish->m_Transform.position.x, pFish->m_Transform.position.y,
			m_StartB.x, m_StartB.y,
			m_EndB.x, m_EndB.y) < pFish->t_radius) {
			inZone = true;
		}
		return inZone;
	}
};
class Screen
{
private:
	Table* m_pTable;
public:
	Screen(Table* pTable);
	~Screen();
public:
	void init();
	bool inScreen(Vector3& pos);
	void onFishLeave(Fish* pFish);
	void onFishEnter(Fish* pFish);
	void update(uint32 delta);
	int32 getFishNum();	
private:
	uint32 m_Time;
	t_LaserZone	m_LaserZone[MAX_SEAT_NUM];
	std::hash_map<t_ObjId, Fish*> m_ScreenFish;
};

NS_WF_END

