#pragma once

#include "WfConnectionSession.h"

NS_WF_BEGIN


class ISessionProxy : public ConnectionSession
{
public:
	ISessionProxy(IService* svr, int32 sessionType) :ConnectionSession(svr, sessionType) {}
	virtual ~ISessionProxy() {};

	int PostMsg(PacketCommand cmd, 
		::google::protobuf::Message* packet,
		int32 msgParamLen,
		uint8* msgParam,
		uint16 srcType,
		uint32 srcId,
		int32 desType,
		int32 transferType,
		uint32 desId
		);
};

NS_WF_END