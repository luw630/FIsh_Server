#include "WfLog.h"
#include "ListenerClient.h"
#include <TinyXml/tinyxml.h>
#include "WfNetMgr.h"
#include "WfSessionType.h"
#include "App/App.h"
NS_WF_BEGIN

ListenerClient::ListenerClient(NetMgr* netMgr) :VerifyListenerSession(ESessionType_Client, netMgr)
{

}


ListenerClient::~ListenerClient()
{
}
bool ListenerClient::Init()
{
	return VerifyListenerSession::Init(Cfg_ClientListener);
}
void ListenerClient::OnNewSession(ConnectionPtr newConnection)
{
	SessionClient* pSession = new SessionClient();
	pSession->BindConnection(newConnection.get());
	m_Sessions[pSession->GetSessionId()] = pSession;
}
void ListenerClient::OnRmvSession(ConnectionPtr removeConnection, bool isTimeoutDisconnect)
{
	SessionClient* session = (SessionClient*)removeConnection->GetSession();
	if (session != nullptr)
	{
		m_Sessions.erase(session->GetSessionId());
		delete session;
		removeConnection->SetSession(nullptr);
	}
}
void ListenerClient::Shutdown()
{

}

Player* ListenerClient::getPlayerBySessionId(t_sessionId id)
{
	auto iter = m_PlayerBySessionId.find(id);
	if (iter == m_PlayerBySessionId.end()) return nullptr;

	return iter->second;
}
Player* ListenerClient::getPlayer(uint32 playerId)
{
	auto iter = m_Player.find(playerId);
	if (iter == m_Player.end()) return nullptr;

	return iter->second;
}
void ListenerClient::delPlayer(Player* pPlayer)
{
	m_Player.erase(pPlayer->playerId);
	m_PlayerBySessionId.erase(pPlayer->sessionId);
	delete 	pPlayer;
}


bool ListenerClient::bindPlayer(t_sessionId id, uint32 playerId, uint32 accountSvrId)
{
	auto tmpIter = m_tmpSession.find(id);
	if (tmpIter != m_tmpSession.end())
	{
		m_tmpSession.erase(tmpIter);

		Player* pPlayer = nullptr;
		auto checkIter = m_Player.find(playerId);
		if (checkIter != m_Player.end())
		{
			pPlayer = checkIter->second;

			auto tmp = m_PlayerBySessionId.find(pPlayer->sessionId);
			if (tmp != m_PlayerBySessionId.end()) {
				m_PlayerBySessionId.erase(tmp);
				Close(pPlayer->sessionId);
			}
		}
		else {
			pPlayer = new Player();
			pPlayer->playerId = playerId;
			m_Player[playerId] = pPlayer;
		}
		pPlayer->sessionId = id;
		pPlayer->m_accountSvrId = accountSvrId;
		pPlayer->m_GameSvrId = -1;
		
		m_PlayerBySessionId[id] = pPlayer;
		return true;
	}
	return false;
}
NS_WF_END