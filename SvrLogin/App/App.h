#pragma once

#include "WfConfig.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "Client/SessionClientMgr.h"
#include "SeesionDb/SeesionDb.h"
#include "SessionMemoryDb/SessionMemoryDb.h"
#include "WfConnectorMgr.h"

NS_WF_BEGIN
class App : public BaseApp ,public IService
{
public:
	App();
	~App();
	SINGLETON_MODE(App);
	// 初始化
	virtual bool OnInit(int argc, char **argv);
	virtual bool OnIniting();

	// 运行
	virtual bool OnRunning(uint delta);

	// 关闭
	virtual bool OnShutdown();
	virtual bool OnShutdowning();

	// 父类继承该命令接口
	virtual void OnCommand(const std::string& command);

	NetMgr* getNetMgr() { return m_NetMgr; }
	SessionClientMgr* getSessionClientMgr() { return m_SessionClientMgr; }
private:
	NetMgr* m_NetMgr;
	SessionClientMgr* m_SessionClientMgr;
public:
	SeesionDb* m_SeesionLoginDb;
	ConnectorMgr<SessionMemoryDb>* m_SessionMemoryDb;
};
NS_WF_END