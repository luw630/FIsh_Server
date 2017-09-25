#pragma once

#include "GameObject.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
#include "D3DHeader.h"
#include "FishRes.h"
NS_WF_BEGIN


class Table;
class Bomb : public GameObject
{
	friend class Table;
public:
	ObjList<Bomb>::Node* m_node;
public:
	Bomb();
	~Bomb();
public:
	bool initBomb(Table* pTable, t_ObjId id, int32 seatId, t_ObjId playerId, const Vector3& birthpos);
	void release();
	void dup(rpc::SC_FishNotifyAddBomb* data);
	bool update(float delta);
	void onEventBomb(int32 index);
	t_ObjId getPlayerId() { return m_playerId; }
	t_ObjId getSeatId() { return m_SeatId; }
private:
	Table* m_pTable;
	t_MapData* m_mapData;		//������ͼ

	t_ObjId		m_SeatId;		//����λ��
	t_ObjId		m_playerId;		//�������

	Vector3		m_Dir;			//�˶�����
	Vector3		m_prePos;		//ǰһ֡λ��
	int32 m_gunPoints;	 //�ڵ���ֵ
	float t_radius;		 //�뾶

	Vector3	m_BombPath[6];	//�ڵ��˶��켣��
	uint8 m_nextIndex;
	float m_nextTime;		//������һ���ʱ��
	float m_runTime;
};
class Table;
class BombMgr
{
public:
	Table* m_pTable;
public:
	BombMgr(Table* pTable);
	~BombMgr();
public:
	void init();
	//ÿ֡����
	void update(uint32 delta);

	bool addObj(Bomb *pObj);
public:
	ObjList<Bomb> m_Bombs;
};

NS_WF_END

