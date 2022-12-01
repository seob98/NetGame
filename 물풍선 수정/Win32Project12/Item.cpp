#include "stdafx.h"
#include "Item.h"


CItem::CItem(POINT _pos, int _size, int _index, int type)
	: pos(_pos), size(_size), index(_index)
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Item/ballon4.bmp", L"ballon3");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Item/needle.bmp", L"needle");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Item/skate.bmp", L"skate");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Item/wave.bmp", L"wave");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Item/maxwave.bmp", L"maxwave");

	rt.left = pos.x - size;
	rt.top = pos.y - size;
	rt.right = pos.x + size;
	rt.bottom = pos.y + size;

	eType = (ItemType)type;
}

void CItem::Draw(HDC hdc)
{
	if (taken)
		return;
	HDC hMemDC;
	floatingValue += 0.07f;
	float bonusValue = sin(floatingValue) * 5.f;
	switch (eType)
	{
	case ballon:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"ballon3");
		break;
	case needle:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"needle");
		break;
	case skate:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"skate");
		break;
	case wave:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"wave");
		break;
	case maxwave:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"maxwave");
		break;
	}

	GdiTransparentBlt(hdc
		, rt.left, rt.top + (int)bonusValue
		, 38, 38 
		, hMemDC
		, 0, 0
		, 38, 38
		, RGB(255, 255, 255));
}

void CItem::CheckCollisionPlayers(std::vector<CPlayer>& _players)
{
	if (taken)
		return;
	RECT temp{};
	
	for (auto& player : _players)
	{
		if (player.GetState() <=4)
		{
			if (IntersectRect(&temp, &rt, &player.GetRect()))
			{
				player.AcquireItem((int)(eType));
				taken = true;
			}
		}
	}
}

void CItem::CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams)
{
	if (taken)
		return;
	RECT temp{};

	for (auto& waterstream : _waterstreams)
		if (IntersectRect(&temp, &rt, &waterstream.GetRect()))
		{
			taken = true;
		}
}

