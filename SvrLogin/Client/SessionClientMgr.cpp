#include "WfLog.h"
#include "SessionClientMgr.h"
#include <TinyXml/tinyxml.h>
#include "WfNetMgr.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "WfLog.h"

NS_WF_BEGIN

SessionClientMgr::SessionClientMgr() 
{

}


SessionClientMgr::~SessionClientMgr()
{
}
bool SessionClientMgr::OnInit()
{
	return true;
}
IOSession* SessionClientMgr::newSession()
{
	IOSession* pSession = nullptr;
	m_SessionResourceLock.lock();
	pSession = new SessionClient();
	m_SessionResourceLock.unlock();
	return pSession;
}
void SessionClientMgr::delSession(IOSession* pSession)
{
	m_SessionResourceLock.lock();
	delete pSession;
	m_SessionResourceLock.unlock();
}
void SessionClientMgr::OnNewSession(IOSession* pSession)
{
	log_dbg("newSession(%llu) %s:%d", pSession->getSessionId(),inet_ntoa(pSession->m_ClientAddr.sin_addr), ntohs(pSession->m_ClientAddr.sin_port));
}
void SessionClientMgr::OnRmvSession(IOSession* pSession)
{


}
NS_WF_END