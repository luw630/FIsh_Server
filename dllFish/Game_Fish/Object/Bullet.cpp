#include "Bullet.h"
#include "Table.h"
#include "FishResMgr.h"
#include "GameService.h"
NS_WF_BEGIN
Bullet::Bullet()
{
	m_node = new ObjList<Bullet>::Node(this);
	m_BulletProperty = 0;
}
Bullet::~Bullet()
{
	WF_SAFE_DELETE(m_node);
}
bool Bullet::init(Table* pTable,t_ObjId id, int32 type, const Vector3& dir, int32 birthpos)
{
	if (birthpos >= MAX_SEAT_NUM || type<0|| type>= rpc::BulletType_max) return false;
	m_bActive = true;
	m_pTable = pTable;
	m_hitFishId = Invalid_ObjId;
	m_mapData = &(m_pTable->m_mapData);
		
	//switch (type)
	//{
	//case rpc::BulletType_bomb: {
	//	m_BulletProperty = m_BulletProperty&BulletProperty_rebound;
	//	m_BulletProperty = m_BulletProperty&BulletProperty_penetration;
	//	m_BulletProperty = m_BulletProperty&BulletProperty_endBomb;
	//}break;
	//case rpc::BulletType_drill: {
	//	m_BulletProperty = m_BulletProperty&BulletProperty_rebound;
	//	m_BulletProperty = m_BulletProperty&BulletProperty_penetration;
	//}break;
	//case rpc::BulletType_electromagnetism: {
	//	m_BulletProperty = m_BulletProperty&BulletProperty_ele;
	//}break;
	//default:
	//	break;
	//}
	m_ObjectId = id;
	m_type = type;
	m_SeatId = birthpos;
	m_Dir = dir;
	m_Dir.normalized();
	m_Transform.position = m_pTable->m_mapData.t_Seat[birthpos] + dir*m_pTable->m_mapData.t_batteryLen;
	m_prePos = m_Transform.position;

	t_bulletData* res = FishResMgr::GetInstance().getBulletData(type);
	if (!res)return false;
	t_radius = res->t_radius;
	m_Speed = res->t_speed;
	m_ReboundCount = res->t_times;

	//log_dbg("create Bullet(%d) pos(%f,%f)", getId(), m_Transform.position.x, m_Transform.position.y);
	return true;
}

bool Bullet::initBomb(Table* pTable, t_ObjId id, int32 seatId, t_ObjId playerId, const Vector3& birthpos)
{
	m_bActive = true;
	m_pTable = pTable;
	m_hitFishId = Invalid_ObjId;
	m_mapData = &(m_pTable->m_mapData);


	m_ObjectId = id;
	m_type = rpc::BulletType_bomb;
	m_SeatId = seatId;
	m_playerId = playerId;

	m_Transform.position = birthpos;
	m_prePos = m_Transform.position;

	m_pTable->randomPos(m_BombPath[0], m_BombPath[1], m_BombPath[2], m_BombPath[3], m_BombPath[4]);
	t_bulletData* res = FishResMgr::GetInstance().getBulletData(m_type);
	if (!res)return false;
	t_radius = res->t_radius;
	m_Speed = res->t_speed;

	return true;
}
void Bullet::release()
{

}

void Bullet::dup(rpc::SC_FishNotifyFireBullet* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_type(m_type);
	data->set_m_seatid(m_SeatId);
	rpc::t_Vector2* dir = data->mutable_m_dir();
	dir->set_x(m_Dir.x);
	dir->set_y(m_Dir.y);
}
void Bullet::dup(rpc::SC_FishNotifyAddBullet* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_type(m_type);
	data->set_m_seatid(m_SeatId);
	rpc::t_Vector2* dir = data->mutable_m_dir();
	dir->set_x(m_Dir.x);
	dir->set_y(m_Dir.y);
	rpc::t_Vector2* pos = data->mutable_m_pos();
	pos->set_x(m_Transform.position.x);
	pos->set_y(m_Transform.position.y);
}
void Bullet::update(float delta)
{
	m_Time += delta;
	m_prePos = m_Transform.position;
	m_Transform.position += m_Dir * delta * m_Speed;

	if (!CheckBoundary()) {
		setUnActive();
	}
	//if (haveProperty(BulletProperty_rebound)){
	//	
	//}

	//if (m_type == BULLET_TYPE_NORMAL){
	//	CheckNorMal();
	//}else if (m_type == BULLET_TYPE_REBOUND) {
	//	if (!CheckBoundary()) {
	//		setUnActive();
	//	}		
	//}	
}
bool Bullet::haveProperty(int32 value)
{
	if ((m_BulletProperty&value) > 0)
	{
		return true;
	}
	return false;
}
bool Bullet::CheckBoundary()
{
	if (m_Transform.position.x > m_mapData->t_RightBottom.x || m_Transform.position.x < m_mapData->t_LeftTop.x ||
		m_Transform.position.y > m_mapData->t_LeftTop.y || m_Transform.position.y < m_mapData->t_RightBottom.y)
	{
		if (m_ReboundCount > 0)
		{

			if (m_Transform.position.x > m_mapData->t_RightBottom.x)
			{
				m_Transform.position.x = m_mapData->t_RightBottom.x;
				m_Dir.x = -m_Dir.x;
			}
			else if (m_Transform.position.x < m_mapData->t_LeftTop.x)
			{
				m_Transform.position.x = m_mapData->t_LeftTop.x;
				m_Dir.x = -m_Dir.x;
			}
			if (m_Transform.position.y > m_mapData->t_LeftTop.y)
			{
				m_Transform.position.y = m_mapData->t_LeftTop.y;
				m_Dir.y = -m_Dir.y;

			}
			else if (m_Transform.position.y < m_mapData->t_RightBottom.y)
			{
				m_Transform.position.y = m_mapData->t_RightBottom.y;
				m_Dir.y = -m_Dir.y;
			}
			m_Time = 0;
			--m_ReboundCount;
			return true;
		}
		else
			return false;
	}
	return true;
}
void Bullet::CheckNorMal()
{
	if (m_Transform.position.x > m_mapData->t_RightBottom.x || m_Transform.position.x < m_mapData->t_LeftTop.x ||
		m_Transform.position.y > m_mapData->t_LeftTop.y || m_Transform.position.y < m_mapData->t_RightBottom.y)
	{
		setUnActive();
	}
}
bool Bullet::enterFish(t_ObjId fishId)
{
	if (m_hitFishId == fishId) {
		return false;
	}
	m_hitFishId = fishId;
	return true;
}

void Bullet::outFish(t_ObjId fishId)
{
	if (m_hitFishId == fishId){
		m_hitFishId = Invalid_ObjId;
	}
}
void Bullet::onUnActive()
{
	if (m_type == rpc::BulletType_drill)
	{
		rpc::SC_FishRmvDrillBomb msg;
		msg.set_m_bulletid(getId());
		msg.set_m_seatid(m_SeatId);
		msg.set_m_playerid(m_playerId);
		msg.set_m_money(0);
		m_pTable->broadCastPacket(rpc::em_SC_FishRmvDrillBomb, &msg);
	}
}
NS_WF_END

