#include "BulletMgr.h"
#include "Table.h"

NS_WF_BEGIN
BulletMgr::BulletMgr(Table* pTable)
{
	m_pTable = pTable;
}
BulletMgr::~BulletMgr()
{

}
void BulletMgr::init()
{

}
void BulletMgr::update(uint32 delta)
{
	auto t = m_Bullets.begin();
	auto end = m_Bullets.end();
	while (t != end) {
		Bullet* pObj = t->m_pObj;

		if (pObj->isActive())
		{
			pObj->update(delta/1000.0f);
			t = t->m_pNext;
		}
		else {
			t = m_Bullets.erase(t);
			m_pTable->m_ObjectFactory.release(pObj);
		}
	}
}
bool  BulletMgr::addObj(Bullet *pObj)
{
	if (!pObj) return false;
	m_Bullets.addNode(pObj->m_node);

	rpc::SC_FishNotifyFireBullet rsp;
	pObj->dup(&rsp);
	m_pTable->broadCastPacket(rpc::em_SC_FishNotifyFireBullet, &rsp);
	return true;
}
NS_WF_END

