#include "stdafx.h"
#include "Obstacle.h"
#include "Block.h"
#include "Item.h"



CObstacle::CObstacle(POINT Pos, int Number, std::vector<CBlock>& blocks, bool _invincible)
{
	pos = Pos;
	index = Number;

	blocks[index].SetCollisionEnable(true);
	inExplosionRange = false;

	Invincible = _invincible;
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
}

void CObstacle::makeItem(std::vector<CItem>& _items)
{
	if (!dead || madeItem)
		return;

	int randNum = rand() % 10;
	if (randNum < 7)
		_items.emplace_back(pos, 19, index);
	madeItem = true;
}

