
#include "IOCPModel.h"
#include "WfLog.h"
USING_NS_WF;

// 每一个处理器上产生多少个线程(为了最大限度的提升服务器性能，详见配套文档)
#define WORKER_THREADS_PER_PROCESSOR 2
// 同时投递的Accept请求的数量(这个要根据实际的情况灵活设置)
#define MAX_POST_ACCEPT              10
// 传递给Worker线程的退出信号
#define EXIT_CODE                    NULL


// 释放指针和句柄资源的宏

// 释放指针宏
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// 释放句柄宏
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// 释放Socket宏
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}



CIOCPModel::CIOCPModel(void):
							m_nThreads(0),
							m_hShutdownEvent(NULL),
							m_hIOCompletionPort(NULL),
							m_phWorkerThreads(NULL),
							m_strIP("127.0.0.1"),
							m_nPort(12345),
							m_lpfnAcceptEx( NULL ),
							m_ListenSocket(INVALID_SOCKET)
{
}

CIOCPModel::~CIOCPModel(void)
{
	// 确保资源彻底释放
	this->Stop();
}

struct WokerThreadParam
{
	CIOCPModel* pIOCPModel;		// 类指针，用于调用类中的函数
	int         nThreadNo;		// 线程编号
};


///////////////////////////////////////////////////////////////////
// 工作者线程：  为IOCP请求服务的工作者线程
//         也就是每当完成端口上出现了完成数据包，就将之取出来进行处理的线程
///////////////////////////////////////////////////////////////////

DWORD WINAPI CIOCPModel::_WorkerThread(LPVOID lpParam)
{    
	WokerThreadParam* pParam = (WokerThreadParam*)lpParam;
	CIOCPModel* pIOCPModel = (CIOCPModel*)pParam->pIOCPModel;
	int nThreadNo = (int)pParam->nThreadNo;

	log_info("工作者线程启动，ID: %d.", nThreadNo);

	OVERLAPPED           *pOverlapped = NULL;
	ULONG_PTR			 key = 0;
	DWORD                dwBytesTransfered = 0;

	// 循环处理请求，知道接收到Shutdown信息为止
	while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPModel->m_hShutdownEvent, 0))
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			pIOCPModel->m_hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&key,
			&pOverlapped,
			INFINITE);


		PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);

		// 判断是否出现了错误
		if( !bReturn )  
		{  
			DWORD dwErr = GetLastError();
			do {
				if (WAIT_TIMEOUT == dwErr) {
					IOSession *pSession = (IOSession*)pIoContext->m_pSession;
					pIOCPModel->PostClose(key);
				}
				else if (ERROR_NETNAME_DELETED == dwErr) {
					log_dbg("检测到客户端异常退出！");
					IOSession *pSession = (IOSession*)pIoContext->m_pSession;
					pIOCPModel->PostClose(key);
					return true;
				}
				else {
					log_err("完成端口操作出现错误，线程退出。错误代码：%d", dwErr);
				}
			} while (false);
			continue;  
		}
		else  
		{  		
			if (pIoContext == nullptr) {
				pIOCPModel->m_NetMgr->rmvSession(key);
			}
			else {
				// 判断是否有客户端断开了
				if ((0 == dwBytesTransfered) && (RECV_POSTED == pIoContext->m_OpType || SEND_POSTED == pIoContext->m_OpType))
				{
					//log_waring("客户端 %s:%d 断开连接.服务端关闭（%s）",inet_ntoa(pSession->m_ClientAddr.sin_addr), ntohs(pSession->m_ClientAddr.sin_port));
					pIOCPModel->m_NetMgr->rmvSession(key);
					continue;
				}
				else
				{
					// 读取传入的参数
					IOSession *pSession = (IOSession*)pIoContext->m_pSession;

					switch (pIoContext->m_OpType)
					{
						// Accept  
					case ACCEPT_POSTED:
					{
						pIOCPModel->HandleAccepet(pIoContext);
					}
					break;
					// RECV
					case RECV_POSTED:
					{
						pIOCPModel->HandleRead(pSession, dwBytesTransfered);
					}
					break;
					// SEND
					case SEND_POSTED:
					{
						pIOCPModel->HandleWrite(pSession, dwBytesTransfered);
					}
					break;
					default:
						// 不应该执行到这里
						log_dbg("_WorkThread中的 pIoContext->m_OpType 参数异常.\n");
						break;
					}
				}
			}
			
		}
	}
	log_dbg("工作者线程 %d 号退出.\n",nThreadNo);
	// 释放线程参数
	RELEASE(lpParam);	
	return 0;
}



