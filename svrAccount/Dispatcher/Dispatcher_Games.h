#pragma once
#include "WfDataType.h"
#include "WfPacketHandlerMgr.h"

NS_WF_BEGIN
class Dispatcher_Games;
typedef void (Dispatcher_Games::*HandleTable)(PacketHeader* , Stream& , int32 , uint8* );

class Dispatcher_Games :public PacketHandlerMgr<HandleTable>
{
public:
	bool Init();
public:
	//进入游戏结果
	void OnHandle_GameA_EnterGameRst(PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
};
NS_WF_END

