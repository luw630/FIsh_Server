#pragma once
#include "WfNetConfig.h"
#include "WfConnection.h"
#include "WfConnectorMgr.h"
#include "SessionGateway/SessionGateway.h"
#include "WfNetMgr.h"
#include <json/json.h>
#include "WfSysUtil.h"

NS_WF_BEGIN

using namespace std;
class ConnectMgr
{
public:
	typedef std::unordered_map<uint32, SessionGateway*>	SessionMap;
	SessionMap	m_Sessions;						// »á»°	
	NetMgr* m_NetMgr;

	string m_AccountName;
	int32 m_StartId;
	int32 m_Num;
	int32 m_SendBulletTime;

public:
	ConnectMgr(NetMgr* netMgr)
	{
		m_NetMgr = netMgr;
	}
	virtual ~ConnectMgr() {};

	virtual bool Init(const string& nodeName)
	{
		if (!loadConInfo()) {
			log_waring("load Base.cfg error");
			return false;
		}

		VerifyConnectorSessionMgrConfigReader reader;
		if (!reader.Load(nodeName))
		{
			log_err("confg error");
			return false;
		}
		if (reader.list.size() < 1) {
			log_err("load Network error");
			return false;
		}

		VerifyConnectorSessionConfig cfg = reader.list[0];
		for (int32 i = m_StartId; i < m_StartId + m_Num; i++)
		{
			cfg.id = i;
			SessionGateway* session = new SessionGateway();
			char buff[64] = { 0 };
			sprintf(buff, "%s_%d", m_AccountName.c_str(), i);
			session->setInfo(buff, m_SendBulletTime, i / 4, i % 4);
			if (INVALID_SESSIONID == m_NetMgr->AddConnection(cfg.id, cfg.opt, session))
			{
				log_err("connect error:ip=%s port=%d\n", cfg.opt.m_IP.c_str(), cfg.opt.m_Port);
				WF_SAFE_DELETE(session);
				return false;
			}
			m_Sessions[cfg.id] = session;
		}

		return true;
	}
	
	bool loadConInfo()
	{
		Json::Value root;
		Json::Reader jsonReader;
		bool ok = jsonReader.parse(SysUtil::ReadFileCPlus("Config/Base.cfg"), root);

		if (ok)
		{
			m_AccountName = root.get("AccountName", "robot").asString();
			m_StartId = root.get("StartId", 1).asInt();
			m_Num = root.get("Num", 1).asInt();
			m_SendBulletTime = root.get("SendBulletTime", 1).asInt();
		}
		else {
			log_err("DbConnectionPool db.cfg error");
			return false;
		}
		return true;
	}
	bool SendPacket(uint32 id, PacketHeader* packet)
	{
		SessionGateway* pSession = m_Sessions[id];
		if (pSession) {
			return pSession->SendPacket(packet);
		}
		return false;
	}

	bool SendPacket(uint32 id, PacketCommand cmd, ::google::protobuf::Message* packet)
	{
		SessionGateway* pSession = m_Sessions[id];
		if (pSession) {
			return pSession->SendPacket(cmd, packet);
		}
		return false;
	}
};
NS_WF_END
