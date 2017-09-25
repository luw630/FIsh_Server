#pragma once

#include "WfCObjMgr.h"
#include "Table.h"
#include "Packets/Packet_Fish.pb.h"

NS_WF_BEGIN

struct t_seatData
{
	int32 m_seatId;		//位置编号
	t_ObjId m_playerId;	//玩家id
	string m_NickName;	//玩家昵称
	int32 m_headid;		//头像id

	void init(int32 seatId)
	{
		m_seatId = seatId;
		m_playerId = Invalid_ObjId;
		m_NickName = "";
		m_headid = 0;
	}
	void clear()
	{
		m_playerId = Invalid_ObjId;
		m_NickName = "";
		m_headid = 0;
	}
};
struct t_TableFishData
{
	int32 m_tableId;						//座子id
	t_seatData m_seatData[MAX_SEAT_NUM];	//座位数据
	void init(int32 tableId)
	{
		m_tableId = tableId;
		for (int i = 0; i < MAX_SEAT_NUM; i++){
			m_seatData[i].init(i);
		}
	}
	void dup(rpc::t_fish_table* data)
	{
		data->set_m_tableid(m_tableId);
		for (int i = 0; i < MAX_SEAT_NUM; i++)
		{
			t_seatData* pObj = &m_seatData[i];
			if (pObj->m_playerId != Invalid_ObjId)
			{
				rpc::t_fish_seat* seat = data->add_m_seats();
				seat->set_m_seatid(pObj->m_seatId);
				seat->set_m_playerid(pObj->m_playerId);
				seat->set_m_playername(pObj->m_NickName);
				seat->set_m_headid(pObj->m_headid);
			}
		}
	}
	void load(const rpc::t_fish_table& data)
	{
		int32 nSize = data.m_seats_size();
		for (int i = 0; i < nSize; i++)
		{
			const rpc::t_fish_seat& seat = data.m_seats(i);
			if (seat.m_seatid() >= 0 && seat.m_seatid() < MAX_SEAT_NUM)
			{
				t_seatData* pObj = &m_seatData[seat.m_seatid()];
				pObj->m_headid = seat.m_headid();
				pObj->m_playerId = seat.m_playerid();
				pObj->m_NickName = seat.m_playername();
			}
		}
	}
	void clear(int32 seatid)
	{
		if (seatid >= 0 && seatid < MAX_SEAT_NUM)
			m_seatData[seatid].clear();
	}
};
class GameFish;
class TableViewMgr 
{
private:
	GameFish* m_gameHandle;
public:
	TableViewMgr(GameFish* handle);
	~TableViewMgr();
public:
	bool Init(int32 num);
	t_TableFishData* getTableData(int32 tableid);
	int32 getTableNum() { return m_TableFishNum; }
private:
	t_TableFishData* m_pTableFish;
	int32 m_TableFishNum;		
};

NS_WF_END

