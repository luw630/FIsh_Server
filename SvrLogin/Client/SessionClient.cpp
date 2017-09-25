#include "WfLog.h"
#include "SessionClient.h"
#include "SessionClientMgr.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_Fish.pb.h"
NS_WF_BEGIN

SessionClient::SessionClient()
{

}
SessionClient::~SessionClient()
{

}
void SessionClient::OnInit()
{
	
}void SessionClient::OnPacket(Wf::abs* newPacket, PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == rpc::EM_CS_Login)
	{
		rpc::CS_Login msg;
		if (msg.m_type() == 0) {
			App::GetInstance().m_SeesionLoginDb->addBuffer(newPacket,SysUtil::getUint32(msg.m_account().c_str(), msg.m_account().size()));
		}
		else {
			log_waring("login type(%d) error,need 0", msg.m_type());
			newPacket->release();
		}
	}	
}
void SessionClient::OnConnected()
{

}
void SessionClient::OnDisconnected()
{

}
NS_WF_END
