//http://blog.csdn.net/PiggyXP
#pragma once

// winsock 2 的头文件和库
#include "WfPacketHeader.h"
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")


#include "IOMgr.h"
#include "IOModel.h"
#include "WfAbsFactory.h"
USING_NS_WF;


// CIOCPModel类
class CIOCPModel: public IOModel
{
public:
	CIOCPModel(void);
	~CIOCPModel(void);
public:
	bool Init(const std::string& sIp,int nPort, IOMgr* ioEvent);
	// 启动服务器
	bool Start();

	//	停止服务器
	void Stop();

	// 加载Socket库
	bool LoadSocketLib();

	// 卸载Socket库，彻底完事
	void UnloadSocketLib() { WSACleanup(); }
protected:

	// 初始化IOCP
	bool CreateIOCP();

	// 初始化Socket
	bool InitListenSocket();

	// 最后释放资源
	void Release();

	// 投递Accept请求
	bool PostAccept( PER_IO_CONTEXT* pAcceptIoContext ); 

	// 投递接收数据请求
	bool PostRead(IOSession* pSession);
	// 投递发送数据请求
	bool PostSend(IOSession* pSession);
	void PostClose(ULONG_PTR key);

	// 在有客户端连入的时候，进行处理
	bool HandleAccepet(PER_IO_CONTEXT* pIoContext );
	// 在有接收的数据到达的时候，进行处理
	bool HandleRead(IOSession *pSession, std::size_t bytes_transferred);

	bool HandleWrite(IOSession *pSession, std::size_t bytes_transferred);

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP(HANDLE handle, ULONG_PTR key);

	// 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);

	// 判断客户端Socket是否已经断开
	bool _IsSocketAlive(SOCKET s);
private:
	IOMgr*						 m_NetMgr;
	HANDLE                       m_hShutdownEvent;              // 用来通知线程系统退出的事件，为了能够更好的退出线程
	HANDLE                       m_hIOCompletionPort;           // 完成端口的句柄
	HANDLE*                      m_phWorkerThreads;             // 工作者线程的句柄指针
	int		                     m_nThreads;                    // 生成的线程数量
	std::string					 m_strIP;                       // 服务器端的IP地址
	int                          m_nPort;                       // 服务器端的监听端口
      
	SOCKET      m_ListenSocket;									//监听的Socket
	std::vector<_PER_IO_CONTEXT*> m_AcceptIoContext;			// 客户端网络操作的上下文数据，

	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs; 
};

