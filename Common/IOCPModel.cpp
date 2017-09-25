
#include "IOCPModel.h"
#include "WfLog.h"
USING_NS_WF;

// ÿһ���������ϲ������ٸ��߳�(Ϊ������޶ȵ��������������ܣ���������ĵ�)
#define WORKER_THREADS_PER_PROCESSOR 2
// ͬʱͶ�ݵ�Accept���������(���Ҫ����ʵ�ʵ�����������)
#define MAX_POST_ACCEPT              10
// ���ݸ�Worker�̵߳��˳��ź�
#define EXIT_CODE                    NULL


// �ͷ�ָ��;����Դ�ĺ�

// �ͷ�ָ���
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
// �ͷž����
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
// �ͷ�Socket��
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
	// ȷ����Դ�����ͷ�
	this->Stop();
}

struct WokerThreadParam
{
	CIOCPModel* pIOCPModel;		// ��ָ�룬���ڵ������еĺ���
	int         nThreadNo;		// �̱߳��
};


///////////////////////////////////////////////////////////////////
// �������̣߳�  ΪIOCP�������Ĺ������߳�
//         Ҳ����ÿ����ɶ˿��ϳ�����������ݰ����ͽ�֮ȡ�������д�����߳�
///////////////////////////////////////////////////////////////////

DWORD WINAPI CIOCPModel::_WorkerThread(LPVOID lpParam)
{    
	WokerThreadParam* pParam = (WokerThreadParam*)lpParam;
	CIOCPModel* pIOCPModel = (CIOCPModel*)pParam->pIOCPModel;
	int nThreadNo = (int)pParam->nThreadNo;

	log_info("�������߳�������ID: %d.", nThreadNo);

	OVERLAPPED           *pOverlapped = NULL;
	ULONG_PTR			 key = 0;
	DWORD                dwBytesTransfered = 0;

	// ѭ����������֪�����յ�Shutdown��ϢΪֹ
	while (WAIT_OBJECT_0 != WaitForSingleObject(pIOCPModel->m_hShutdownEvent, 0))
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			pIOCPModel->m_hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&key,
			&pOverlapped,
			INFINITE);


		PER_IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);

		// �ж��Ƿ�����˴���
		if( !bReturn )  
		{  
			DWORD dwErr = GetLastError();
			do {
				if (WAIT_TIMEOUT == dwErr) {
					IOSession *pSession = (IOSession*)pIoContext->m_pSession;
					pIOCPModel->PostClose(key);
				}
				else if (ERROR_NETNAME_DELETED == dwErr) {
					log_dbg("��⵽�ͻ����쳣�˳���");
					IOSession *pSession = (IOSession*)pIoContext->m_pSession;
					pIOCPModel->PostClose(key);
					return true;
				}
				else {
					log_err("��ɶ˿ڲ������ִ����߳��˳���������룺%d", dwErr);
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
				// �ж��Ƿ��пͻ��˶Ͽ���
				if ((0 == dwBytesTransfered) && (RECV_POSTED == pIoContext->m_OpType || SEND_POSTED == pIoContext->m_OpType))
				{
					//log_waring("�ͻ��� %s:%d �Ͽ�����.����˹رգ�%s��",inet_ntoa(pSession->m_ClientAddr.sin_addr), ntohs(pSession->m_ClientAddr.sin_port));
					pIOCPModel->m_NetMgr->rmvSession(key);
					continue;
				}
				else
				{
					// ��ȡ����Ĳ���
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
						// ��Ӧ��ִ�е�����
						log_dbg("_WorkThread�е� pIoContext->m_OpType �����쳣.\n");
						break;
					}
				}
			}
			
		}
	}
	log_dbg("�������߳� %d ���˳�.\n",nThreadNo);
	// �ͷ��̲߳���
	RELEASE(lpParam);	
	return 0;
}



//====================================================================================
//
//				    ϵͳ��ʼ������ֹ
//
//====================================================================================



