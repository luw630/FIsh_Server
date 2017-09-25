#include "Dispatcher_Packets.h"

NS_WF_BEGIN

bool Dispatcher_Packets::Init()
{
	m_DispatcherGames.Init();
	m_DispatcherGateWay.Init();
	m_DispatcherPlayer.Init();
	m_DispatcherProxy.Init();
	return true;
}

NS_WF_END