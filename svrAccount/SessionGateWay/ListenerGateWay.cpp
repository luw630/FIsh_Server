#include "WfLog.h"
#include "ListenerGateWay.h"
#include <TinyXml/tinyxml.h>
#include "WfNetMgr.h"
#include "WfSessionType.h"
NS_WF_BEGIN

ListenerGateWay::ListenerGateWay(NetMgr* netMgr) :VerifyListenerSession(ESessionType_GateWay, netMgr)
{

}


ListenerGateWay::~ListenerGateWay()
{
}
bool ListenerGateWay::Init()
{
	return VerifyListenerSession::Init(Cfg_GateWayListener);
}
void ListenerGateWay::OnNewSession(ConnectionPtr newConnection)
{
	SessionGateWay* pSession = new SessionGateWay();
	pSession->BindConnection(newConnection.get());
	m_Sessions[pSession->GetSessionId()] = pSession;
}
void ListenerGateWay::OnRmvSession(ConnectionPtr removeConnection, bool isTimeoutDisconnect)
{
	SessionGateWay* session = (SessionGateWay*)removeConnection->GetSession();
	if (session != nullptr)
	{
		m_Sessions.erase(session->GetSessionId());
		delete session;
		removeConnection->SetSession(nullptr);
	}
}
void ListenerGateWay::Shutdown()
{

}
void ListenerGateWay::delSession(SessionGateWay* pSession)
{
	if (pSession)
	{
		if (pSession->IsConnected())
		{
			pSession->GetConnection()->Close();
		}
	}
}
SessionGateWay* ListenerGateWay::getSessionGateWay(t_sessionId id)
{
	return (SessionGateWay*)GetSession(id);
}
SessionGateWay* ListenerGateWay::getSessionGateWayByNodeId(int32 id)
{
	auto t = m_mapSessionIdByNodeId.find(id);
	if (t != m_mapSessionIdByNodeId.end())
	{
		return getSessionGateWay(t->second);
	}
	return nullptr;
}
void ListenerGateWay::Update(uint delta)
{

}
void ListenerGateWay::bindGateway(SessionGateWay* pSession, int32 gatewayId)
{
	auto t = m_mapSessionIdByNodeId.find(gatewayId);
	if (t != m_mapSessionIdByNodeId.end())
	{
		if (t->second != pSession->GetSessionId())
		{
			//Ìß³öold
			SessionGateWay* old = getSessionGateWay(t->second);
			if (old) {
				old->GetConnection()->Close();
			}
		}
	}
	pSession->setGateWayId(gatewayId);
	pSession->setCheck(true);
	m_mapSessionIdByNodeId[gatewayId] = pSession->GetSessionId();
}
NS_WF_END