#pragma once
#include "WfConfig.h"
#include "SessionClient.h"
#include "WfPacketHandlerMgr.h"
#include "WfVerifyListenerSession.h"


NS_WF_BEGIN

class SessionClientMgr : public IOMgr, public PacketHandlerMgr<ClientPacketHandler>
{
public:
	SessionClientMgr();
	virtual ~SessionClientMgr();
	virtual bool OnInit();
	
	virtual IOSession* newSession();
	virtual void delSession(IOSession* pSession);

	virtual void OnNewSession(IOSession* pSession);
	virtual void OnRmvSession(IOSession* pSession);
private:
	std::mutex m_SessionResourceLock;
};

NS_WF_END