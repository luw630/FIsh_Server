#pragma once

#include "WfConfig.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "WfConnectorMgr.h"
#include "SessionClient/ListenerClient.h"
#include "SessionGameSvr/SessionGameSvr.h"
#include "SessionAccount/SessionAccount.h"
#include "SessionMemoryDb/SessionMemoryDb.h"
NS_WF_BEGIN
class App : public BaseApp ,public IService
{
public:
	App();
	~App();
	SINGLETON_MODE(App);
	// ��ʼ��
	virtual bool OnInit(int argc, char **argv);
	virtual bool OnIniting();

	// ����
	virtual bool OnRunning(uint delta);

	// �ر�
	virtual bool OnShutdown();
	virtual bool OnShutdowning();

	// ����̳и�����ӿ�
	virtual void OnCommand(const std::string& command);


	NetMgr* getNetMgr() { return m_NetMgr; }
	ListenerClient* getListenerClient() { return m_SessionClientMgr; }
	ConnectorMgr<SessionGameSvr>* getSessionGameSvr() { return m_SessionGameSvr; }
	ConnectorMgr<SessionAccount>* getSessionAccount() { return m_SessionAccount; }
	ConnectorMgr<SessionMemoryDb>* getSessionMemoryDb() { return m_SessionMemoryDb; }
private:
	NetMgr* m_NetMgr;
	ListenerClient* m_SessionClientMgr;
	ConnectorMgr<SessionGameSvr>* m_SessionGameSvr;
	ConnectorMgr<SessionAccount>* m_SessionAccount;
	ConnectorMgr<SessionMemoryDb>* m_SessionMemoryDb;
};

NS_WF_END