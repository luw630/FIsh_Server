#pragma once

#include "ISessionProxy.h"


NS_WF_BEGIN


class SessionProxy : public ISessionProxy
{
public:
	SessionProxy();
	virtual ~SessionProxy();

	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual int OnPacket(PacketHeader* packet, Stream& stream);
};

class SessionProxyMgr
{
private:
	NetMgr* m_NetMgr;
public:
	SessionProxy* m_session;
public:
	SessionProxyMgr(NetMgr* netMgr)
	{
		m_session = nullptr;
		m_NetMgr = netMgr;
	}
	bool Init();
};
NS_WF_END