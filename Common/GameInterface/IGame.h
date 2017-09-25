#pragma once

#include "WfConfig.h"
#include "IGameService.h"
#include "ISessionGateWay.h"
#include "ISessionAccount.h"
#include "Packets/Packet_Base.pb.h"
#include "IAppProxy.h"

NS_WF_BEGIN

class IGame
{
public:
	IAppProxy* m_AppProxy;
public:
	IGame(IAppProxy* AppProxy) :m_AppProxy(AppProxy) {}
	~IGame() { m_AppProxy = nullptr; }
public:
	//��ʼ����Դ
	virtual bool Init(int32 workThreadNum) = NULL;
	//��ʼ������
	virtual void Start() = NULL;
	//�رշ���
	virtual void Stop() = NULL;
	//���̸߳���
	virtual void OnUpdate(uint delta) = NULL;
public:
	//�����̵߳İ�
	virtual void OnWorkThreadPacket(PacketHeader* packet) = NULL;
	//����Account�İ�
	virtual void OnAccountPacket(uint32 srcId,PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam) = NULL;
	//�������صİ�
	virtual void OnGateWayPacket(ISessionGateWay* pSession, PacketHeader* packet, Stream& stream) = NULL;
	//����֪ͨGame���������
	virtual void OnPlayerOffLine(uint32 playerId, int32 gatewayId,uint64 sessionId) = NULL;
};

NS_WF_END

