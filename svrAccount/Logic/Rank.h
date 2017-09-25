#pragma once
#include "WfDataType.h"
#include <google/protobuf/message.h>
#include "Packets/Packet_Lobby.pb.h"

NS_WF_BEGIN
#define MAX_RANK_NUM		100	//排行榜最多个数
#define MAX_RANK_SHOWNUM	10	//排行榜显示个数

struct RankItem
{
	uint32 idx;	//索引
	t_PlayerId		playerId;
	std::string		playerName;
	int32 playerHeadId;				//头像id
	int64 money;
	void Serialize(FILE* file)
	{
		fwrite(&playerId, sizeof(t_PlayerId), 1, file);
		int len = playerName.size();
		fwrite(&len, sizeof(int), 1, file);
		if (len > 0){
			fwrite(playerName.data(), 1, len, file);
		}
		fwrite(&playerHeadId, sizeof(int32), 1, file);
		fwrite(&money, sizeof(int64), 1, file);
	}
	void DeSerialize(FILE* file)
	{
		fread(&playerId, sizeof(t_PlayerId), 1, file);
		char buff[128] = { 0 };
		int len = 0;
		fread(&len, sizeof(int), 1, file);
		if (len > 0) {
			playerName.resize(len);
			fread((char*)playerName.data(), 1, len, file);
		}else {
			playerName = "";
		}
		fread(&playerHeadId, sizeof(int32), 1, file);
		fread(&money, sizeof(int64), 1, file);
	}
};

class Rank 
{
public:
	Rank();
	virtual ~Rank();
	bool init();
	void load();//从文件中导入排行榜
	void save();
	void update(uint delta);
	void dump(::google::protobuf::RepeatedPtrField< rpc::t_RankItem >* datas);
	//RankItem要外部创建，处理时回收内存
	void postPlayerMoneyChange(t_PlayerId playerId, std::string	playerName, int32 headId, int64 money);
	void onEventPlayerMoneyChange(RankItem* data);
private:
	std::hash_map<t_PlayerId, RankItem*> m_rankMap;
	RankItem* m_ranks[MAX_RANK_NUM];
	int32 m_ranksNum;
	uint32 m_saveTime;
	bool m_needSave;
	std::list<RankItem*> m_RcvPostData;			
};


NS_WF_END