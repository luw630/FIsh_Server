#pragma once

#include "WfCObjMgr.h"
#include "Table.h"

NS_WF_BEGIN

class GameFish;
class TableMgr 
{
private:
	GameFish* m_gameHandle;
public:
	TableMgr(GameFish* handle);
	~TableMgr();
public:
	bool Init(IGameService* service,int32 num,int32 tableNum);
	Table* getTable(int32 tableid);
	int32 getTableNum() { return m_TableFishNum; }
private:
	Table* m_pTableFish;
	int32 m_TableFishNum;		//×À×Ó¸öÊý
};

NS_WF_END

