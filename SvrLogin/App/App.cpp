#include "WfConfig.h"
#include "WfLog.h"
#include "App.h"

#include "WfRandom.h"
#include "WfNetMgr.h"
#include "SeesionDb/SeesionDb.h"
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
	m_SessionClientMgr = new SessionClientMgr();
	m_SeesionLoginDb = new SeesionDb();
	m_SessionMemoryDb = new ConnectorMgr<SessionMemoryDb>(m_NetMgr);

	// init NetMgr
	if (!m_NetMgr->Init(1))
	{
		log_info("NetMgr::GetInstance().Init failure\n");
		return false;
	}
	log_info("init NetMgr OK\n");

	if (!m_SeesionLoginDb->Intial())
	{
		log_info("connect account db failure\n");
		return false;
	}
	log_info("connect account db OK\n");
	
	if (!m_SessionMemoryDb->Init(Cfg_MemoryDbConnector))
	{
		log_err("connect gameSvr failure\n");
		return false;
	}
	log_info("connect gameSvr ok\n");

	// start NetMgr
	if (!m_NetMgr->Start())
	{
		log_info("NetMgr::GetInstance().Start failure\n");
		return false;
	}
	log_info("start NetMgr\n");

	if (!m_SessionClientMgr->Init())
	{
		log_info("ClientListener::GetInstance().Init failure\n");
		return false;
	}
	log_info("init ClientListener OK\n");
	return true;
}

bool App::OnIniting()
{
	m_NetMgr->Update();
	if (!m_SessionMemoryDb->IsAllConnected())
		return true;

	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{

	static uint last = 0;
	Random::SetSeed_Now();
	m_SessionClientMgr->update(delta);
	m_SeesionLoginDb->update();
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