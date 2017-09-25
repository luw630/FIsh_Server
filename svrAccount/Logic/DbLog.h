#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"
#include "WfPacketHeader.h"

NS_WF_BEGIN

class DbLog
{
public:
	DbLog();
	virtual ~DbLog();
public:
	void Log_PlayerLogin(t_PlayerId playerid, time_t time);
	void Log_PlayerQuit(t_PlayerId playerid, time_t time);
};
NS_WF_END