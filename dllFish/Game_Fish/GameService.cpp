#pragma once
#include "WfSysUtil.h"
#include "GameService.h"
#include "WfIService.h"
#include "ITable.h"
#include "WfLog.h"
#include "IGame.h"
#include "GameFish.h"

#define UpdateFps 50
NS_WF_BEGIN

GameService::GameService()
{
	m_bActive = true;
	m_nowTick = 0;
	m_LastUpdateTick = 0;
	m_id = -1;
	m_SendPackets = new LockFreeList<abs*>();
}
GameService::~GameService()
{
	WF_SAFE_DELETE(m_SendPackets);
}

void GameService::run()
{
	int32 rst = 0;
	m_LastUpdateTick = SysUtil::GetMSTime();
	int32 fpsTime = 1000 / UpdateFps;
	while (m_bActive) {
		m_nowTick = SysUtil::GetMSTime();
		uint32 tmp = TIMEPOINTDIFF(m_nowTick, m_LastUpdateTick);

		for (auto &t : m_Tables)
		{
			if (t->isActive())
			{
				t->Update(tmp);
			}
		}

		if (tmp < fpsTime) {
			Sleep(fpsTime - tmp);
		}
		m_LastUpdateTick = m_nowTick;
	}
	log_dbg("closed Service(%d)", m_id);
}
void GameService::pushTable(ITable *pTable)
{
	m_Tables.push_back(pTable);
}
void GameService::sendPacket(abs* msg)
{
	abs** pNewPacket = m_SendPackets->WriteLock();
	if (pNewPacket)
	{
		*pNewPacket = msg;
		m_SendPackets->WriteUnlock();
	}
}
void GameService::update()
{
	int32 max_Process_num = 100;

	while ((--max_Process_num) >= 0)
	{
		abs** pPacket = m_SendPackets->ReadLock();
		if (pPacket) {
			abs* packet = *pPacket;
			m_SendPackets->ReadUnlock();
			m_game->OnWorkThreadPacket((PacketHeader*)packet->getData());
			packet->release();
		}
		else {
			break;
		}
	}
}
void GameService::init(int32 id, GameFish* game)
{
	m_id = id;
	m_game = game;
}
GameFish* GameService::getGameFish()
{
	return m_game;
}
NS_WF_END

