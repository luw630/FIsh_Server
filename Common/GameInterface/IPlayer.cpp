#include "IPlayer.h"
NS_WF_BEGIN
IPlayer::IPlayer(IGame* pGame)
{
	m_IGame = pGame;
	m_PlayerStatus = -1;
	m_gateWayId = -1;
	m_Verify = false;
}
void IPlayer::setVerify(int32 gateWayId/* = -1*/)
{ 
	m_gateWayId = gateWayId;
	if (m_gateWayId == -1)
	{
		m_Verify = false;
	}else {
		m_Verify = true;
	}
}
bool IPlayer::SendPacket(PacketHeader* packet)
{
	return m_IGame->m_AppProxy->SendPacket(m_gateWayId, packet);
}

NS_WF_END

