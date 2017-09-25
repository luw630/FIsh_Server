#pragma once

#include "WfConfig.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "SessionGateway/SessionGateway.h"
#include "ConnectMgr.h"
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
private:	
	NetMgr* m_NetMgr;
	ConnectMgr* m_ConnectMgr;
};

NS_WF_END