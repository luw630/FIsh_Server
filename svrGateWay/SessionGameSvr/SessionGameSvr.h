#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN
class SessionGameSvr : public ConnectionSession
{
public:
	SessionGameSvr();
	virtual ~SessionGameSvr();

	virtual void OnConnected();
	virtual void OnDisconnected();
	int OnPacket(PacketHeader* packet, Stream& stream);
};

NS_WF_END