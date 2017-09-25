#include "SeesionDb.h"
#include <json/json.h>
#include "WfHttpPost.h"
#include "WfLog.h"
#include "App/App.h"
#include "Packets/Packet_Login.pb.h"
#include "Packets/Packet_ErrCode.pb.h"
#include "Packets/Packet_DB.pb.h"
#include <google/protobuf/message.h>

using namespace rpc;

NS_WF_BEGIN
SeesionDb::SeesionDb() :IWorkMgr(&App::GetInstance())
{

}


SeesionDb::~SeesionDb()
{
}
bool SeesionDb::OnIntial()
{
	Register(em_SD_LoadData, OnHandle_SD_LoadData); 
	Register(em_SD_InsertPlayer, OnHandle_SD_InsertPlayer);
	Register(em_SD_SavePlayer, OnHandle_SD_SavePlayer);
	Register(em_SD_FindPlayer, OnHandle_SD_FindPlayer);
	
	
	return true;
}
void SeesionDb::OnHandlePacket(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	TaskPacketHandler handle = GetPacketHandler(pHeader->m_Command);
	if (handle){
		handle(pHandle, connectId, pHeader, stream);
	}
	else{
		log_err("error:can't  find ClientPacketHandler(0x%x)", pHeader->m_Command);
	}
}
void SeesionDb::OnPacket(uint64 connectId, PacketHeader* packet, Stream& stream)
{
	if (packet->m_Command == em_DS_PlayerData)
	{
		OnHandle_DS_PlayerData(connectId, packet, stream);
	}
	else if (packet->m_Command == em_DS_FindPlayerRst)
	{
		OnHandle_FindPlayerRst(connectId, packet, stream);	
	}
}
void SeesionDb::OnPacket(PacketHeader* packet, Stream& stream)
{

}

