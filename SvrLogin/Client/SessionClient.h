/********************************************************************
created:	2012-08-23
author:		lilin ()
summary:	客户端连接
*********************************************************************/
#pragma once

#include "IOMgr.h"

NS_WF_BEGIN
class App;
class SessionClient : public IOSession
{
public:
	SessionClient();
	virtual ~SessionClient();

	virtual void OnInit();
	virtual void OnConnected();
	virtual void OnDisconnected(void);
	virtual void OnPacket(Wf::abs* newPacket, PacketHeader* packet, Stream& stream);
};

// 消息回调函数
typedef void(*ClientPacketHandler)(SessionClient* session, PacketHeader* packet, Stream& stream);

NS_WF_END
