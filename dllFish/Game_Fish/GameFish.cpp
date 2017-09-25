#include "GameFish.h"
#include "Config/WfLoadCfg.h"
#include "FishResMgr.h"
#include "Packets/Packet_Server.pb.h"


NS_WF_BEGIN
extern "C"
{
	IGame* _stdcall CreateGame(IAppProxy* AppProxy)
	{
		return new GameFish(AppProxy);
	}
}


GameFish::GameFish(IAppProxy* AppProxy):IGame(AppProxy)
{
	m_AppProxy = AppProxy;
	m_pTableViewMgr = nullptr;
	m_pTableMgr = nullptr;
	m_PlayerMgr = nullptr;
	m_CfgGame = nullptr;
	m_DispatcherFish = nullptr;
}
GameFish::~GameFish()
{

}
bool GameFish::Init(int32 workThreadNum)
{
	if (workThreadNum <= 0 && workThreadNum > 50) {
		log_waring("workThreadNum(%d) error", workThreadNum);
		return false;
	}
		
	m_GameServiceNum = workThreadNum;
	if (m_GameServiceNum > 0) {
		m_pGameService = new GameService[m_GameServiceNum];
		for (int32 i = 0; i < m_GameServiceNum; i++)
		{
			m_pGameService[i].init(i, this);
		}
	}
	else {
		log_waring("work thread num(%d) error", m_GameServiceNum);
		return false;
	}
	return OnInit();
}
//开始服务器
void GameFish::Start()
{
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].start();
	}
}
//关闭服务
void GameFish::Stop()
{
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].stop();
	}
}

bool GameFish::OnInit()
{
	m_pTableViewMgr = new TableViewMgr(this);
	m_pTableMgr = new TableMgr(this);
	m_DispatcherFish = new DispatcherRoom(this);
	m_PlayerMgr = new PlayerMgr(this);
	m_CfgGame = new Cfg_Game();

	if (!FishResMgr::GetInstance().load())
	{
		log_waring("load resource error.");
		return false;
	}
	log_info("load resource ok");
	
	if (!m_CfgGame->load()){
		log_waring("load Config/Base.cfg error.");
		return false;
	}
	log_info("load Config/Base.cfg ok");

	if (!m_DispatcherFish->Init()){
		return false;
	}

	if (!m_pTableMgr->Init(m_pGameService, m_GameServiceNum, m_CfgGame->getTableNum())){
		log_waring("init tableMgr failed.");
		return false;
	}
	if (!m_pTableViewMgr->Init(m_CfgGame->getTableNum())) {
		log_waring("init TableViewMgr failed.");
		return false;
	}

	return true;
}


void GameFish::OnUpdate(uint delta)
{
	//更新工作线程的包
	for (int32 i = 0; i < m_GameServiceNum; i++)
	{
		m_pGameService[i].update();
	}
}
void GameFish::OnWorkThreadPacket(PacketHeader* packet)
{
	if (packet->m_Command > rpc::em_Fish_Begin)
	{
		Player* pPlayer = m_PlayerMgr->getObjectById(packet->m_PacketParam);
		if (pPlayer) {
			if (!pPlayer->SendMsg(packet))
			{
				log_waring("send failed");
			}
		}else {
			log_waring("cmd(%d) not find playerObj(%d)", packet->m_Command, packet->m_PacketParam);
		}
	}else {
		if (packet->m_Command == emTableFishPacket_t_EnterTableRst)
		{
			OnHandlePlayerEnterTable(packet);
		}else if (packet->m_Command == emTableFishPacket_t_PlayerLeaveTable) {
			OnHandlePlayerLeaveTable(packet);
		}		
	}
}

