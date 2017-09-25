#pragma once

#include "WfConfig.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "SeesionDb/SeesionDb.h"
#include "WfConnectorMgr.h"
#include "SessionProxy/SessionProxy.h"

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

	SeesionDb* getSeesionDb() {return m_SeesionDb;}
	NetMgr* getNetMgr() { return m_NetMgr; }
	SessionProxyMgr* m_SessionProxyMgr;
private:
	SeesionDb* m_SeesionDb;
	NetMgr* m_NetMgr;	
};

NS_WF_END


