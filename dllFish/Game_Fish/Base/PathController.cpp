#include "PathController.h"
#include "Fish.h"
#include "FishResMgr.h"
#include "WfLog.h"
#include "GameObject.h"

NS_WF_BEGIN
PathController::PathController()
{
	m_pGameObject = nullptr;
}
bool PathController::init(GameObject* pGameObject, PathData* path, float speed)
{
	if (!pGameObject) return false;
	if (!path) return false;

	m_pGameObject = pGameObject;
	m_path = path;	
	runningTime = 0;
	percentage = 0;
	m_time = path->m_Len / speed;
	return true;
}
bool PathController::initByTime(GameObject* pGameObject, PathData* path, float fTime)
{
	if (!pGameObject) return false;
	if (!path) return false;

	m_pGameObject = pGameObject;
	m_path = path;
	runningTime = 0;
	percentage = 0;
	m_time = fTime;
	return true;
}

void PathController::Update(float delta)
{		
	ApplyMoveToPathTargets();
	UpdatePercentage(delta);
}

void PathController::onStart()
{

}

void PathController::UpdatePercentage(float delta)
{
	runningTime += delta;
	percentage = runningTime / m_time;
	if (runningTime >= m_time){
		runningTime = m_time;
		percentage = 1.0;
	}
}
void PathController::ApplyMoveToPathTargets()
{
	Vector3 preUpdate = m_pGameObject->m_Transform.position;
	float t = easeInOutCubic(0, 1, percentage);

	m_pGameObject->m_Transform.position = m_path->path->Interp(Clamp(t, 0, 1));
	//log_dbg("percentage(%f) pos(%f,%f,%f)", percentage,
	//	m_pGameObject->m_Transform.position.x,
	//	m_pGameObject->m_Transform.position.y,
	//	m_pGameObject->m_Transform.position.z);
	if (percentage >= 0.99999) {
		m_pGameObject->setUnActive();
	}
	//need physics?
	//²Î¿¼ÊµÏÖ
}
float PathController::PathLength(Vector3* path, int32 pathNum)
{
	float pathLength = 0;
	int vector3sLen = 0;
	Vector3* vector3s = PathControlPointGenerator(path, pathNum, vector3sLen);

	//Line Draw:
	Vector3 prevPt = Interp(vector3s, vector3sLen, 0);
	int SmoothAmount = pathNum * 20;
	for (int i = 1; i <= SmoothAmount; i++) {
		float pm = (float)i / SmoothAmount;
		Vector3 currPt = Interp(vector3s, vector3sLen, pm);
		pathLength += Vector3::Distance(prevPt, currPt);
		prevPt = currPt;
	}

	return pathLength;
}
Vector3* PathController::PathControlPointGenerator(Vector3* path, int32 pathNum, int32& outNum)
{
	Vector3* suppliedPath = nullptr;
	Vector3* vector3s = nullptr;

	//create and store path points:
	suppliedPath = path;

	//populate calculate path;
	int offset = 2;
	int vector3sLen = pathNum + offset;
	vector3s = new Vector3[vector3sLen];
	outNum = vector3sLen;
	memcpy(vector3s + 1, suppliedPath, sizeof(Vector3)*pathNum);

	//populate start and end control points:
	//vector3s[0] = vector3s[1] - vector3s[2];
	vector3s[0] = vector3s[1] + (vector3s[1] - vector3s[2]);
	vector3s[vector3sLen - 1] = vector3s[vector3sLen - 2] + (vector3s[vector3sLen - 2] - vector3s[vector3sLen - 3]);

	//is this a closed, continuous loop? yes? well then so let's make a continuous Catmull-Rom spline!
	if (vector3s[1] == vector3s[vector3sLen - 2]) {
		Vector3* tmpLoopSpline = new Vector3[vector3sLen];
		int tmpLoopSplineLen = vector3sLen;
		memcpy(tmpLoopSpline, vector3s, sizeof(Vector3)*vector3sLen);
		tmpLoopSpline[0] = tmpLoopSpline[tmpLoopSplineLen - 3];
		tmpLoopSpline[tmpLoopSplineLen - 1] = tmpLoopSpline[2];
		delete[] vector3s;
		vector3s = new Vector3[tmpLoopSplineLen];
		outNum = tmpLoopSplineLen;
		memcpy(vector3s, tmpLoopSpline, sizeof(Vector3)*tmpLoopSplineLen);
		delete[] tmpLoopSpline;
	}

	return vector3s;
}
Vector3 PathController::Interp(Vector3* pts, int32 ptsSize, float t)
{
	int numSections = ptsSize - 3;
	int currPt = min(FloorToInt(t * (float)numSections), numSections - 1);
	float u = t * (float)numSections - (float)currPt;

	Vector3 a = pts[currPt];
	Vector3 b = pts[currPt + 1];
	Vector3 c = pts[currPt + 2];
	Vector3 d = pts[currPt + 3];

	return .5f * (
		(-a + 3.0f * b - 3.0f * c + d) * (u * u * u)
		+ (2.0f * a - 5.0f * b + 4.0f * c - d) * (u * u)
		+ (-a + c) * u
		+ 2.0f * b
		);
}
float PathController::easeInOutCubic(float start, float end, float value) 
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * value * value * value + start;
	value -= 2;
	return end * 0.5f * (value * value * value + 2) + start;
}
void PathController::BuildPath(PathData* res)
{
	if (!res) return;
	if (!res->m_NodeList) return;

	bool plotStart = false;
	int offset = 2;

	//build calculated path:
	int vector3sLen = res->m_NodeNum + offset;
	Vector3* vector3s = new Vector3[vector3sLen];
	offset = 1;

	//populate calculate path;
	memcpy(vector3s + offset, res->m_NodeList, sizeof(Vector3)*res->m_NodeNum);

	//populate start and end control points:
	//vector3s[0] = vector3s[1] - vector3s[2];
	vector3s[0] = vector3s[1] + (vector3s[1] - vector3s[2]);
	vector3s[vector3sLen - 1] = vector3s[vector3sLen - 2] + (vector3s[vector3sLen - 2] - vector3s[vector3sLen - 3]);

	//is this a closed, continuous loop? yes? well then so let's make a continuous Catmull-Rom spline!
	if (vector3s[1] == vector3s[vector3sLen - 2]) {
		Vector3* tmpLoopSpline = new Vector3[vector3sLen];
		memcpy(tmpLoopSpline, vector3s, sizeof(Vector3)*vector3sLen);
		tmpLoopSpline[0] = tmpLoopSpline[vector3sLen - 3];
		tmpLoopSpline[vector3sLen - 1] = tmpLoopSpline[2];
		delete[] vector3s;
		vector3s = new Vector3[vector3sLen];
		memcpy(vector3s, tmpLoopSpline, sizeof(Vector3)*vector3sLen);
		delete[] tmpLoopSpline;
	}

	//create Catmull-Rom path:
	if (res->path != nullptr)
	{
		log_err("error");
		return;
	}
	res->path = new CRSpline(vector3s, vector3sLen);
	res->m_Len = PathLength(vector3s, vector3sLen);
	delete[] vector3s;
}
NS_WF_END