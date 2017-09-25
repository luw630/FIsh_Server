#pragma once

enum ESessionType
{
	ESessionType_LoginSession=10,
	ESessionType_Client = 20,
	ESessionType_DbSession = 30,
	ESessionType_LoginSvr = 40,
	ESessionType_Proxy = 50,
	ESessionType_LoginProxy = 60,
	ESessionType_GateWay = 70,
	ESessionType_Account = 80,
};


#define SessionCfg_Client	"SessionClient"
#define SessionCfg_Login	"SessionLogin"
#define SessionCfg_SessionGateWay	"ISessionGateWay"
#define SessionCfg_SessionGame	"SessionGame"

#define Cfg_MemoryDbConnector	"MemoryDbConnector"
#define Cfg_ClientListener	"ClientListener"
#define Cfg_GameConnector	"GameConnector"
#define Cfg_ProxyConnector	"ProxyConnector"
#define Cfg_AccountConnector	"AccountConnector"
#define Cfg_GateWayListener	"GateWayListener"
#define Cfg_AccountListener	"AccountListener"