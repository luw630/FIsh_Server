#pragma once
#include "WfDataType.h"
#include "WfPacketHeader.h"
NS_WF_BEGIN

template<class _Session,class T>
class Dispatcher
{
public:
	typedef void (T::*Handle)(_Session* , PacketHeader* , Stream& );
private:
	hash_map<PacketCommand, Handle> m_handles;
public:
	virtual bool Init() { return true; }
	Handle getHandle(PacketCommand cmd)
	{
		auto t = m_handles.find(cmd);
		if (t != m_handles.end())
			return t->second;
		return nullptr;
	}
	void Register(PacketCommand cmd, Handle handle) 
	{
		auto t = m_handles.find(cmd);
		if (t != m_handles.end()) {
			log_err("repeat cmd(%d)", cmd);
			return;
		}
		m_handles[cmd] = handle;
	}
};
NS_WF_END

