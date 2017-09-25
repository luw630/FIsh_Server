#include "FishResMgr.h"
#include "WfLog.h"
#include "PathController.h"
#include "WfCsvParse.h"
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "WfStringUtil.h"
#include "WfRandom.h"
USING_NS_WF;
FishResMgr::FishResMgr()
{
	m_fishPowerData = nullptr;
	m_fishPowerNum = 0;
}
bool FishResMgr::load()
{

	if (!loadFishData("")) {
		log_waring("loadFishData error");
		return false;
	}
	log_info("loadFishData ok");

	if (!loadPath()){
		log_waring("loadPath error");
		return false;
	}
	log_info("loadPath ok");

	if (!loadGroup()) {
		log_waring("loadGroup error");
		return false;
	}
	log_info("loadGroup ok");

	if (!loadBullet()) {
		log_waring("loadBullet error");
		return false;
	}
	log_info("loadGroup ok");

	if (!loadMap()) {
		log_waring("loadMap error");
		return false;
	}
	log_info("loadMap ok");
	
	if (!loadFishBattle()) {
		log_waring("loadFishBattle error");
		return false;
	}
	log_info("loadFishBattle ok");

	return true;
}
bool FishResMgr::loadFishData(const string& pcDir)
{
	t_fishData*	fishList[FISH_TYPE_MAX] = { nullptr };
	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = true;
	int num = 0;
	do {
		try
		{
			bool rst = parse.reaCsvNew("Data/t_FishBean.csv", pt);
			if (!rst) return false;

			for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
			{
				boost::property_tree::ptree row_of_list = (*iter).second;
				int32 id = boost::lexical_cast<int32_t>(row_of_list.get("[ID]", "0"));
				if (id < 0 || id >= FISH_TYPE_MAX) {
					log_waring("fish config error,id(%d)", id);
					rst = false;
					break;
				}
				else if (fishList[id] != nullptr) {
					log_waring("fish config repeat,id(%d)", id);
					rst = false;
					break;
				}
				t_fishData* pObj = new t_fishData();
				pObj->t_id = id;
				pObj->t_minFishRadio = boost::lexical_cast<int32_t>(row_of_list.get("minFishRadio", "0"));
				pObj->t_maxFishRadio = boost::lexical_cast<int32_t>(row_of_list.get("maxFishRadio", "0"));
				pObj->t_Stypes = boost::lexical_cast<int32_t>(row_of_list.get("Stypes", "0"));
				pObj->t_Rate = boost::lexical_cast<int32_t>(row_of_list.get("Rate", "0"));
				string value = row_of_list.get("Scenes", "0");
				std::vector<std::string> tmp = StringUtil::Split(value, ",");
				pObj->t_Scenes = 0;
				for (auto&t : tmp)
				{
					if (t.compare("0") == 0)
					{
						pObj->t_Scenes = pObj->t_Scenes | 1;
					}
					else if(t.compare("1") == 0){
						pObj->t_Scenes = pObj->t_Scenes | 2;
					}
					else if (t.compare("2") == 0) {
						pObj->t_Scenes = pObj->t_Scenes | 4;
					}
					else if (t.compare("3") == 0) {
						pObj->t_Scenes = pObj->t_Scenes | 8;
					}
					else {
						log_err("t_fishBean.csv item(%d) \"Scenes\" error", id);
						return false;
					}
				}
				pObj->t_FishPath = boost::lexical_cast<int32_t>(row_of_list.get("FishPath", "0"));
				pObj->t_Life = boost::lexical_cast<float>(row_of_list.get("Life", "0"))*boost::lexical_cast<float>(row_of_list.get("LifeMul", "0"));
				pObj->t_radius = boost::lexical_cast<float>(row_of_list.get("t_radius", "0"));
				

				fishList[pObj->t_id] = pObj;
				++num;
			}
		}
		catch (...) {
			rst = false;
			break;
		}
	} while (false);

	if (!rst) {
		for (int i = 0; i < FISH_TYPE_MAX; i++) {
			if (fishList[i] != nullptr) {
				delete fishList[i];
				fishList[i] = nullptr;
			}
		}
	}
	else {
		memcpy(m_FishDataList, fishList, sizeof(t_fishData*)*FISH_TYPE_MAX);
	}

	if (rst) {
		m_fishPowerNum = 0;
		for (int i = 0; i < FISH_TYPE_MAX; i++) {
			t_fishData* data = m_FishDataList[i];
			if (data != nullptr && data->t_Rate>0) {
				
				m_fishPowerNum += data->t_Rate;
			}
		}
		m_fishPowerData = new uint8[m_fishPowerNum];
		int offset = 0;
		for (int i = 0; i < FISH_TYPE_MAX; i++) {
			t_fishData* data = m_FishDataList[i];
			if (data != nullptr && data->t_Rate>0) {
				memset(m_fishPowerData + offset, i, data->t_Rate);
				offset += data->t_Rate;
			}
		}
	}
	return rst;
}
bool FishResMgr::loadPath()
{
	int type = -1;
	while (true) {
		int8 path[128] = { 0 };
		sprintf(path, "Data/BinaryFishPath/%d.binary", ++type);

		FILE* handle = fopen(path, "rb+");
		if (handle) {
			t_PathType* pathType = new t_PathType();
			fseek(handle, 0, SEEK_SET);
			int len = 0;
			len = ftell(handle);
			int32 num = 0;
			fread(&num, 4, 1, handle);			
			int pathDataNum = 0;
			
			len = ftell(handle); //文件长度
			while (num > 0)
			{
				
				PathData* path = new PathData(type,pathDataNum);
				path->m_NodeNum = num;
				path->m_NodeList = new Vector3[path->m_NodeNum];
				for (int i = 0; i < num;i++)
				{
					fread(&(path->m_NodeList[i].x), sizeof(float), 1, handle);
					fread(&(path->m_NodeList[i].y), sizeof(float), 1, handle);
					fread(&(path->m_NodeList[i].z), sizeof(float), 1, handle);
					len = ftell(handle); //文件长度
				}
				fread(&num, 4, 1, handle);
				len = ftell(handle); //文件长度
				PathController::BuildPath(path);
				pathType->m_PathList.push_back(path);

				++pathDataNum;
			}
			pathType->t_num = pathDataNum;
			pathType->t_id = type;
			fclose(handle);
			m_PathType[pathType->t_id] = pathType;
		}
		else {
			break;
		}
	}
	//PathData* path1 = new PathData();
	//path1->m_NodeNum = 5;
	//path1->m_NodeList = new Vector3[path1->m_NodeNum];
	//path1->m_NodeList[0] = Vector3(0, 4, 0);
	//path1->m_NodeList[1] = Vector3(2, 3, 0);
	//path1->m_NodeList[2] = Vector3(4, 4, 0);
	//path1->m_NodeList[3] = Vector3(6, 2, 0);
	//path1->m_NodeList[4] = Vector3(8, 5, 0);
	//m_PathList.push_back(path1);
	//PathController::BuildPath(path1);
	return true;
}
bool FishResMgr::loadGroup(const string& pcDir)
{
	{
		GroupData *groupData = new GroupData;
		m_GroupList.push_back(groupData);
		GroupData &pd = *groupData;

		////pd.FrontPosition;
		//pd.PathCount = 2;
		//pd.pPathList = new USHORT[pd.PathCount];
		//for (int i = 0; i < pd.PathCount; ++i)
		//{
		//	pd.pPathList[i] = 0;
		//	if (getPathData(pd.pPathList[i]) == nullptr)
		//	{
		//		log_waring("groupdate have not path(%d): %d", pd.pPathList[i]);
		//		return false;
		//	}
		//}
		pd.GroupDataCount = 2;
		pd.pGroupDataArray = new GroupData::GroupItem[pd.GroupDataCount];
		for (int i = 0; i < pd.GroupDataCount; ++i)
		{
			GroupData::GroupItem &gd = pd.pGroupDataArray[i];
			gd.FishIndex = i+1;	//鱼的索引
			
			if (getFishData(gd.FishIndex) == nullptr)
			{
				log_waring("not find fishData(%d)", gd.FishIndex);
				return false;
			}

			gd.FishNum = 2;		//鱼个数
			gd.pFishPosList = new Vector3[gd.FishNum];
			gd.pFishPosList[0] = Vector3(0, 0, 0);
			gd.pFishPosList[1] = Vector3(1, 1, 1);
		}
	}

	return true;
}
bool FishResMgr::loadBullet()
{
	t_bulletData*	bulletList[rpc::BulletType_max] = { nullptr };
	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = true;
	int num = 0;
	do {
		try
		{
			bool rst = parse.reaCsvNew("Data/t_BulletBean.csv", pt);
			if (!rst) return false;

			for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
			{
				boost::property_tree::ptree row_of_list = (*iter).second;
				int32 id = boost::lexical_cast<int32_t>(row_of_list.get("t_id", "0"));
				if (id < 0 || id >= rpc::BulletType_max) {
					log_waring("bullet config error,id(%d)", id);
					rst = false;
					break;
				}
				else if (bulletList[id] != nullptr) {
					log_waring("bullet config repeat,id(%d)", id);
					rst = false;
					break;
				}
				t_bulletData* pObj = new t_bulletData();
				pObj->t_id = id;
				pObj->t_range = boost::lexical_cast<float>(row_of_list.get("t_range", "0"));
				pObj->t_width = boost::lexical_cast<float>(row_of_list.get("t_width", "0"));
				pObj->t_radius = boost::lexical_cast<float>(row_of_list.get("t_radius", "0"));
				pObj->t_speed = boost::lexical_cast<float>(row_of_list.get("t_speed", "0"));
				pObj->t_times = boost::lexical_cast<int>(row_of_list.get("t_times", "0"));

				bulletList[pObj->t_id] = pObj;
				++num;
			}
		}
		catch (...) {
			rst = false;
			break;
		}
	} while (false);

	if (!rst) {
		for (int i = 0; i < rpc::BulletType_max; i++) {
			if (bulletList[i] != nullptr) {
				delete bulletList[i];
				bulletList[i] = nullptr;
			}
		}
	}
	else {
		memcpy(m_BulletList, bulletList, sizeof(m_BulletList));
	}
	return rst;
}

