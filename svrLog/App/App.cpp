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
	m_SeesionDb = new SeesionDb();

	m_SessionProxyMgr = new SessionProxyMgr(m_NetMgr);

	

	// init NetMgr
	if (!m_NetMgr->Init(1))
	{
		log_err("NetMgr::GetInstance().Init failure\n");
		return false;
	}
	log_info("init NetMgr OK\n");

	if (!m_SeesionDb->Intial())
	{
		log_err("DB Init failure\n");
		return false;
	}
	log_info("DB Init Ok\n");
	

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
	return true;
}

bool App::OnIniting()
{
	m_NetMgr->Update();

	//if (!m_SessionGameSvr->IsAllConnected())
	//	return true;
	if (!m_SessionProxyMgr->m_session->IsConnected())
		return true;

	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{
	static uint last = 0;
	Random::SetSeed_Now();
	m_NetMgr->Update();
	m_SeesionDb->update();
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