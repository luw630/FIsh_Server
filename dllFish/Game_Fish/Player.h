#pragma once

#include "IPlayer.h"
#include "Packets/Packet_Base.pb.h"

NS_WF_BEGIN

enum emPlayerStatus {
	emPlayerStatus_Lobby = 1,			//������
	emPlayerStatus_PreGame = 2,			//׼��������Ϸ
	emPlayerStatus_Game = 3,			//��Ϸ��
};
class GameFish;
class Table;
class Player : public IPlayer
{
private:
	GameFish* m_gameHandle;
public:
	Player(GameFish* handle);
	virtual ~Player();

	virtual void OnEventEnterGame();
	virtual bool SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
public:
	bool Init(t_ObjId playerId, const rpc::t_EnterGamePlayerData& playerData);
	emPlayerStatus getStatus() { return m_PlayerStatus; }
	void setStatus(emPlayerStatus status) { m_PlayerStatus = status; }
	t_ObjId getTableId() { return m_tableId; }
	t_ObjId getSeatId() { return m_seatId; }
	string getNickName() { return m_nickName; }
	void setTableId(t_ObjId id) { m_tableId = id; }
	void setSeatId(t_ObjId id) { m_seatId = id; }

	int32 getHeadId() { return m_headId; }
	void setHeadId(int32 id) { m_headId = id; }

	t_ObjId getLockFishid() { return m_lockFishid; }
	void LockFish(Table* pTable, t_ObjId fishid);

	Def_MemberVariable(time_t, BombSendTime);	//�ڵ�����ʱ��
	Def_MemberVariable(int32, BombType);		//��̨����
	Def_MemberVariable(int32, FireCost);		//���ñ���
private:
	emPlayerStatus m_PlayerStatus;	
	
	t_ObjId m_tableId;	//��������id
	t_ObjId m_seatId;	//����λ��id
	int32 m_headId;		//ͷ��id
	string m_nickName;	
	int64 m_money;
	t_ObjId m_lockFishid;	//�����ڴ���fish id
};

NS_WF_END

