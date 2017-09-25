#include "Rank.h"
#include "WfLog.h"

NS_WF_BEGIN

Rank::Rank()
{

}
Rank::~Rank()
{

}
bool Rank::init()
{
	memset(&m_ranks, 0, sizeof(m_ranks));
	m_ranksNum = 0;
	m_saveTime = 0;
	m_needSave = false;
	return true;
}
void Rank::load()
{
	m_ranksNum = 0;
	FILE* handle = fopen("./db/RankData.db", "rb+");
	if (handle) {
		fseek(handle, 0, SEEK_SET);
		int32 num = 0;
		fread(&num, 4, 1, handle);
		num = num > MAX_RANK_NUM ? MAX_RANK_NUM : num;
		for (int i = 0; i < num; i++)
		{
			RankItem* obj = new RankItem();
			obj->DeSerialize(handle);
			m_ranks[m_ranksNum] = obj;
			m_rankMap[obj->playerId] = obj;
			++m_ranksNum;
		}
		fclose(handle);
	}
	m_saveTime = 0;
	m_needSave = false;
}
void Rank::save()
{
	if (!m_needSave) return;

	FILE* handle = fopen("./db/RankData.db.bak", "w+");
	if (handle) {
		fwrite(&m_ranksNum, 4, 1, handle);
		for (int i=0;i<m_ranksNum;i++)
		{
			m_ranks[i]->Serialize(handle);
		}
		fclose(handle);
		remove("./db/RankData.db");
		if (rename("./db/RankData.db.bak", "./db/RankData.db")) {
			log_waring("error");
		}
	}
}
void Rank::update(uint delta)
{
	int dealNum = 0;
	while (!m_RcvPostData.empty() && dealNum<20)
	{
		++dealNum;
		RankItem* obj = m_RcvPostData.front();
		m_RcvPostData.pop_front();
		onEventPlayerMoneyChange(obj);
	}

	m_saveTime += delta;
	if (m_saveTime < 5000) return;

	m_saveTime -= 5000;
	save();
}
void Rank::dump(::google::protobuf::RepeatedPtrField< rpc::t_RankItem >* datas)
{
	int num = m_ranksNum > MAX_RANK_SHOWNUM ? MAX_RANK_SHOWNUM : m_ranksNum;
	for (int i = 0; i < num; i++)
	{
		RankItem* obj = m_ranks[i];
		Assert(obj);
		rpc::t_RankItem* data = datas->Add();
		data->set_m_index(i+1);
		data->set_m_playerid(obj->playerId);
		data->set_m_name(obj->playerName);
		data->set_m_money(obj->money);
		data->set_m_headid(obj->playerHeadId);
	}
}
void Rank::postPlayerMoneyChange(t_PlayerId playerId, std::string	playerName, int32 headId, int64 money)
{
	RankItem* data = new RankItem();
	data->money = money;
	data->playerId = playerId;
	data->playerName = playerName;
	data->playerHeadId = headId;
	m_RcvPostData.push_back(data);
}
void Rank::onEventPlayerMoneyChange(RankItem* data)
{
	auto t = m_rankMap.find(data->playerId);
	if (t != m_rankMap.end())
	{
		RankItem* oldObj = t->second;
		//ÊÍ·Å¶ÔÏó
		m_rankMap.erase(t);
		int32 moveNum = m_ranksNum - oldObj->idx - 1;
		if (moveNum>0) {
			memmove(m_ranks + oldObj->idx, m_ranks + oldObj->idx + 1, moveNum*sizeof(RankItem*));
		}
		--m_ranksNum;//99
		m_ranks[m_ranksNum] = nullptr;//
		delete oldObj;
	}
	if (m_ranksNum == 0){
		m_ranks[m_ranksNum] = data;
		++m_ranksNum;
	}else{
		bool bEnter = false;
		if (m_ranksNum == MAX_RANK_NUM)
		{
			if (data->money > m_ranks[m_ranksNum - 1]->money){	
				RankItem* needMve = m_ranks[m_ranksNum - 1];
				--m_ranksNum;
				delete needMve;
				bEnter = true;
			}
		}else {
			bEnter = true;
		}

		if (bEnter){
			int i = m_ranksNum;
			while (i > 0)
			{
				if (data->money > m_ranks[i - 1]->money) {
					m_ranks[i] = m_ranks[i - 1];
				}
				else {
					break;
				}
				--i;
			}
			m_ranks[i] = data;
			++m_ranksNum;
		}
		else {
			delete data;
		}
	}
	for (int i = 0; i < m_ranksNum; i++){
		m_ranks[i]->idx = i;
	}
}
NS_WF_END
