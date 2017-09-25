#pragma once
#include "WfConfig.h"
#include "WfRWBuffer.h"
USING_NS_WF;
// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#define MAX_BUFFER_LEN        8192  


//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE  
{  
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;

typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED		m_Overlapped;		// 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET         m_sockAccept;		// 这个网络操作所使用的Socket
	WSABUF         m_wsaBuf;			// WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[64];		// 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE m_OpType;			// 标识网络操作的类型(对应上面的枚举)
	void*		   m_pSession;
	// 初始化
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));  
		m_sockAccept = INVALID_SOCKET;
		ZeroMemory(m_szBuffer, 64);
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = 64;
		m_OpType     = NULL_POSTED;
	}
	// 释放掉Socket
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