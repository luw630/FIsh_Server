#pragma once
#include "WfDataType.h"
#include "WfPacketHeader.h"

NS_WF_BEGIN



enum emTableFishPacket
{
	emTableFishPacket_t_EnterTable = 1,				//进入桌子
	emTableFishPacket_t_ReEnterTable = 2,			//重新进入桌子
	emTableFishPacket_t_PlayerOffLine = 3,			//通知Table玩家离线

	//table to main thread
	emTableFishPacket_t_EnterTableRst= 4,
	emTableFishPacket_t_PlayerLeaveTable = 5,
};


class Player;
//进入座子
struct t_EnterTable : public PacketHeader 
{
	int32 m_seatId;			//位置id
	Player* m_player;
};
//重新进入桌子
struct t_ReEnterTable : public PacketHeader
{
	Player* m_player;
};
struct t_PlayerOffLine : public PacketHeader
{
	t_ObjId m_playerId;
	t_ObjId m_seatId;
	t_sessionId m_sessionId;
};

//进入座子结果
struct t_EnterTableRst : public PacketHeader
{
	int32 m_rst;			//0成功，1位置上有人了
	int32 m_tableid;		//桌子id
	int32 m_seatId;			//位置id
	Player* m_player;
};
enum emPlayerLeaveTableType
{
	emPlayerLeaveTableType_goHome = 0,				//0离开桌子，回到房间（player主动离开）
	emPlayerLeaveTableType_leaveFish = 1,			//离开游戏
	emPlayerLeaveTableType_leaveFishSessionIdInvalid = 2,	//离开游戏，并且会话失效
};
struct t_PlayerLeaveTable : public PacketHeader
{
	
	//1离开捕鱼游戏
	//1离开捕鱼游戏，对应的会话id网络断开
	int32 m_type;		
	t_sessionId m_sessionId;
	t_ObjId m_playerId;
	int32 m_tableid;		//桌子id
	int32 m_seatId;			//位置id
};


NS_WF_END