void GameFish::OnAccountPacket(uint32 srcId, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam)
{
	if (packet->m_Command == rpc::EM_AGame_EnterGame) {
		m_DispatcherFish->OnHandle_AGame_EnterGame(srcId, packet, stream, msgParamLen, msgParam);	
	}else if (packet->m_Command == rpc::EM_AGame_LeaveGame){
		m_DispatcherFish->OnHandle_AGame_LeaveGame(srcId, packet, stream, msgParamLen, msgParam);
	}
}
void GameFish::OnGateWayPacket(ISessionGateWay* pSession, PacketHeader* packet, Stream& stream)
{
	Player* pPlayer = m_PlayerMgr->getObjectById(packet->m_PacketParam);
	if (pPlayer)
	{   
		auto handler = m_DispatcherFish->getHandle(packet->m_Command); 
		if (handler != nullptr) {
			(m_DispatcherFish->*handler)(pPlayer, packet, stream);
		}else if(packet->m_Command > rpc::em_SC_FishEnterTableRst){
			if (pPlayer->getStatus() == emPlayerStatus_Game)
			{
				Table* pTable = m_pTableMgr->getTable(pPlayer->getTableId());
				if (!pTable) {
					log_err("not find table(%d)", pPlayer->getTableId());
					return;
				}
				abs* buffer = pTable->newBuffer_Rcv();
				*(t_ObjId*)(buffer->getData()) = pPlayer->getSeatId();
				memcpy(buffer->getData()+sizeof(t_ObjId), packet, packet->m_Length);
				pTable->pushMsg(buffer);
			}else {
				log_waring("player in table,cand deal with Cmd(%d)", packet->m_Command);
			}			
		}else {
			log_waring("not find cmd(%d)", packet->m_Command);
		}
	}else {
		log_waring("not find player(%d)", packet->m_PacketParam);
	}
}
void GameFish::OnPlayerOffLine(uint32 playerId, int32 gatewayId, uint64 sessionId)
{
	Player* pPlayer = m_PlayerMgr->getObjectById(playerId);
	if (pPlayer)
	{
		switch (pPlayer->getStatus())
		{
		case emPlayerStatus_Lobby: {
			t_PlayerLeaveTable msg;
			msg.m_type = emPlayerLeaveTableType_leaveFishSessionIdInvalid;
			msg.m_sessionId = sessionId;
			msg.m_playerId = playerId;
			msg.m_tableid = Invalid_ObjId;
			msg.m_seatId = Invalid_ObjId;
			OnHandlePlayerLeaveTable(&msg);
		}break;
		case emPlayerStatus_PreGame:
		case emPlayerStatus_Game:
		{
			Table* pTable = m_pTableMgr->getTable(pPlayer->getTableId());
			if (!pTable) {
				log_err("not find table(%d)", pPlayer->getTableId());
				return;
			}
			abs* buffer = pTable->newBuffer_Rcv();
			*(t_ObjId*)(buffer->getData()) = Invalid_ObjId;
			t_PlayerOffLine* data = (t_PlayerOffLine*)(buffer->getData() + sizeof(t_ObjId));

			data->m_PacketParam = 0;
			data->m_Length = sizeof(t_PlayerOffLine);
			data->m_Command = emTableFishPacket_t_PlayerOffLine;
			data->m_playerId = pPlayer->getId();
			data->m_seatId = pPlayer->getSeatId();
			data->m_sessionId = sessionId;
			pTable->pushMsg(buffer);
		}break;
		default:
			break;
		}
	}
}
void GameFish::OnHandlePlayerEnterTable(PacketHeader* packet)
{
	t_EnterTableRst* msg = (t_EnterTableRst*)packet;
	
	Player* pPlayer = msg->m_player;
	if (!pPlayer) {
		log_err("error");
		return;
	}
	if (msg->m_rst == 0)
	{
		//设置玩家的状态
		pPlayer->setStatus(emPlayerStatus_Game);
		pPlayer->setTableId(msg->m_tableid);
		pPlayer->setSeatId(msg->m_seatId);

		//设置座子上位置的显示
		t_TableFishData* pData = m_pTableViewMgr->getTableData(msg->m_tableid);
		if (!pData || msg->m_seatId >= MAX_SEAT_NUM || msg->m_seatId<0) {
			log_err("not find table(%d) seat(%d)", msg->m_tableid, msg->m_seatId);
			return;
		}
		t_seatData& seatData = pData->m_seatData[msg->m_seatId];
		seatData.m_playerId = pPlayer->getId();
		seatData.m_NickName = pPlayer->getNickName();
		seatData.m_headid = pPlayer->getHeadId();

	}else if(msg->m_rst == 1){
		pPlayer->setStatus(emPlayerStatus_Lobby);
	}
}
void GameFish::OnHandlePlayerLeaveTable(PacketHeader* packet)
{
	t_PlayerLeaveTable* msg = (t_PlayerLeaveTable*)packet;
	
	Player* pPlayer = nullptr;
	t_TableFishData* table = m_pTableViewMgr->getTableData(msg->m_tableid);
	if (table) {
		table->clear(msg->m_seatId);
	}

	switch (msg->m_type)
	{
	case emPlayerLeaveTableType_goHome: {
		pPlayer = (Player*)m_PlayerMgr->getObjectById(msg->m_playerId);
		if (pPlayer)
		{
			if (pPlayer->getStatus() != emPlayerStatus_Lobby) {
				
				pPlayer->setStatus(emPlayerStatus_Lobby);
			}
		}
	}break;
	case emPlayerLeaveTableType_leaveFish:
	{
		pPlayer = m_PlayerMgr->rmvObject(msg->m_playerId);
		if (pPlayer){
			delete pPlayer;
		}
	}break;
	case emPlayerLeaveTableType_leaveFishSessionIdInvalid:
	{
		pPlayer = m_PlayerMgr->rmvObject(msg->m_playerId);
		if (pPlayer) {
			delete pPlayer;
		}
	}break;
	default:break;
	}
}

#ifdef _MSC_VER
BOOL WINAPI DllMain(HINSTANCE hinstDLL, uint32_t fdwReason, LPVOID lpReserved)
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{

	}
	break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif
NS_WF_END

