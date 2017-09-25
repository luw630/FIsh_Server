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
	//检查范围
	bool CheckBoundary();
	void CheckNorMal();	
	//获取炮弹类型
	int32 getType() { return m_type; }
	//钻头炮进入鱼，当再次进入不同鱼时，则离开头条鱼
	bool enterFish(t_ObjId fishId); 
	//钻头炮离开鱼
	void outFish(t_ObjId fishId);

	t_ObjId getPlayerId() { return m_playerId; }
	t_ObjId getSeatId() { return m_SeatId; }
	virtual void onUnActive();
private:
	Table* m_pTable;
	t_MapData* m_mapData;
	int32		m_type;			//类型
	uint32		m_BulletProperty;//属性
	int32		m_SeatId;		//出生位置
	t_ObjId		m_playerId;		//玩家id
	uint8		m_ReboundCount;	//反弹次数
	Vector3		m_Dir;			//运行方向
	Vector3		m_prePos;		//前一帧位置
	float		m_Time;			//运行时间

	int32 m_gunPoints;	 //炮弹分值
	float t_radius;		 //半径
	t_ObjId m_hitFishId;


	Vector3	m_BombPath[5];	//炮弹运动轨迹点
};
NS_WF_END

