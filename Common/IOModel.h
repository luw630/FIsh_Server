#pragma once
#include "WfConfig.h"
#include "WfPacketHeader.h"

class IOMgr;
class IOSession;
class IOModel
{
public:
	virtual bool Init(const std::string& sIp, int nPort, IOMgr* ioEvent) = NULL;
	// 启动服务器
	virtual bool Start() = NULL;
	//	停止服务器
	virtual void Stop() = NULL;
	virtual bool PostRead(IOSession* pSession) = NULL;
	virtual bool PostSend(IOSession* pSession) = NULL;
	virtual void PostClose(ULONG_PTR key) = NULL;
};