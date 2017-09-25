#pragma once

#include "WfPacketHeader.h"

NS_WF_BEGIN

class ISessionGateWay
{
public:
	virtual int32 getGatewayId() = NULL;
	virtual void setGatewayId(int32 id) = NULL;
};

NS_WF_END

