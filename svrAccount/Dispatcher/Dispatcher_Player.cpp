#include "Dispatcher_Player.h"
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Bank.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"
#include "WfCommonPackets.h"
#include "App/App.h"

NS_WF_BEGIN

bool Dispatcher_Player::Init()
{
	Register(rpc::em_CS_EnterGame, &Dispatcher_Player::OnHandle_CS_EnterGame);
	Register(rpc::em_CS_LeaveGame, &Dispatcher_Player::OnHandle_CS_LeaveGame);
	Register(rpc::em_CS_Rank, &Dispatcher_Player::OnHandle_CS_Rank);
	Register(rpc::em_CS_Announcement, &Dispatcher_Player::OnHandle_CS_Announcement);
	Register(rpc::em_CS_FriendList, &Dispatcher_Player::OnHandle_CS_FriendList);
	Register(rpc::em_CS_FriendReqList, &Dispatcher_Player::OnHandle_CS_FriendReqList);
	Register(rpc::em_CS_FriendFind, &Dispatcher_Player::OnHandle_CS_FriendFind);
	Register(rpc::em_CS_ReqAddFriend, &Dispatcher_Player::OnHandle_CS_ReqAddFriend);
	Register(rpc::em_CS_DelFriend, &Dispatcher_Player::OnHandle_CS_DelFriend);
	Register(rpc::em_CS_FriendVerifyOtherReq, &Dispatcher_Player::OnHandle_CS_FriendVerifyOtherReq);
	Register(rpc::em_CS_FriendSendMsg, &Dispatcher_Player::OnHandle_CS_FriendSendMsg);
	Register(rpc::em_CS_FriendMsgRecord, &Dispatcher_Player::OnHandle_CS_FriendMsgRecord);

	Register(rpc::EM_Friend_ReqAddFriend, &Dispatcher_Player::OnHandle_Friend_ReqAddFriend);
	Register(rpc::EM_Friend_AgreeAddFiend, &Dispatcher_Player::OnHandle_Friend_AgreeAddFiend);
	Register(rpc::EM_Friend_RefuseAddFiend, &Dispatcher_Player::OnHandle_Friend_RefuseAddFiend);
	Register(rpc::EM_Friend_SendMsg, &Dispatcher_Player::OnHandle_Friend_SendMsg);

	Register(rpc::em_CS_BankSetPwd, &Dispatcher_Player::OnHandle_CS_BankSetPwd);
	Register(rpc::em_CS_BankChangePwd, &Dispatcher_Player::OnHandle_CS_BankChangePwd);
	Register(rpc::em_CS_BankStoreMoney, &Dispatcher_Player::OnHandle_CS_BankStoreMoney);
	Register(rpc::em_CS_BankDrawMoney, &Dispatcher_Player::OnHandle_CS_BankDrawMoney);
	return true;
}
void Dispatcher_Player::OnHandle_CS_EnterGame(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	//if (pPlayer->m_playerPos != Player::emPlayerPos_Lobby) {
	//	log_waring("player(%d) ingame(%d),need outGame",pPlayer->getPlayerId(),pPlayer->getGameId());
	//	return;
	//}
	log_dbg("rcv CS_EnterGame");
	rpc::CS_EnterGame msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	if (msg.m_gameid() != GameId_FishLowLevel) {
		log_waring("player(%d) CS_EnterGame,gameid(%d) error", pPlayer->getPlayerId(), msg.m_gameid());
		return;
	}

	rpc::AGame_EnterGame req;
	req.set_m_playerid(pPlayer->getPlayerId());
	req.set_m_gameid(msg.m_gameid());
	req.set_m_gatewayid(pPlayer->getGateWayId());
	rpc::t_EnterGamePlayerData* playerData = req.mutable_m_playerdata();
	playerData->set_m_playerid(pPlayer->getPlayerId());
	playerData->set_m_headid(pPlayer->getHeadId());
	playerData->set_m_nickname(pPlayer->getNickName());
	playerData->set_m_money(pPlayer->getMoney());
	pPlayer->m_playerPos = Player::emPlayerPos_EnterGame;
	pPlayer->setGameId(msg.m_gameid());
	App::GetInstance().SendPacketToGameSvr(rpc::EM_AGame_EnterGame, &req, pPlayer->getGameId());
}
void Dispatcher_Player::OnHandle_CS_LeaveGame(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	if (pPlayer->m_playerPos == Player::emPlayerPos_Lobby) {
		pPlayer->SendPacket(rpc::em_SC_LeaveGameRst,nullptr);
		return;
	}

	rpc::AGame_LeaveGame req;
	req.set_m_playerid(pPlayer->getPlayerId());
	
	App::GetInstance().SendPacketToGameSvr(rpc::EM_AGame_LeaveGame,&req, pPlayer->getGameId());
}

