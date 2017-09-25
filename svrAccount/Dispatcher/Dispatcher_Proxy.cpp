#include "Dispatcher_Proxy.h"
#include "Packets/Packet_Server.pb.h"
#include "Packets/Packet_Lobby.pb.h"
#include "App/App.h"
#include "WfCommonPackets.h"
#include <json/json.h>
NS_WF_BEGIN

bool Dispatcher_Proxy::Init()
{
	Register(rpc::EM_SvrManager_Post, &Dispatcher_Proxy::OnHandle_SvrManager_Post);
	m_HandleSvrManagerMap.insert(make_pair("/player/online/number", &Dispatcher_Proxy::OnHandle_player_online_number));
	m_HandleSvrManagerMap.insert(make_pair("/notic/system/get", &Dispatcher_Proxy::OnHandle_notic_system_get));
	m_HandleSvrManagerMap.insert(make_pair("/notic/system/add", &Dispatcher_Proxy::OnHandle_notic_system_add));
	m_HandleSvrManagerMap.insert(make_pair("/notic/system/edit", &Dispatcher_Proxy::OnHandle_notic_system_edit));
	m_HandleSvrManagerMap.insert(make_pair("/notic/system/delete", &Dispatcher_Proxy::OnHandle_notic_system_delete));
	return true;
}

void Dispatcher_Proxy::OnHandle_SvrManager_Post(SessionProxy* session, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam)
{
	rpc::SvrManager_Post msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());
	HandleSvrManagerMap::iterator iter = m_HandleSvrManagerMap.find(msg.cur());

	string rspString = "";
	rpc::SvrManager_Response rsp;
	rsp.set_postid(msg.postid());

	if (iter != m_HandleSvrManagerMap.end())
	{
		HandleSvrManager handle = iter->second;		
		(this->*handle)(session, msg.body(), rspString);
	}
	rsp.set_body(rspString);
	App::GetInstance().m_SessionProxyMgr->m_session->PostMsg(
		rpc::EM_SvrManager_Response,
		&rsp,
		msgParamLen,
		msgParam,
		emSvrType_Account,
		App::GetInstance().GetAppId(),
		emSvrType_Manager,
		trans_p2p,
		1
	);
}

void Dispatcher_Proxy::OnHandle_player_online_number(SessionProxy* session, const string& data, string& rspStr)
{
	char buff[256] = { 0 }; 
	sprintf(buff, "{\"status\":\"T\", \"data\" : {\"player_online_number_hai_wang_2\":\"%d\",\
		\"player_online_number_jin_chan_bu_yu\":\"%d\",\
		\"player_online_number\":\"%d\"}}", 3,3, App::GetInstance().getPlayerMgr()->getOnlinePlayerNum());
	rspStr = buff;	
}
void Dispatcher_Proxy::OnHandle_notic_system_get(SessionProxy* session, const string& data, string& rspStr)
{
	rspStr = App::GetInstance().m_Announcement->getJsonData();
}
void Dispatcher_Proxy::OnHandle_notic_system_add(SessionProxy* session, const string& data, string& rspStr)
{
	bool rst = false;
	int32 id = 0;
	do {
		Json::Value root;
		Json::Reader jsonReader;
		bool ok = jsonReader.parse(data, root);
		if (!ok) break;
		
		Json::Value data = root["data"];
		if (data.isNull()) break;
		Json::Value items = data["items"];
		if (items.isNull()) break;

		uint32 startTime = items.get("startTime", 0).asInt();
		uint32 endTime = items.get("endTime", 0).asInt();
		string title = items.get("title", "").asString();
		string content = items.get("content", "").asString();

		id = App::GetInstance().m_Announcement->addItem(startTime, endTime, title, content);
		rst = true;
	} while (false);
	char buff[128] = { 0 };
		

	if (rst) {
		sprintf(buff, "{\"status\":\"T\", \"data\" : {\"items\":{\"id\":\"%d\"}}}", id);
	}else {
		sprintf(buff, "{\"status\":\"F\", \"data\" : {}}");
	}
	rspStr = buff;
}
void Dispatcher_Proxy::OnHandle_notic_system_edit(SessionProxy* session, const string& data, string& rspStr)
{
	bool rst = false;
	int32 id = 0;
	do {
		Json::Value root;
		Json::Reader jsonReader;
		bool ok = jsonReader.parse(data, root);
		if (!ok) break;

		Json::Value data = root["data"];
		if (data.isNull()) break;
		Json::Value items = data["items"];
		if (items.isNull()) break;

		id = root.get("id", -1).asInt();
		uint32 startTime = root.get("startTime", 0).asInt();
		uint32 endTime = root.get("endTime", 0).asInt();
		string title = root.get("title", "").asString();
		string content = root.get("content", "").asString();

		App::GetInstance().m_Announcement->updateItem(id,startTime, endTime, title, content);
		rst = true;
	} while (false);
	char buff[128] = { 0 };


	if (rst) {
		sprintf(buff, "{\"status\":\"T\", \"data\" : {\"items\":{\"id\":\"%d\"}}}", id);
	}
	else {
		sprintf(buff, "{\"status\":\"F\", \"data\" : {}}");
	}
	rspStr = buff;
}
void Dispatcher_Proxy::OnHandle_notic_system_delete(SessionProxy* session, const string& data, string& rspStr)
{
	bool rst = false;
	int32 id = 0;
	do {
		Json::Value root;
		Json::Reader jsonReader;
		bool ok = jsonReader.parse(data, root);
		if (!ok) break;

		Json::Value data = root["data"];
		if (data.isNull()) break;
		Json::Value items = data["items"];
		if (items.isNull()) break;

		id = root.get("id", -1).asInt();
		App::GetInstance().m_Announcement->delItem(id);
		rst = true;
	} while (false);
	char buff[128] = { 0 };

	if (rst) {
		sprintf(buff, "{\"status\":\"T\", \"data\" : {\"items\":{\"id\":\"%d\"}}}", id);
	}
	else {
		sprintf(buff, "{\"status\":\"F\", \"data\" : {}}");
	}
	rspStr = buff;
}
NS_WF_END