#pragma once
#include "IGame.h"
#include "TableMgr.h"
#include "TableViewMgr.h"
#include "PlayerMgr.h"
#include "Cfg/Cfg_Game.h"
#include "DispatcherRoom.h"
#include "GameService.h"

NS_WF_BEGIN

class GameFish : public IGame
{
protected:
	GameService* m_pGameService;			//工作线程
	int32 m_GameServiceNum;					//工作线程个数
public:
	GameFish(IAppProxy* AppProxy);
	~GameFish();
public:
	//初始化资源
	virtual bool Init(int32 workThreadNum);
	//开始服务器
	virtual void Start();
	//关闭服务
	virtual void Stop();

	virtual void OnUpdate(uint delta);
public:
	virtual void OnWorkThreadPacket(PacketHeader* packet);
	virtual void OnAccountPacket(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
	virtual void OnGateWayPacket(ISessionGateWay* pSession, PacketHeader* packet, Stream& stream);
	virtual void OnPlayerOffLine(uint32 playerId, int32 gatewayId, uint64 sessionId);
private:
	bool OnInit();
public:
	TableMgr* getTableMgr() { return m_pTableMgr; }
	Cfg_Game* getCfgGame() { return m_CfgGame; }
	PlayerMgr* getPlayerMgr() { return m_PlayerMgr; }
	TableViewMgr* getTableViewMgr() { return m_pTableViewMgr; }
private:
	TableViewMgr* m_pTableViewMgr;	//房间中桌子的显示数据
	TableMgr* m_pTableMgr;				
	PlayerMgr* m_PlayerMgr;					//房间中的Player管理
	Cfg_Game* m_CfgGame;
	DispatcherRoom* m_DispatcherFish;		//房间分发消息
private:
	//scene通知房间 玩家进入游戏成功
	void OnHandlePlayerEnterTable(PacketHeader* packet);
	//scene通知房间 玩家离开游戏成功
	void OnHandlePlayerLeaveTable(PacketHeader* packet);
};

NS_WF_END

