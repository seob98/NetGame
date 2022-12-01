#include "stdafx.h"
#include "Obstacle.h"
#include "Block.h"
#include "Item.h"



CObstacle::CObstacle(POINT Pos, int Number, std::vector<CBlock>& blocks, bool _invincible,int item_type)
{
	pos = Pos;
	index = Number;

	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Tile/Tile1.bmp", L"Type1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Tile/Tile2.bmp", L"Type2");

	blocks[index].SetCollisionEnable(true);
	inExplosionRange = false;

	Invincible = _invincible;
	itemtype = item_type;
}

void CObstacle::CheckExplosionRange(std::vector<CBlock>& _blocks)
{
	if (inExplosionRange)
	{
		if (!Invincible)
		{
			dead = true;
			_blocks[index].SetCollisionEnable(false);
		}
	}
}

void CObstacle::draw(HDC hdc)
{
	if (dead)
		return;
	HDC hMemDC{};
	//switch (eType)
	//{
	//case Type1:
	//	hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Type1");
	//	break;
	//case Type2:
	//	hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Type2");
	//	break;
	//}

	if(Invincible)
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Type2");
	else
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Type1");

	GdiTransparentBlt(hdc
		, pos.x - 22, pos.y - 22
		, 40, 40
		, hMemDC
		, 0, 0
		, 38, 38
		, RGB(255, 255, 255));
}

void CObstacle::makeItem(std::vector<CItem>& _items)
{
	if (!dead || madeItem || itemtype == -1)
		return;

	//int randNum = rand() % 10;
	//if (randNum < 7)
	//	_items.emplace_back(pos, 19, index);
	//madeItem = true;

	_items.emplace_back(pos, 19, index, itemtype);
	madeItem = true;
}

