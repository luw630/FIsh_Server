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
	t_ObjId		m_playerId;		//���id
	int32		m_SeatId;		//����λ��
	Vector3		m_Dir;			//���з���
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
	//ÿ֡����
	void update(uint32 delta);
	
	bool addObj(Laser *pObj);
public:
	ObjList<Laser> m_Laser;
};

NS_WF_END

