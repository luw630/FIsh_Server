/********************************************************************
created:	2012-08-23
author:		lilin ()
summary:	�ͻ�������
*********************************************************************/
#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN

class SessionClient : public ConnectionSession
{
public:
	SessionClient();
	virtual ~SessionClient();
	virtual void OnConnected();
	virtual void OnDisconnected(void);
	int OnPacket(PacketHeader* packet, Stream& stream);
private:
	void onHandleLoginGateWayOld(PacketHeader* packet, Stream& stream);
	void onHandleLoginGateWay(PacketHeader* packet, Stream& stream);
};

// ��Ϣ�ص�����
typedef void(*ClientPacketHandler)(SessionClient* session, PacketHeader* packet, Stream& stream);

NS_WF_END
