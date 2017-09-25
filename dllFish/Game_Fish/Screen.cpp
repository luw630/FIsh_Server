#include "Screen.h"
#include "Table.h"

NS_WF_BEGIN
Screen::Screen(Table* pTable)
{
	m_pTable = pTable;
}
Screen::~Screen()
{

}
void Screen::init()
{
	for (int i = 0; i < MAX_SEAT_NUM; i++)
	{
		m_LaserZone[i].m_seatId = i;
		m_LaserZone[i].m_Active = false;
		m_LaserZone[i].m_money = 100;
	}
	m_Time = 0;
}
bool Screen::inScreen(Vector3& pos)
{
	if (pos.x > m_pTable->m_mapData.t_RightBottom.x || pos.x < m_pTable->m_mapData.t_LeftTop.x ||
		pos.y > m_pTable->m_mapData.t_LeftTop.y || pos.y < m_pTable->m_mapData.t_RightBottom.y)
	{
		return false;
	}
	return true;
}
void Screen::onFishLeave(Fish* pFish)
{
	pFish->setScreen(false);
	m_ScreenFish.erase(pFish->getId());
	m_pTable->onFishLeave(pFish);
}
void Screen::onFishEnter(Fish* pFish)
{
	pFish->setScreen(true);
	m_ScreenFish.insert(make_pair(pFish->getId(), pFish));
}
void Screen::update(uint32 delta)
{
	m_Time += delta;
	int32 point = 0;
	for (auto &t : m_ScreenFish)
	{
		Fish* fish = t.second;
		if (fish->isActive())
		{
			for (int i = 0; i < MAX_SEAT_NUM; i++)
			{
				if (m_LaserZone[i].isInzone(fish))
				{
					if (m_pTable->isFishDead(
						m_LaserZone[i].m_money,
						fish->t_maxFishRadio,
						fish->t_minFishRadio,
						point))
					{

					}
				}
			}
		}
	}
}
int32 Screen::getFishNum()
{
	return m_ScreenFish.size();
}
NS_WF_END

