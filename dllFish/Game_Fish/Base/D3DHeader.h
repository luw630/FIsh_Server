#pragma once

#include "WfConfig.h"
#include "D3DMatrix.h"
#include "D3DQuaternion.h"
#include "D3DVector2.h"
#include "D3DVector3.h"
using namespace std;

typedef D3DXVECTOR2		Vector2;
typedef D3DXVECTOR3		Vector3;
typedef D3DXMATRIX		Matrix;
typedef D3DXMATRIX		Matrix4x4;
typedef D3DXQUATERNION	Quaternion;
typedef D3DXQUATERNION  Rotation;

//两个 3-D 向量的线性内插值
//V1 + s(V2-V1)
inline D3DXVECTOR3 *D3DXVec3Lerp(
	D3DXVECTOR3 *pOut,
	CONST D3DXVECTOR3 *pV1,
	CONST D3DXVECTOR3 *pV2,
	FLOAT s
)
{
	*pOut = *pV1 + (*pV2 - *pV1)*s;
	return pOut;
}
inline D3DXQUATERNION* D3DXQuaternionSlerp(
	D3DXQUATERNION *pOut,
	const D3DXQUATERNION *pQ1,
	const D3DXQUATERNION *pQ2,
	FLOAT	t
)
{
	return pOut;
}

inline Vector3 MulAdd(const Vector3 &p1, const Vector3 & p2, const Vector3 & p3)
{
	return Vector3(
		p1.x * p2.x + p3.x,
		p1.y * p2.y + p3.y,
		p1.z * p2.z + p3.z);
}

inline Quaternion MulAdd(const Quaternion &p1, const Quaternion & p2, const Quaternion & p3)
{
	return Quaternion(
		p1.x * p2.x + p3.x,
		p1.y * p2.y + p3.y,
		p1.z * p2.z + p3.z,
		p1.w * p2.w + p3.w);
}
inline Vector3 Lerp(const Vector3 &p1, const Vector3 & p2, float t)
{
	Vector3 vec3;
	D3DXVec3Lerp(&vec3, &p1, &p2, t);
	return vec3;
}
inline Quaternion Slerp(const Quaternion &p1, const Quaternion &p2, float t)
{
	Quaternion rot;
	D3DXQuaternionSlerp(&rot, &p1, &p2, t);
	return rot;
}
inline float LerpFloat(float a, float b, float t)
{
	return a + (b - a) * t;
}

inline float Clamp(float value, float minvalue, float maxvalue)
{
	return min(max(value, minvalue), maxvalue);
}

inline int FloorToInt(float value)
{
	return (int)value;	//need to do
}