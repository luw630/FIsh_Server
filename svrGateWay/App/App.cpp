#include "WfConfig.h"
#include "WfLog.h"
#include "App.h"
#include "WfRandom.h"
#include "WfNetMgr.h"
#include <boost/filesystem.hpp>
#include "WfSessionType.h"

NS_WF_BEGIN

App::App()
{


}

App::~App()
{
}

bool App::OnInit(int argc, char **argv)
{
	Random::SetSeed_Now();

	m_NetMgr = new NetMgr(this);
	m_SessionClientMgr = new ListenerClient(m_NetMgr);
	m_SessionGameSvr = new ConnectorMgr<SessionGameSvr>(m_NetMgr);
	m_SessionAccount = new ConnectorMgr<SessionAccount>(m_NetMgr);
	m_SessionMemoryDb = new ConnectorMgr<SessionMemoryDb>(m_NetMgr);
	// init NetMgr
	if (!m_NetMgr->Init(1))
	{
		log_err("NetMgr::GetInstance().Init failure\n");
		return false;
	}
	log_info("init NetMgr OK\n");

	if (!m_SessionClientMgr->Init())
	{
		log_err("ClientListener::GetInstance().Init failure\n");
		return false;
	}
	log_info("init ClientListener OK\n");
	
	if (!m_SessionGameSvr->Init(Cfg_GameConnector))
	{
		log_err("connect gameSvr failure\n");
		return false;
	}
	log_info("connect gameSvr ok\n");
	
	if (!m_SessionAccount->Init(Cfg_AccountConnector))
	{
		log_err("connect Account failure\n");
		return false;
	}
	log_info("connect Account ok\n");

	if (!m_SessionMemoryDb->Init(Cfg_MemoryDbConnector))
	{
		log_err("connect MemoryDb failure\n");
		return false;
	}
	log_info("connect MemoryDb ok\n");
	
	// start NetMgr
	if (!m_NetMgr->Start())
	{
		log_info("NetMgr::GetInstance().Start failure\n");
		return false;
	}
	log_info("start NetMgr\n");
	return true;
}

bool App::OnIniting()
{
	m_NetMgr->Update();
	if (!m_SessionGameSvr->IsAllConnected())
		return true;

	if (!m_SessionAccount->IsAllConnected())
		return true;
	
	if (!m_SessionMemoryDb->IsAllConnected())
		return true;
	
	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{

	static uint last = 0;
	Random::SetSeed_Now();
	m_NetMgr->Update();
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



NS_WF_END