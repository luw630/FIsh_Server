#pragma once

#include "WfCObjMgr.h"
#include "WfLockFreeList.h"
#include "WfAbsFactory.h"
#include <google/protobuf/message.h>
#include "WfPacketHeader.h"

NS_WF_BEGIN

class IGameService;
class IGame;
class ITable : public CObj
{
public:
	ITable();
	~ITable();
public:
	virtual bool onInit() = NULL;
	//是否是活动的
	virtual bool isActive() = NULL;	
	//每帧更新
	virtual void onUpdate(uint32 delta) = NULL;
	//分发消息
	virtual void onDispatcherMsg(abs* buffer) = NULL;
	//广播包
	virtual void broadCastPacket(PacketCommand cmd, ::google::protobuf::Message* packet) = NULL;
	bool Init(t_ObjId id, IGameService* service, IGame* game);
	//推送消息
	void pushMsg(abs* msg);
	//向外部发送消息
	void sendPacket(abs* msg);
	void sendPacket(t_ObjId playerId,PacketCommand cmd, ::google::protobuf::Message* packet);
	//工作线程更新
	void Update(uint32 delta);

	IGameService* getGameService();
	IGame* getGame();
	abs* newBuffer_Rcv();
	abs* newBuffer_Send();
private:
	LockFreeList<abs*>*	m_RecvPackets;			// 收到的包
protected:
	IGameService* m_GameService;
	IGame* m_game;
};

NS_WF_END

