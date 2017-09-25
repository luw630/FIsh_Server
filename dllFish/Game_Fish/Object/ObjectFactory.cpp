#include "Table.h"
#include "ObjectFactory.h"

NS_WF_BEGIN
ObjectFactory::ObjectFactory(Table* pTable)
{
	m_pTable = pTable;
	m_ObjectId = START_OBJECT_ID;
}

ObjectFactory::~ObjectFactory()
{
	m_pTable = nullptr;
}
Fish* ObjectFactory::newFish(int32 fishType, PathData* pPath)
{
	if (++m_ObjectId == MAX_OBJECT_NUM)
		m_ObjectId = START_OBJECT_ID;

	Fish* pObj = nullptr;
	auto t = m_Fishs.popHead();
	if (t != nullptr)
	{
		pObj = t->m_pObj;
	}
	else {
		pObj = new Fish();
	}
	if (!pObj->init(m_pTable, m_ObjectId, fishType, pPath)) {
		pObj->release();
		m_Fishs.addNode(pObj->m_node);
		return nullptr;
	}
	return pObj;
}
Bullet* ObjectFactory::newBullet(int32 type, const Vector3& dir, int32 birthpos)
{
	if (++m_ObjectId == MAX_OBJECT_NUM)
		m_ObjectId = START_OBJECT_ID;

	Bullet* pObj = nullptr;
	auto t = m_Bullets.popHead();
	if (t != nullptr)
	{
		pObj = t->m_pObj;
	}
	else {
		pObj = new Bullet();
	}
	if (!pObj->init(m_pTable, m_ObjectId, type, dir, birthpos)) {
		pObj->release();
		m_Bullets.addNode(pObj->m_node);
		return nullptr;
	}
	return pObj;
}
Laser* ObjectFactory::newLaser(const Vector3& dir, int32 birthpos)
{
	if (++m_ObjectId == MAX_OBJECT_NUM)
		m_ObjectId = START_OBJECT_ID;

	Laser* pObj = nullptr;
	auto t = m_Lasers.popHead();
	if (t != nullptr)
	{
		pObj = t->m_pObj;
	}
	else {
		pObj = new Laser();
	}
	if (!pObj->init(m_pTable, m_ObjectId, dir, birthpos)) {
		pObj->release();
		m_Lasers.addNode(pObj->m_node);
		return nullptr;
	}
	return pObj;
}
Bomb* ObjectFactory::newBomb(t_ObjId seatId, t_ObjId playerId, const Vector3& birthpos)
{
	if (++m_ObjectId == MAX_OBJECT_NUM)
		m_ObjectId = START_OBJECT_ID;

	Bomb* pObj = nullptr;
	auto t = m_Bombs.popHead();
	if (t != nullptr)
	{
		pObj = t->m_pObj;
	}
	else {
		pObj = new Bomb();
	}
	if (!pObj->initBomb(m_pTable, m_ObjectId, seatId, playerId, birthpos)) {
		pObj->release();
		m_Bombs.addNode(pObj->m_node);
		return nullptr;
	}
	return pObj;
}

void ObjectFactory::release(Fish* obj)
{
	obj->release();
	m_Fishs.addNode(obj->m_node);
}
void ObjectFactory::release(Bullet* obj)
{
	obj->release();
	m_Bullets.addNode(obj->m_node);
}
void ObjectFactory::release(Bomb* obj)
{
	obj->release();
	m_Bombs.addNode(obj->m_node);
}
void ObjectFactory::release(Laser* obj)
{
	obj->release();
	m_Lasers.addNode(obj->m_node);
}
NS_WF_END

