#pragma once
#include "WfConfig.h"

using namespace std;

typedef std::string t_AccountId;
typedef uint32 t_PlayerId;

#define  nPlayerNameLen 64
typedef struct {
	int8  data[nPlayerNameLen];
}t_playerName;