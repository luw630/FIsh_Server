#pragma once

#include "WfConfig.h"

NS_WF_BEGIN
class IAppProxy
{
public:
	virtual bool SendPacket(t_ObjId gatewayId, PacketHeader* packet) = NULL;
	virtual void SendPacketToAccountSvr(PacketCommand cmd, ::google::protobuf::Message* packet) = NULL;
};

NS_WF_END