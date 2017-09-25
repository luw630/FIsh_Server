#pragma once
#include "WfConfig.h"
#include "FishRes.h"
#include <math.h>

struct BulletStartPosData
{
	Vector3 Center;
	Vector3 Pos;
	Vector3 Dir;
	float   Length;
};
//统计运行时信息数据
class RuntimeInfo
{
public:
	static bool Init();
	static float            BulletSpeed;
	static float            InvShortMaxValue;
	static float            InvUShortMaxValue;

	static Vector3          NearLeftTopPoint;
	static Vector3          NearRightBottomPoint;
	static Vector3          FarLeftTopPoint;
	static Vector3          FarRightBottomPoint;
	static Vector3          NearLeftTopFlagPoint;
	static Vector3          NearRightBottomFlagPoint;

	static float			ScreenScalingY;
	static float			ScreenScalingX;
	static Matrix			ProjectMatrix;

	static float			NearX;
	static float			NearY;
};