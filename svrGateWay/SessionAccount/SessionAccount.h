#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN
class SessionAccount : public ConnectionSession
{
public:
	SessionAccount();
	virtual ~SessionAccount();

	virtual void OnConnected();
	virtual void OnDisconnected();
	int OnPacket(PacketHeader* packet, Stream& stream);
private:
	void OnHandle_AG_LoginRst(PacketHeader* packet, Stream& stream);
};

// 消息回调函数
typedef void(*AccountPacketHandler)(SessionAccount* session, PacketHeader* packet, Stream& stream);

NS_WF_END