////////////////////////////////////////////////////////////////////
// ��ʼ��WinSock 2.2
bool CIOCPModel::LoadSocketLib()
{    
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	// ����(һ�㶼�����ܳ���)
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
//	����������
bool CIOCPModel::Start()
{
	// ����ϵͳ�˳����¼�֪ͨ
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// ��ʼ��IOCP
	if (false == CreateIOCP())
	{
		log_waring("��ʼ��IOCPʧ�ܣ�");
		return false;
	}

	// ��ʼ��Socket
	if( false== InitListenSocket() )
	{
		log_waring("Listen Socket��ʼ��ʧ�ܣ�");
		this->Release();
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////
//	��ʼ����ϵͳ�˳���Ϣ���˳���ɶ˿ں��߳���Դ
void CIOCPModel::Stop()
{
	if(m_ListenSocket !=INVALID_SOCKET )
	{
		// ����ر���Ϣ֪ͨ
		SetEvent(m_hShutdownEvent);

		for (int i = 0; i < m_nThreads; i++)
		{
			// ֪ͨ���е���ɶ˿ڲ����˳�
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
		}

		// �ȴ����еĿͻ�����Դ�˳�
		WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);

		// ����ͻ����б���Ϣ
		m_NetMgr->clearAllConnect();

		// �ͷ�������Դ
		this->Release();
		log_info("ֹͣ����");
	}	
}


////////////////////////////////
// ��ʼ����ɶ˿�
bool CIOCPModel::CreateIOCP()
{
	// ������һ����ɶ˿�
	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if ( NULL == m_hIOCompletionPort)
	{
		log_waring("������ɶ˿�ʧ�ܣ��������: %d!", WSAGetLastError());
		return false;
	}

	// ���ݱ����еĴ�����������������Ӧ���߳���
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_nThreads = WORKER_THREADS_PER_PROCESSOR * si.dwNumberOfProcessors;
	
	// Ϊ�������̳߳�ʼ�����
	m_phWorkerThreads = new HANDLE[m_nThreads];
	
	// ���ݼ�����������������������߳�
	DWORD nThreadID;
	for (int i = 0; i < m_nThreads; i++)
	{
		WokerThreadParam* pThreadParams = new WokerThreadParam;
		pThreadParams->pIOCPModel = this;
		pThreadParams->nThreadNo  = i+1;
		m_phWorkerThreads[i] = ::CreateThread(0, 0, _WorkerThread, (void *)pThreadParams, 0, &nThreadID);
	}
	log_dbg(" ���� _WorkerThread %d ��.\n", m_nThreads);

	return true;
}


/////////////////////////////////////////////////////////////////
// ��ʼ��Socket
bool CIOCPModel::InitListenSocket()
{
	// AcceptEx �� GetAcceptExSockaddrs ��GUID�����ڵ�������ָ��
	GUID GuidAcceptEx = WSAID_ACCEPTEX;  
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// ��������ַ��Ϣ�����ڰ�Socket
	struct sockaddr_in ServerAddress;


	// ��Ҫʹ���ص�IO�������ʹ��WSASocket������Socket���ſ���֧���ص�IO����
	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_ListenSocket)
	{
		log_waring("��ʼ��Socketʧ�ܣ��������: %d.\n", WSAGetLastError());
		return false;
	}

	// ��Listen Socket������ɶ˿���
	if( NULL== CreateIoCompletionPort( (HANDLE)m_ListenSocket, m_hIOCompletionPort,(DWORD)m_ListenSocket, 0))
	{  
		log_waring("�� Listen Socket����ɶ˿�ʧ�ܣ��������: %d/n", WSAGetLastError());
		RELEASE_SOCKET(m_ListenSocket);
		return false;
	}

	// ����ַ��Ϣ
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	// ������԰��κο��õ�IP��ַ�����߰�һ��ָ����IP��ַ 
	//ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);                      
	ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.c_str());         
	ServerAddress.sin_port = htons(m_nPort);                          

	// �󶨵�ַ�Ͷ˿�
	if (SOCKET_ERROR == bind(m_ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		log_waring("bind()����ִ�д���.");
		return false;
	}

	// ��ʼ���м���
	if (SOCKET_ERROR == listen(m_ListenSocket,SOMAXCONN))
	{
		log_waring("Listen()����ִ�г��ִ���.\n");
		return false;
	}


	// ʹ��AcceptEx��������Ϊ���������WinSock2�淶֮���΢�������ṩ����չ����
	// ������Ҫ�����ȡһ�º�����ָ�룬
	// ��ȡAcceptEx����ָ��
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
		log_waring("WSAIoctl δ�ܻ�ȡAcceptEx����ָ�롣�������: %d\n", WSAGetLastError());
		this->Release();
		return false;  
	}  

	// ��ȡGetAcceptExSockAddrs����ָ�룬Ҳ��ͬ��
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
		log_waring("WSAIoctl δ�ܻ�ȡGuidGetAcceptExSockAddrs����ָ�롣�������: %d\n", WSAGetLastError());
		this->Release();
		return false; 
	}  


	// ΪAcceptEx ׼��������Ȼ��Ͷ��AcceptEx I/O����
	for( int i=0;i<MAX_POST_ACCEPT;i++ )
	{
		// �½�һ��IO_CONTEXT
		PER_IO_CONTEXT* pAcceptIoContext = new _PER_IO_CONTEXT();
		if( this->PostAccept(pAcceptIoContext))
		{
			m_AcceptIoContext.push_back(pAcceptIoContext);
		}else {
			delete pAcceptIoContext;
			return false;
		}
	}

	log_dbg("Ͷ�� %d ��AcceptEx�������",MAX_POST_ACCEPT );
	return true;
}

