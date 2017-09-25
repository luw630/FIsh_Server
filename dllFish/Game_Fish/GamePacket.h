#pragma once
#include "WfDataType.h"
#include "WfPacketHeader.h"

NS_WF_BEGIN



enum emTableFishPacket
{
	emTableFishPacket_t_EnterTable = 1,				//��������
	emTableFishPacket_t_ReEnterTable = 2,			//���½�������
	emTableFishPacket_t_PlayerOffLine = 3,			//֪ͨTable�������

	//table to main thread
	emTableFishPacket_t_EnterTableRst= 4,
	emTableFishPacket_t_PlayerLeaveTable = 5,
};


class Player;
//��������
struct t_EnterTable : public PacketHeader 
{
	int32 m_seatId;			//λ��id
	Player* m_player;
};
//���½�������
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

//�������ӽ��
struct t_EnterTableRst : public PacketHeader
{
	int32 m_rst;			//0�ɹ���1λ����������
	int32 m_tableid;		//����id
	int32 m_seatId;			//λ��id
	Player* m_player;
};
enum emPlayerLeaveTableType
{
	emPlayerLeaveTableType_goHome = 0,				//0�뿪���ӣ��ص����䣨player�����뿪��
	emPlayerLeaveTableType_leaveFish = 1,			//�뿪��Ϸ
	emPlayerLeaveTableType_leaveFishSessionIdInvalid = 2,	//�뿪��Ϸ�����һỰʧЧ
};
struct t_PlayerLeaveTable : public PacketHeader
{
	
	//1�뿪������Ϸ
	//1�뿪������Ϸ����Ӧ�ĻỰid����Ͽ�
	int32 m_type;		
	t_sessionId m_sessionId;
	t_ObjId m_playerId;
	int32 m_tableid;		//����id
	int32 m_seatId;			//λ��id
};


NS_WF_END

