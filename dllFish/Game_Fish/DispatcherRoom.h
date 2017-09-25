#pragma once
#include "WfConfig.h"
#include "Player.h"
#include "Dispatcher.h"
#include "Player.h"

NS_WF_BEGIN
class GameFish;
class DispatcherRoom : public Dispatcher<Player, DispatcherRoom>
{
private:
	GameFish* m_gameHandle;
public:
	DispatcherRoom(GameFish* handle);
	bool Init();
public:
	void EnterTable(Player* pPlayer, int32 tableId, int32 seatId);
	void EnterGameOk(Player* pPlayer, int32 gateWaySvrId,int32 gameId);
	void OnHandle_AGame_EnterGame(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
	void OnHandle_AGame_LeaveGame(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
	void OnHandle_CS_FishGameData(Player* pPlayer, PacketHeader* packet, Stream& stream);
	//½øÈë×À×Ó
	void OnHandle_CS_FishEnterTable(Player* pPlayer , PacketHeader* packet, Stream& stream);
};
NS_WF_END

