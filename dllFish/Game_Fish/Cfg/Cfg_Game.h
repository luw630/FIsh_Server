#pragma once

#include "WfConfig.h"
#include <json/json.h>
#include "WfSysUtil.h"
#include "FishRes.h"

NS_WF_BEGIN

class Cfg_Game
{
public:
	Cfg_Game();
	bool load();
	int32 getTableNum() { return m_TableNum; }
private:
	bool loadScoreCfg(int32 index);
public:
	int32 m_TableNum;		//���Ӹ���
	int32 m_MaxPlayer;		//����������
	int32 m_MapId;			//��ͼid

	int32 t_step;		//�Ѷȵȼ��������ף����ף��ѣ����ѣ����ѣ�
	int32 t_placeType;	//�������ͣ�С,��,��
	int32 t_radio;		//��ˮ����(����Ϊ��ˮ����,����Ϊ��ˮ����)
	int32 t_minScore;	//��Сѹ��
	int32 t_maxScore;	//���ѹ��
	int32 t_coinPoint;	//Ͷ�ұ���
	uint32 t_totalGunPoints;	//�ܷ��ڷ�����ֵ
	uint32 t_totalFishPoints;	//�ܻ�����÷ֳ�ֵ

	t_MapData* m_pMapData;
};

NS_WF_END

