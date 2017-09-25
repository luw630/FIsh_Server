#include "Player.h"
#include "App/App.h"
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Server.pb.h"

NS_WF_BEGIN
Player::Player() 
{
	m_GateWayId = -1;
	m_SessionId = INVALID_SESSIONID;
	m_PlayerId = -1;
	m_AccountId = "";
	m_NickName = "";
	m_Sex = 0;
	m_HeadId = 0;
	m_Money = 0;
	m_state = emPlayerState_new;
}


Player::~Player()
{
}

bool Player::Init(
	t_PlayerId playerId,
	const rpc::t_playerBaseData* baseInfo,
	const rpc::t_FriendDatas* friendData,
	const rpc::t_MailDatas* mailData,
	int32 gatewayId,
	t_sessionId sessionId)
{
	m_playerPos = emPlayerPos_Lobby;
	m_PlayerId = playerId;
	m_GateWayId = gatewayId;
	m_SessionId = sessionId;

	Assert(baseInfo);
	setplayerBaseData(baseInfo);
	if (friendData) {
		m_FriendModule.init(friendData);
	}
	if (mailData) {
		m_MailModule.init(mailData);
	}
	return true;
}
void Player::dumpPlayerBaseData(rpc::t_playerBaseData* data)
{
	data->set_nickname(m_NickName);
	data->set_accountid(m_AccountId);
	data->set_headid(m_HeadId);
	data->set_money(m_Money);
	data->set_sex(m_Sex);
}
void Player::dumpFriendData(rpc::t_FriendDatas* data)
{

}
void Player::dumpMailData(rpc::t_MailDatas* data)
{

}
bool Player::SendPacket(PacketHeader* packet)
{
	
	return false;
}

bool Player::SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet)
{
	SessionGateWay* pSessionGateWay = App::GetInstance().getListenerGateWay()->getSessionGateWayByNodeId(m_GateWayId);
	if (!pSessionGateWay)
	{
		log_waring("not find SessionGateWay by nodeId(%d)", m_GateWayId);
		return false;
	}

	uint8 s_sendBuffer[8192];
	PacketHeader* header = (PacketHeader*)s_sendBuffer;
	header->m_Command = cmd;
	header->m_PacketParam = m_PlayerId;
	if (packet)
	{
		int32 size = packet->ByteSize();
		int32 nSpace = 8192 - sizeof(PacketHeader);
		if (size > nSpace) {
			log_err("space is mall");
			return false;
		}
		else {
			bool rst = packet->SerializeToArray(header + 1, size);
			if (!rst) {
				log_err("unknow error");
				return false;
			}
		}

		header->m_Length = sizeof(PacketHeader) + size;
	}
	else {
		header->m_Length = sizeof(PacketHeader);
	}
	return pSessionGateWay->SendPacket(header);
}

void Player::setplayerBaseData(const rpc::t_playerBaseData* baseInfo)
{
	m_NickName = baseInfo->nickname();
	m_AccountId = baseInfo->accountid();
	m_HeadId = baseInfo->headid();
	m_Money = baseInfo->money();
	m_Sex = baseInfo->sex();

	if (baseInfo->has_bankdata()) {
		m_BankModule.loadData(baseInfo->bankdata());
	}
}

void Player::OnEventNotify(PacketHeader* packet)
{
	Dispatcher_Player& dispatcher = App::GetInstance().getDispatcherPackets()->m_DispatcherPlayer;
	auto handler = dispatcher.getHandle(packet->m_Command);
	if (handler != nullptr) {
		(dispatcher.*handler)(this, packet, Stream((uint8*)(packet + 1), packet->m_Length - sizeof(PacketHeader)));
	}
	else {
		log_waring("not find cmd(%d)", packet->m_Command);
	}
}
void Player::OnEventEnterLobby()
{
	if (m_playerPos == emPlayerPos_EnterGame||
		m_playerPos == emPlayerPos_Game)
	{
		rpc::AGame_EnterGame req;
		req.set_m_playerid(m_PlayerId);
		req.set_m_gameid(m_GameId);
		req.set_m_gatewayid(m_GateWayId);
		rpc::t_EnterGamePlayerData* playerData = req.mutable_m_playerdata();
		playerData->set_m_playerid(m_PlayerId);
		playerData->set_m_nickname(m_NickName);
	}

	rpc::SC_PlayerInfo rsp;
	rsp.set_m_playerid(m_PlayerId);
	rsp.set_m_nickname(m_NickName);
	rsp.set_m_sex(m_Sex);
	rsp.set_m_headid(m_HeadId);
	rsp.set_m_money(m_Money);
	SendPacket(rpc::em_SC_PlayerInfo, &rsp);

	FriendSystem* pFriendSystem = App::GetInstance().getFriendSystem();
	pFriendSystem->onEventPlayerOnline(this);

}
void Player::OnEventEnterGame(int32 rst, int32 gameid)
{
	if (rst != 0) {
		log_waring("player(%d) enter Game(%d) Failed", m_PlayerId, gameid);
	}
}
NS_WF_END