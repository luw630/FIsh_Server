#include "LaserMgr.h"
#include "Table.h"

NS_WF_BEGIN

Laser::Laser()
{
	m_node = new ObjList<Laser>::Node();
	m_pTable = nullptr;
	m_ObjectId = Invalid_ObjId;
	m_SeatId = Invalid_ObjId;
	m_playerId = Invalid_ObjId;
}
Laser::~Laser()
{
	WF_SAFE_DELETE(m_node);
}
bool Laser::init(Table* pTable, t_ObjId id, const Vector3& dir, int32 birthpos)
{
	m_pTable = pTable;
	m_ObjectId = id;
	m_SeatId = birthpos;
	m_playerId = m_pTable->getSeat(birthpos)->getPlayerId();
	m_Dir = dir;
	return true;
}
void Laser::update(float delta)
{

}
void Laser::dup(rpc::SC_FishNotifyFireBullet* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_type(rpc::BulletType_laser);
	data->set_m_seatid(m_SeatId);
	rpc::t_Vector2* dir = data->mutable_m_dir();
	dir->set_x(m_Dir.x);
	dir->set_y(m_Dir.y);
}
void Laser::dup(rpc::SC_FishNotifyAddBullet* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_type(rpc::BulletType_laser);
	data->set_m_seatid(m_SeatId);
	rpc::t_Vector2* dir = data->mutable_m_dir();
	dir->set_x(m_Dir.x);
	dir->set_y(m_Dir.y);
}
void Laser::release()
{

}
LaserMgr::LaserMgr(Table* pTable)
{
	m_pTable = pTable;
}
LaserMgr::~LaserMgr()
{

}
void LaserMgr::init()
{

}
void LaserMgr::update(uint32 delta)
{
	auto t = m_Laser.begin();
	auto end = m_Laser.end();
	while (t != end) {
		Laser* pObj = t->m_pObj;

		if (pObj->isActive())
		{
			pObj->update(delta/1000.0f);
			t = t->m_pNext;
		}
		else {
			t = m_Laser.erase(t);
			m_pTable->m_ObjectFactory.release(pObj);
		}
	}
}
bool  LaserMgr::addObj(Laser *pObj)
{
	if (!pObj) return false;
	m_Laser.addNode(pObj->m_node);

	rpc::SC_FishNotifyFireBullet rsp;
	pObj->dup(&rsp);
	m_pTable->broadCastPacket(rpc::em_SC_FishNotifyFireBullet, &rsp);
	return true;
}
NS_WF_END

