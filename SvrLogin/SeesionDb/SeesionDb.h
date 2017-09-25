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
	static void OnHandle_PacketLogin_CS_Register(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
	static void OnHandle_PacketLogin_CS_Login(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream);
};
NS_WF_END

