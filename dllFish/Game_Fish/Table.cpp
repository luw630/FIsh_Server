#include "Table.h"
#include "DispatcherTable.h"
#include "GameFish.h"
#include "WfRandom.h"

NS_WF_BEGIN


Table::Table() : m_Dispatcher(this), m_BulletMgr(this), m_BombMgr(this), m_FishMgr(this), m_FishLauncher(this), m_ObjectFactory(this), m_Screen(this)
{
	m_bActive = false;
}
Table::~Table()
{

}
bool Table::onInit()
{
	m_bActive = false;
	m_Dispatcher.Init();

	memcpy(&m_mapData, ((GameFish*)m_game)->getCfgGame()->m_pMapData, sizeof(t_MapData));

	//初始化椅子
	for (int i = 0; i < MAX_SEAT_NUM; i++){
		m_Seats[i].Init(i,this);
	}
	bool rst = false;
	m_FishLauncher.init();
	m_Screen.init();
	return true;
}
bool Table::isActive()
{
	return true;
}
void Table::onUpdate(uint32 delta)
{
	m_FishLauncher.update(delta);
	m_FishMgr.update(delta);
	m_BulletMgr.update(delta);
	m_BombMgr.update(delta);
	Collide(this);
	onLockBombUpdate(delta);
	m_Screen.update(delta);
}

void Table::onDispatcherMsg(abs* buffer)
{
	t_ObjId seatid = *(t_ObjId*)(buffer->getData());
	PacketHeader* packet = (PacketHeader*)(buffer->getData()+ sizeof(t_ObjId));
	auto handle = GetPacketHandler(packet->m_Command);
	if (handle){
		(m_Dispatcher.*handle)(seatid,packet,Stream((uint8*)(packet+1), packet->m_Length-sizeof(PacketHeader)));
	}else{
		log_dbg("table not find handle(%d)", packet->m_Command);
	}
}
void Table::broadCastPacket(PacketCommand cmd, ::google::protobuf::Message* packet)
{
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		Seat* pSeat = &m_Seats[i];
		Player* pPlayer = pSeat->getPlayer();
		if (pPlayer != nullptr)
		{
			sendPacket(pPlayer->getId(), cmd, packet);
		}
	}
}

void Table::Collide(Table* pTable)
{
	int32 point = 0;
	ObjList<Bullet>& bullets = pTable->m_BulletMgr.m_Bullets;
	ObjList<Fish>& fishs = pTable->m_FishMgr.m_Fishs;
	auto t = bullets.begin();
	auto end = bullets.end();
	while (t != end) {
		Bullet* bullet = t->m_pObj;
		if (bullet->isActive())
		{
			auto fishT = fishs.begin();
			auto fishEnd = fishs.end();
			while (fishT != fishEnd) {
				Fish* fish = fishT->m_pObj;
				if (fish->isActive())
				{
					float distance = PointToSegDist(
						fish->m_Transform.position.x, fish->m_Transform.position.y,
						bullet->m_prePos.x, bullet->m_prePos.y,
						bullet->m_Transform.position.x, bullet->m_Transform.position.y);


					if (distance < (bullet->t_radius + fish->t_radius)) 
					{
						bool bHit = false;
						if (bullet->getType() == rpc::BulletType_normal) {
							bullet->setUnActive();
							bHit = true;
						}
						else if (bullet->getType() == rpc::BulletType_drill) {
							if (bullet->enterFish(fish->getId()))
							{
								bHit = true;
							}
						}
						if (bHit) {
							fish->t_fishRadio = Random::RandomInt(fish->t_minFishRadio, fish->t_maxFishRadio);
							if (pTable->isFishDead(
								bullet->m_gunPoints,
								fish->t_maxFishRadio,
								fish->t_minFishRadio,
								point))
							{
								fish->m_KillSeatId = bullet->getSeatId();
								fish->m_KillPlayerId = bullet->getPlayerId();
								rpc::SC_FishNotifyHitFish notify;
								notify.set_m_bulletid(bullet->getId());
								notify.set_m_fishid(fish->getId());
								notify.set_m_seatid(bullet->getSeatId());
								notify.set_m_playerid(bullet->getPlayerId());
								notify.set_m_money(point);
								pTable->broadCastPacket(rpc::em_SC_FishNotifyHitFish, &notify);
								log_dbg("............hit fish(%d)", fish->getId());
								fish->setUnActive();
							}
						}
						break;
					}else {
						if (bullet->getType() == rpc::BulletType_drill) {
							bullet->outFish(fish->getId());
						}
					}
				}
				fishT = fishT->m_pNext;
			}
		}
		t = t->m_pNext;
	}
}
void Table::randomPos(Vector3& center, Vector3& pos1, Vector3& pos2, Vector3& pos3, Vector3& pos4)
{
	center.x = (m_mapData.t_RightBottom.x + m_mapData.t_LeftTop.x) / 2;
	center.y = (m_mapData.t_RightBottom.y + m_mapData.t_LeftTop.y) / 2;
	center.z = 0;

	pos1.x = m_mapData.t_LeftTop.x;
	pos1.y = Random::Randomfloat(m_mapData.t_RightBottom.y, m_mapData.t_LeftTop.y);
	pos1.z = 0;

	pos2.x = Random::Randomfloat(m_mapData.t_RightBottom.x, m_mapData.t_LeftTop.x);
	pos2.y = m_mapData.t_LeftTop.y;
	pos2.z = 0;

	pos3.x = m_mapData.t_RightBottom.x;
	pos3.y = Random::Randomfloat(m_mapData.t_RightBottom.y, m_mapData.t_LeftTop.y);
	pos3.z = 0;

	pos4.x = Random::Randomfloat(m_mapData.t_RightBottom.x, m_mapData.t_LeftTop.x);
	pos4.y = m_mapData.t_RightBottom.y;
	pos4.z = 0;
}

