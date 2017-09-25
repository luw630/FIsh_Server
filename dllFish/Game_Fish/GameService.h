#pragma once
#include "IGameService.h"
#include "FishDead.h"

NS_WF_BEGIN
class GameFish;
class GameService : public IGameService
{
public:
	GameService();
	virtual ~GameService();
public:
	virtual void run();
	virtual void pushTable(ITable *pTable);
	virtual void sendPacket(abs* msg);
	virtual void update();	//主线程调用
	virtual abs* newBuffer_Rcv() { return m_recvFactory.newBuffer(); }
	virtual abs* newBuffer_Send() { return m_sendFactory.newBuffer(); }

	void init(int32 id, GameFish* game);
	GameFish* getGameFish();
	FishDead m_FishDead;
private:
	LockFreeList<abs*>*	m_SendPackets;
	absFactory	m_recvFactory;
	absFactory	m_sendFactory;

	std::list<ITable*> m_Tables;
	uint32 m_LastUpdateTick;
	uint32 m_nowTick;
	bool m_bActive;
	int32 m_id;
	GameFish* m_game;
};

NS_WF_END