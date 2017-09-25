#pragma once
#include "WfProxyHead.h"


NS_WF_BEGIN

#define BASEPXYHEADLENGTH    18


enum emSvrType {
	emSvrType_Unknow = 0,
	emSvrType_Game = 1,
	emSvrType_Account = 2,	
	emSvrType_GateWay = 3,
	emSvrType_Db = 4,

	emSvrType_Center = 6,
	emSvrType_Payment = 7,
	emSvrType_Proxy = 8,
	emSvrType_Manager = 9,
};

enum emMemoryDbPacket {
	emMemoryDbPacket_SetData = 10,
	emMemoryDbPacket_OutData = 11,
	emMemoryDbPacket_SetDataRst = 12,
	emMemoryDbPacket_OutDataRst = 13,
};

class MemoryDbPacket_SetData : public ISerializePacket
{
public:
	virtual bool Serialize(BaseStream& os) const
	{
		os << m_key;
		os << m_Data;
		os << m_callParam;
		return true;
	}
	virtual bool DeSerialize(BaseStream& is)
	{
		is >> m_key;
		is >> m_Data;
		is >> m_callParam;
		return true;
	}
public:
	std::string	m_key;
	std::string	m_Data;
	std::string	m_callParam;
};
class MemoryDbPacket_OutData : public ISerializePacket
{
public:
	virtual bool Serialize(BaseStream& os) const
	{
		os << m_key;
		os << m_callParam;
		return true;
	}
	virtual bool DeSerialize(BaseStream& is)
	{
		is >> m_key;
		is >> m_callParam;
		return true;
	}
public:
	std::string	m_key;
	std::string	m_callParam;
};
class MemoryDbPacket_SetDataRst : public ISerializePacket
{
public:
	virtual bool Serialize(BaseStream& os) const
	{
		os << m_key;
		os << m_callParam;
		return true;
	}
	virtual bool DeSerialize(BaseStream& is)
	{
		is >> m_key;
		is >> m_callParam;
		return true;
	}
public:
	std::string	m_key;
	std::string	m_callParam;
};
class MemoryDbPacket_OutDataRst : public ISerializePacket
{
public:
	virtual bool Serialize(BaseStream& os) const
	{
		os << m_key;
		os << m_Data;
		os << m_callParam;
		return true;
	}
	virtual bool DeSerialize(BaseStream& is)
	{
		is >> m_key;
		is >> m_Data;
		is >> m_callParam;
		return true;
	}
public:
	std::string	m_key;
	std::string	m_Data;
	std::string	m_callParam;
};
#define AccountSvr_Num		1

//”Œœ∑id
#define GameId_FishLowLevel	1
#define AccountSvrId		1
NS_WF_END