void Table::setData(uint32 totalGunPoints, uint32 totalFishPoints, int32 placeType, int32 radio, int32 step)
{
	((GameService*)m_GameService)->m_FishDead.initFishPoints(totalFishPoints, m_totalFishPoints);
	((GameService*)m_GameService)->m_FishDead.initGunPoints(totalGunPoints, m_totalGunPoints);
	t_placeType = placeType;
	t_radio = radio;
	t_step = step;
}
Seat* Table::getSeat(t_ObjId seatId)
{
	if (seatId >= MAX_SEAT_NUM) return nullptr;
	return &m_Seats[seatId];
}
void Table::rmvPlayer(Seat* pSeat)
{
	Assert(pSeat == getSeat(pSeat->getId()));
	pSeat->unbindPlayer();

	Player* pPlayer = pSeat->getPlayer();
	if (pPlayer)
	{
		for (int i = 0; i < MAX_SEAT_NUM; i++)
		{
			Seat* otherSeat = getSeat(i);
			Player* otherPlayer = otherSeat->getPlayer();
			if (otherPlayer != nullptr)
			{
				rpc::SC_FishNotifyRmvPlayer rsp;
				rsp.set_m_seatid(pSeat->getId());
				sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyRmvPlayer, &rsp);
			}
		}
	}
}
void Table::onAddPlayer(Player* pPlayer)
{
	Seat* seat = getSeat(pPlayer->getSeatId());

	//发送玩家自己的信息
	rpc::SC_FishNotifyAddPlayerMySelf playerMyself;
	seat->dup(&playerMyself);
	sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddPlayerMySelf, &playerMyself);

	//发送鱼
	{
		auto t = m_FishMgr.m_Fishs.begin();
		auto end = m_FishMgr.m_Fishs.end();
		while (t != end)
		{
			Fish* pObj = t->m_pObj;

			rpc::SC_FishNotifyAddFish rsp;
			pObj->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddFish, &rsp);
			t = t->m_pNext;
		}
	}
		
	//发送子弹
	{
		auto t = m_BulletMgr.m_Bullets.begin();
		auto end = m_BulletMgr.m_Bullets.end();
		while (t != end)
		{
			Bullet* pObj = t->m_pObj;

			rpc::SC_FishNotifyAddBullet rsp;
			pObj->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddBullet, &rsp);
			t = t->m_pNext;
		}
	}

	//发送其它玩家给自己
	for (int i = 0 ; i < MAX_SEAT_NUM ; i++)
	{
		Seat* otherSeat = getSeat(i);
		Player* otherPlayer = otherSeat->getPlayer();
		if (otherPlayer != nullptr && otherPlayer!= pPlayer)
		{
			rpc::SC_FishNotifyAddPlayer rsp;
			otherSeat->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddPlayer, &rsp);
		}
	}
	
	//发送自己给其他玩家
	rpc::SC_FishNotifyAddPlayer playerData;
	seat->dup(&playerData);
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		Seat* otherSeat = getSeat(i);
		Player* otherPlayer = otherSeat->getPlayer();
		if (otherPlayer != nullptr && otherPlayer != pPlayer)
		{
			sendPacket(otherPlayer->getId(), rpc::em_SC_FishNotifyAddPlayer, &playerData);
		}
	}

}
void Table::onPlayerReEnter(Player* pPlayer, Seat* pSeat)
{
	Assert(pSeat->getPlayer() == pPlayer);

	//发送玩家自己的信息
	rpc::SC_FishNotifyAddPlayerMySelf playerMyself;
	pSeat->dup(&playerMyself);
	sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddPlayerMySelf, &playerMyself);

	//发送鱼
	{
		auto t = m_FishMgr.m_Fishs.begin();
		auto end = m_FishMgr.m_Fishs.end();
		while (t != end)
		{
			Fish* pObj = t->m_pObj;

			rpc::SC_FishNotifyAddFish rsp;
			pObj->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddFish, &rsp);
			t = t->m_pNext;
		}
	}

	//发送子弹
	{
		auto t = m_BulletMgr.m_Bullets.begin();
		auto end = m_BulletMgr.m_Bullets.end();
		while (t != end)
		{
			Bullet* pObj = t->m_pObj;

			rpc::SC_FishNotifyAddBullet rsp;
			pObj->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddBullet, &rsp);
			t = t->m_pNext;
		}
	}

	//发送其它玩家给自己
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		Seat* otherSeat = getSeat(i);
		Player* otherPlayer = otherSeat->getPlayer();
		if (otherPlayer != nullptr && otherPlayer != pPlayer)
		{
			rpc::SC_FishNotifyAddPlayer rsp;
			otherSeat->dup(&rsp);
			sendPacket(pPlayer->getId(), rpc::em_SC_FishNotifyAddPlayer, &rsp);
		}
	}

}
void Table::dupPlayers(google::protobuf::RepeatedPtrField<rpc::t_FishPlayer>* datas)
{
	for (int i = 0; i < MAX_SEAT_NUM; i++) {
		Player* pPlayer = m_Seats[i].getPlayer();
		if (pPlayer != nullptr)
		{
			rpc::t_FishPlayer* data = datas->Add();
			m_Seats[i].dup(data);
		}
	}
}

