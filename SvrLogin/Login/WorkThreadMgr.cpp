#include "WorkThreadMgr.h"
#include "WfPacketPool.h"

NS_WF_BEGIN

WorkThreadMgr::WorkThreadMgr()
{
}


WorkThreadMgr::~WorkThreadMgr()
{
}
bool WorkThreadMgr::Intial(uint32 nSize)
{
	m_ThreadNum = nSize;
	bool ok = true;

	m_pWorkThread = new WorkThread*[nSize];
	for (int i = 0; i < m_ThreadNum; i++)
	{
		m_pWorkThread[i] = new WorkThread();
		m_pWorkThread[i]->start();
	}
	return ok;
}
void WorkThreadMgr::addUnHandlePacket(int64 sessionId, PacketHeader* pHeader)
{
	WorkThread* pWorkThread = getWorkThread(sessionId);
	pWorkThread->addUnHandlePacket(sessionId, pHeader);
}
void WorkThreadMgr::addFinishPacket(int64 sessionId, PacketHeader* pHeader)
{

}
void WorkThreadMgr::update(int64 elapse)
{
	for (int i = 0; i < m_ThreadNum; i++)
	{
		int readNum = 0;
		WorkThread* pDBWork = m_pWorkThread[i];
		PacketBuffer* pBuffer = pDBWork->GetHandlePacketBuffer();
		while (pBuffer && readNum <= 20)
		{
			++readNum;
			//int64 sessionId = *(int64*)pBuffer->getHeader();
			//Server::GetInstance()->SendPacket(sessionId, pBuffer->getHeader() + sizeof(int64), pBuffer->getDataLen() - sizeof(int64));
			//PacketPool::GetInstance()->DelPacket(pBuffer);
			//pBuffer = pDBWork->GetHandlePacketBuffer();
		}
	}
}
WorkThread* WorkThreadMgr::getWorkThread(uint64 sessionId)
{
	return m_pWorkThread[sessionId%m_ThreadNum];
}
int WorkThreadMgr::OnHandle_C2S_Login(WorkThread* pWorkThread, int64 sessionId, PacketHeader* pHeader, Stream& stream)
{
	return 0;
}
NS_WF_END