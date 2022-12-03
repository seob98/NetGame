#include "stdafx.h"
#include "Item.h"


CItem::CItem(POINT _pos, int _size, int _index)
	: pos(_pos), size(_size), index(_index)
{
	rt.left = pos.x - size;
	rt.top = pos.y - size;
	rt.right = pos.x + size;
	rt.bottom = pos.y + size;

	eType = (ItemType)(rand() % 5);
}

void CItem::Draw(HDC hdc)
{

}

void CItem::CheckCollisionPlayers(std::vector<CPlayer*> _players)
{
	if (taken)
		return;
	RECT temp{};
	
	for (auto& player : _players)
	{
		if (player->GetState() <=4)
		{
			RECT temp2 = player->GetRect();
			if (IntersectRect(&temp, &rt, &temp2))
			{
				player->AcquireItem((int)(eType));
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
	{
		RECT temp2 = waterstream.GetRect();
		if (IntersectRect(&temp, &rt, &temp2))
		{
			taken = true;
		}
	}
}

