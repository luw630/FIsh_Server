#pragma once
#include "WfConfig.h"
#include "WorkThread.h"

NS_WF_BEGIN

class WorkThreadMgr
{
public:
	WorkThreadMgr();
	~WorkThreadMgr();
	SINGLETON_MODE(WorkThreadMgr);
public:
	bool Intial(uint32 nSize);
	void addUnHandlePacket(int64 sessionId, PacketHeader* pHeader);
	void addFinishPacket(int64 sessionId, PacketHeader* pHeader);
	void update(int64 elapse);
private:
	uint32 m_ThreadNum;
	WorkThread **m_pWorkThread;
	WorkThread* getWorkThread(uint64 sessionId);
public:
	static int OnHandle_C2S_Login(WorkThread* pWorkThread, int64 sessionId, PacketHeader* pHeader, Stream& stream);
};

NS_WF_END