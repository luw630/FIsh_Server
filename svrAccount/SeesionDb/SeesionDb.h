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
	virtual void OnPacket(uint64 connectId, PacketHeader* packet, Stream& stream);
	virtual void OnPacket(PacketHeader* packet, Stream& stream);
private:
	static void OnHandle_SD_LoadData(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
	static void OnHandle_SD_InsertPlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
	static void OnHandle_SD_SavePlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
	static void OnHandle_SD_FindPlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);


	void OnHandle_DS_PlayerData(t_sessionId connectId, PacketHeader* pHeader, Stream& stream);
	void OnHandle_FindPlayerRst(t_sessionId connectId, PacketHeader* pHeader, Stream& stream);
};
NS_WF_END