void Dispatcher_Player::OnHandle_CS_Rank(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::SC_RankData rsp;
	App::GetInstance().m_Rank->dump(rsp.mutable_m_data());
	pPlayer->SendPacket(rpc::em_SC_RankData, &rsp);
}
//公告
void Dispatcher_Player::OnHandle_CS_Announcement(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::SC_Announcement rsp;
	App::GetInstance().m_Announcement->dump(rsp.mutable_m_data());
	pPlayer->SendPacket(rpc::em_SC_Announcement, &rsp);
}
//好友
void Dispatcher_Player::OnHandle_CS_FriendList(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	FriendModule& module = pPlayer->m_FriendModule;
	rpc::SC_FriendList rsp;
	module.dumpFriend(rsp.mutable_m_data());
	pPlayer->SendPacket(rpc::em_SC_FriendList, &rsp);
}
void Dispatcher_Player::OnHandle_CS_FriendReqList(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	FriendModule& module = pPlayer->m_FriendModule;
	rpc::SC_FriendReqList rsp;
	module.dumpFriendReq(rsp.mutable_m_data());
	pPlayer->SendPacket(rpc::em_SC_FriendReqList, &rsp);
}
void Dispatcher_Player::OnHandle_CS_FriendFind(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_FriendFind msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	Player* player = App::GetInstance().getPlayerMgr()->getOnlinePlayer(msg.m_playerid());
	if (player)
	{
		rpc::SC_FriendFindRst rsp;
		rsp.set_bfind(0);
		rsp.set_playerid(player->getPlayerId());
		rsp.set_name(player->getNickName());
		rsp.set_headid(player->getHeadId());
		pPlayer->SendPacket(rpc::em_SC_FriendFindRst, &rsp);
		return;
	}

	rpc::SD_FindPlayer req;
	req.set_playerid(msg.m_playerid());
	rpc::param_FindPlayer* param = req.mutable_param();
	param->set_playerid(pPlayer->getPlayerId());
	param->set_selectplayerid(msg.m_playerid());
	App::GetInstance().getSeesionDb()->addBuffer(rpc::em_SD_FindPlayer, &req, INVALID_SESSIONID, msg.m_playerid());
}

