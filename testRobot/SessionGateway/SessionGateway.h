#pragma once

#include "WfConnection.h"
#include "WfConnectionSession.h"

NS_WF_BEGIN
class SessionGateway : public ConnectionSession
{
public:
	SessionGateway();
	virtual ~SessionGateway();

	virtual void OnConnected();
	virtual void OnDisconnected();
	int OnPacket(PacketHeader* packet, Stream& stream);
	void Update(uint32 delta);
public:
	void setInfo(
		const std::string& str,
		int32 sendBulletTime,
		int32 tableId,
		int32 seatId)
	{
		m_account = str;
		m_SendBulletTime = sendBulletTime;
		m_tableId = tableId;
		m_seatId = seatId;
	}
private:
	enum emLoginState {
		emLoginState_unLogin = 0,
		emLoginState_Login = 1,
		emLoginState_GateWay = 2,
		emLoginState_GateWayed = 3,
	};
	emLoginState m_loginState;	//��¼�Ƿ�ɹ�
	std::string m_loginSession;
	std::string m_account;
	int32 m_SendBulletTime;
	int32 m_tableId;
	int32 m_seatId;

	bool m_bBattle;	//�Ƿ��������
	uint64 m_BattleTime;
};

NS_WF_END