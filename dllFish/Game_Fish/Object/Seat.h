#pragma once
#include "WfCObjMgr.h"
#include "Packets/Packet_Fish.pb.h"
#include "../GamePacket.h"

NS_WF_BEGIN

class Table;
class Seat : public CObj
{
public:
	Seat();
	~Seat();
public:
	void Init(int32 id, Table* table);
	bool havePlayer();
	bool bindPlayer(Player* pPlayer);
	void unbindPlayer();
	void dup(rpc::SC_FishNotifyAddPlayer* data);
	void dup(rpc::SC_FishNotifyAddPlayerMySelf* data);
	void dup(rpc::t_FishPlayer* data);
public:
	Player* getPlayer();
	t_ObjId getPlayerId();
private:
	Table* m_table;
	Player* m_player;
};

NS_WF_END

