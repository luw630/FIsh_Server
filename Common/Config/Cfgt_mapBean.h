#pragma once
#include "WfDataType.h"
#include <unordered_map>

using namespace std;

class Cfgt_mapBeanMgr;
struct STt_mapBean
{
	friend class Cfgt_mapBeanMgr;
	
	int32 m_t_id;	//Id
	string m_t_name;	//名字
	string m_t_prefab;	//资源名(寻路地形scene1_path.bytes)
	string m_t_jubian;	//剧情或者视频
};

class Cfgt_mapBeanMgr
{
	friend class LoadCfg;
public:
	Cfgt_mapBeanMgr(){};
	~Cfgt_mapBeanMgr(){};
	SINGLETON_MODE(Cfgt_mapBeanMgr);

	const STt_mapBean* getItem(int32 id)
	{
		unordered_map<int32, STt_mapBean*>::iterator iter = m_mapData.find(id);
		if (iter != m_mapData.end())
		{
			return iter->second;
		}
		return nullptr;
	}
private:
	bool loadCfg();
	unordered_map<int32,STt_mapBean*> m_mapData;
};
