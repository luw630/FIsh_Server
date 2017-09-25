#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"
#include "Packets/Packet_Friend.pb.h"
#include "Packets/Packet_DB.pb.h"
#include "WfPacketHeader.h"

NS_WF_BEGIN

struct t_FriendSystemRecordItem
{
	t_FriendSystemRecordItem()
	{
		memset(m_fileName, 0, sizeof(m_fileName));
		m_fileHandle = nullptr;
		m_fileOpenTime = 0;
	}
	char m_fileName[64];	//文件名
	FILE* m_fileHandle;		//文件句柄
	uint64 m_fileOpenTime;	//文件打开时间
};
class Player;

class FriendSystem
{
public:
	FriendSystem();
	virtual ~FriendSystem();

	void init();
	void update(uint delta);
	void postPacket(PacketCommand cmd, ::google::protobuf::Message* packet,t_PlayerId playerId);
	void onEventPlayerOnline(Player* player);
private:
	void save();
	uint32 m_saveTime;
	bool m_needSave;
	hash_map<t_PlayerId, t_FriendSystemRecordItem*> m_records;	//文件记录
};


NS_WF_END