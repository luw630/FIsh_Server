#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"

NS_WF_BEGIN
struct AnnouncementItem
{
	int32	m_id;
	uint32	m_startTime;
	uint32	m_endTime;
	string	m_title;
	string	m_content;
	AnnouncementItem();
	void Serialize(FILE* fd);
	void DeSerialize(FILE* fd);
};

class Announcement
{
public:
	Announcement();
	virtual ~Announcement();
	bool init();
	int32 addItem(uint32 startTime, uint32 endTime,const string& title, const string& content);
	bool delItem(int32 id);
	bool updateItem(int32 id, uint32 startTime, uint32 endTime, const string& title, const string& content);
	void dump(::google::protobuf::RepeatedPtrField< rpc::t_AnnouncementItem >* datas);
	string getJsonData();
	void update(uint delta);
	void save();
private:
	int32 m_index;
	uint32 m_saveTime;
	bool m_needSave;
	map<int32, AnnouncementItem*> m_Datas;
};


NS_WF_END