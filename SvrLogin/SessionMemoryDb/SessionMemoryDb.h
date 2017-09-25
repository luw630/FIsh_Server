#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN
class SessionMemoryDb : public ConnectionSession
{
public:
	SessionMemoryDb();
	virtual ~SessionMemoryDb();

	virtual void OnConnected();
	virtual void OnDisconnected();
	int OnPacket(PacketHeader* packet, Stream& stream);
};
NS_WF_END