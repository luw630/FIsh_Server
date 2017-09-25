#pragma once

#include "WfConfig.h"

typedef struct D3DVECTOR {
	float x, y, z;
} D3DVECTOR;

typedef struct D3DXVECTOR3 : public D3DVECTOR
{
public:
	D3DXVECTOR3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
	D3DXVECTOR3(FLOAT _x, FLOAT _y, FLOAT _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}
	~D3DXVECTOR3(){}

	// assignment operators
	D3DXVECTOR3& operator += (CONST D3DXVECTOR3& obj)
	{
		this->x += obj.x;
		this->y += obj.y;
		this->z += obj.z;
		return *this;
	}
	D3DXVECTOR3& operator -= (CONST D3DXVECTOR3& obj)
	{
		this->x -= obj.x;
		this->y -= obj.y;
		this->z -= obj.z;
		return *this;
	}
	D3DXVECTOR3& operator *= (FLOAT value)
	{
		this->x *= value;
		this->y *= value;
		this->z *= value;
		return *this;
	}
	D3DXVECTOR3& operator /= (FLOAT value)
	{
		this->x /= value;
		this->y /= value;
		this->z /= value;
		return *this;
	}

	// binary operators
	D3DXVECTOR3 operator + (CONST D3DXVECTOR3& obj) const
	{
		D3DXVECTOR3 tmp(*this);
		tmp += obj;
		return tmp;
	}
	D3DXVECTOR3 operator - (CONST D3DXVECTOR3& obj) const
	{
		D3DXVECTOR3 tmp(*this);
		tmp -= obj;
		return tmp;
	}
	D3DXVECTOR3 operator * (FLOAT value) const
	{
		D3DXVECTOR3 tmp(*this);
		tmp *= value;
		return tmp;
	}
	D3DXVECTOR3 operator / (FLOAT value) const
	{
		D3DXVECTOR3 tmp(*this);
		tmp /= value;
		return tmp;
	}

	// unary operators
	D3DXVECTOR3 operator + () const
	{
		return D3DXVECTOR3(x, y, z);
	}
	D3DXVECTOR3 operator - () const
	{
		return D3DXVECTOR3(-x,-y,-z);
	}

	friend D3DXVECTOR3 operator * (FLOAT value, CONST struct D3DXVECTOR3& obj)
	{
		return D3DXVECTOR3(obj.x*value, obj.y*value, obj.z*value);
	}

	bool operator == (CONST D3DXVECTOR3& obj) const
	{
		if ((fabs(x - obj.x) < Espinon) &&
			(fabs(y - obj.y) < Espinon) &&
			(fabs(z - obj.z) < Espinon))
			return true;
		return false;
	}
	bool operator != (CONST D3DXVECTOR3& obj) const
	{
		if ((fabs(x - obj.x) >= Espinon) ||
			(fabs(y - obj.y) >= Espinon) ||
			(fabs(z - obj.z) >= Espinon))
			return true;
		return false;
	}
	const D3DXVECTOR3& normalized()
	{
		float len = sqrt(x*x + y*y + z*z);
		x = x / len;
		y = y / len;
		z = z / len;
		return *this;
	}
	static float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b)
	{
		return sqrt((a.x - b.x) * (a.x - b.x)
			+ (a.y - b.y) * (a.y - b.y)
			+ (a.z - b.z) * (a.z - b.z));
	}
	//向量积
	//A×B=( A.y*B.z - A.z*B.y)i+(A.z*B.x - A.x*B.z )j+(A.x*B.y - A.y*B.x)k
	static D3DXVECTOR3 multiplication_cross(D3DXVECTOR3& a, D3DXVECTOR3& b)
	{
		return D3DXVECTOR3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}

} D3DXVECTOR3, *LPD3DXVECTOR3;




