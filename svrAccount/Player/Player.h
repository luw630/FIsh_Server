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
	Def_MemberVariable(t_AccountId, AccountId);	//�˺�id
	Def_MemberVariable(int32, GateWayId);		//����id
	Def_MemberVariable(t_sessionId, SessionId);	//�˺ŻỰ
	Def_MemberVariable(int32, GameId);		//����id

	Def_MemberVariable(string, NickName);		//�ǳ�
	Def_MemberVariable(int32, Sex);				//�Ա�0δ���ã�1�У�2Ů��
	Def_MemberVariable(int32, HeadId);			//ͷ��id
	Def_MemberVariable(int64, Money);			//���
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
	// ����Ϣ
	bool SendPacket(PacketHeader* packet);
	bool SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
private:
	void setplayerBaseData(const rpc::t_playerBaseData* baseInfo);
public:
	//֪ͨ�¼�
	void OnEventNotify(PacketHeader* packet);
	//��ҵ�¼��Ϸ�����¼�
	void OnEventEnterLobby();
	//��������Ϸ�¼�
	void OnEventEnterGame(int32 rst,int32 gameid);
};
NS_WF_END
