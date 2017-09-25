#pragma once
#include "IGame.h"
#include "TableMgr.h"
#include "TableViewMgr.h"
#include "PlayerMgr.h"
#include "Cfg/Cfg_Game.h"
#include "DispatcherRoom.h"
#include "GameService.h"

NS_WF_BEGIN

class GameFish : public IGame
{
protected:
	GameService* m_pGameService;			//�����߳�
	int32 m_GameServiceNum;					//�����̸߳���
public:
	GameFish(IAppProxy* AppProxy);
	~GameFish();
public:
	//��ʼ����Դ
	virtual bool Init(int32 workThreadNum);
	//��ʼ������
	virtual void Start();
	//�رշ���
	virtual void Stop();

	virtual void OnUpdate(uint delta);
public:
	virtual void OnWorkThreadPacket(PacketHeader* packet);
	virtual void OnAccountPacket(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
	virtual void OnGateWayPacket(ISessionGateWay* pSession, PacketHeader* packet, Stream& stream);
	virtual void OnPlayerOffLine(uint32 playerId, int32 gatewayId, uint64 sessionId);
private:
	bool OnInit();
public:
	TableMgr* getTableMgr() { return m_pTableMgr; }
	Cfg_Game* getCfgGame() { return m_CfgGame; }
	PlayerMgr* getPlayerMgr() { return m_PlayerMgr; }
	TableViewMgr* getTableViewMgr() { return m_pTableViewMgr; }
private:
	TableViewMgr* m_pTableViewMgr;	//���������ӵ���ʾ����
	TableMgr* m_pTableMgr;				
	PlayerMgr* m_PlayerMgr;					//�����е�Player����
	Cfg_Game* m_CfgGame;
	DispatcherRoom* m_DispatcherFish;		//����ַ���Ϣ
private:
	//scene֪ͨ���� ��ҽ�����Ϸ�ɹ�
	void OnHandlePlayerEnterTable(PacketHeader* packet);
	//scene֪ͨ���� ����뿪��Ϸ�ɹ�
	void OnHandlePlayerLeaveTable(PacketHeader* packet);
};

NS_WF_END

