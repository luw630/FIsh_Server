#pragma once

#include "WfConfig.h"
#include "IGameService.h"
#include "ISessionGateWay.h"
#include "ISessionAccount.h"
#include "Packets/Packet_Base.pb.h"
#include "IAppProxy.h"

NS_WF_BEGIN

class IGame
{
public:
	IAppProxy* m_AppProxy;
public:
	IGame(IAppProxy* AppProxy) :m_AppProxy(AppProxy) {}
	~IGame() { m_AppProxy = nullptr; }
public:
	//初始化资源
	virtual bool Init(int32 workThreadNum) = NULL;
	//开始服务器
	virtual void Start() = NULL;
	//关闭服务
	virtual void Stop() = NULL;
	//主线程更新
	virtual void OnUpdate(uint delta) = NULL;
public:
	//工作线程的包
	virtual void OnWorkThreadPacket(PacketHeader* packet) = NULL;
	//处理Account的包
	virtual void OnAccountPacket(uint32 srcId,PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam) = NULL;
	//处理网关的包
	virtual void OnGateWayPacket(ISessionGateWay* pSession, PacketHeader* packet, Stream& stream) = NULL;
	//网关通知Game，玩家离线
	virtual void OnPlayerOffLine(uint32 playerId, int32 gatewayId,uint64 sessionId) = NULL;
};

NS_WF_END

