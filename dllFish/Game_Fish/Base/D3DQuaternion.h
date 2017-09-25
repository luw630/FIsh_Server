#pragma once

#include "WfConfig.h"

typedef struct D3DXQUATERNION {
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT w;
public:
	D3DXQUATERNION();
	D3DXQUATERNION(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
} D3DXQUATERNION, *LPD3DXQUATERNION;