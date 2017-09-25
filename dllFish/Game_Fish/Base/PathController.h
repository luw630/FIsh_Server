/********************************************************************
created:	
author:		李林
summary:	运动轨迹控制
*********************************************************************/
#pragma once
#include "WfConfig.h"
#include "D3DHeader.h"
#include "FishRes.h"
#define SPEED_UNIT 0.02222222f


NS_WF_BEGIN
class GameObject;
class PathController
{
public:
	GameObject* m_pGameObject;
	PathData* m_path;
	float runningTime, percentage;
public:
	PathController();
	bool init(GameObject* pGameObject, PathData* m_path, float speed);
	bool initByTime(GameObject* pGameObject, PathData* m_path, float fTime);
	void Update(float delta);

public:
	void onStart();
private:
	void UpdatePercentage(float delta);
	void ApplyMoveToPathTargets();
	float m_time;		//总时间
public:
	static float PathLength(Vector3* path, int32 pathNum);	//计算长度
	static Vector3* PathControlPointGenerator(Vector3* path, int32 pathNum, int32& outNum);
	static Vector3 Interp(Vector3* pts, int32 ptsSize, float t);
	static float easeInOutCubic(float start, float end, float value);
	static void BuildPath(PathData* res);
};
NS_WF_END