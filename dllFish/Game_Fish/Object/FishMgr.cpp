#include "FishMgr.h"
#include "Table.h"
#include "Packets/Packet_Fish.pb.h"

NS_WF_BEGIN
FishMgr::FishMgr(Table* pTable)
{
	m_pTable = pTable;
}
FishMgr::~FishMgr()
{

}
void FishMgr::init()
{

}
void FishMgr::update(uint32 delta)
{
	{
		auto t = m_Fishs.begin();
		auto end = m_Fishs.end();
		while (t != end) {
			Fish* pObj = t->m_pObj;

			if (pObj->isActive())
			{
				pObj->update(delta);
				t = t->m_pNext;
			}
			else {
				t = m_Fishs.erase(t);
				m_pTable->m_ObjectFactory.release(pObj);
			}
		}
	}
	{
		auto t = m_Fishs.begin();
		auto end = m_Fishs.end();
		while (t != end) {
			Fish* pObj = t->m_pObj;

			if (pObj->isActive()) {
				bool value = m_pTable->m_Screen.inScreen(pObj->m_Transform.position);
				if (pObj->isScreen() && value == false)
				{
					m_pTable->m_Screen.onFishLeave(pObj);
				}
				else if (!pObj->isScreen() && value == true)
				{
					m_pTable->m_Screen.onFishEnter(pObj);
				}
			}
			t = t->m_pNext;
		}
	}	
}
void FishMgr::addFish(Fish *pFish)
{
	Assert(pFish);
	m_Fishs.addNode(pFish->m_node);

	rpc::SC_FishNotifyFireFish rsp;
	pFish->dup(&rsp);
	m_pTable->broadCastPacket(rpc::em_SC_FishNotifyFireFish, &rsp);
}
Fish* FishMgr::getBigFish()
{
	Fish *pFish = nullptr;
	int32 num = 0;
	auto t = m_Fishs.begin();
	auto end = m_Fishs.end();
	while (t != end) {
		Fish* pObj = t->m_pObj;

		if (pObj->isActive())
		{
			if (pObj->t_maxFishRadio > num)
			{
				num = pObj->t_maxFishRadio;
				pFish = pObj;
			}
		}
		t = t->m_pNext;
	}
	return pFish;
}
Fish* FishMgr::getFish(t_ObjId id)
{
	Fish *pFish = nullptr;
	auto t = m_Fishs.begin();
	auto end = m_Fishs.end();
	while (t != end) {
		Fish* pObj = t->m_pObj;

		if (pObj->getId() == id)
		{
			if (pObj->isActive())
			{
				pFish = pObj;
				break;
			}
			else {
				break;
			}
		}
		t = t->m_pNext;
	}
	return pFish;
}
NS_WF_END