void Dispatcher_Player::OnHandle_CS_ReqAddFriend(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_ReqAddFriend msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
	
	//不能请求自己
	if (msg.m_playerid() == pPlayer->getPlayerId()){
		log_waring("can't request myself.playerId(%u)", pPlayer->getPlayerId());
		return;
	}

	rpc::Friend_ReqAddFriend notify;
	notify.set_playerid(pPlayer->getPlayerId());
	notify.set_name(pPlayer->getNickName());
	notify.set_headid(pPlayer->getHeadId());

	FriendSystem* pFriendSystem = App::GetInstance().getFriendSystem();
	pFriendSystem->postPacket(rpc::EM_Friend_ReqAddFriend, &notify, msg.m_playerid());
}
void Dispatcher_Player::OnHandle_CS_DelFriend(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_DelFriend msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	FriendModule& module = pPlayer->m_FriendModule;
	module.delFriend(msg.m_friendid());
}
void Dispatcher_Player::OnHandle_CS_FriendVerifyOtherReq(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_FriendVerifyOtherReq msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	FriendModule& module = pPlayer->m_FriendModule;
	string name = "";
	int32 headId = 0;
	int64 money = 0;
	if (module.rmvFriendReqData(msg.m_playerid(), name, headId, money))
	{
		if (msg.m_agree() == 1)
		{
			rpc::Friend_AgreeAddFiend postMsg;
			postMsg.set_playerid(pPlayer->getPlayerId());
			postMsg.set_name(pPlayer->getNickName());
			postMsg.set_headid(pPlayer->getHeadId());
			postMsg.set_money(pPlayer->getMoney());

			FriendSystem* pFriendSystem = App::GetInstance().getFriendSystem();
			pFriendSystem->postPacket(rpc::EM_Friend_AgreeAddFiend, &postMsg, msg.m_playerid());

			if (module.addFriend(msg.m_playerid(), name, headId))
			{
				rpc::SC_AddFriend notify;
				rpc::t_FriendItem* item = notify.mutable_m_data();
				item->set_m_headid(headId);
				item->set_m_name(name);
				item->set_m_playerid(msg.m_playerid());
				item->set_m_money(money);
				pPlayer->SendPacket(rpc::em_SC_AddFriend, &notify);
			}
		}
		else {
			rpc::Friend_RefuseAddFiend postMsg;
			postMsg.set_playerid(pPlayer->getPlayerId());
			postMsg.set_name(pPlayer->getNickName());
			postMsg.set_headid(pPlayer->getHeadId());
			postMsg.set_money(pPlayer->getMoney());

			FriendSystem* pFriendSystem = App::GetInstance().getFriendSystem();
			pFriendSystem->postPacket(rpc::EM_Friend_RefuseAddFiend, &postMsg, msg.m_playerid());
		}
	}else {
		log_waring("not find friendReq playerid(%d) reqPlayerId(%d)", pPlayer->getPlayerId(), msg.m_playerid());
	}
}
void Dispatcher_Player::OnHandle_CS_FriendSendMsg(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_FriendSendMsg msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::Friend_SendMsg postMsg;
	postMsg.set_playerid(pPlayer->getPlayerId());
	postMsg.set_name(pPlayer->getNickName());
	postMsg.set_headid(pPlayer->getHeadId());
	postMsg.set_content(msg.m_content());

	FriendSystem* pFriendSystem = App::GetInstance().getFriendSystem();
	pFriendSystem->postPacket(rpc::EM_Friend_RefuseAddFiend, &postMsg, msg.m_playerid());
}
void Dispatcher_Player::OnHandle_CS_FriendMsgRecord(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	
}
void Dispatcher_Player::OnHandle_Friend_ReqAddFriend(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::Friend_ReqAddFriend msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::SC_NotifyReqAddFriend rsp;
	rsp.set_playerid(msg.playerid());
	rsp.set_name(msg.name());
	rsp.set_headid(msg.headid());
	pPlayer->SendPacket(rpc::em_SC_NotifyReqAddFriend, &rsp);
}
void Dispatcher_Player::OnHandle_Friend_AgreeAddFiend(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::Friend_AgreeAddFiend msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::SC_FriendNotifyVerifyRst rsp;
	rsp.set_agree(1);
	rsp.set_playerid(msg.playerid());
	rsp.set_name(msg.name());
	rsp.set_headid(msg.headid());
	pPlayer->SendPacket(rpc::em_SC_FriendNotifyVerifyRst, &rsp);


	int64 money = 0;
	FriendModule& module = pPlayer->m_FriendModule;
	if (module.addFriend(msg.playerid(), msg.name(), msg.headid()))
	{
		rpc::SC_AddFriend notify;
		rpc::t_FriendItem* item = notify.mutable_m_data();
		item->set_m_headid(msg.headid());
		item->set_m_name(msg.name());
		item->set_m_playerid(msg.playerid());
		item->set_m_money(money);
		pPlayer->SendPacket(rpc::em_SC_AddFriend, &notify);
	}
}
void Dispatcher_Player::OnHandle_Friend_RefuseAddFiend(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::Friend_RefuseAddFiend msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::SC_FriendNotifyVerifyRst rsp;
	rsp.set_agree(0);
	rsp.set_playerid(msg.playerid());
	rsp.set_name(msg.name());
	rsp.set_headid(msg.headid());
	pPlayer->SendPacket(rpc::em_SC_FriendNotifyVerifyRst, &rsp);
}
void Dispatcher_Player::OnHandle_Friend_SendMsg(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::Friend_SendMsg msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	rpc::SC_NotifyFriendMsg rsp;
	rsp.set_m_playerid(msg.playerid());
	rsp.set_m_content(msg.content());
	pPlayer->SendPacket(rpc::em_SC_NotifyFriendMsg, &rsp);
}
void Dispatcher_Player::OnHandle_CS_BankSetPwd(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_BankSetPwd msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
}
void Dispatcher_Player::OnHandle_CS_BankChangePwd(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_BankChangePwd msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
}
void Dispatcher_Player::OnHandle_CS_BankStoreMoney(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_BankStoreMoney msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
}
void Dispatcher_Player::OnHandle_CS_BankDrawMoney(Player* pPlayer, PacketHeader* packet, Stream& stream)
{
	rpc::CS_BankDrawMoney msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
}
NS_WF_END