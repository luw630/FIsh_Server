#pragma once

#include "WfDataType.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "SessionGateWay/ListenerGateWay.h"
#include "SeesionDb/SeesionDb.h"
#include "WfConnectorMgr.h"
#include "Player/PlayerMgr.h"
#include "Dispatcher/Dispatcher_Packets.h"
#include "SessionProxy/SessionProxy.h"
#include "Logic/Rank.h"
#include "Logic/Announcement.h"
#include "Logic/FriendSystem.h"
#include "Logic/DbLog.h"

NS_WF_BEGIN
class App : public BaseApp ,public IService
{
public:
	App();
	~App();
	SINGLETON_MODE(App);
	// 初始化
	virtual bool OnInit(int argc, char **argv);
	virtual bool OnIniting();

	// 运行
	virtual bool OnRunning(uint delta);

	// 关闭
	virtual bool OnShutdown();
	virtual bool OnShutdowning();

	// 父类继承该命令接口
	virtual void OnCommand(const std::string& command);

	SeesionDb* getSeesionDb() {return m_SeesionDb;}
	NetMgr* getNetMgr() { return m_NetMgr; }
	ListenerGateWay* getListenerGateWay() { return m_SessionClientMgr; }
	PlayerMgr* getPlayerMgr() { return m_PlayerMgr; }
	FriendSystem* getFriendSystem() { return m_FriendSystem; }
	Dispatcher_Packets* getDispatcherPackets() { return &m_Dispatcher; }
	Rank* m_Rank;
	DbLog* m_DbLog;
	Announcement* m_Announcement;
	SessionProxyMgr* m_SessionProxyMgr;
	void SendPacketToGameSvr(PacketCommand cmd,	::google::protobuf::Message* packet, uint32 gameId);
private:
	SeesionDb* m_SeesionDb;
	NetMgr* m_NetMgr;	
	ListenerGateWay* m_SessionClientMgr;
	Dispatcher_Packets m_Dispatcher;

	PlayerMgr* m_PlayerMgr;
	FriendSystem* m_FriendSystem;	//好友系统
};

NS_WF_END


