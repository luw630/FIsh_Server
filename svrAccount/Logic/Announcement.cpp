#include "Announcement.h"
#include "WfLog.h"
#include <json/json.h>

NS_WF_BEGIN

AnnouncementItem::AnnouncementItem()
{
	m_id = 0;
	m_startTime = 0;
	m_endTime = 0;
	m_title = "";
	m_content = "";
}
void AnnouncementItem::Serialize(FILE* fd)
{
	fwrite(&m_id, 4, 1, fd);
	fwrite(&m_startTime, 4, 1, fd);
	fwrite(&m_endTime, 4, 1, fd);
	int32 len = m_title.size();
	fwrite(&len, 4, 1, fd);
	if (len > 0) {
		fwrite((char*)m_title.data(), 1, len, fd);
	}
	len = m_content.size();
	fwrite(&len, 4, 1, fd);
	if (len > 0) {
		fwrite((char*)m_content.data(), 1, len, fd);
	}
}
void AnnouncementItem::DeSerialize(FILE* fd)
{
	fread(&m_id, 4, 1, fd);
	fread(&m_startTime, 4, 1, fd);
	fread(&m_endTime, 4, 1, fd);
	int32 len = 0;
	fread(&len, 4, 1, fd);
	if (len > 0) {
		m_title.resize(len);
		fread((char*)m_title.data(), 1, len, fd);
	}
	fread(&len, 4, 1, fd);
	if (len > 0) {
		m_content.resize(len);
		fread((char*)m_content.data(), 1, len, fd);
	}
}
Announcement::Announcement()
{
	m_index = 0;
	m_saveTime = 0;
	m_needSave = false;
}
Announcement::~Announcement()
{

}

bool Announcement::init()
{
	FILE* handle = fopen("./db/Announcement.db", "rb+");
	if (handle) {
		fseek(handle, 0, SEEK_SET);
		int32 num = 0;
		fread(&m_index, 4, 1, handle);
		fread(&num, 4, 1, handle);
		for (int i = 0; i < num; i++)
		{
			AnnouncementItem* item = new AnnouncementItem();
			item->DeSerialize(handle);
			m_Datas[item->m_id] = item;
		}
		fclose(handle);
	}
	m_saveTime = 0;
	m_needSave = false;
	return true;
}
void Announcement::dump(::google::protobuf::RepeatedPtrField< rpc::t_AnnouncementItem >* datas)
{
	uint32 curTime = time(0);
	for (auto&t : m_Datas)
	{
		AnnouncementItem* obj = t.second;
		if (obj->m_startTime <= curTime && obj->m_endTime >= curTime)
		{
			rpc::t_AnnouncementItem* data = datas->Add();
			data->set_m_id(obj->m_id);
			data->set_m_title(obj->m_title);
			data->set_m_content(obj->m_content);
		}
	}
}
int32 Announcement::addItem(uint32 startTime, uint32 endTime, const string& title, const string& content)
{
	++m_index;
	AnnouncementItem* obj = new AnnouncementItem();
	obj->m_id = m_index;
	obj->m_startTime = startTime;
	obj->m_endTime = endTime;
	obj->m_title = title;
	obj->m_content = content;

	m_Datas[obj->m_id] = obj;

	m_needSave = true;
	return m_index;
}
bool Announcement::delItem(int32 id)
{
	auto t = m_Datas.find(id);
	if (t == m_Datas.end())
	{
		return false;
	}
	AnnouncementItem* obj = t->second;
	m_Datas.erase(t);
	delete obj;

	m_needSave = true;
	return true;
}
bool Announcement::updateItem(int32 id, uint32 startTime, uint32 endTime, const string& title, const string& content)
{
	auto t = m_Datas.find(id);
	if (t == m_Datas.end())
	{
		return false;
	}
	m_needSave = true;
	AnnouncementItem* obj = t->second;
	obj->m_startTime = startTime;
	obj->m_endTime = endTime;
	obj->m_title = title;
	obj->m_content = content;
	return true;
}
string Announcement::getJsonData()
{
	Json::Value root;
	Json::Value data;
	Json::Value items;
	for (auto&t : m_Datas)
	{
		Json::Value item;
		AnnouncementItem* obj = t.second;
		item["id"] = obj->m_id;
		item["startTime"] = obj->m_startTime;
		item["endTime"] = obj->m_endTime;
		item["title"] = obj->m_title;
		item["content"] = obj->m_content;
		items.append(item);
	}
	data["items"] = items;
	root["data"] = data;
	return root.toStyledString();
}
void Announcement::update(uint delta)
{
	m_saveTime += delta;
	if (m_saveTime < 5000) return;

	m_saveTime -= 5000;
	save();
}
void Announcement::save()
{
	if (!m_needSave) return;

	FILE* handle = fopen("./db/Announcement.db.bak", "w+");
	if (handle) {
		int32 num = m_Datas.size();
		fwrite(&m_index, 4, 1, handle);
		fwrite(&num, 4, 1, handle);
		for (auto&t : m_Datas)
		{
			fwrite(&(t.first), 4, 1, handle);
			t.second->Serialize(handle);
		}
		fclose(handle);
		remove("./db/Announcement.db");
		if (rename("./db/Announcement.db.bak", "./db/Announcement.db")) {
			log_waring("error");
		}
	}
}
NS_WF_END
