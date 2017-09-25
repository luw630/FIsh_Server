/********************************************************************
created:	2012-08-23
author:		lilin ()
summary:	客户端连接
*********************************************************************/
#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN

class SessionGateWay : public ConnectionSession
{
	Def_MemberVariable(int32, GateWayId);
public:
	SessionGateWay();
	virtual ~SessionGateWay();
	virtual void OnConnected();
	virtual void OnDisconnected(void);
	int OnPacket(PacketHeader* packet, Stream& stream);
	void setNodeId(int32 id) { m_NodeId = id; }
	void setCheck(bool value) { m_bCheck = value; }
private:
	bool m_bCheck;	//是否验证
	int32 m_NodeId;	//节点id
private:
	void OnHandle_GA_Login(PacketHeader* packet, Stream& stream);
	void OnHandle_GA_Offline(PacketHeader* packet, Stream& stream);
	void OnHandle_SvrNodeLogin(PacketHeader* packet, Stream& stream);
};

// 消息回调函数
typedef void(*ClientPacketHandler)(SessionGateWay* session, PacketHeader* packet, Stream& stream);

NS_WF_END
