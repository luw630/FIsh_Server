#pragma once

#include "WfConfig.h"
#include "D3DHeader.h"

using namespace std;

#define MAX_SEAT_NUM	4	//椅子个数


enum SkillType
{
	SKILL_LIGHTING,
	SKILL_FREEZE,
	SKILL_TORNADO,
	SKILL_DISASTER,
	SKILL_LOCK,
	SKILL_MAX
};

//点到线段的最短距离
//http://blog.sina.com.cn/s/blog_5d5c80840101bnhw.html

inline float PointToSegDist(float x, float y, float x1, float y1, float x2, float y2)
{
	float cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	float d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	float r = cross / d2;
	float px = x1 + (x2 - x1) * r;
	float py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y1) * (py - y1));
}

//计算point点是否在经过a,b两点平行线内部（dir为平行线方向）
inline bool PointInParallellines(Vector3& a, Vector3& b, Vector3& dir, Vector3& point)
{
	Vector3 _a = point - a;
	Vector3 _b = point - b;
	if (Vector3::multiplication_cross(_a, dir).normalized()
		== Vector3::multiplication_cross(dir, _b).normalized()
		)
		return true;
	return false;
}

typedef unsigned short  FISHID;

