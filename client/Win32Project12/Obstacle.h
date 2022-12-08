#pragma once

class CItem;

class CObstacle
{
public:
	enum Type { Type1, Type2 };
public:
	CObstacle(POINT Pos, int Number, std::vector<CBlock>& _blocks, bool _invincible, int item_type);

private:
	Type eType = Type1;
	POINT pos{};
	int index{};			//가로순으로 1,2,3.. 번째 블럭.
	bool inExplosionRange{};
	bool dead{};
	bool Invincible{};
	int itemtype{};	// 서버에서 받아서 저장하는 블럭의 아이템타입
public:
	//POINT GetPos() { return pos; }
	const int GetIndex() { return index; }
	void SetInsideExplosionRange() { inExplosionRange = true; }
	void CheckExplosionRange(std::vector<CBlock>& _blocks);

	POINT GetPos() { return pos; }
	bool GetDead() { return dead; }
	bool madeItem{false};
public:
	void draw(HDC hdc);
	void makeItem(std::vector<CItem>& _items);
};

