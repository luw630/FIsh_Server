#pragma once
#include "WfConfig.h"
#include "WfThread.h"
#include "WfPacketPool.h"
#include "WfPacketHeader.h"

USING_NS_WF;

#define  HandlePacketBufferMax	1024*64

class WorkThread : public ZThread
{
public:
	WorkThread();
	~WorkThread();

public:
	void addUnHandlePacket(int64 sessionId, PacketHeader* pHeader){};
	void addFinishPacket(int64 sessionId, PacketHeader* pHeader){};
	PacketBuffer* GetHandlePacketBuffer(){ return NULL; };

private:
	uint8 s_HandlePacketBuffer[HandlePacketBufferMax];
	bool mRun;
	bool mExited;

	std::mutex	m_queDbMsgLock;
	std::list<PacketBuffer*> m_listUnHandleBuffer;            // DB线程消息BUFFER
	std::list<PacketBuffer*> m_listFinishBuffer;
	std::mutex	m_queHandleDbMsgLock;
};

