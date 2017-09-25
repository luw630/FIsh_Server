#pragma once

#include "GameObject.h"
#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
NS_WF_BEGIN
class Table;

class Laser : public GameObject
{
	friend class Table;
public:
	ObjList<Laser>::Node* m_node;
public:
	Laser();
	~Laser();
public:
	bool init(Table* pTable, t_ObjId id, const Vector3& dir, int32 birthpos);
	void update(float delta); 
	void dup(rpc::SC_FishNotifyFireBullet* data);
	void dup(rpc::SC_FishNotifyAddBullet* data);
	void release();
private:
	Table* m_pTable;
	t_ObjId		m_playerId;		//玩家id
	int32		m_SeatId;		//出生位置
	Vector3		m_Dir;			//运行方向
};

class LaserMgr
{
public:
	Table* m_pTable;
public:
	LaserMgr(Table* pTable);
	~LaserMgr();
public:
	void init();
	//每帧更新
	void update(uint32 delta);
	
	bool addObj(Laser *pObj);
public:
	ObjList<Laser> m_Laser;
};

NS_WF_END

