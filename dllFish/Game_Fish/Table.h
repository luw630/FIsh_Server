#pragma once
#include "WfConfig.h"
#include "WfCObjMgr.h"
#include "Object/Seat.h"
#include "Object/Fish.h"
#include "Object/Bullet.h"
#include "Player.h"
#include "IGame.h"
#include "ITable.h"
#include "GamePacket.h"
#include "DispatcherTable.h"
#include "WfObjList.h"
#include "google/protobuf/message.h"

#include "FishUtility.h"
#include "FishLauncher.h"

#include "FishMgr.h"
#include "BulletMgr.h"
#include "ObjectFactory.h"
#include "Screen.h"

NS_WF_BEGIN


class Table : public ITable,public PacketHandlerMgr<HandleTable>
{
	friend class DispatcherTable;
public:
	Table();
	~Table();
public:
	virtual bool onInit();
	virtual bool isActive();
	//每帧更新
	virtual void onUpdate(uint32 delta);
	virtual void onDispatcherMsg(abs* buffer);
	virtual void broadCastPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
private:
	//碰撞检测
	static void Collide(Table* pTable);
public:
	void randomPos(Vector3& center, Vector3& pos1, Vector3& pos2, Vector3& pos3, Vector3& pos4);
	void setData(uint32 totalGunPoints, uint32 totalFishPoints, int32 placeType, int32 radio, int32 step);
	Seat* getSeat(t_ObjId seatId);

	void rmvPlayer(Seat* pSeat);
	void onAddPlayer(Player* pPlayer);		//seat处添加了玩家
	void onPlayerReEnter(Player* pPlayer, Seat* pSeat);	//玩家重新进入
	void dupPlayers(google::protobuf::RepeatedPtrField<rpc::t_FishPlayer>* datas);
	void onLockBombUpdate(uint32 delta);	//锁定炮更新
	void onFishLeave(Fish* pFish);			//鱼离开，（死亡或者离开屏幕区域）
	
	
	bool isFishDead(
		int gunPoints,      // - 击中鱼炮弹分值
		int maxFishRadio,   // - 设定的鱼的最大倍率
		int minFishRadio,   // - 设定的鱼的最小倍率
		int &point			// - 击中得分
	);
public:
	t_MapData m_mapData;
	Screen m_Screen;
	bool m_bActive;			//是否活着
	BulletMgr m_BulletMgr;
	BombMgr m_BombMgr;
	FishMgr m_FishMgr;
	Seat	m_Seats[MAX_SEAT_NUM];				//位置
	DispatcherTable	m_Dispatcher;			//分发消息	
	FishLauncher		m_FishLauncher;			//鱼的发射器
	ObjectFactory		m_ObjectFactory;
	uint32 m_totalGunPoints;			// - 总击中鱼炮弹分累计
	uint32 m_totalFishPoints;			// - 总击中鱼得分累计
	int32 t_placeType;					// - 场地类型
	int32 t_radio;						// - 抽水比例
	int32 t_step;						// - 难度等级)
};

NS_WF_END