//====================================================================================
//
//				    系统初始化和终止
//
//====================================================================================



////////////////////////////////////////////////////////////////////
// 初始化WinSock 2.2
bool CIOCPModel::LoadSocketLib()
{    
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// 错误(一般都不可能出现)
	if (NO_ERROR != nResult)
	{
		log_waring("WSAStartup failed");
		return false; 
	}
	return true;
}
bool CIOCPModel::Init(const std::string& sIp, int nPort, IOMgr* ioEvent)
{
	m_strIP = sIp;
	m_nPort = nPort;
	m_NetMgr = ioEvent;
	return true;
}
//////////////////////////////////////////////////////////////////
//	启动服务器
bool CIOCPModel::Start()
{
	// 建立系统退出的事件通知
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// 初始化IOCP
	if (false == CreateIOCP())
	{
		log_waring("初始化IOCP失败！");
		return false;
	}

	// 初始化Socket
	if( false== InitListenSocket() )
	{
		log_waring("Listen Socket初始化失败！");
		this->Release();
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////
//	开始发送系统退出消息，退出完成端口和线程资源
void CIOCPModel::Stop()
{
	if(m_ListenSocket !=INVALID_SOCKET )
	{
		// 激活关闭消息通知
		SetEvent(m_hShutdownEvent);

		for (int i = 0; i < m_nThreads; i++)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}

		// 等待所有的客户端资源退出
		WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

		// 清除客户端列表信息
		m_NetMgr->clearAllConnect();

		// 释放其他资源
		this->Release();
		log_info("停止监听");
	}	
}


////////////////////////////////
// 初始化完成端口
bool CIOCPModel::CreateIOCP()
{
	// 建立第一个完成端口
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == m_hIOCompletionPort)
	{
		log_waring("建立完成端口失败！错误代码: %d!", WSAGetLastError());
		return false;
	}

	// 根据本机中的处理器数量，建立对应的线程数
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_nThreads = WORKER_THREADS_PER_PROCESSOR * si.dwNumberOfProcessors;
	
	// 为工作者线程初始化句柄
	m_phWorkerThreads = new HANDLE[m_nThreads];
	
	// 根据计算出来的数量建立工作者线程
	DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		WokerThreadParam* pThreadParams = new WokerThreadParam;
		pThreadParams->pIOCPModel = this;
		pThreadParams->nThreadNo  = i+1;
		m_phWorkerThreads[i] = ::CreateThread(0, 0, _WorkerThread, (void *)pThreadParams, 0, &nThreadID);
	}
	log_dbg(" 建立 _WorkerThread %d 个.\n", m_nThreads);

	return true;
}


/////////////////////////////////////////////////////////////////
// 初始化Socket
bool CIOCPModel::InitListenSocket()
{
	// AcceptEx 和 GetAcceptExSockaddrs 的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// 服务器地址信息，用于绑定Socket
	struct sockaddr_in ServerAddress;


	// 需要使用重叠IO，必须得使用WSASocket来建立Socket，才可以支持重叠IO操作
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_ListenSocket)
	{
		log_waring("初始化Socket失败，错误代码: %d.\n", WSAGetLastError());
		return false;
	}

	// 将Listen Socket绑定至完成端口中
	if( NULL== CreateIoCompletionPort( (HANDLE)m_ListenSocket, m_hIOCompletionPort,(DWORD)m_ListenSocket, 0))
	{  
		log_waring("绑定 Listen Socket至完成端口失败！错误代码: %d/n", WSAGetLastError());
		RELEASE_SOCKET(m_ListenSocket);
		return false;
	}

	// 填充地址信息
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// 这里可以绑定任何可用的IP地址，或者绑定一个指定的IP地址 
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.c_str());         
	ServerAddress.sin_port = htons(m_nPort);                          

	// 绑定地址和端口
	if (SOCKET_ERROR == bind(m_ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		log_waring("bind()函数执行错误.");
		return false;
	}

	// 开始进行监听
	if (SOCKET_ERROR == listen(m_ListenSocket,SOMAXCONN))
	{
		log_waring("Listen()函数执行出现错误.\n");
		return false;
	}


	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针，
	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;  
	if(SOCKET_ERROR == WSAIoctl(
		m_ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx), 
		&m_lpfnAcceptEx, 
		sizeof(m_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		log_waring("WSAIoctl 未能获取AcceptEx函数指针。错误代码: %d\n", WSAGetLastError());
		this->Release();
		return false;  
	}  

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if(SOCKET_ERROR == WSAIoctl(
		m_ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs), 
		&m_lpfnGetAcceptExSockAddrs, 
		sizeof(m_lpfnGetAcceptExSockAddrs),   
		&dwBytes, 
		NULL, 
		NULL))  
	{  
		log_waring("WSAIoctl 未能获取GuidGetAcceptExSockAddrs函数指针。错误代码: %d\n", WSAGetLastError());
		this->Release();
		return false; 
	}  


	// 为AcceptEx 准备参数，然后投递AcceptEx I/O请求
	for( int i=0;i<MAX_POST_ACCEPT;i++ )
	{
		// 新建一个IO_CONTEXT
		PER_IO_CONTEXT* pAcceptIoContext = new _PER_IO_CONTEXT();
		if( this->PostAccept(pAcceptIoContext))
		{
			m_AcceptIoContext.push_back(pAcceptIoContext);
		}else {
			delete pAcceptIoContext;
			return false;
		}
	}

	log_dbg("投递 %d 个AcceptEx请求完毕",MAX_POST_ACCEPT );
	return true;
}

