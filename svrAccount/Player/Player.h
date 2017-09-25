#pragma once
#include "WfDataType.h"
#include "WfNetMgr.h"
#include "Packets/Packet_DB.pb.h"
#include "SessionGateWay/SessionGateWay.h"
#include "Logic/FriendModule.h"
#include "Logic/MailModule.h"
#include "Logic/BankModule.h"
using namespace std;
typedef string t_AccountId;
typedef uint32 t_PlayerId;
NS_WF_BEGIN
class Player
{
public:
	enum emPlayerState {
		emPlayerState_new,
		emPlayerState_online,
		emPlayerState_offline
	};
	enum emPlayerPos {
		emPlayerPos_Lobby,
		emPlayerPos_EnterGame,
		emPlayerPos_Game,
	};
public:
	Player();
	~Player();
public:
	emPlayerState m_state;
	emPlayerPos m_playerPos;
	FriendModule m_FriendModule;
	MailModule m_MailModule;
	BankModule m_BankModule;
	Def_MemberVariable(t_PlayerId, PlayerId);
	Def_MemberVariable(t_AccountId, AccountId);	//账号id
	Def_MemberVariable(int32, GateWayId);		//网关id
	Def_MemberVariable(t_sessionId, SessionId);	//账号会话
	Def_MemberVariable(int32, GameId);		//网关id

	Def_MemberVariable(string, NickName);		//昵称
	Def_MemberVariable(int32, Sex);				//性别（0未设置，1男，2女）
	Def_MemberVariable(int32, HeadId);			//头像id
	Def_MemberVariable(int64, Money);			//金币
public:
	bool Init(t_PlayerId playerId, 
		const rpc::t_playerBaseData* baseInfo, 
		const rpc::t_FriendDatas* friendData,
		const rpc::t_MailDatas* mailData,
		int32 gatewayId = -1, 
		t_sessionId sessionId = INVALID_SESSIONID);

	void dumpPlayerBaseData(rpc::t_playerBaseData* data);
	void dumpFriendData(rpc::t_FriendDatas* data);
	void dumpMailData(rpc::t_MailDatas* data);
	// 发消息
	bool SendPacket(PacketHeader* packet);
	bool SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
private:
	void setplayerBaseData(const rpc::t_playerBaseData* baseInfo);
public:
	//通知事件
	void OnEventNotify(PacketHeader* packet);
	//玩家登录游戏大厅事件
	void OnEventEnterLobby();
	//进入子游戏事件
	void OnEventEnterGame(int32 rst,int32 gameid);
};
NS_WF_END
