#include "MailModule.h"


NS_WF_BEGIN

MailModule::MailModule()
{

}
MailModule::~MailModule()
{

}
void MailModule::init(const rpc::t_MailDatas* mailData)
{
	int size = mailData->mails_size();
	for (int i = 0; i < size; i++) {
		const rpc::t_MailItemData& obj = mailData->mails(i);
		t_MailData* item = new t_MailData();
		item->load(obj);
		m_mails.push_back(item);
	}
}
//void MailModule::dump(::google::protobuf::RepeatedPtrField< rpc::t_FriendItem >* datas)
//{
//	for (auto&t : m_friends)
//	{
//		t_FriendData* obj = t.second;
//		rpc::t_FriendItem* data = datas->Add();
//		data->set_m_playerid(obj->playerId);
//		data->set_m_name(obj->playerName.data);
//		data->set_m_money(obj->money);
//		data->set_m_headid(obj->playerHeadId);
//	}
//}
NS_WF_END
