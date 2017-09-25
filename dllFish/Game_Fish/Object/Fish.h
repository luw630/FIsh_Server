#pragma once

#include "WfCObjMgr.h"
#include "WfObjList.h"
#include "Packets/Packet_Fish.pb.h"
#include "PathController.h"
#include "GameObject.h"


NS_WF_BEGIN
class Table;
class Fish : public GameObject
{
	friend class Table;
public:
	ObjList<Fish>::Node* m_node;
private:
	Table* m_pTable;
	int32 m_type;			//����
	PathController  m_PathCtrl;
	bool m_bScreen;
public:
	int32 t_fishRadio;					//��ı���
	int32 t_maxFishRadio;				//�趨����������
	int32 t_minFishRadio;				//�趨�������С����
	float t_radius;						//�뾶
public:
	Fish();
	~Fish();

	t_ObjId m_KillSeatId;	//ɱ�����seatid
	t_ObjId m_KillPlayerId; //ɱ�����playerid
public:
	/*
	������
	t_ObjId id			//����id
	int32 fishType		//������
	int32 pathIndx		//�˶��켣����
	*/
	bool init(Table* pTable,t_ObjId id, int32 fishType, PathData* pPath);
	void dup(rpc::SC_FishNotifyFireFish* data);
	void dup(rpc::SC_FishNotifyAddFish* data);
	bool update(uint32 delta);
	void release();
	void setDeadByBomb(t_ObjId seatId, t_ObjId playerId);	//��ը������
	void setScreen(bool value) { m_bScreen = value; }
	bool isScreen() { return m_bScreen; }
	virtual void onUnActive();
};

NS_WF_END

