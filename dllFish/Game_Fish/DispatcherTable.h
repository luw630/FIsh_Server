#pragma once
#include "WfConfig.h"
#include "Player.h"
#include "Dispatcher.h"
#include "Player.h"
#include "WfPacketHandlerMgr.h"
#include "GamePacket.h"

NS_WF_BEGIN
class Table;
class DispatcherTable
{
private:
	Table* m_pTable;
public:
	DispatcherTable(Table* handle);
	bool Init();
public:
	void OnHandle_EnterTable(t_ObjId seatId, PacketHeader* header, Stream& stream);
	void OnHandle_ReEnterTable(t_ObjId seatId, PacketHeader* header, Stream& stream);
	void OnHandle_PlayerOffLine(t_ObjId seatId, PacketHeader* header, Stream& stream);
	
	void OnHandle_CS_FishChangeFireCost(t_ObjId seatId, PacketHeader* header, Stream& stream);
	void OnHandle_CS_FishReqSwitchCannon(t_ObjId seatId, PacketHeader* header, Stream& stream);
	void OnHandle_CS_FishSendBullet(t_ObjId seatId, PacketHeader* header, Stream& stream);
	void OnHandle_CS_FishNotifyHitted(t_ObjId seatId, PacketHeader* header, Stream& stream);
};
typedef void (DispatcherTable::*HandleTable)(t_ObjId seatId,PacketHeader* header,Stream& stream);

NS_WF_END

