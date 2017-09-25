#include "TableViewMgr.h"
#include "GameFish.h"

NS_WF_BEGIN
TableViewMgr::TableViewMgr(GameFish* handle)
{
	m_gameHandle = handle;
	m_pTableFish = nullptr;
	m_TableFishNum = 0;
}
TableViewMgr::~TableViewMgr()
{

}
bool TableViewMgr::Init(int32 num)
{
	m_pTableFish = new t_TableFishData[num];
	m_TableFishNum = num;
	for (int i = 0; i < num; i++)
	{
		m_pTableFish[i].init(i);
	}
	return true;
}
t_TableFishData* TableViewMgr::getTableData(int32 tableid)
{
	if (tableid < 0 && tableid >= m_TableFishNum) return nullptr;
	return &m_pTableFish[tableid];
}
NS_WF_END

