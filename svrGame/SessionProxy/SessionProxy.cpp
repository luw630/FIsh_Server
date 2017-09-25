#include "SessionProxy.h"
#include "WfLog.h"
#include "WfSysUtil.h"
#include "WfPacketPool.h"
#include "WfSessionType.h"
#include "App/App.h"
#include "Packets/Packet_Server.pb.h"
#include "WfCommonPackets.h"
#include "Packets/Packet_DB.pb.h"
NS_WF_BEGIN


bool SessionProxyMgr::Init()
{
	VerifyConnectorSessionMgrConfigReader reader;
	if (!reader.Load(Cfg_ProxyConnector))
	{
		log_err("confg error");
		return false;
	}
	vector<VerifyConnectorSessionConfig>::iterator iter = reader.list.begin();
	for (; iter != reader.list.end(); ++iter)
	{
		VerifyConnectorSessionConfig& cfg = *iter;
		SessionProxy* session = new SessionProxy();

		if (INVALID_SESSIONID == m_NetMgr->AddConnection(cfg.id, cfg.opt, session))
		{
			log_err("connect error:ip=%s port=%d\n", cfg.opt.m_IP.c_str(), cfg.opt.m_Port);
			WF_SAFE_DELETE(session);
			return false;
		}
		m_session = session;
		return true;
	}
	return false;
}
SessionProxy::SessionProxy()
	: ISessionProxy(&App::GetInstance(), ESessionType_Client)
{

}
SessionProxy::~SessionProxy()
{

}

int SessionProxy::OnPacket(PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == EInnerPacketCommand_Ping)
	{
		PacketPing* ping = (PacketPing*)packet;
		OnPing(ping->m_Tick);
	}
	else if (packet->m_Command == EInnerPacketCommand_Pong)
	{
		PacketPong* ping = (PacketPong*)packet;
		OnPong(ping->m_Tick);
	}else {
		CProxyHead proxyHead;
		proxyHead.DeSerialize(stream);
		int32 msgParamLen = 0;
		uint8* msgParam = nullptr;
		stream >> msgParamLen;
		if (msgParamLen > 0) {
			stream.BytesDeserialize(msgParam, msgParamLen);
		}

		if (proxyHead.m_srcType == emSvrType_Account)
		{
			App::GetInstance().getGame()->OnAccountPacket(proxyHead.m_srcId,packet, Stream(stream.GetOffsetPointer(), stream.GetSize() - stream.GetOffset()), msgParamLen, msgParam);
		}
	}
	return 0;
}

void SessionProxy::OnConnected()
{
	PacketConProxy stConnServerInfo;
	stConnServerInfo.m_entityType = emSvrType_Game;
	stConnServerInfo.m_nServerID = App::GetInstance().GetAppId();
	SendPacket(&stConnServerInfo);
}
void SessionProxy::OnDisconnected()
{
	ConnectionSession::OnDisconnected();
}
NS_WF_END