void SeesionDb::OnHandle_SD_LoadData(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	log_dbg("rcv:SD_LoadPlayer");
	SD_LoadData msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	MysqlConnection* pCon = pHandle->getConnection();

	DS_PlayerData rsp;
	
	rsp.set_m_accountid(msg.m_accountid());
	rsp.set_m_param(msg.m_param());
	rsp.set_m_rst(100);	
	rsp.set_m_svrnodeid(msg.m_svrnodeid());
	t_PlayerId playerId = 0;
	do
	{
		char sql[128] = { 0 };
		QueryResult result;
		sprintf(sql, "SELECT playerid,baseinfo,frienddata,maildata FROM t_chess_player WHERE accountid = '%s';", msg.m_accountid().c_str());
		if (!pCon->ExecuteSQL(sql, &result)){
			log_err("run sql(%s) error", sql);
			break;
		}
		rsp.set_m_rst(1);
		if (!result.FetchRow())
		{
			break;
		}
		playerId = result.GetUint(0, 0);
		const char* playerBaseData = result.GetValue(1);
		uint32 playerBaseDataLen = result.GetValueLength(1);

		const char* friendData = result.GetValue(2);
		uint32 friendDataLen = result.GetValueLength(2);

		const char* mailData = result.GetValue(3);
		uint32 mailDataLen = result.GetValueLength(3);

		if (playerBaseDataLen > 0){
			rpc::t_playerBaseData* playerData = rsp.mutable_m_playerinfo();
			playerData->ParseFromArray(playerBaseData, playerBaseDataLen);
		}

		if (friendDataLen > 0) {
			rpc::t_FriendDatas* friendDatas = rsp.mutable_m_friend();
			friendDatas->ParseFromArray(friendData, friendDataLen);
		}

		if (mailDataLen > 0) {
			rpc::t_MailDatas* mailDatas = rsp.mutable_m_mail();
			mailDatas->ParseFromArray(mailData, mailDataLen);
		}

		rsp.set_m_rst(0);
	} while (false);
	rsp.set_m_playerid(playerId);

	pHandle->SendPacket(em_DS_PlayerData,&rsp, connectId);
}
void SeesionDb::OnHandle_SD_InsertPlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	log_dbg("rcv:SD_InsertPlayer");
	SD_InsertPlayer msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	t_playerBaseData* playerData = msg.mutable_m_data();	
	MysqlConnection* pCon = pHandle->getConnection();

	MysqlBind bindArgs(1);
	vector<uint8*>	blobDatas;

	uint blobLength = playerData->ByteSize();
	uint8* blobBuffer = new uint8[blobLength>0 ? blobLength : 1];
	if (blobLength > 0)
	{
		playerData->SerializeToArray(blobBuffer, blobLength);
		blobDatas.push_back(blobBuffer);
		bindArgs.BindBlob(0, blobBuffer, blobLength);
	}

	QueryResult sqlResult;
	char sql[256] = { 0 };
	sprintf(sql, "INSERT INTO t_chess_player(accountid,baseinfo) VALUES ('%s',?);", msg.m_accountid().c_str());
	//sprintf(sql, "UPDATE t_chess_player SET datas = ? WHERE accountid = '%s'", msg.m_accountid().c_str());

	if (pCon->ExecuteBlob(sql, &sqlResult, bindArgs.GetBindCount(), bindArgs.GetBinds())){
	}else {
		log_err("run sql(%s) error", sql);
	}

	for (size_t i = 0; i < blobDatas.size(); ++i)
	{
		delete[]blobDatas[i];
	}
	blobDatas.clear();
}
void SeesionDb::OnHandle_SD_SavePlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{

}
void SeesionDb::OnHandle_SD_FindPlayer(TaskHandle* pHandle, uint64 connectId, PacketHeader* pHeader, Stream& stream)
{
	log_dbg("rcv:SD_LoadPlayer");
	SD_FindPlayer msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	MysqlConnection* pCon = pHandle->getConnection();

	DS_FindPlayerRst rsp;
	rpc::param_FindPlayer* param = rsp.mutable_param();
	param->CopyFrom(msg.param());
	rsp.set_bfind(100);
	t_PlayerId playerId = 0;
	do
	{
		char sql[128] = { 0 };
		QueryResult result;
		sprintf(sql, "SELECT baseinfo FROM t_chess_player WHERE playerid = '%d';", msg.playerid());
		if (!pCon->ExecuteSQL(sql, &result)) {
			log_err("run sql(%s) error", sql);
			break;
		}
		rsp.set_bfind(1);
		if (!result.FetchRow())
		{
			break;
		}

		const char* playerBaseData = result.GetValue(0);
		uint32 playerBaseDataLen = result.GetValueLength(0);

		if (playerBaseDataLen > 0) {
			rpc::t_playerBaseData playerData;
			playerData.ParseFromArray(playerBaseData, playerBaseDataLen);

			rsp.set_headid(playerData.headid());
			rsp.set_name(playerData.nickname());
		}else {
			break;
		}
		rsp.set_bfind(0);
	} while (false);
	
	pHandle->SendPacket(em_DS_FindPlayerRst, &rsp, connectId);
}
void SeesionDb::OnHandle_DS_PlayerData(t_sessionId connectId, PacketHeader* pHeader, Stream& stream)
{
	log_dbg("rcv:DS_PlayerData");
	DS_PlayerData msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	t_PlayerId playerid = msg.m_playerid();
	t_AccountId accountId = msg.m_accountid();
	int32 gatewayId = msg.m_svrnodeid();
	t_sessionId param = msg.m_param();

	if (msg.m_rst() == 100)
	
	{
		log_dbg("select Db error account(%s),playerId(%d)", accountId.c_str(), playerid);
		return ;
	}
	else if (msg.m_rst() == 1)	
	{
		//没有查询到玩家
		SD_InsertPlayer dbMsg;
		dbMsg.set_m_accountid(accountId);
		t_playerBaseData* playerData = dbMsg.mutable_m_data();
		Player player;
		player.setAccountId(accountId);
		player.dumpPlayerBaseData(playerData);

		addBuffer(em_SD_InsertPlayer, &dbMsg, connectId, getIndex(accountId));

		SD_LoadData req;
		req.set_m_accountid(accountId);
		req.set_m_param(connectId);
		req.set_m_svrnodeid(msg.m_svrnodeid());
		App::GetInstance().getSeesionDb()->addBuffer(em_SD_LoadData, &req, connectId, getIndex(accountId));

	}
	else if(msg.m_rst() == 0){
		PlayerMgr* pPlayerMgr = App::GetInstance().getPlayerMgr();
		Player* pPlayer = pPlayerMgr->getPlayer(playerid);
		if (pPlayer)
		{
			pPlayerMgr->LoginOk(pPlayer, gatewayId, param);
			return;
		}
		else {
			Player* pPlayer = new Player();
			const rpc::t_playerBaseData* baseInfo = nullptr;
			const rpc::t_FriendDatas* friendData = nullptr;
			const rpc::t_MailDatas* mailData = nullptr;
			
			if (msg.has_m_playerinfo()){
				baseInfo = &msg.m_playerinfo();
			}
			if (msg.has_m_friend()){
				friendData = &msg.m_friend();
			}
			if(msg.has_m_mail()){ 
				mailData = &msg.m_mail();
			}

			if (!pPlayer->Init(	playerid,
				baseInfo, 
				friendData,
				mailData,
				gatewayId, 
				param)) 
			{
				delete pPlayer;
				log_err("init player error");
				return;
			}
			pPlayerMgr->LoginOk(pPlayer, gatewayId, param);
		}
	}
}
void SeesionDb::OnHandle_FindPlayerRst(t_sessionId connectId, PacketHeader* pHeader, Stream& stream)
{
	DS_FindPlayerRst msg;
	msg.ParseFromArray(stream.GetBuffer(), stream.GetSize());

	PlayerMgr* pPlayerMgr = App::GetInstance().getPlayerMgr();
	Player* pPlayer = pPlayerMgr->getOnlinePlayer(msg.param().playerid());
	if (pPlayer)
	{
		rpc::SC_FriendFindRst rsp;
		
		if (msg.bfind() == 0)
		{
			rsp.set_bfind(0);
		}else {
			rsp.set_bfind(1);
		}
		rsp.set_playerid(msg.param().selectplayerid());
		rsp.set_name(msg.name());
		rsp.set_headid(msg.headid());
		pPlayer->SendPacket(rpc::em_SC_FriendFindRst, &rsp);
	}
}
NS_WF_END