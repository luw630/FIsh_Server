#include "TableMgr.h"
#include "GameFish.h"

NS_WF_BEGIN
TableMgr::TableMgr(GameFish* handle)
{
	m_gameHandle = handle;
	m_pTableFish = nullptr;
	m_TableFishNum = 0;
}
TableMgr::~TableMgr()
{
	WF_SAFE_DELETE_ARRAY(m_pTableFish);
}
bool TableMgr::Init(IGameService* service, int32 num, int32 tableNum)
{
	m_pTableFish = new Table[tableNum];
	m_TableFishNum = tableNum;
	
	//≥ı ºªØtable
	for (int32 i = 0; i < m_TableFishNum; i++){
		int32 tmp = i % num;
		if (!m_pTableFish[i].Init(i,&service[tmp], m_gameHandle))
		{
			log_err("init table(%d) error", i);
		}else {
			service[tmp].pushTable(&m_pTableFish[i]);
			m_pTableFish[i].setData(100, 100, 1, 100, 1);
		}
	}
	return true;
}
Table* TableMgr::getTable(int32 tableid)
{
	if (tableid < 0 || tableid >= m_TableFishNum) return nullptr;
	return &m_pTableFish[tableid];
}
NS_WF_END