////////////////////////////////////////////////////////////
//	最后释放掉所有资源
void CIOCPModel::Release()
{
	// 关闭系统退出事件句柄
	RELEASE_HANDLE(m_hShutdownEvent);

	// 释放工作者线程句柄指针
	for( int i=0;i<m_nThreads;i++ )
	{
		RELEASE_HANDLE(m_phWorkerThreads[i]);
	}
	
	RELEASE(m_phWorkerThreads);

	// 关闭IOCP句柄
	RELEASE_HANDLE(m_hIOCompletionPort);

	// 关闭监听Socket
	RELEASE_SOCKET(m_ListenSocket);
	for (auto&t : m_AcceptIoContext)
	{
		delete t;
	}
	m_AcceptIoContext.clear();

	log_info("释放资源完毕.\n");
}


//====================================================================================
//
//				    投递完成端口请求
//
//====================================================================================


//////////////////////////////////////////////////////////////////
// 投递Accept请求
bool CIOCPModel::PostAccept( PER_IO_CONTEXT* pAcceptIoContext )
{
	Assert( INVALID_SOCKET!= m_ListenSocket);

	// 准备参数
	DWORD dwBytes = 0;  
	pAcceptIoContext->m_OpType = ACCEPT_POSTED;  
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptIoContext->m_Overlapped;
	
	// 为以后新连入的客户端先准备好Socket( 这个是与传统accept最大的区别 ) 
	pAcceptIoContext->m_sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);  
	if( INVALID_SOCKET==pAcceptIoContext->m_sockAccept )  
	{  
		log_waring("创建用于Accept的Socket失败！错误代码: %d", WSAGetLastError()); 
		return false;  
	} 

	// 投递AcceptEx
	if(FALSE == m_lpfnAcceptEx(m_ListenSocket, pAcceptIoContext->m_sockAccept, p_wbuf->buf, 0,
								sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol))  
	{  
		if(WSA_IO_PENDING != WSAGetLastError())  
		{  
			log_waring("投递 AcceptEx 请求失败，错误代码: %d", WSAGetLastError());
			return false;  
		}  
	} 

	return true;
}

////////////////////////////////////////////////////////////
// 在有客户端连入的时候，进行处理
// 流程有点复杂，你要是看不懂的话，就看配套的文档吧....
// 如果能理解这里的话，完成端口的机制你就消化了一大半了

// 总之你要知道，传入的是ListenSocket的Context，我们需要复制一份出来给新连入的Socket用
// 原来的Context还是要在上面继续投递下一个Accept请求
//
bool CIOCPModel::HandleAccepet(PER_IO_CONTEXT* pIoContext )
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

	///////////////////////////////////////////////////////////////////////////
	// 1. 首先取得连入客户端的地址信息
	// 这个 m_lpfnGetAcceptExSockAddrs 不得了啊~~~~~~
	// 不但可以取得客户端和本地端的地址信息，还能顺便取出客户端发来的第一组数据，老强大了...
	this->m_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, 0,  
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);  

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. 这里需要注意，这里传入的这个是ListenSocket上的Context，这个Context我们还需要用于监听下一个连接
	// 所以我还得要将ListenSocket上的Context复制出来一份为新连入的Socket新建一个SocketContext

	IOSession* pSession = m_NetMgr->newSession();
	uint64 sessionId = m_NetMgr->newSessionId();
	pSession->init(sessionId,pIoContext->m_sockAccept, ClientAddr);
	
	// 参数设置完毕，将这个Socket和完成端口绑定(这也是一个关键步骤)
	if( false==this->_AssociateWithIOCP((HANDLE)pIoContext->m_sockAccept, sessionId))
	{
		m_NetMgr->delSession(pSession);
		return false;
	}  

	// 绑定完毕之后，就可以开始在这个Socket上投递完成请求了
	if (false == pSession->PostRead())
	{
		m_NetMgr->delSession(pSession);
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. 如果投递成功，那么就把这个有效的客户端信息，加入到ContextList中去(需要统一管理，方便释放资源)
	m_NetMgr->accept(pSession);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. 使用完毕之后，把Listen Socket的那个IoContext重置，然后准备投递新的AcceptEx
	//pIoContext->ResetBuffer();
	return this->PostAccept( pIoContext ); 	
}

