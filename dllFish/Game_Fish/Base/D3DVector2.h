#pragma once

#include "WfConfig.h"

typedef struct D3DXVECTOR2
{
public:
	D3DXVECTOR2() {
		x = 0.0f;
		y = 0.0f;
	};
	D3DXVECTOR2(FLOAT _x, FLOAT _y)
	{
		this->x = _x;
		this->y = _y;
	}

	// assignment operators
	D3DXVECTOR2& operator += (CONST D3DXVECTOR2& obj)
	{
		this->x += obj.x;
		this->y += obj.y;
		return *this;
	}
	D3DXVECTOR2& operator -= (CONST D3DXVECTOR2& obj)
	{
		this->x -= obj.x;
		this->y -= obj.y;
		return *this;
	}
	D3DXVECTOR2& operator *= (FLOAT value)
	{
		this->x *= value;
		this->y *= value;
		return *this;
	}
	D3DXVECTOR2& operator /= (FLOAT value)
	{
		this->x /= value;
		this->y /= value;
		return *this;
	}

	// binary operators
	D3DXVECTOR2 operator + (CONST D3DXVECTOR2& obj) const
	{
		D3DXVECTOR2 tmp(*this);
		tmp += obj;
		return tmp;
	}
	D3DXVECTOR2 operator - (CONST D3DXVECTOR2& obj) const
	{
		D3DXVECTOR2 tmp(*this);
		tmp -= obj;
		return tmp;
	}
	D3DXVECTOR2 operator * (FLOAT value) const
	{
		D3DXVECTOR2 tmp(*this);
		tmp *= value;
		return tmp;
	}
	D3DXVECTOR2 operator / (FLOAT value) const
	{
		D3DXVECTOR2 tmp(*this);
		tmp /= value;
		return tmp;
	}

	friend D3DXVECTOR2 operator * (FLOAT, CONST D3DXVECTOR2&);

	bool operator == (CONST D3DXVECTOR2& obj) const
	{
		if ((fabs(x - obj.x) < Espinon) &&
			(fabs(y - obj.y) < Espinon))
			return true;
		return false;
	}
	bool operator != (CONST D3DXVECTOR2& obj) const
	{
		if ((fabs(x - obj.x) >= Espinon) ||
			(fabs(y - obj.y) >= Espinon))
			return true;
		return false;
	}


public:
	FLOAT x, y;
} D3DXVECTOR2, *LPD3DXVECTOR2;



