#include "Seat.h"
#include "../Table.h"
NS_WF_BEGIN
Seat::Seat()
{
	m_player = nullptr;
	m_table = nullptr;
}
Seat::~Seat()
{

}
void Seat::Init(int32 id, Table* table)
{
	m_ObjectId = id;
	m_table = table;
}
bool Seat::havePlayer()
{
	if (m_player == nullptr)
		return false;
	return true;
}
bool Seat::bindPlayer(Player* pPlayer)
{
	m_player = pPlayer;
	pPlayer->setTableId(m_table->getId());
	pPlayer->setSeatId(getId());
	return true;
}
void Seat::unbindPlayer()
{
	if (m_player)
	{
		m_player->setTableId(Invalid_ObjId);
		m_player->setSeatId(Invalid_ObjId);
		m_player = nullptr;
	}
}
void Seat::dup(rpc::SC_FishNotifyAddPlayer* data)
{
	if (m_player)
	{
		data->set_m_playerid(m_player->getId());
		data->set_m_playername(m_player->getNickName());
		data->set_m_seatid(getId());
		data->set_m_bomb(m_player->getBombType());
	}else {
		data->set_m_seatid(getId());
		data->set_m_playerid(-1);
		data->set_m_playername("");
		data->set_m_bomb(1);
	}

}
void Seat::dup(rpc::SC_FishNotifyAddPlayerMySelf* data)
{
	if (m_player)
	{
		data->set_m_seatid(getId());
		data->set_m_playerid(m_player->getId());
		data->set_m_playername(m_player->getNickName());
		data->set_m_bulletnum(100);
		data->set_m_bomb(m_player->getBombType());
	}
	else {
		data->set_m_seatid(getId());
		data->set_m_playerid(-1);
		data->set_m_playername("");
		data->set_m_bulletnum(100);
		data->set_m_bomb(1);

	}
	data->set_m_seatid(getId());

	data->set_m_bulletnum(100);
}
void Seat::dup(rpc::t_FishPlayer* data)
{
	if (m_player)
	{
		data->set_m_seatid(m_ObjectId);
		data->set_m_playerid(m_player->getId());
		data->set_m_playername(m_player->getNickName());
		data->set_m_headid(m_player->getHeadId());
	}
}
Player* Seat::getPlayer()
{
	return m_player;
}
t_ObjId Seat::getPlayerId()
{
	if (m_player)
	{
		return m_player->getId();
	}
	return Invalid_ObjId;
}
NS_WF_END

