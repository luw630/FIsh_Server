#include "Bomb.h"
#include "Table.h"
#include "FishResMgr.h"
#include "GameService.h"
#include "Packets/Packet_Fish.pb.h"
NS_WF_BEGIN
Bomb::Bomb()
{
	m_node = new ObjList<Bomb>::Node(this);
}
Bomb::~Bomb()
{
	WF_SAFE_DELETE(m_node);
}

bool Bomb::initBomb(Table* pTable, t_ObjId id, int32 seatId, t_ObjId playerId, const Vector3& birthpos)
{
	m_bActive = true;
	m_pTable = pTable;
	m_mapData = &(m_pTable->m_mapData);


	m_ObjectId = id;

	m_SeatId = seatId;
	m_playerId = playerId;

	m_Transform.position = birthpos;
	m_prePos = m_Transform.position;

	m_BombPath[0] = birthpos;
	m_pTable->randomPos(m_BombPath[1], m_BombPath[2], m_BombPath[3], m_BombPath[4], m_BombPath[5]);
	t_bulletData* res = FishResMgr::GetInstance().getBulletData(rpc::BulletType_bomb);
	m_Dir = m_BombPath[0] - birthpos;
	m_Dir.normalized();

	if (!res)return false;
	t_radius = res->t_radius;
	m_Speed = res->t_speed;

	m_nextIndex = 1;
	m_nextTime = Vector3::Distance(m_BombPath[m_nextIndex], m_BombPath[m_nextIndex - 1]) / m_Speed;
	m_runTime = 0;
	return true;
}
void Bomb::release()
{

}
void Bomb::dup(rpc::SC_FishNotifyAddBomb* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_playerid(m_playerId);
	data->set_m_seatid(m_SeatId);
	for (int i = 0; i < 6; i++) {
		Vector3& pos =	m_BombPath[1];
		rpc::t_Vector2* value = data->add_m_path();
		value->set_x(pos.x);
		value->set_y(pos.y);
	}
}
bool Bomb::update(float delta)
{
	if (m_nextIndex >= 6) return false;
	m_runTime += delta;

	while (m_runTime > 0)
	{
		if (m_runTime < m_nextTime)
		{
			m_runTime = 0;
			m_prePos = m_Transform.position;
			m_Transform.position += m_BombPath[m_nextIndex - 1] + m_Dir*m_runTime / m_nextTime;
			
		}else {
			//发送到达目的地改变方向
			m_runTime -= m_nextTime;
			m_Transform.position = m_BombPath[m_nextIndex];
			onEventBomb(m_nextIndex);
			++m_nextIndex;
			if (m_nextIndex >= 6) return false;
			m_nextTime = Vector3::Distance(m_BombPath[m_nextIndex], m_BombPath[m_nextIndex - 1]) /m_Speed;
		}
	}
	return true;
}
void Bomb::onEventBomb(int32 index)
{
	int32 point = 0;
	Vector3& pos = m_BombPath[index];
	ObjList<Fish>& fishs = m_pTable->m_FishMgr.m_Fishs;
	auto fishT = fishs.begin();
	auto fishEnd = fishs.end();
	while (fishT != fishEnd) {
		Fish* fish = fishT->m_pObj;
		if (fish->isActive())
		{
			float distance = Vector3::Distance(pos, fish->m_Transform.position);
			if (distance < (fish->t_radius + t_radius)) {
				if (m_pTable->isFishDead(
					m_gunPoints,
					fish->t_maxFishRadio,
					fish->t_minFishRadio,
					point))
				{
					fish->setDeadByBomb(m_SeatId, m_playerId);
				}
			}
		}
	}
}


BombMgr::BombMgr(Table* pTable)
{
	m_pTable = pTable;
}
BombMgr::~BombMgr()
{

}
void BombMgr::init()
{

}
void BombMgr::update(uint32 delta)
{
	auto t = m_Bombs.begin();
	auto end = m_Bombs.end();
	while (t != end) {
		Bomb* pObj = t->m_pObj;

		if (pObj->isActive())
		{
			if (pObj->update(delta / 1000.0f)) {
				t = t->m_pNext;
			}
			else {
				pObj->setUnActive();
				t = m_Bombs.erase(t);
				m_pTable->m_ObjectFactory.release(pObj);
			}
		}
		else {
			t = m_Bombs.erase(t);
			m_pTable->m_ObjectFactory.release(pObj);
		}
	}
}
bool  BombMgr::addObj(Bomb *pObj)
{
	if (!pObj) return false;
	m_Bombs.addNode(pObj->m_node);

	rpc::SC_FishNotifyAddBomb rsp;
	pObj->dup(&rsp);
	m_pTable->broadCastPacket(rpc::em_SC_FishNotifyAddBomb, &rsp);
	return true;
}
NS_WF_END

