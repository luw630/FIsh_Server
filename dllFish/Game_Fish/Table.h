#pragma once
#include "WfConfig.h"
#include "WfCObjMgr.h"
#include "Object/Seat.h"
#include "Object/Fish.h"
#include "Object/Bullet.h"
#include "Player.h"
#include "IGame.h"
#include "ITable.h"
#include "GamePacket.h"
#include "DispatcherTable.h"
#include "WfObjList.h"
#include "google/protobuf/message.h"

#include "FishUtility.h"
#include "FishLauncher.h"

#include "FishMgr.h"
#include "BulletMgr.h"
#include "ObjectFactory.h"
#include "Screen.h"

NS_WF_BEGIN


class Table : public ITable,public PacketHandlerMgr<HandleTable>
{
	friend class DispatcherTable;
public:
	Table();
	~Table();
public:
	virtual bool onInit();
	virtual bool isActive();
	//ÿ֡����
	virtual void onUpdate(uint32 delta);
	virtual void onDispatcherMsg(abs* buffer);
	virtual void broadCastPacket(PacketCommand cmd, ::google::protobuf::Message* packet);
private:
	//��ײ���
	static void Collide(Table* pTable);
public:
	void randomPos(Vector3& center, Vector3& pos1, Vector3& pos2, Vector3& pos3, Vector3& pos4);
	void setData(uint32 totalGunPoints, uint32 totalFishPoints, int32 placeType, int32 radio, int32 step);
	Seat* getSeat(t_ObjId seatId);

	void rmvPlayer(Seat* pSeat);
	void onAddPlayer(Player* pPlayer);		//seat����������
	void onPlayerReEnter(Player* pPlayer, Seat* pSeat);	//������½���
	void dupPlayers(google::protobuf::RepeatedPtrField<rpc::t_FishPlayer>* datas);
	void onLockBombUpdate(uint32 delta);	//�����ڸ���
	void onFishLeave(Fish* pFish);			//���뿪�������������뿪��Ļ����
	
	
	bool isFishDead(
		int gunPoints,      // - �������ڵ���ֵ
		int maxFishRadio,   // - �趨����������
		int minFishRadio,   // - �趨�������С����
		int &point			// - ���е÷�
	);
public:
	t_MapData m_mapData;
	Screen m_Screen;
	bool m_bActive;			//�Ƿ����
	BulletMgr m_BulletMgr;
	BombMgr m_BombMgr;
	FishMgr m_FishMgr;
	Seat	m_Seats[MAX_SEAT_NUM];				//λ��
	DispatcherTable	m_Dispatcher;			//�ַ���Ϣ	
	FishLauncher		m_FishLauncher;			//��ķ�����
	ObjectFactory		m_ObjectFactory;
	uint32 m_totalGunPoints;			// - �ܻ������ڵ����ۼ�
	uint32 m_totalFishPoints;			// - �ܻ�����÷��ۼ�
	int32 t_placeType;					// - ��������
	int32 t_radio;						// - ��ˮ����
	int32 t_step;						// - �Ѷȵȼ�)
};

NS_WF_END

