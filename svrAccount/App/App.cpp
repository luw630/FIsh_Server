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


}

App::~App()
{
}

bool App::OnInit(int argc, char **argv)
{
	m_FriendSystem = new FriendSystem();
	m_PlayerMgr = new PlayerMgr();
	m_Rank = new Rank();
	m_Announcement = new Announcement();
	m_DbLog = new DbLog();

	Random::SetSeed_Now();

	m_NetMgr = new NetMgr(this);
	m_SessionClientMgr = new ListenerGateWay(m_NetMgr);
	m_SeesionDb = new SeesionDb();
	m_SessionProxyMgr = new SessionProxyMgr(m_NetMgr);

	

	if (!m_Dispatcher.Init())
	{
		log_err("Dspatcher init failure\n");
		return false;
	}
	log_info("Dspatcher init OK\n");
	

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

	m_FriendSystem->init();

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

	if (!m_SessionProxyMgr->m_session->IsConnected())
		return true;

	return BaseApp::OnIniting();
}

bool App::OnRunning(uint delta)
{
	Random::SetSeed_Now();
	m_NetMgr->Update();
	m_SeesionDb->update();
	m_FriendSystem->update(delta);
	m_PlayerMgr->update(delta);
	m_Rank->update(delta);
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


void App::SendPacketToGameSvr(PacketCommand cmd, ::google::protobuf::Message* packet, uint32 gameId)
{
	m_SessionProxyMgr->m_session->PostMsg(
		cmd,
		packet,
		0,
		nullptr,
		emSvrType_Account,
		GetAppId(),
		emSvrType_Game,
		trans_p2p,
		gameId
	);
}
NS_WF_END