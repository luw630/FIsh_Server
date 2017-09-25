#pragma once

#include "IGame.h"
#include "WfLog.h"

NS_WF_BEGIN

IGame::IGame()
{
	m_AppProxy = nullptr;
	m_pGameService = nullptr;
	m_GameServiceNum = 0;
}
IGame::~IGame()
{

}

void IGame::Start()
{
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].start();
	}
}
void IGame::Stop()
{
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].stop();
	}
}
void IGame::updateWorkThreadPacket()
{
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].update();
	}
}
NS_WF_END

