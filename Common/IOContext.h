#pragma once
#include "WfConfig.h"
#include "WfRWBuffer.h"
USING_NS_WF;
// ���������� (1024*8)
// ֮����Ϊʲô����8K��Ҳ��һ�������ϵľ���ֵ
// ���ȷʵ�ͻ��˷�����ÿ�����ݶ��Ƚ��٣���ô�����õ�СһЩ��ʡ�ڴ�
#define MAX_BUFFER_LEN        8192  


//////////////////////////////////////////////////////////////////
// ����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // ��־Ͷ�ݵ�Accept����
	SEND_POSTED,                       // ��־Ͷ�ݵ��Ƿ��Ͳ���
	RECV_POSTED,                       // ��־Ͷ�ݵ��ǽ��ղ���
	NULL_POSTED                        // ���ڳ�ʼ����������

}OPERATION_TYPE;

typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED		m_Overlapped;		// ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
	SOCKET         m_sockAccept;		// ������������ʹ�õ�Socket
	WSABUF         m_wsaBuf;			// WSA���͵Ļ����������ڸ��ص�������������
	char           m_szBuffer[64];		// �����WSABUF�������ַ��Ļ�����
	OPERATION_TYPE m_OpType;			// ��ʶ�������������(��Ӧ�����ö��)
	void*		   m_pSession;
	// ��ʼ��
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		m_sockAccept = INVALID_SOCKET;
		ZeroMemory(m_szBuffer, 64);
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = 64;
		m_OpType     = NULL_POSTED;
	}
	// �ͷŵ�Socket
	~_PER_IO_CONTEXT()
	{
		if( m_sockAccept!=INVALID_SOCKET )
		{
			//closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
		m_pSession = nullptr;
	}
} PER_IO_CONTEXT, *PPER_IO_CONTEXT;