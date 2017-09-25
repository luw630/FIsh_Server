#include "Fish.h"
#include "FishResMgr.h"
#include "Table.h"

NS_WF_BEGIN
Fish::Fish()
{
	m_node = new ObjList<Fish>::Node(this);
}
Fish::~Fish()
{
	delete m_node;
}
bool Fish::init(Table* pTable, t_ObjId id, int32 fishType, PathData* pPath)
{
	t_fishData* res = FishResMgr::GetInstance().getFishData(fishType);
	if (!res)return false;
	m_bActive = true;
	t_radius = res->t_radius;
	t_maxFishRadio = res->t_maxFishRadio;
	t_minFishRadio = res->t_minFishRadio;

	m_KillSeatId = Invalid_ObjId;
	m_KillPlayerId = Invalid_ObjId;

	m_ObjectId = id;
	m_bActive = true;
	m_pTable = pTable;
	m_type = fishType;
	m_Speed = pPath->m_Len / res->t_Life;

	bool rst = false;
	//设置起始位置
	m_Transform.setPosition(pPath->m_NodeList[0]);

	//初始化path
	rst = m_PathCtrl.initByTime(this, pPath, res->t_Life);
	if (!rst) return false;
	m_bScreen = false;
	return true;
}

void Fish::dup(rpc::SC_FishNotifyFireFish* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_fishtype(m_type);
	data->set_m_pathtype(m_PathCtrl.m_path->m_type);
	data->set_m_pathid(m_PathCtrl.m_path->m_index);
}
void Fish::dup(rpc::SC_FishNotifyAddFish* data)
{
	data->set_m_objid(m_ObjectId);
	data->set_m_fishtype(m_type);
	data->set_m_pathtype(m_PathCtrl.m_path->m_type);
	data->set_m_pathid(m_PathCtrl.m_path->m_index);
	data->set_m_runper(m_PathCtrl.percentage);
}
bool Fish::update(uint32 delta)
{
	//路径更新
	m_PathCtrl.Update(delta/1000.0f);
	return m_bActive;
}
void Fish::setDeadByBomb(t_ObjId seatId, t_ObjId playerId)
{
	m_KillSeatId = seatId;
	m_KillPlayerId = playerId;
	setUnActive();
}
void Fish::onUnActive()
{
	//客户端自行处理
	//rpc::SC_FishNotifyRmvFish notify;
	//notify.set_m_objid(getId());
	//m_pTable->broadCastPacket(rpc::em_SC_FishNotifyRmvFish, &notify);

	m_pTable->m_Screen.onFishLeave(this);

	//炸弹蟹
	if (m_type == 122 && m_KillPlayerId != Invalid_ObjId){
		Bomb* pBomb = m_pTable->m_ObjectFactory.newBomb(m_KillSeatId,m_KillPlayerId,m_Transform.position);
		if (!pBomb) return;
		m_pTable->m_BombMgr.addObj(pBomb);
	}
}
void Fish::release()
{

}
NS_WF_END

