/********************************************************************
created:	2012-08-23
author:		lilin ()
summary:	客户端连接
*********************************************************************/
#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"
#include "ISessionGateWay.h"
NS_WF_BEGIN

class SessionGateWay : public ConnectionSession,public ISessionGateWay
{
public:
	SessionGateWay();
	virtual ~SessionGateWay();
	virtual int32 getGatewayId() { return m_GatewayId; }
	virtual void setGatewayId(int32 id) { m_GatewayId = id; }

	virtual void OnConnected();
	virtual void OnDisconnected(void);
	int OnPacket(PacketHeader* packet, Stream& stream);
	void setCheck(bool value) { m_bCheck = value; }
private:
	bool m_bCheck;	//是否验证
	int32 m_GatewayId;
private:
	void OnHandle_SvrNodeLogin(PacketHeader* packet, Stream& stream);
};

// 消息回调函数
typedef void(*GateWayPacketHandler)(SessionGateWay* session, PacketHeader* packet, Stream& stream);

NS_WF_END
