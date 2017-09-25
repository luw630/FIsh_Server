#include "Cfg_Game.h"
#include <json/json.h>
#include "WfSysUtil.h"
#include "WfLog.h"
#include "WfCsvParse.h"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "FishResMgr.h"

NS_WF_BEGIN

Cfg_Game::Cfg_Game()
{
	m_TableNum = 100;
}
bool Cfg_Game::load()
{
	Json::Value root;
	Json::Reader jsonReader;
	bool ok = jsonReader.parse(SysUtil::ReadFileCPlus("Config/Base.cfg"), root);

	if (ok)
	{
		m_TableNum = root.get("TableNum", 100).asInt();
		m_MaxPlayer = root.get("MaxPlayerNum", 2000).asInt();
		m_MapId = root.get("MapId", 1).asInt();
	}else {
		log_err("DbConnectionPool db.cfg error");
		return false;
	}
	int32 ScoreCfgIndex = root.get("ScoreCfgIndex", 1).asInt();
	ok = loadScoreCfg(ScoreCfgIndex);
	if (!ok) return false;

	m_pMapData = FishResMgr::GetInstance().getMapData(m_MapId);
	if (!m_pMapData) {
		log_waring("not find mapres(%d)", m_MapId);
		return false;
	}
	return true;
}
bool Cfg_Game::loadScoreCfg(int32 index)
{
	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = false;
	try
	{
		rst = parse.reaCsvNew("Data/t_ScoreBean.csv", pt);
		if (!rst) {
			log_waring("Data/t_ScoreBean.csv error");
			return false;
		} 

		for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
		{
			boost::property_tree::ptree row_of_list = (*iter).second;
			int32 id = boost::lexical_cast<int32_t>(row_of_list.get("t_id", "0"));
			if (id != index) {
				continue;
			}

			t_step = boost::lexical_cast<int32_t>(row_of_list.get("t_step", "0"));
			t_placeType = boost::lexical_cast<int32_t>(row_of_list.get("t_placeType", "0"));
			t_radio = boost::lexical_cast<int32_t>(row_of_list.get("t_radio", "0"));
			t_minScore = boost::lexical_cast<int32_t>(row_of_list.get("t_minScore", "0"));
			t_maxScore = boost::lexical_cast<int32_t>(row_of_list.get("t_maxScore", "0"));
			t_coinPoint = boost::lexical_cast<int32_t>(row_of_list.get("t_coinPoint", "0"));
			t_totalGunPoints = boost::lexical_cast<int32_t>(row_of_list.get("t_totalGunPoints", "0"));
			t_totalFishPoints = boost::lexical_cast<int32_t>(row_of_list.get("t_totalFishPoints", "0"));
			rst = true;
			break;
		}
	}
	catch (...) {
		log_waring("Data/t_ScoreBean.csv error");
	}

	return rst;
}

NS_WF_END

