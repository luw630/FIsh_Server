#pragma once
#include "WfDataType.h"
#include "SessionGateWay.h"
#include "WfPacketHandlerMgr.h"
#include "WfVerifyListenerSession.h"


NS_WF_BEGIN

class ListenerGateWay : public VerifyListenerSession, public PacketHandlerMgr<GateWayPacketHandler>
{
public:
	ListenerGateWay(NetMgr* netMgr);
	virtual ~ListenerGateWay();

	virtual bool Init();
	virtual void OnNewSession(ConnectionPtr newConnection);
	virtual void OnRmvSession(ConnectionPtr removeConnection, bool isTimeoutDisconnect);
	virtual void Shutdown();
	void delSession(SessionGateWay* pSession);

	SessionGateWay* getSessionGateWay(t_sessionId id);
	SessionGateWay* getSessionGateWayByNodeId(int32 id);
	void Update(uint delta);
	void bindNodeId(SessionGateWay* pSession, int32 nodeId);
private:
	map<int32, t_sessionId> m_mapSessionIdByNodeId;
};
NS_WF_END