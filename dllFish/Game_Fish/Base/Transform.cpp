#include "Transform.h"


Transform::Transform()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
}
void Transform::setPosition(const Vector3& pos)
{
	position = pos;
}