void Table::onLockBombUpdate(uint32 delta)
{
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		Seat* pSeat = &m_Seats[i];
		Player* pPlayer = pSeat->getPlayer();
		if (pPlayer && pPlayer->getBombType() == rpc::BulletType_electromagnetism)
		{
			if (pPlayer->getLockFishid() == Invalid_ObjId) {
				Fish* pFish = m_FishMgr.getBigFish();
				if (pFish)
				{
					pPlayer->LockFish(this,pFish->getId());
				}
			}
		}
	}
}
void Table::onFishLeave(Fish* pFish)
{
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		Player* pPlayer = m_Seats[i].getPlayer();
		if (pPlayer && pPlayer->getBombType() == rpc::BulletType_electromagnetism)
		{
			if (pPlayer->getLockFishid() == pFish->getId())
			{
				pPlayer->LockFish(this,Invalid_ObjId);
			}
		}
	}
}
bool Table::isFishDead(
	int gunPoints,      // - 击中鱼炮弹分值
	int maxFishRadio,   // - 设定的鱼的最大倍率
	int minFishRadio,   // - 设定的鱼的最小倍率
	int &point			// - 击中得分
)
{
	FishDead& fishDead = ((GameService*)m_GameService)->m_FishDead;
	int t_fishRadio = Random::RandomInt(minFishRadio, maxFishRadio);
	uint32 pre = m_totalFishPoints;
	if (fishDead.isFishDead(
		//是否杀死鱼
		gunPoints,
		t_fishRadio,
		maxFishRadio,
		minFishRadio,
		m_totalGunPoints,
		m_totalFishPoints,
		t_placeType,
		t_radio,
		t_step)) 
	{
		point = m_totalFishPoints - pre;
		return true;
	}
	return false;
}
NS_WF_END

