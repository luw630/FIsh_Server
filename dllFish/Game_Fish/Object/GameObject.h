#pragma once

#include "WfCObjMgr.h"
#include "Transform.h"

NS_WF_BEGIN
class ComBox
{
public:
	ComBox() 
	{
		m_Boxs = nullptr;
		m_BoxNum = 0;
	}
	~ComBox() 
	{
		WF_SAFE_DELETE_ARRAY(m_Boxs);
		m_BoxNum = 0;
	}
	struct circleZone{
		float radius;	//�༭
		Vector3 offset;	//ƫ����
	};
	circleZone* m_Boxs;
	int32 m_BoxNum;

	ComBox & operator = (const ComBox & box)
	{
		if (&box != this)
		{
			if (m_Boxs != nullptr){
				WF_SAFE_DELETE_ARRAY(m_Boxs);
				m_BoxNum = 0;
			}
			if (box.m_BoxNum > 0)
			{
				m_Boxs = new circleZone[box.m_BoxNum];
				m_BoxNum = box.m_BoxNum;
				memcpy(m_Boxs, box.m_Boxs, sizeof(circleZone*)*m_BoxNum);
			}
		}
		return *this;
	}
};
class GameObject : public CObj
{
public:	
	bool m_bActive;			//�Ƿ����
	Transform m_Transform;	//�ƶ����
	ComBox m_Box;
	float m_Speed;			//�˶��ٶ�
public:
	GameObject();
	virtual ~GameObject() {};
	void setUnActive();
	bool isActive() { return m_bActive; }
	virtual void onUnActive() {};
	void setComBox(ComBox& box);
};

NS_WF_END