////////////////////////////////////////////////////////////
//	����ͷŵ�������Դ
void CIOCPModel::Release()
{
	// �ر�ϵͳ�˳��¼����
	RELEASE_HANDLE(m_hShutdownEvent);

	// �ͷŹ������߳̾��ָ��
	for( int i=0;i<m_nThreads;i++ )
	{
		RELEASE_HANDLE(m_phWorkerThreads[i]);
	}
	
	RELEASE(m_phWorkerThreads);

	// �ر�IOCP���
	RELEASE_HANDLE(m_hIOCompletionPort);

	// �رռ���Socket
	RELEASE_SOCKET(m_ListenSocket);
	for (auto&t : m_AcceptIoContext)
	{
		delete t;
	}
	m_AcceptIoContext.clear();

	log_info("�ͷ���Դ���.\n");
}


//====================================================================================
//
//				    Ͷ����ɶ˿�����
//
//====================================================================================


//////////////////////////////////////////////////////////////////
// Ͷ��Accept����
bool CIOCPModel::PostAccept( PER_IO_CONTEXT* pAcceptIoContext )
{
	Assert( INVALID_SOCKET!= m_ListenSocket);

	// ׼������
	DWORD dwBytes = 0;  
	pAcceptIoContext->m_OpType = ACCEPT_POSTED;  
	WSABUF *p_wbuf   = &pAcceptIoContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptIoContext->m_Overlapped;
	
	// Ϊ�Ժ�������Ŀͻ�����׼����Socket( ������봫ͳaccept�������� ) 
	pAcceptIoContext->m_sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);  
	if( INVALID_SOCKET==pAcceptIoContext->m_sockAccept )  
	{  
		log_waring("��������Accept��Socketʧ�ܣ��������: %d", WSAGetLastError()); 
		return false;  
	} 

	// Ͷ��AcceptEx
	if(FALSE == m_lpfnAcceptEx(m_ListenSocket, pAcceptIoContext->m_sockAccept, p_wbuf->buf, 0,
								sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, p_ol))  
	{  
		if(WSA_IO_PENDING != WSAGetLastError())  
		{  
			log_waring("Ͷ�� AcceptEx ����ʧ�ܣ��������: %d", WSAGetLastError());
			return false;  
		}  
	} 

	return true;
}

////////////////////////////////////////////////////////////
// ���пͻ��������ʱ�򣬽��д���
// �����е㸴�ӣ���Ҫ�ǿ������Ļ����Ϳ����׵��ĵ���....
// ������������Ļ�����ɶ˿ڵĻ������������һ�����

// ��֮��Ҫ֪�����������ListenSocket��Context��������Ҫ����һ�ݳ������������Socket��
// ԭ����Context����Ҫ���������Ͷ����һ��Accept����
//
bool CIOCPModel::HandleAccepet(PER_IO_CONTEXT* pIoContext )
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;  
	int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);  

	///////////////////////////////////////////////////////////////////////////
	// 1. ����ȡ������ͻ��˵ĵ�ַ��Ϣ
	// ��� m_lpfnGetAcceptExSockAddrs �����˰�~~~~~~
	// ��������ȡ�ÿͻ��˺ͱ��ض˵ĵ�ַ��Ϣ������˳��ȡ���ͻ��˷����ĵ�һ�����ݣ���ǿ����...
	this->m_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, 0,  
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);  

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. ������Ҫע�⣬���ﴫ��������ListenSocket�ϵ�Context�����Context���ǻ���Ҫ���ڼ�����һ������
	// �����һ���Ҫ��ListenSocket�ϵ�Context���Ƴ���һ��Ϊ�������Socket�½�һ��SocketContext

	IOSession* pSession = m_NetMgr->newSession();
	uint64 sessionId = m_NetMgr->newSessionId();
	pSession->init(sessionId,pIoContext->m_sockAccept, ClientAddr);
	
	// ����������ϣ������Socket����ɶ˿ڰ�(��Ҳ��һ���ؼ�����)
	if( false==this->_AssociateWithIOCP((HANDLE)pIoContext->m_sockAccept, sessionId))
	{
		m_NetMgr->delSession(pSession);
		return false;
	}  

	// �����֮�󣬾Ϳ��Կ�ʼ�����Socket��Ͷ�����������
	if (false == pSession->PostRead())
	{
		m_NetMgr->delSession(pSession);
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. ���Ͷ�ݳɹ�����ô�Ͱ������Ч�Ŀͻ�����Ϣ�����뵽ContextList��ȥ(��Ҫͳһ���������ͷ���Դ)
	m_NetMgr->accept(pSession);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 5. ʹ�����֮�󣬰�Listen Socket���Ǹ�IoContext���ã�Ȼ��׼��Ͷ���µ�AcceptEx
	//pIoContext->ResetBuffer();
	return this->PostAccept( pIoContext ); 	
}

