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
	static uint m_MaxSendPacketSize;	// ���Ͱ������size
	static uint m_MaxRecvPacketSize;	// ���հ������size
	static uint m_SendBufferSize;		// ���Ͱ��Ļ����С
	static uint m_RecvBufferSize;		// ���հ��Ļ����С
};

class IOSession
{
public:
	IOSession();
	~IOSession();
	uint64 m_sessionId;
	SOCKET      m_Socket;                                  // ÿһ���ͻ������ӵ�Socket
	SOCKADDR_IN m_ClientAddr;                              // �ͻ��˵ĵ�ַ
	_PER_IO_CONTEXT m_OverlappedRead;
	_PER_IO_CONTEXT m_OverlappedWrite;
	RWBuffer		m_SendBuffer;			// ���ͻ���
	RWBuffer		m_RecvBuffer;			// �հ�����
	Wf::abs*		m_CurrentPacket;		// ��ǰ���ڽ��������ݰ�
	size_t			m_CurrentPacketOffset;	// ��ǰ��������ƫ��
	bool			m_IsReading;			// �Ƿ����ڽ�������,�Ѿ�������async_read_some
	LockFreeList<Wf::abs*>* m_SendList;		// �����б�
	std::mutex		m_SendListLock;
	bool			m_IsClosed;				// �رձ��
	time_t			m_CloseTime;
	AtomicBool		m_IsSending;			// �Ƿ����ڷ�������

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
	time_t							m_LastPingTime;				// �ϴ�ping��ʱ��
	time_t							m_LastOnPingTime;			// �ϴ�OnPing��ʱ��
	uint							m_timeout;
};

class IOMgr
{
public:
	bool Init();
	void accept(IOSession* pSession);
	uint64 newSessionId();
	void rmvSession(uint64 sessionId);
	// ����ͻ����б���Ϣ
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
	//���̸߳���
	void update(uint32 delta);
	//����˹ر�socket
	void close(uint64 sessionId);
private:
	static absFactoryLock	m_PacketFactory;	
	std::mutex m_sessionLock;
	std::hash_map<uint64, IOSession*>	m_sessions;
	std::vector<IOSession*> m_sessionShutdown;
	std::mutex m_needCloseLock;
};