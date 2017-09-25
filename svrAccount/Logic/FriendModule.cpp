#include "FriendModule.h"


NS_WF_BEGIN

FriendModule::FriendModule()
{
	_TestData();
}
FriendModule::~FriendModule()
{

}
void FriendModule::init(const rpc::t_FriendDatas* friendData)
{
	int size = friendData->friends_size();
	for (int i = 0; i < size; i++){
		const rpc::t_FriendItemData& obj = friendData->friends(i);
		t_FriendData* item = new t_FriendData();
		item->load(obj);
		m_friends[item->playerId] = item;
	}

	size = friendData->friendreq_size();
	for (int i = 0; i < size; i++) {
		const rpc::t_FriendReqItemData& obj = friendData->friendreq(i);
		t_FriendReqData* item = new t_FriendReqData();
		item->load(obj);
		m_friendReqs.push_back(item); 
	}
}
void FriendModule::dumpFriend(::google::protobuf::RepeatedPtrField< rpc::t_FriendItem >* datas)
{
	for (auto&t : m_friends)
	{
		t_FriendData* obj = t.second;
		rpc::t_FriendItem* data = datas->Add();
		data->set_m_playerid(obj->playerId);
		data->set_m_name(obj->playerName);
		data->set_m_money(obj->money);
		data->set_m_headid(obj->playerHeadId);
	}
}
void FriendModule::dumpFriendReq(::google::protobuf::RepeatedPtrField< rpc::t_FriendReqItem >* datas)
{
	for (auto&t : m_friendReqs)
	{
		t_FriendReqData* obj = t;
		rpc::t_FriendReqItem* data = datas->Add();
		data->set_m_playerid(obj->playerId);
		data->set_m_name(obj->playerName);
		data->set_m_time(obj->reqTime);
		data->set_m_headid(obj->playerHeadId);
	}
}
t_FriendData* FriendModule::findFriend(t_PlayerId playerId)
{
	auto t = m_friends.find(playerId);
	if (t == m_friends.end())
	{
		return nullptr;
	}
	return t->second;
}
void FriendModule::delFriend(t_PlayerId playerId)
{
	t_FriendData* data = nullptr;
	auto t = m_friends.find(playerId);
	if (t != m_friends.end()){
		data = t->second;
	}

	if (data)
	{
		m_friends.erase(t);
		delete data;
	}
}
bool FriendModule::rmvFriendReqData(t_PlayerId playerId, string& outName, int32& outheadId, int64& money)
{
	vector<t_FriendReqData*>::iterator iter = m_friendReqs.begin();
	vector<t_FriendReqData*>::iterator end = m_friendReqs.end();
	bool rst = false;
	while (iter != end)
	{
		t_FriendReqData* data = *iter;
		if (data->playerId == playerId){
			iter = m_friendReqs.erase(iter);
			outName = data->playerName;
			outheadId = data->playerHeadId;
			money = data->money;
			delete data;
			rst = true;
		}else {
			iter++;
		}
	}	
	return rst;
}
bool FriendModule::addFriend(t_PlayerId playerId,const string& name, int32 headId)
{
	if (findFriend(playerId)){
		return false;
	}
	t_FriendData* data = new t_FriendData();
	data->playerId = playerId;
	data->playerName = name;
	data->playerHeadId = headId;
	m_friends.insert(std::make_pair(data->playerId, data));
	return true;
}
void FriendModule::_TestData()
{
	t_FriendData* data_200 = new t_FriendData();
	data_200->playerId = 200;
	data_200->playerName = "player_200";
	data_200->playerHeadId = 0;
	m_friends.insert(std::make_pair(data_200->playerId, data_200));

	t_FriendData* data_201 = new t_FriendData();
	data_201->playerId = 201;
	data_201->playerName = "player_201";
	data_201->playerHeadId = 0;
	m_friends.insert(std::make_pair(data_201->playerId, data_201));

	t_FriendData* data_202 = new t_FriendData();
	data_202->playerId = 202;
	data_202->playerName = "player_202";
	data_202->playerHeadId = 0;
	m_friends.insert(std::make_pair(data_202->playerId, data_202));
}
NS_WF_END
