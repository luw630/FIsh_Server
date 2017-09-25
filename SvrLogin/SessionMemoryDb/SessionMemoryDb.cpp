#include "SessionMemoryDb.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Server.pb.h"
#include "WfCommonPackets.h"
#include "Packets/Packet_Login.pb.h"
NS_WF_BEGIN

SessionMemoryDb::SessionMemoryDb()
	: ConnectionSession(&App::GetInstance(), ESessionType_Client)
{

}
SessionMemoryDb::~SessionMemoryDb()
{

}

int SessionMemoryDb::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == emMemoryDbPacket_SetDataRst) {
		MemoryDbPacket_SetDataRst msg;
		msg.DeSerialize(stream);
		rpc::SC_LoginRst rsp;
		rsp.set_m_rst(0);
		rsp.set_m_sessionid(msg.m_key);
		uint64 sessionId = TypeConvert::StrToUint64(msg.m_callParam);
		App::GetInstance().getSessionClientMgr()->SendPacket(sessionId, rpc::EM_SC_LoginRst, &rsp);
		App::GetInstance().getSessionClientMgr()->close(sessionId);
	}
	else if (packet->m_Command == emMemoryDbPacket_OutDataRst) {
		
	}
	return 0;
}
void SessionMemoryDb::OnConnected()
{
	//rpc::Sys_SvrNodeLogin req;
	//req.set_m_nodeid(App::GetInstance().GetAppId());
	//req.set_m_key(NodeKey);
	//SendPacket(rpc::EM_Sys_SvrNodeLogin, &req);
	//log_dbg("send Sys_SvrNodeLogin to GameSvr");
}
void SessionMemoryDb::OnDisconnected()
{
	ConnectionSession::OnDisconnected();
}
NS_WF_END
