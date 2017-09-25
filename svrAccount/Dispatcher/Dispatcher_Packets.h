#pragma once
#include "Dispatcher_Games.h"
#include "Dispatcher_GateWay.h"
#include "Dispatcher_Player.h"
#include "Dispatcher_Proxy.h"
NS_WF_BEGIN

class Dispatcher_Packets
{
public:
	bool Init();
public:
	Dispatcher_Games m_DispatcherGames;
	Dispatcher_GateWay m_DispatcherGateWay;
	Dispatcher_Player m_DispatcherPlayer;
	Dispatcher_Proxy m_DispatcherProxy;
};
NS_WF_END

