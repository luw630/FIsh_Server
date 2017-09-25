#include "FriendSystem.h"
#include "App/App.h"

NS_WF_BEGIN

FriendSystem::FriendSystem()
{

}
FriendSystem::~FriendSystem()
{
	
}
void FriendSystem::init()
{
	FILE* handle = fopen("./db/friendSystem.db", "r+");
	if (handle) {
		fseek(handle, 0, SEEK_SET);
		int32 num = 0;
		fread(&num, 4,1, handle);
		for (int i = 0; i < num; i++)
		{
			uint32 playerId = 0;
			fread(&playerId, 4, 1, handle);
			t_FriendSystemRecordItem* item = new t_FriendSystemRecordItem();
			sprintf(item->m_fileName, "./db/player/player_%u.db", playerId);
			m_records[playerId] = item;
		}
		fclose(handle);
	}
	m_saveTime = 0;
	m_needSave = false;
}
void FriendSystem::save()
{
	if (!m_needSave) return;

	FILE* handle = fopen("./db/friendSystem.db.bak", "w+");
	if (handle) {
		int32 num = m_records.size();
		fwrite(&num, 4, 1, handle);
		for (auto&t : m_records)
		{
			fwrite(&(t.first), 4, 1, handle);
		}
		fclose(handle);
		remove("./db/friendSystem.db");
		if (rename("./db/friendSystem.db.bak", "./db/friendSystem.db")){
			log_waring("error");
		}
	}
}
void FriendSystem::update(uint delta)
{
	m_saveTime += delta;
	if (m_saveTime < 5000) return;
	
	m_saveTime -= 5000;
	save();
}
void FriendSystem::postPacket(PacketCommand cmd, ::google::protobuf::Message* packet, t_PlayerId playerId)
{
	PlayerMgr* playerMgr = App::GetInstance().getPlayerMgr();
	Player* player = playerMgr->getOnlinePlayer(playerId);
	

	//序列化
	int32 end = 0;
	char buff[1024 * 8];
	PacketHeader* header = (PacketHeader*)buff;
	header->m_Command = cmd;
	header->m_PacketParam = 0;
	int32 size = 0;
	if (packet)
	{
		size = packet->ByteSize();
		bool rst = packet->SerializeToArray(header + 1, size);
		if (!rst) {
			log_err("unknow error");
			return;
		}
	}
	header->m_Length = sizeof(PacketHeader) + size;

	if (player)
	{
		player->OnEventNotify(header);
	}
	else
	{		
		//查找item
		t_FriendSystemRecordItem* item = nullptr;
		auto t = m_records.find(playerId);
		if (t != m_records.end()){
			item = t->second;
		}
		else {
			item = new t_FriendSystemRecordItem();
			sprintf(item->m_fileName, "./db/player/player_%u.db", playerId);
			m_records[playerId] = item;
			item->m_fileHandle = fopen(item->m_fileName, "w");
			fflush(item->m_fileHandle);
			m_needSave = true;
		}

		//打开文件句柄
		if (item->m_fileHandle == nullptr)
		{
			item->m_fileHandle = fopen(item->m_fileName, "a+");
			if (item->m_fileHandle == nullptr) {
				log_err("open file(%s) error", item->m_fileName);
				return;
			}
		}
		//写数据
		fwrite(header,1, header->m_Length, item->m_fileHandle);
		item->m_fileOpenTime = time(0);
		fflush(item->m_fileHandle);
	}	
}
void FriendSystem::onEventPlayerOnline(Player* player)
{
	auto t = m_records.find(player->getPlayerId());
	if (t != m_records.end()) {
		t_FriendSystemRecordItem* item = t->second;

		//打开文件句柄
		if (item->m_fileHandle == nullptr){
			item->m_fileHandle = fopen(item->m_fileName, "rb+");
			if (!item->m_fileHandle) {
				log_err("open file(%s) error", item->m_fileName);
				return;
			}
		}
		fseek(item->m_fileHandle, 0, SEEK_END);
		int32 len = ftell(item->m_fileHandle);
		rewind(item->m_fileHandle);

#define pageSize  4096
#define buffSize  1024*100

		char buff[buffSize];
		int32 nBegin = 0;
		int32 nEnd = 0;

		int32 readNum = 0;
		PacketHeader* newPacket = nullptr;

		while(readNum < len) {
			//分页读取文件
			int32 tmp = fread((buff+ nEnd), pageSize, 1, item->m_fileHandle);
			if (tmp > 0) {
				readNum += pageSize;
				nEnd += pageSize;
			}else if (tmp == 0)	{
				nEnd += (len - readNum);
				readNum = len;
			}else {
				return;
			}
			
			if (buffSize - nEnd <= abs_Size) {
				memmove(buff, buff + nBegin, nBegin);
				nEnd -= nBegin;
				nBegin = 0;
			}

			//解析数据
			int32 nData = nEnd - nBegin;
			while (nData >= sizeof(PacketHeader)) {
				PacketHeader* packet = (PacketHeader*)(buff + nBegin);
				if (packet->m_Length <= nData)
				{
					player->OnEventNotify(packet);
					nBegin += packet->m_Length;
					nData = nEnd - nBegin;
				}else {
					break;
				}
			}
		}
		//删除记录
		m_records.erase(t);
		fclose(item->m_fileHandle);
		remove(item->m_fileName);
		delete item;
		m_needSave = true;
	}
}
NS_WF_END
