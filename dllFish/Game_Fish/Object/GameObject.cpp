#include "GameObject.h"

NS_WF_BEGIN
GameObject::GameObject()
{
	m_bActive = true;
	m_Speed = 1.0f;
}

void GameObject::setUnActive()
{
	m_bActive = false;
	onUnActive();
}
void GameObject::setComBox(ComBox& box)
{
	m_Box = box;
}
NS_WF_END

