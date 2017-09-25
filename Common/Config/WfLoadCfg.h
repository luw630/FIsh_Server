#pragma once
#include "WfDataType.h"

class LoadCfg 
{
	LoadCfg() { m_bLoad = false; };
	~LoadCfg() {};
public:
	SINGLETON_MODE(LoadCfg);
    bool loadConfig();
	bool isLoad() { return m_bLoad; };
private:
	bool m_bLoad;
};


