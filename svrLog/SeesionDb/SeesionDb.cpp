#include "SeesionDb.h"
#include "WfLog.h"
#include "App/App.h"
#include "Packets/Packet_DBLog.pb.h"
#include <google/protobuf/message.h>


using namespace rpc;

NS_WF_BEGIN
SeesionDb::SeesionDb() :IWorkMgr(&App::GetInstance())
{

}


SeesionDb::~SeesionDb()
{
}
bool SeesionDb::OnIntial()
{
	Register(em_Log_PlayerLogin, OnHandle_Log_PlayerLogin);
	Register(em_Log_PlayerQuit, OnHandle_Log_PlayerQuit);
	Register(em_Log_EnterFishGame, OnHandle_Log_EnterFishGame);
	Register(em_Log_LeaveFishGame, OnHandle_Log_LeaveFishGame);
	Register(em_Log_EnterEnterTable, OnHandle_Log_EnterEnterTable);
	Register(em_Log_EnterLeaveTable, OnHandle_Log_EnterLeaveTable);
	Register(em_Log_BattleSendBullet, OnHandle_Log_BattleSendBullet);
	Register(em_Log_BattleKillFish, OnHandle_Log_BattleKillFish);
	
	return true;
}
void SeesionDb::OnHandlePacket(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	TaskPacketHandler handle = GetPacketHandler(pHeader->m_Command);
	if (handle){
		handle(pHandle, connectId, pHeader, stream);
	}
	else{
		log_err("error:can't  find ClientPacketHandler(0x%x)", pHeader->m_Command);
	}
}
void SeesionDb::OnPacket(t_sessionId connectId, PacketHeader* packet, Stream& stream)
{
	App::GetInstance().m_SessionProxyMgr->m_session->SendPacket(packet);
}
//玩家登录
void SeesionDb::OnHandle_Log_PlayerLogin(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	rpc::Log_PlayerLogin msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	MysqlConnection* pCon = pHandle->getConnection();

	QueryResult sqlResult;
	char sql[256] = { 0 };
	sprintf(sql, "INSERT INTO t_Log_PlayerLogin(m_playerid,m_time) VALUES ('%u','%u');", msg.playerid(),msg.time());
		
	QueryResult result;
	if (!pCon->ExecuteSQL(sql, &result)) {
		log_err("run sql(%s) error", sql);
		return;
	}
}
//玩家退出
void SeesionDb::OnHandle_Log_PlayerQuit(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	
}
//进入捕鱼游戏
void SeesionDb::OnHandle_Log_EnterFishGame(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
//离开捕鱼游戏
void SeesionDb::OnHandle_Log_LeaveFishGame(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
//进入座子
void SeesionDb::OnHandle_Log_EnterEnterTable(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
//离开座子	
void SeesionDb::OnHandle_Log_EnterLeaveTable(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
//发射子弹
void SeesionDb::OnHandle_Log_BattleSendBullet(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
//杀死鱼
void SeesionDb::OnHandle_Log_BattleKillFish(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}

NS_WF_END