#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
#include "PathController.h"
#include "GameObject.h"


NS_WF_BEGIN
class Table;
class Fish : public GameObject
{
	friend class Table;
public:
	ObjList<Fish>::Node* m_node;
private:
	Table* m_pTable;
	int32 m_type;			//类型
	PathController  m_PathCtrl;
	bool m_bScreen;
public:
	int32 t_fishRadio;					//鱼的倍率
	int32 t_maxFishRadio;				//设定的鱼的最大倍率
	int32 t_minFishRadio;				//设定的鱼的最小倍率
	float t_radius;						//半径
public:
	Fish();
	~Fish();

	t_ObjId m_KillSeatId;	//杀死鱼的seatid
	t_ObjId m_KillPlayerId; //杀死鱼的playerid
public:
	/*
	参数：
	t_ObjId id			//对象id
	int32 fishType		//鱼类型
	int32 pathIndx		//运动轨迹索引
	*/
	bool init(Table* pTable,t_ObjId id, int32 fishType, PathData* pPath);
	void dup(rpc::SC_FishNotifyFireFish* data);
	void dup(rpc::SC_FishNotifyAddFish* data);
	bool update(uint32 delta);
	void release();
	void setDeadByBomb(t_ObjId seatId, t_ObjId playerId);	//被炸弹砸死
	void setScreen(bool value) { m_bScreen = value; }
	bool isScreen() { return m_bScreen; }
	virtual void onUnActive();
};

NS_WF_END

