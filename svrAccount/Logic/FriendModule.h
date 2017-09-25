#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"

NS_WF_BEGIN

struct t_FriendData
{
	t_PlayerId		playerId;
	string	playerName;
	int32 playerHeadId;				//头像id
	int64 money;
	void load(const rpc::t_FriendItemData& obj)
	{
		playerId = obj.playerid();
		playerName = obj.name();
		playerHeadId = obj.headid();
		money = obj.money();
	}
};
struct t_FriendReqData
{
	t_PlayerId		playerId;
	string	playerName;
	int32 playerHeadId;				//头像id
	int64 money;
	int32 reqTime;
	void load(const rpc::t_FriendReqItemData& obj)
	{
		playerId = obj.playerid();
		playerName = obj.name();
		playerHeadId = obj.headid();
		reqTime = obj.time();
		money = obj.money();
	}
};
class FriendModule
{
public:
	FriendModule();
	virtual ~FriendModule();

	void init(const rpc::t_FriendDatas* friendData);
	void dumpFriend(::google::protobuf::RepeatedPtrField< rpc::t_FriendItem >* datas);
	void dumpFriendReq(::google::protobuf::RepeatedPtrField< rpc::t_FriendReqItem >* datas);
	t_FriendData* findFriend(t_PlayerId playerId);
	void delFriend(t_PlayerId playerId);	
	bool rmvFriendReqData(t_PlayerId playerId,string& outName,int32& outheadId,int64& money);
	//添加好友，已经添加则返回false
	bool addFriend(t_PlayerId playerId, const string& name, int32 headId);
	void _TestData();
private:
	map<int32, t_FriendData*> m_friends;
	vector<t_FriendReqData*> m_friendReqs;
};


NS_WF_END