#pragma once

#include "WfCObjMgr.h"
#include "WfLockFreeList.h"
#include "WfAbsFactory.h"
#include <google/protobuf/message.h>
#include "WfPacketHeader.h"

NS_WF_BEGIN

class IGameService;
class IGame;
class ITable : public CObj
{
public:
	ITable();
	~ITable();
public:
	virtual bool onInit() = NULL;
	//�Ƿ��ǻ��
	virtual bool isActive() = NULL;	
	//ÿ֡����
	virtual void onUpdate(uint32 delta) = NULL;
	//�ַ���Ϣ
	virtual void onDispatcherMsg(abs* buffer) = NULL;
	//�㲥��
	virtual void broadCastPacket(PacketCommand cmd, ::google::protobuf::Message* packet) = NULL;
	bool Init(t_ObjId id, IGameService* service, IGame* game);
	//������Ϣ
	void pushMsg(abs* msg);
	//���ⲿ������Ϣ
	void sendPacket(abs* msg);
	void sendPacket(t_ObjId playerId,PacketCommand cmd, ::google::protobuf::Message* packet);
	//�����̸߳���
	void Update(uint32 delta);

	IGameService* getGameService();
	IGame* getGame();
	abs* newBuffer_Rcv();
	abs* newBuffer_Send();
private:
	LockFreeList<abs*>*	m_RecvPackets;			// �յ��İ�
protected:
	IGameService* m_GameService;
	IGame* m_game;
};

NS_WF_END

