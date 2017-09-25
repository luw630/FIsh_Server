#include "SeesionDb.h"
#include "WfCommonPackets.h"
#include <json/json.h>
#include "WfHttpPost.h"
#include "WfLog.h"
#include "App/App.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_ErrCode.pb.h"
#include "WfGuid.h"

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
	Register(EM_CS_Login, OnHandle_PacketLogin_CS_Login);
	Register(EM_CS_Register, OnHandle_PacketLogin_CS_Register);
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
void SeesionDb::OnPacket(uint64 connectId, PacketHeader* packet, Stream& stream)
{
	if (connectId == INVALID_SESSIONID)
	{
		App::GetInstance().m_SessionMemoryDb->SendPacket(1, packet);
	}
}
void SeesionDb::OnPacket(PacketHeader* packet, Stream& stream)
{

}
void SeesionDb::OnHandle_PacketLogin_CS_Register(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	rpc::CS_Register msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	uint rst = 0;
	MysqlConnection* pCon = pHandle->getConnection();
	string sSessionId = "";
	do
	{
		char buff[128] = { 0 };
		sprintf(buff, "CALL pro_account_register('%s','%s');", msg.m_account().c_str(), msg.m_pwd().c_str());
		QueryResult result;
		if (!pCon->ExecuteSQL(buff, &result))
		{
			rst = Error_Unknow;
			log_err("operate db error(%s)", buff);
			break;
		}

		if (!result.FetchRow())
		{
			rst = -100;
			break;
		}
		int32 dbrst = TypeConvert::ToInt(result.GetValue(0));
		if (dbrst == -1) {
			//用户已被注册
			rst = -1;
			break;
		}

		result.Close();
	} while (false);

	SC_RegisterRst rsp;
	rsp.set_m_rst(rst);

	App::GetInstance().getSessionClientMgr()->SendPacket(connectId, rpc::EM_SC_RegisterRst, &rsp);
	App::GetInstance().getSessionClientMgr()->close(connectId);
}
void SeesionDb::OnHandle_PacketLogin_CS_Login(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	rpc::CS_Login msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	uint rst = 0;
	MysqlConnection* pCon = pHandle->getConnection();
	string sSessionId = "";
	do
	{
		char buff[128] = { 0 };
		sprintf(buff, "CALL pro_account_login('%s','%s');", msg.m_account().c_str(), msg.m_pwd().c_str());
		QueryResult result;
		if (!pCon->ExecuteSQL(buff, &result))
		{
			rst = Error_Unknow;
			log_err("operate db error(%s)", buff);
			break;
		}

		if (!result.FetchRow())
		{
			rst = -100;
			break;
		}
		int32 dbrst = TypeConvert::ToInt(result.GetValue(0));
		if (dbrst == -1) {
			//用户不存在
			rst = -1;
			break;
		}else if(dbrst == -2){
			//密码错误
			rst = -2;
			break;
		}

		result.Close();
		memset(buff, 0, 128);

		sprintf(buff, "CALL get_gameaccountid('platform_%s');", msg.m_account().c_str());
		if (!pCon->ExecuteSQL(buff, &result))
		{
			rst = -100;
			log_waring("operate db error(%s)", buff);
			break;
		}

		if (!result.FetchRow())
		{
			rst = -100;
			log_err("operate db error(%s)", buff);
			break;
		}
		
		
		int32 gameAccountid = TypeConvert::ToInt(result.GetValue(0));
		
		char data[128] = { 0 };
		sprintf(data, "{ \"account\":\"platform_%s\" }", msg.m_account().c_str());
		MemoryDbPacket_SetData setData;
		setData.m_key = Guid::CreateGuidStr();
		setData.m_Data = data;
		memset(data, 0, 128);
		sprintf(data, "%llu", connectId);
		setData.m_callParam = data;
		pHandle->SendPacket(emMemoryDbPacket_SetData, &setData);
	} while (false);
	
	if (rst != 0) {
		SC_LoginRst rsp;
		rsp.set_m_rst(rst);
		rsp.set_m_sessionid(sSessionId);
		App::GetInstance().getSessionClientMgr()->SendPacket(connectId, rpc::EM_SC_LoginRst, &rsp);
		App::GetInstance().getSessionClientMgr()->close(connectId);
	}
}
NS_WF_END