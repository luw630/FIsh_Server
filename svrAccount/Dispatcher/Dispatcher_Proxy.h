#pragma once
#include "WfDataType.h"
#include "Dispatcher.h"
#include "../SessionProxy/SessionProxy.h"
#include "WfPacketHandlerMgr.h"
NS_WF_BEGIN

class Dispatcher_Proxy;
typedef void (Dispatcher_Proxy::*HandleSessionProxy)(SessionProxy* session, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);

typedef void (Dispatcher_Proxy::*HandleSvrManager)(SessionProxy* session, const string& data, string& rspStr);


class Dispatcher_Proxy :public PacketHandlerMgr<HandleSessionProxy>
{
	typedef std::hash_map< std::string, HandleSvrManager >	HandleSvrManagerMap;
	HandleSvrManagerMap m_HandleSvrManagerMap;
public:
	bool Init(); 
public:
	void OnHandle_SvrManager_Post(SessionProxy* session, PacketHeader* packet, Stream& stream, int32 msgParamLen, uint8* msgParam);
	void OnHandle_player_online_number(SessionProxy* session, const string& data,string& rspStr);
	void OnHandle_notic_system_get(SessionProxy* session, const string& data, string& rspStr);
	void OnHandle_notic_system_add(SessionProxy* session, const string& data, string& rspStr);
	void OnHandle_notic_system_edit(SessionProxy* session, const string& data, string& rspStr);
	void OnHandle_notic_system_delete(SessionProxy* session, const string& data, string& rspStr);
};



NS_WF_END

