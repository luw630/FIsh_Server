#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"

NS_WF_BEGIN

struct t_MailData
{
	//t_PlayerId		sendPlayerId;
	//string	sendName;
	//int32	sendHeadId;

	string	title;
	string content;				
	int32 rcvTime;
	void load(const rpc::t_MailItemData& obj)
	{
		title = obj.title();
		content = obj.content();
		rcvTime = obj.time();
	}
};

class MailModule
{
public:
	MailModule();
	virtual ~MailModule();

	void init(const rpc::t_MailDatas* mailData);
	//void dump(::google::protobuf::RepeatedPtrField< rpc::t_FriendItem >* datas);
private:
	vector<t_MailData*> m_mails;
};


NS_WF_END