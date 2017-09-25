#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Fish.h"
NS_WF_BEGIN
class Table;
class FishMgr
{
public:
	Table* m_pTable;
public:
	FishMgr(Table* pTable);
	~FishMgr();
public:
	void init();
	//每帧更新
	void update(uint32 delta);	
	void addFish(Fish *pFish);
	Fish* getBigFish();
	Fish* getFish(t_ObjId id);
public:
	ObjList<Fish>	m_Fishs;	//活动的鱼
};

NS_WF_END