////////////////////////////////////////////////////////////////////
// 投递接收数据请求
bool CIOCPModel::PostRead(IOSession* pSession)
{
	PER_IO_CONTEXT* pIoContext = &(pSession->m_OverlappedRead);
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	size_t tailSize = 0;
	int8* tailData = (int8*)pSession->m_RecvBuffer.GetTailSpaceData(tailSize);
	p_wbuf->buf = tailData;
	p_wbuf->len = tailSize;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pSession->m_IsReading = true;
	// 初始化完成后，，投递WSARecv请求
	int nBytesRecv = WSARecv( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		log_dbg("投递第一个WSARecv失败！");
		return false;
	}

	return true;
}
bool CIOCPModel::PostSend(IOSession* pSession)
{
	PER_IO_CONTEXT* pIoContext = &(pSession->m_OverlappedWrite);
	// 初始化变量
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
	size_t writeSize = 0;
	int8* writeData = (int8*)(int8*)pSession->m_SendBuffer.GetTailUnreadData(writeSize);
	p_wbuf->buf = writeData;
	p_wbuf->len = writeSize;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	// 初始化完成后，，投递WSASend请求
	int nBytesWrite = WSASend(pSession->m_Socket, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL);

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesWrite) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		log_dbg("投递第一个WSASend失败！");
		return false;
	}

	return true;
}
void CIOCPModel::PostClose(ULONG_PTR key)
{
	PostQueuedCompletionStatus(m_hIOCompletionPort, 0, key,NULL);
}
/////////////////////////////////////////////////////////////////
// 在有接收的数据到达的时候，进行处理
bool CIOCPModel::HandleRead(IOSession *pSession, std::size_t bytes_transferred)
{
	if (pSession != nullptr && bytes_transferred > 0)
	{
		pSession->m_RecvBuffer.WriteComplete(bytes_transferred);
		if (!pSession->ParsePacketBuffer())
		{
			pSession->m_IsReading = false;
			PostClose(pSession->getSessionId());
			return false;
		}
		return pSession->PostRead();
	}
	return true;
}

bool CIOCPModel::HandleWrite(IOSession *pSession, std::size_t bytes_transferred)
{
	pSession->m_SendBuffer.ReadComplete(bytes_transferred);
	pSession->m_IsSending.Set(false);
	pSession->PostWrite();
	return true;
}

/////////////////////////////////////////////////////
// 将句柄(Socket)绑定到完成端口中
bool CIOCPModel::_AssociateWithIOCP(HANDLE handle, ULONG_PTR key)
{
	// 将用于和客户端通信的SOCKET绑定到完成端口中
	HANDLE hTemp = CreateIoCompletionPort(handle, m_hIOCompletionPort, key, 0);

	if (NULL == hTemp)
	{
		log_dbg(("执行CreateIoCompletionPort()出现错误.错误代码：%d"),GetLastError());
		return false;
	}

	return true;
}




//====================================================================================
//
//				    ContextList 相关操作
//
//====================================================================================


//====================================================================================
//
//				       其他辅助函数定义
//
//====================================================================================



/////////////////////////////////////////////////////////////////////
// 判断客户端Socket是否已经断开，否则在一个无效的Socket上投递WSARecv操作会出现异常
// 使用的方法是尝试向这个socket发送数据，判断这个socket调用的返回值
// 因为如果客户端网络异常断开(例如客户端崩溃或者拔掉网线等)的时候，服务器端是无法收到客户端断开的通知的

bool CIOCPModel::_IsSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);
	if (-1 == nByteSent) return false;
	return true;
}







