#pragma once
#include "WfConfig.h"
#include "SessionClient.h"
#include "WfPacketHandlerMgr.h"
#include "WfVerifyListenerSession.h"


NS_WF_BEGIN
struct Player {
	t_sessionId sessionId;
	uint32 playerId;

	uint32 m_accountSvrId;	//ÕËºÅ·þÎñÆ÷id
	uint32 m_GameSvrId;
};
class ListenerClient : public VerifyListenerSession, public PacketHandlerMgr<ClientPacketHandler>
{
public:
	ListenerClient(NetMgr* netMgr);
	virtual ~ListenerClient();

	virtual bool Init();
	virtual void OnNewSession(ConnectionPtr newConnection);
	virtual void OnRmvSession(ConnectionPtr removeConnection, bool isTimeoutDisconnect);
	virtual void Shutdown();
public:
	Player* getPlayerBySessionId(t_sessionId id);
	Player* getPlayer(uint32 playerId);
	void delPlayer(Player* pPlayer);
	bool bindPlayer(t_sessionId id, uint32 playerId, uint32 accountSvrId);
public:
	std::set<t_sessionId>	m_tmpSession;
	std::hash_map<t_sessionId, Player*>	m_PlayerBySessionId;
	std::hash_map<uint32, Player*> m_Player;
};

NS_WF_END
