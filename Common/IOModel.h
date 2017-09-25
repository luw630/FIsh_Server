#pragma once
#include "WfConfig.h"
#include "WfPacketHeader.h"

class IOMgr;
class IOSession;
class IOModel
{
public:
	virtual bool Init(const std::string& sIp, int nPort, IOMgr* ioEvent) = NULL;
	// ����������
	virtual bool Start() = NULL;
	//	ֹͣ������
	virtual void Stop() = NULL;
	virtual bool PostRead(IOSession* pSession) = NULL;
	virtual bool PostSend(IOSession* pSession) = NULL;
	virtual void PostClose(ULONG_PTR key) = NULL;
};