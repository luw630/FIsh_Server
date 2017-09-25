#pragma once

#include "IPlayer.h"
#include "Packets/Packet_Base.pb.h"

NS_WF_BEGIN

enum emPlayerStatus {
	emPlayerStatus_Lobby = 1,			//大厅中
	emPlayerStatus_PreGame = 2,			//准备进入游戏
	emPlayerStatus_Game = 3,			//游戏中
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

	Def_MemberVariable(time_t, BombSendTime);	//炮弹发射时间
	Def_MemberVariable(int32, BombType);		//炮台类型
	Def_MemberVariable(int32, FireCost);		//设置倍率
private:
	emPlayerStatus m_PlayerStatus;	
	
	t_ObjId m_tableId;	//所在桌子id
	t_ObjId m_seatId;	//所在位置id
	int32 m_headId;		//头像id
	string m_nickName;	
	int64 m_money;
	t_ObjId m_lockFishid;	//锁定炮错定的fish id
};

NS_WF_END

