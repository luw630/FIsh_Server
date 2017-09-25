#include "WfConfig.h"
#include "WfLog.h"
#include "App.h"
#include "WfRandom.h"
#include "WfNetMgr.h"
#include <boost/filesystem.hpp>
#include "WfStringUtil.h"
#include "Packets/Packet_Friend.pb.h"
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
	m_ConnectMgr = new ConnectMgr(m_NetMgr);
	// init NetMgr
	if (!m_NetMgr->Init(1))
	{
		log_info("NetMgr::GetInstance().Init failure\n");
		return false;
	}
	log_info("init NetMgr OK\n");
	
	if (!m_ConnectMgr->Init("SessionGateway"))
	{
		log_info("m_ConnectMgr failure\n");
		return false;
	}
	log_info("m_ConnectMgr ok\n");

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
	

	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{

	static uint last = 0;
	Random::SetSeed_Now();
	m_NetMgr->Update();

	for (auto& t : m_ConnectMgr->m_Sessions)
	{
		SessionGateway* session = t.second;
		session->Update(delta);
	}
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