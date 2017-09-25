#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Bullet.h"
#include "Fish.h"
#include "Bomb.h"
#include "LaserMgr.h"
#define START_OBJECT_ID 1
#define MAX_OBJECT_NUM 0xffff

NS_WF_BEGIN
class Table;
class ObjectFactory
{
public:
	ObjectFactory(Table* pTable);
	~ObjectFactory();
public:
	Fish* newFish(int32 fishType, PathData* pPath);
	Bullet* newBullet(int32 type, const Vector3& dir, int32 birthpos);
	Laser* newLaser(const Vector3& dir, int32 birthpos);
	Bomb* newBomb(t_ObjId seatId, t_ObjId playerId, const Vector3& birthpos);
	void release(Fish* obj);
	void release(Bullet* obj);
	void release(Bomb* obj);
	void release(Laser* obj);
private:
	ObjList<Fish> m_Fishs;
	ObjList<Bullet> m_Bullets;
	ObjList<Laser> m_Lasers;
	ObjList<Bomb> m_Bombs;
	int32 m_ObjectId;	//鱼的id,自动增加
	Table* m_pTable;
};

NS_WF_END