////////////////////////////////////////////////////////////////////
// Ͷ�ݽ�����������
bool CIOCPModel::PostRead(IOSession* pSession)
{
	PER_IO_CONTEXT* pIoContext = &(pSession->m_OverlappedRead);
	// ��ʼ������
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf   = &pIoContext->m_wsaBuf;
	size_t tailSize = 0;
	int8* tailData = (int8*)pSession->m_RecvBuffer.GetTailSpaceData(tailSize);
	p_wbuf->buf = tailData;
	p_wbuf->len = tailSize;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	pSession->m_IsReading = true;
	// ��ʼ����ɺ󣬣�Ͷ��WSARecv����
	int nBytesRecv = WSARecv( pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

	// �������ֵ���󣬲��Ҵ���Ĵ��벢����Pending�Ļ����Ǿ�˵������ص�����ʧ����
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		log_dbg("Ͷ�ݵ�һ��WSARecvʧ�ܣ�");
		return false;
	}

	return true;
}
bool CIOCPModel::PostSend(IOSession* pSession)
{
	PER_IO_CONTEXT* pIoContext = &(pSession->m_OverlappedWrite);
	// ��ʼ������
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
	size_t writeSize = 0;
	int8* writeData = (int8*)(int8*)pSession->m_SendBuffer.GetTailUnreadData(writeSize);
	p_wbuf->buf = writeData;
	p_wbuf->len = writeSize;
	OVERLAPPED *p_ol = &pIoContext->m_Overlapped;

	// ��ʼ����ɺ󣬣�Ͷ��WSASend����
	int nBytesWrite = WSASend(pSession->m_Socket, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL);

	// �������ֵ���󣬲��Ҵ���Ĵ��벢����Pending�Ļ����Ǿ�˵������ص�����ʧ����
	if ((SOCKET_ERROR == nBytesWrite) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		log_dbg("Ͷ�ݵ�һ��WSASendʧ�ܣ�");
		return false;
	}

	return true;
}
void CIOCPModel::PostClose(ULONG_PTR key)
{
	PostQueuedCompletionStatus(m_hIOCompletionPort, 0, key,NULL);
}
/////////////////////////////////////////////////////////////////
// ���н��յ����ݵ����ʱ�򣬽��д���
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
// �����(Socket)�󶨵���ɶ˿���
bool CIOCPModel::_AssociateWithIOCP(HANDLE handle, ULONG_PTR key)
{
	// �����ںͿͻ���ͨ�ŵ�SOCKET�󶨵���ɶ˿���
	HANDLE hTemp = CreateIoCompletionPort(handle, m_hIOCompletionPort, key, 0);

	if (NULL == hTemp)
	{
		log_dbg(("ִ��CreateIoCompletionPort()���ִ���.������룺%d"),GetLastError());
		return false;
	}

	return true;
}




//====================================================================================
//
//				    ContextList ��ز���
//
//====================================================================================


//====================================================================================
//
//				       ����������������
//
//====================================================================================



/////////////////////////////////////////////////////////////////////
// �жϿͻ���Socket�Ƿ��Ѿ��Ͽ���������һ����Ч��Socket��Ͷ��WSARecv����������쳣
// ʹ�õķ����ǳ��������socket�������ݣ��ж����socket���õķ���ֵ
// ��Ϊ����ͻ��������쳣�Ͽ�(����ͻ��˱������߰ε����ߵ�)��ʱ�򣬷����������޷��յ��ͻ��˶Ͽ���֪ͨ��

bool CIOCPModel::_IsSocketAlive(SOCKET s)
{
	int nByteSent=send(s,"",0,0);
	if (-1 == nByteSent) return false;
	return true;
}







