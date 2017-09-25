#pragma once

#include "GameObject.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
#include "D3DHeader.h"
#include "FishRes.h"
NS_WF_BEGIN


class Table;
class Bullet : public GameObject
{
	friend class Table;
public:
	ObjList<Bullet>::Node* m_node;
public:
	Bullet();
	~Bullet();
public:
	bool init(Table* pTable,t_ObjId id, int32 type, const Vector3& dir, int32 birthpos);
	bool initBomb(Table* pTable, t_ObjId id, int32 seatId, t_ObjId playerId, const Vector3& birthpos);
	void release();	
	void dup(rpc::SC_FishNotifyFireBullet* data);
	void dup(rpc::SC_FishNotifyAddBullet* data);
	void update(float delta);
	bool haveProperty(int32 value);
	//��鷶Χ
	bool CheckBoundary();
	void CheckNorMal();	
	//��ȡ�ڵ�����
	int32 getType() { return m_type; }
	//��ͷ�ڽ����㣬���ٴν��벻ͬ��ʱ�����뿪ͷ����
	bool enterFish(t_ObjId fishId); 
	//��ͷ���뿪��
	void outFish(t_ObjId fishId);

	t_ObjId getPlayerId() { return m_playerId; }
	t_ObjId getSeatId() { return m_SeatId; }
	virtual void onUnActive();
private:
	Table* m_pTable;
	t_MapData* m_mapData;
	int32		m_type;			//����
	uint32		m_BulletProperty;//����
	int32		m_SeatId;		//����λ��
	t_ObjId		m_playerId;		//���id
	uint8		m_ReboundCount;	//��������
	Vector3		m_Dir;			//���з���
	Vector3		m_prePos;		//ǰһ֡λ��
	float		m_Time;			//����ʱ��

	int32 m_gunPoints;	 //�ڵ���ֵ
	float t_radius;		 //�뾶
	t_ObjId m_hitFishId;


	Vector3	m_BombPath[5];	//�ڵ��˶��켣��
};
NS_WF_END

