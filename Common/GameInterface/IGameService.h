#pragma once

#include "WfConfig.h"
#include "WfIService.h"
#include "ITable.h"
#include "WfLockFreeList.h"
#include "WfPacketHeader.h"
#include "WfAbsFactory.h"
NS_WF_BEGIN
#define UpdateFps 50

class IGameService : public IService
{
public:
	virtual void pushTable(ITable *pTable) = NULL;
	virtual void sendPacket(abs* msg) = NULL;
	virtual void update() = NULL;
	virtual abs* newBuffer_Rcv() = NULL;
	virtual abs* newBuffer_Send() = NULL;
};

NS_WF_END

