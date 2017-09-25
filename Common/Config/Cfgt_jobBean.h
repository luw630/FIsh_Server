#pragma once
#include "WfDataType.h"
#include <unordered_map>

class Cfgt_jobBeanMgr;
struct STt_jobBean
{
	friend class Cfgt_jobBeanMgr;
	
	int32 m_t_id;	//Id
	int32 m_t_render;	//
	string m_t_name;	//
	string m_t_desc;	//
	string m_t_skill;	//¼¼ÄÜid
};

class Cfgt_jobBeanMgr
{
	friend class LoadCfg;
public:
	Cfgt_jobBeanMgr(){};
	~Cfgt_jobBeanMgr(){};
	SINGLETON_MODE(Cfgt_jobBeanMgr);

	const STt_jobBean* getItem(int32 id)
	{
		unordered_map<int32, STt_jobBean*>::iterator iter = m_mapData.find(id);
		if (iter != m_mapData.end())
		{
			return iter->second;
		}
		return nullptr;
	}
private:
	bool loadCfg();
	unordered_map<int32,STt_jobBean*> m_mapData;
};
