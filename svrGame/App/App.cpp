#include "WfDataType.h"
#include "WfLog.h"
#include "App.h"
#include "WfRandom.h"
#include "WfNetMgr.h"
#include <boost/filesystem.hpp>
#include "WfSessionType.h"
#include "WfCommonPackets.h"

NS_WF_BEGIN

App::App()
{
	m_NetMgr = nullptr;
	m_ListenerGateWay = nullptr;
	m_Game = nullptr;
}

App::~App()
{ 
}
extern "C" {
	typedef  IGame* (__stdcall *CreateGameProc)(IAppProxy* AppProxy);
}
IGame* App::loadGame()
{
	IGame* pGame = nullptr;
#ifdef _MSC_VER
	HMODULE pDllHandle = ::LoadLibrary("dllFish.dll");
	if (pDllHandle != nullptr)
	{
		CreateGameProc pf_CreateGame = (CreateGameProc)GetProcAddress(pDllHandle, "CreateGame");
		if (!pf_CreateGame)
		{
			FreeLibrary(pDllHandle);
			return nullptr;
		}
		pGame = pf_CreateGame(this);
	}
	else
	{
		return nullptr;
	}
#else
#endif // _MSC_VER

	return pGame;
}
bool App::SendPacket(t_ObjId gatewayId, PacketHeader* packet)
{
	SessionGateWay* pSessionGateWay = m_ListenerGateWay->getSessionGateWayByNodeId(gatewayId);
	if (!pSessionGateWay)
	{
		log_waring("not find SessionGateWay by nodeId(%d)", gatewayId);
		return false;
	}

	return pSessionGateWay->SendPacket(packet);
}
bool App::OnInit(int argc, char **argv)
{
	Random::SetSeed_Now();

	m_NetMgr = new NetMgr(this);
	m_ListenerGateWay = new ListenerGateWay(m_NetMgr);
	m_SessionProxyMgr = new SessionProxyMgr(m_NetMgr);


	// init NetMgr
	if (!m_NetMgr->Init(2))
	{
		log_err("NetMgr::GetInstance().Init failure\n");
		return false;
	}
	log_info("init NetMgr OK\n");

	if (!m_ListenerGateWay->Init())
	{
		log_err("ListenerGateWay Init failure\n");
		return false;
	}
	log_info("ListenerGateWay Init OK\n");
	
	if (!m_SessionProxyMgr->Init())
	{
		log_err("connect proxy failure\n");
		return false;
	}
	log_info("connect proxy ok\n");


	// start NetMgr
	if (!m_NetMgr->Start())
	{
		log_info("NetMgr::GetInstance().Start failure\n");
		return false;
	}
	log_info("start NetMgr\n");

	m_Game = loadGame();

	if (!m_Game) {
		log_err("load Game error\n");
		return false;
	}
	if (!m_Game->Init(GetWorkThreadNum()))
	{
		log_err("init Game failure\n");
		return false;
	}
	log_info("init Game success\n");
	m_Game->Start();
	log_info("start workService");
	return true;
}

bool App::OnIniting()
{
	if (!m_SessionProxyMgr->m_session->IsConnected())
		return true;
	
	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{
	static uint last = 0;
	Random::SetSeed_Now();
	m_NetMgr->Update();
	m_Game->OnUpdate(delta);
	return true;
}
bool App::OnShutdown()
{

	log_info("OnShutdown begin\n");
	
	
	log_info("OnShutdown end\n");
	return true;
}

bool App::OnShutdowning()
{

	return BaseApp::OnShutdowning();
}

void App::OnCommand(const std::string& command)
{
	
}

void App::SendPacketToAccountSvr(PacketCommand cmd, ::google::protobuf::Message* packet)
{
	m_SessionProxyMgr->m_session->PostMsg(
		cmd,
		packet,
		0,
		nullptr,
		emSvrType_Game,
		GetAppId(),
		emSvrType_Account,
		trans_p2p,
		AccountSvrId
	);
}
NS_WF_END