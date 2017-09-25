#pragma once
#include "IWorkMgr.h"
#include "WfPacketHandlerMgr.h"

NS_WF_BEGIN
class SeesionDb : public IWorkMgr, public PacketHandlerMgr<TaskPacketHandler>
{
public:
	SeesionDb();
	~SeesionDb();
public:
	virtual bool OnIntial();
	virtual void OnHandlePacket(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
	virtual void OnPacket(t_sessionId connectId, PacketHeader* packet, Stream& stream);
private:
	static void OnHandle_Log_PlayerLogin(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//玩家登录
	static void OnHandle_Log_PlayerQuit(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//玩家退出
	static void OnHandle_Log_EnterFishGame(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//进入捕鱼游戏
	static void OnHandle_Log_LeaveFishGame(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//离开捕鱼游戏
	static void OnHandle_Log_EnterEnterTable(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//进入座子
	static void OnHandle_Log_EnterLeaveTable(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//离开座子	
	static void OnHandle_Log_BattleSendBullet(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//发射子弹
	static void OnHandle_Log_BattleKillFish(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);//杀死鱼
};
NS_WF_END

