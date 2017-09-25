//http://blog.csdn.net/PiggyXP
#pragma once

// winsock 2 ��ͷ�ļ��Ϳ�
#include "WfPacketHeader.h"
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")


#include "IOMgr.h"
#include "IOModel.h"
#include "WfAbsFactory.h"
USING_NS_WF;


// CIOCPModel��
class CIOCPModel: public IOModel
{
public:
	CIOCPModel(void);
	~CIOCPModel(void);
public:
	bool Init(const std::string& sIp,int nPort, IOMgr* ioEvent);
	// ����������
	bool Start();

	//	ֹͣ������
	void Stop();

	// ����Socket��
	bool LoadSocketLib();

	// ж��Socket�⣬��������
	void UnloadSocketLib() { WSACleanup(); }
protected:

	// ��ʼ��IOCP
	bool CreateIOCP();

	// ��ʼ��Socket
	bool InitListenSocket();

	// ����ͷ���Դ
	void Release();

	// Ͷ��Accept����
	bool PostAccept( PER_IO_CONTEXT* pAcceptIoContext ); 

	// Ͷ�ݽ�����������
	bool PostRead(IOSession* pSession);
	// Ͷ�ݷ�����������
	bool PostSend(IOSession* pSession);
	void PostClose(ULONG_PTR key);

	// ���пͻ��������ʱ�򣬽��д���
	bool HandleAccepet(PER_IO_CONTEXT* pIoContext );
	// ���н��յ����ݵ����ʱ�򣬽��д���
	bool HandleRead(IOSession *pSession, std::size_t bytes_transferred);

	bool HandleWrite(IOSession *pSession, std::size_t bytes_transferred);

	// ������󶨵���ɶ˿���
	bool _AssociateWithIOCP(HANDLE handle, ULONG_PTR key);

	// �̺߳�����ΪIOCP�������Ĺ������߳�
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);

	// �жϿͻ���Socket�Ƿ��Ѿ��Ͽ�
	bool _IsSocketAlive(SOCKET s);
private:
	IOMgr*						 m_NetMgr;
	HANDLE                       m_hShutdownEvent;              // ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�
	HANDLE                       m_hIOCompletionPort;           // ��ɶ˿ڵľ��
	HANDLE*                      m_phWorkerThreads;             // �������̵߳ľ��ָ��
	int		                     m_nThreads;                    // ���ɵ��߳�����
	std::string					 m_strIP;                       // �������˵�IP��ַ
	int                          m_nPort;                       // �������˵ļ����˿�
      
	SOCKET      m_ListenSocket;									//������Socket
	std::vector<_PER_IO_CONTEXT*> m_AcceptIoContext;			// �ͻ���������������������ݣ�

	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; 
};

