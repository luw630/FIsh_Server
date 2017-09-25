#include "SessionMemoryDb.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Server.pb.h"
#include "WfCommonPackets.h"
#include "Packets/Packet_Login.pb.h"
#include <json/json.h>
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
		
	}
	else if (packet->m_Command == emMemoryDbPacket_OutDataRst) {
		MemoryDbPacket_OutDataRst msg;
		msg.DeSerialize(stream);
		
		std::string accounts = "";
		bool rst = false;
		do {
			Json::Value root;
			Json::Reader jsonReader;
			bool ok = jsonReader.parse(msg.m_Data, root);

			if (!ok) break;

			accounts = root.get("account", "").asString();
			rst = true;
		} while (false);


		uint64 sessionId = TypeConvert::StrToUint64(msg.m_callParam);
		if (!rst) {
			rpc::SC_LoginGateWayRst rsp;
			rsp.set_m_rst(1);
			
			App::GetInstance().getListenerClient()->SendPacket(sessionId, rpc::EM_SC_LoginRst, &rsp);
			App::GetInstance().getListenerClient()->Close(sessionId);
			return 0;
		}
		else {
			//Í¨ÖªAccount£¬Íæ¼ÒµÇÂ¼
			rpc::GA_Login req;
			req.set_m_account(accounts);
			req.set_m_param(sessionId);
			App::GetInstance().getSessionAccount()->SendPacket(1, rpc::EM_GA_Login, &req);
		}
	}
	return 0;
}
void SessionMemoryDb::OnConnected()
{

}
void SessionMemoryDb::OnDisconnected()
{
	ConnectionSession::OnDisconnected();
}
NS_WF_END
