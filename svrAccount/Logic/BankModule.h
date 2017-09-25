#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"
#include "Packets/Packet_DB.pb.h"

NS_WF_BEGIN

class BankModule
{
	Def_MemberVariable(bool, HavePwd);
	Def_MemberVariable(string, Pwd);
	Def_MemberVariable(int64, Money);
public:
	BankModule();
	virtual ~BankModule();
	void loadData(const ::rpc::t_BankDatas& data);
private:

};
NS_WF_END