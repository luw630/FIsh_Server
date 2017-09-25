#pragma once
#include "WfConfig.h"
#include "IOContext.h"
#include "WfPacketHeader.h"
#include "IOModel.h"
#include "WfAbsFactory.h"
#include <google/protobuf/message.h>
#include "WfAtomic.h"

USING_NS_WF;
struct _PER_IO_CONTEXT;
#define MAX_RCVBUFFER_LEN        8192  
#define MAX_SENDBUFFER_LEN        8192  

class ConCfg
{
public:
	static uint m_MaxSendPacketSize;	// 发送包的最大size
	static uint m_MaxRecvPacketSize;	// 接收包的最大size
	static uint m_SendBufferSize;		// 发送包的缓存大小
	static uint m_RecvBufferSize;		// 接收包的缓存大小
};

class IOSession
{
public:
	IOSession();
	~IOSession();
	uint64 m_sessionId;
	SOCKET      m_Socket;                                  // 每一个客户端连接的Socket
	SOCKADDR_IN m_ClientAddr;                              // 客户端的地址
	_PER_IO_CONTEXT m_OverlappedRead;
	_PER_IO_CONTEXT m_OverlappedWrite;
	RWBuffer		m_SendBuffer;			// 发送缓存
	RWBuffer		m_RecvBuffer;			// 收包缓存
	Wf::abs*		m_CurrentPacket;		// 当前正在解析的数据包
	size_t			m_CurrentPacketOffset;	// 当前解析包的偏移
	bool			m_IsReading;			// 是否正在接受数据,已经请求了async_read_some
	LockFreeList<Wf::abs*>* m_SendList;		// 发送列表
	std::mutex		m_SendListLock;
	bool			m_IsClosed;				// 关闭标记
	time_t			m_CloseTime;
	AtomicBool		m_IsSending;			// 是否正在发送数据

	void init(uint64 sessionId, SOCKET s, SOCKADDR_IN* clientAddr);
	void release();
	uint64 getSessionId() { return m_sessionId; }	
	virtual void OnInit() {};
	virtual void OnPacket(Wf::abs* newPacket, PacketHeader* packet, Stream& stream) {};
	virtual void OnPacket(PacketHeader* packet, Stream& stream) {};
	void SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
	void SendPacket(PacketCommand cmd, ISerializePacket* packet);	
	void SendPacket(PacketHeader* packet);	
	bool ParsePacketBuffer();
	bool ParseAPacketFromBuffer(RWBuffer& buffer, bool& continueParse, Wf::abs*& newPacket);
	bool PostRead();
	bool PostWrite();
	void OnPing(uint last);
	void OnPong(uint last);
public:
	time_t							m_LastPingTime;				// 上次ping的时间
	time_t							m_LastOnPingTime;			// 上次OnPing的时间
	uint							m_timeout;
};

class IOMgr
{
public:
	bool Init();
	void accept(IOSession* pSession);
	uint64 newSessionId();
	void rmvSession(uint64 sessionId);
	// 清除客户端列表信息
	void clearAllConnect();
	
	virtual bool OnInit() = NULL;
	virtual IOSession* newSession() = NULL;
	virtual void delSession(IOSession* pSession) = NULL;
	virtual void OnNewSession(IOSession* pSession) = NULL;
	virtual void OnRmvSession(IOSession* pSession) = NULL;
	void SendPacket(uint64 sessionId, PacketCommand cmd, ::google::protobuf::Message* packet);
	void SendPacket(uint64 sessionId, PacketCommand cmd, ISerializePacket* packet);
	void SendPacket(uint64 sessionId, PacketHeader* packet);

	static Wf::abs* createBuffer(uint32 size);
	static IOModel* m_IoModel;
	//主线程更新
	void update(uint32 delta);
	//服务端关闭socket
	void close(uint64 sessionId);
private:
	static absFactoryLock	m_PacketFactory;	
	std::mutex m_sessionLock;
	std::hash_map<uint64, IOSession*>	m_sessions;
	std::vector<IOSession*> m_sessionShutdown;
	std::mutex m_needCloseLock;
};