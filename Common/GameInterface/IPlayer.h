#pragma once

#include "WfCObjMgr.h"
#include "WfNetMgr.h"
#include "IGame.h"
using namespace std;
NS_WF_BEGIN

class IPlayer : public CObj
{
protected:
	IGame* m_IGame;
public:
	IPlayer(IGame* pGame) {
		m_IGame = pGame;
		m_gateWayId = -1;
	}
	virtual ~IPlayer() {};

	virtual void OnEventEnterGame() = NULL;			//进入游戏事件
	virtual bool SendPacket(PacketCommand cmd, ::google::protobuf::Message* packet) = NULL;
public:
	void setGateWayId(int32 gateWayId) { m_gateWayId = gateWayId; }
	bool SendMsg(PacketHeader* packet) {
		return m_IGame->m_AppProxy->SendPacket(m_gateWayId, packet);
	}
private:
	int32 m_gateWayId;		//网关id
};

NS_WF_END