bool FishResMgr::loadMap()
{
	m_MapDatas.clear();

	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = true;

	try
	{
		bool rst = parse.reaCsvNew("Data/t_MapBean.csv", pt);
		if (!rst) return false;

		for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
		{
			boost::property_tree::ptree row_of_list = (*iter).second;
			int32 id = boost::lexical_cast<int32_t>(row_of_list.get("t_id", "0"));

			t_MapData* pObj = new t_MapData();
			pObj->t_id = id;
			bool rst = false;
			do {
				if (!ParseVector3(row_of_list.get("t_LeftTop", ""), pObj->t_LeftTop)) break;
				if (!ParseVector3(row_of_list.get("t_RightBottom", ""), pObj->t_RightBottom)) break;
				pObj->t_batteryLen = boost::lexical_cast<float>(row_of_list.get("t_batteryLen", "0"));
				pObj->t_SeatNum = boost::lexical_cast<int32_t>(row_of_list.get("t_SeatNum", "0"));
				if (pObj->t_SeatNum < 1 || pObj->t_SeatNum>SEAT_NUM_MAX) { break; }
				bool bReadSeat = true;
				for (int i = 0; i < pObj->t_SeatNum; i++) {
					char buff[64] = { 0 };
					sprintf(buff, "t_Seat%d", i + 1);
					if (!ParseVector3(row_of_list.get(buff, ""), pObj->t_Seat[i])) { bReadSeat = false; break;}
				}
				rst = bReadSeat && true;
			} while (false);
			if (!rst) {
				log_waring("MapData(%d) error", id);
				delete pObj;
				return false;
			}
			else {
				m_MapDatas[pObj->t_id] = pObj;
			}
		}
	}
	catch (...) {
		rst = false;
	}
	return rst;
}
bool FishResMgr::loadGroup()
{
	t_GroupData*	groupList[FISH_GROUP_MAX] = { nullptr };
	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = true;
	int num = 0;
	do {
		try
		{
			bool rst = parse.reaCsvNew("Data/t_FishGroup.csv", pt);
			if (!rst) return false;

			for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
			{
				boost::property_tree::ptree row_of_list = (*iter).second;
				int32 id = boost::lexical_cast<int32_t>(row_of_list.get("id", "0"));
				if (id < 0 || id >= FISH_GROUP_MAX) {
					log_waring("t_fishGroup config error,id(%d)", id);
					rst = false;
					break;
				}
				else if (groupList[id] != nullptr) {
					log_waring("t_fishGroup config repeat,id(%d)", id);
					rst = false;
					break;
				}
				t_GroupData* pObj = new t_GroupData();
				pObj->m_id = id;
				pObj->m_fishId = boost::lexical_cast<int>(row_of_list.get("fishid", "0"));
				pObj->m_intervalTime = boost::lexical_cast<int>(row_of_list.get("intervalTime", "0"));
				pObj->m_perLineFishNum = boost::lexical_cast<int>(row_of_list.get("perLineFishNum", "0"));
				pObj->m_PathType = boost::lexical_cast<int>(row_of_list.get("path", "0"));
				
				groupList[pObj->m_id] = pObj;
				++num;
			}
		}
		catch (...) {
			rst = false;
			break;
		}
	} while (false);

	if (!rst) {
		for (int i = 0; i < FISH_GROUP_MAX; i++) {
			if (groupList[i] != nullptr) {
				delete groupList[i];
				groupList[i] = nullptr;
			}
		}
	}
	else {
		memcpy(m_GroupListNew, groupList, sizeof(m_GroupListNew));
		m_GroupListNewIndex.clear();
		for (int i = 0; i < FISH_GROUP_MAX; i++) {
			if (groupList[i] != nullptr) {
				m_GroupListNewIndex.push_back(i);
			}
		}
	}
	return rst;
}
bool FishResMgr::loadFishBattle()
{
	t_bulletData*	bulletList[rpc::BulletType_max] = { nullptr };
	boost::property_tree::ptree pt;
	CsvParse parse;
	bool rst = true;
	int num = 0;
	do {
		try
		{
			bool rst = parse.reaCsvNew("Data/t_FishBattleBeen.csv", pt);
			if (!rst) return false;

			for (boost::property_tree::ptree::iterator iter = pt.begin(); iter != pt.end(); ++iter)
			{
				boost::property_tree::ptree row_of_list = (*iter).second;

				m_CfgFishBattle.t_id = boost::lexical_cast<int>(row_of_list.get("t_id", "1"));
				m_CfgFishBattle.t_screenFishNum = boost::lexical_cast<int>(row_of_list.get("t_screenFishNum", "30"));
				m_CfgFishBattle.t_generalLauncherTime = boost::lexical_cast<int>(row_of_list.get("t_generalLauncherTime", "180")) * 1000;
				m_CfgFishBattle.t_changeBossTime = boost::lexical_cast<int>(row_of_list.get("t_changeBossTime", "10")) * 1000;
				m_CfgFishBattle.t_bossLifeTime = boost::lexical_cast<int>(row_of_list.get("t_bossLifeTime", "60")) * 1000;
				m_CfgFishBattle.t_changeLevelTime = boost::lexical_cast<int>(row_of_list.get("t_changeLevelTime", "5")) * 1000;
				break;
			}
		}
		catch (...) {
			rst = false;
			break;
		}
	} while (false);
	return rst;
}
GroupData* FishResMgr::getGroupData(int32 index)
{
	if (index >= m_GroupList.size()) return nullptr;
	return m_GroupList[index];
}
PathData* FishResMgr::getPathData(int32 index)
{
	if (index >= m_PathList.size()) return nullptr;
	return m_PathList[index];	
}
t_MapData* FishResMgr::getMapData(int32 mapId)
{
	auto t = m_MapDatas.find(mapId);
	if (t == m_MapDatas.end()) {
		return nullptr;
	}
	return t->second;
}
t_fishData* FishResMgr::getFishData(int32 index)
{
	if (index<0 || index >= FISH_TYPE_MAX) return nullptr;
	return m_FishDataList[index];
}
t_bulletData* FishResMgr::getBulletData(int32 index)
{
	if (index<0 ||index >= rpc::BulletType_max) return nullptr;
	return m_BulletList[index];
}
t_PathType* FishResMgr::getPathType(int32 id)
{
	auto t = m_PathType.find(id);
	if (t == m_PathType.end())
	{
		return nullptr;
	}
	return t->second;
}
t_fishData* FishResMgr::randomFish()
{
	int32 tmp = Random::RandomValue();
	tmp = tmp%m_fishPowerNum;
	return m_FishDataList[m_fishPowerData[tmp]];
}
t_GroupData* FishResMgr::getGroupDataNew(int32 index)
{
	if (index<0 || index >= FISH_GROUP_MAX) return nullptr;
	return m_GroupListNew[index];
}
t_GroupData* FishResMgr::randomGroupData()
{
	int32 value = Random::RandomInt(0, m_GroupListNewIndex.size() - 1);
	return getGroupDataNew(value);
}
const t_CfgFishBattle* FishResMgr::getCfgFishBattle()
{
	return &m_CfgFishBattle;
}