#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Bullet.h"
NS_WF_BEGIN
class Table;
class BulletMgr
{
public:
	Table* m_pTable;
public:
	BulletMgr(Table* pTable);
	~BulletMgr();
public:
	void init();
	//Ã¿Ö¡¸üÐÂ
	void update(uint32 delta);
	
	bool addObj(Bullet *pObj);
public:
	ObjList<Bullet> m_Bullets;
};

NS_WF_END

