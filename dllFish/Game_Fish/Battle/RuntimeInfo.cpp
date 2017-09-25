
#include "RuntimeInfo.h"

float   RuntimeInfo::BulletSpeed = 40;
float   RuntimeInfo::InvShortMaxValue = 1.0f / SHRT_MAX;
float   RuntimeInfo::InvUShortMaxValue = 1.0f / USHRT_MAX;
float	RuntimeInfo::NearX = 0;;
float	RuntimeInfo::NearY = 0;
Vector3  RuntimeInfo::NearLeftTopPoint;
Vector3  RuntimeInfo::NearRightBottomPoint;
Vector3  RuntimeInfo::FarLeftTopPoint;
Vector3  RuntimeInfo::FarRightBottomPoint;
Vector3  RuntimeInfo::NearLeftTopFlagPoint;
Vector3  RuntimeInfo::NearRightBottomFlagPoint;
Matrix   RuntimeInfo::ProjectMatrix;
float RuntimeInfo::ScreenScalingY = 0;
float RuntimeInfo::ScreenScalingX = 0;
bool RuntimeInfo::Init()
{
	
	return false;
}