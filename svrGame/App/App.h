#pragma once

#include "WfDataType.h"
#include "WfBaseApp.h"
#include "WfIService.h"
#include "WfNetMgr.h"
#include "SessionGateWay/ListenerGateWay.h"
#include "WfConnectorMgr.h"
#include "SessionProxy/SessionProxy.h"
#include "IGame.h"
#include "IAppProxy.h"

NS_WF_BEGIN
class App : public BaseApp ,public IService,public IAppProxy
{
public:
	App();
	~App();
	SINGLETON_MODE(App);

	IGame* loadGame();
	virtual bool SendPacket(t_ObjId gatewayId, PacketHeader* packet);
	virtual void SendPacketToAccountSvr(PacketCommand cmd, ::google::protobuf::Message* packet);

	// ��ʼ��
	virtual bool OnInit(int argc, char **argv);
	virtual bool OnIniting();

	// ����
	virtual bool OnRunning(uint delta);

	// �ر�
	virtual bool OnShutdown();
	virtual bool OnShutdowning();

	// ����̳и�����ӿ�
	virtual void OnCommand(const std::string& command);

	NetMgr* getNetMgr() { return m_NetMgr; }
	ListenerGateWay* getListenerGateWay() { return m_ListenerGateWay; }
	IGame* getGame() { return m_Game; }
	SessionProxyMgr* m_SessionProxyMgr;
private:
	NetMgr* m_NetMgr;					//�������
	ListenerGateWay* m_ListenerGateWay;	//��������
	IGame* m_Game;						//��Ϸ�ӿ�
};



NS_WF_END