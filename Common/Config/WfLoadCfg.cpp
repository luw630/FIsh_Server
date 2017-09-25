#include "WfLoadCfg.h"
#include "WfCsvParse.h"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "WfLog.h"


#include "Cfgt_mapBean.h"
#include "Cfgt_jobBean.h"
USING_NS_WF;

bool Cfgt_mapBeanMgr::loadCfg()
{
	for (unordered_map<int32, STt_mapBean*>::iterator iter = m_mapData.begin();iter != m_mapData.end();iter++)
	{
		delete iter->second;
	}
	m_mapData.clear();
	boost::property_tree::ptree pt;
	CsvParse parse;
	std::string strTemp;
	try
	{
		bool rst = parse.reaCsvNew("Data/t_mapBean.csv", pt);
		if (!rst) return false;
		for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
		{
			STt_mapBean* pObj = new STt_mapBean();
			boost::property_tree::ptree row_of_list = (*iter).second;
			
			pObj->m_t_id = boost::lexical_cast<int32_t>(row_of_list.get("t_id", "0").empty()?"0":row_of_list.get("t_id", "0"));
			pObj->m_t_name = row_of_list.get("t_name", "");
			pObj->m_t_prefab = row_of_list.get("t_prefab", "");
			pObj->m_t_jubian = row_of_list.get("t_jubian", "");

			unordered_map<int32, STt_mapBean*>::iterator mapIter = m_mapData.find(pObj->m_t_id);
			if (mapIter != m_mapData.end()){
				log_waring("t_mapBean have repeat");
				delete pObj;
			}else {
				m_mapData.insert(make_pair(pObj->m_t_id, pObj));
			}
		}
	}catch (...){
		return false;
	}
	return true;
}
bool Cfgt_jobBeanMgr::loadCfg()
{
	for (unordered_map<int32, STt_jobBean*>::iterator iter = m_mapData.begin();iter != m_mapData.end();iter++)
	{
		delete iter->second;
	}
	m_mapData.clear();
	boost::property_tree::ptree pt;
	CsvParse parse;
	std::string strTemp;
	try
	{
		bool rst = parse.reaCsvNew("Data/t_jobBean.csv", pt);
		if (!rst) return false;
		for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
		{
			STt_jobBean* pObj = new STt_jobBean();
			boost::property_tree::ptree row_of_list = (*iter).second;
			
			pObj->m_t_id = boost::lexical_cast<int32_t>(row_of_list.get("t_id", "0").empty()?"0":row_of_list.get("t_id", "0"));
			pObj->m_t_render = boost::lexical_cast<int32_t>(row_of_list.get("t_render", "0").empty()?"0":row_of_list.get("t_render", "0"));
			pObj->m_t_name = row_of_list.get("t_name", "");
			pObj->m_t_desc = row_of_list.get("t_desc", "");
			pObj->m_t_skill = row_of_list.get("t_skill", "");

			unordered_map<int32, STt_jobBean*>::iterator mapIter = m_mapData.find(pObj->m_t_id);
			if (mapIter != m_mapData.end()){
				log_waring("t_jobBean have repeat");
				delete pObj;
			}else {
				m_mapData.insert(make_pair(pObj->m_t_id, pObj));
			}
		}
	}catch (...){
		return false;
	}
	return true;
}
bool LoadCfg::loadConfig()
{
	m_bLoad = false;
	do
	{
		bool rst = false;
		rst = Cfgt_mapBeanMgr::GetInstance().loadCfg(); if (!rst) { log_err("load t_mapBean error"); break; }
		rst = Cfgt_jobBeanMgr::GetInstance().loadCfg(); if (!rst) { log_err("load t_jobBean error"); break; }
		m_bLoad = true;
	} while (false);
	return m_bLoad;
}
