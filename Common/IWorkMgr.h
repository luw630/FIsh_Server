#pragma once
#include "WfDataType.h"
#include "WfPacketPool.h"
#include <thread>
#include <condition_variable>
#include "WfPacketHeader.h"
#include "WfMysqlConnection.h"
#include <google/protobuf/message.h>
#include "WfAbsFactory.h"
#include "WfLockFreeList.h"
#include "WfCObjMgr.h"

NS_WF_BEGIN

class IWorkMgr;
class IService;
class TaskHandle
{
	friend class IWorkMgr;
public:
	TaskHandle();
	~TaskHandle();
	void init(IWorkMgr* workMgr);
	void sendBuffer(abs* pBuffer);
	void addBuffer(abs* pBuffer);
	void pingMysql(uint32 delta);
	void update();
	
public:
	void SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet, t_sessionId connectId = INVALID_SESSIONID);
	void SendPacket(PacketCommand cmd, ISerializePacket* packet, t_sessionId connectId = INVALID_SESSIONID);	
	void SendPacket(PacketHeader* pHeader, t_sessionId connectId = INVALID_SESSIONID);
	//通过proxy发送到AccountSvr
	void SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet, int32 msgParamLen, uint8* msgParam, t_ObjId playerId, t_sessionId connectId = INVALID_SESSIONID);

	void OnHandlePacket(uint64 connectId, PacketHeader* pHeader, Stream& stream);
	MysqlConnection* getConnection(){ return &m_connect; };

	abs* newBuffer_Send() { return m_sendFactory.newBuffer(); }
	
private:
	uint m_LastUpdateTick;
	uint m_pingMysqlDelt;

	IWorkMgr* m_workMgr;
	LockFreeList<abs*>*	m_SendPackets;
	LockFreeList<abs*>*	m_RcvPackets;
	absFactory	m_sendFactory;
	absFactory	m_recvFactory;
	MysqlConnection m_connect;
};
typedef void(*TaskPacketHandler)(TaskHandle* handle, uint64 connectId, PacketHeader* pHeader, Stream& stream);

inline uint32 getIndex(const string& data)
{
	uint32 value = 0;
	int nSize = data.size();
	for (int i = 0; i < nSize; i++)
	{
		value += data[i];
	}
	return value;
}
class IWorkMgr
{
	friend class TaskHandle;
public:
	IService* m_svr;
public:
	IWorkMgr(IService* svr);
	~IWorkMgr();

	bool  Intial();
	void update();
	void addBuffer(PacketCommand cmd, ::google::protobuf::Message* packet, t_sessionId sessionId = INVALID_SESSIONID, uint32 accountId = 0);
	void addBuffer(PacketHeader* pHeader, t_sessionId sessionId = INVALID_SESSIONID, uint32 accountId = 0);
	void addBuffer(Wf::abs* newPacket, uint32 accountId = 0);

	virtual bool OnIntial() = NULL;
	virtual void OnHandlePacket(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream) = NULL;
	virtual void OnPacket(t_sessionId connectId, PacketHeader* packet, Stream& stream) = NULL;
private:
	static void task(TaskHandle* pHandle);
	static DbConnectionOpt m_conOpt;
	int32 m_threadNum;	
	std::vector<std::thread> m_threads;
	std::vector<TaskHandle*> m_workHandle;
};
NS_WF_END

