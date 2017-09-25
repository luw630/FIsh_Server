#pragma once
#include "WfDataType.h"
#include "Player/Player.h"
#include "Dispatcher.h"

NS_WF_BEGIN

class Dispatcher_Player : public Dispatcher<Player, Dispatcher_Player>
{
public:
	bool Init();
public:
	//进入游戏
	void OnHandle_CS_EnterGame(Player* pPlayer , PacketHeader* packet, Stream& stream);
	//离开游
	void OnHandle_CS_LeaveGame(Player* pPlayer, PacketHeader* packet, Stream& stream);
	
	//排行榜
	void OnHandle_CS_Rank(Player* pPlayer, PacketHeader* packet, Stream& stream);
	//公告
	void OnHandle_CS_Announcement(Player* pPlayer, PacketHeader* packet, Stream& stream);
	//好友
	void OnHandle_CS_FriendList(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_FriendReqList(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_FriendFind(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_ReqAddFriend(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_DelFriend(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_FriendVerifyOtherReq(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_FriendSendMsg(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_FriendMsgRecord(Player* pPlayer, PacketHeader* packet, Stream& stream);

	void OnHandle_Friend_ReqAddFriend(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_Friend_AgreeAddFiend(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_Friend_RefuseAddFiend(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_Friend_SendMsg(Player* pPlayer, PacketHeader* packet, Stream& stream);
	//银行
	void OnHandle_CS_BankSetPwd(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_BankChangePwd(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_BankStoreMoney(Player* pPlayer, PacketHeader* packet, Stream& stream);
	void OnHandle_CS_BankDrawMoney(Player* pPlayer, PacketHeader* packet, Stream& stream);
};
NS_WF_END

