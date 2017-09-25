#pragma once
#include "WfDataType.h"
#include "Dispatcher.h"
#include "../SessionGateWay/SessionGateWay.h"

NS_WF_BEGIN
class Dispatcher_GateWay : public Dispatcher<SessionGateWay, Dispatcher_GateWay>
{
public:
	bool Init();
public:

};
NS_WF_END

