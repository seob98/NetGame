#pragma once
class CBlock
{
public:
	CBlock(POINT Pos, int Size, int Number)
		: pos(Pos), size(Size), index(Number), collisionEnabled(false)
	{
		rt.left = Pos.x - Size;
		rt.top = Pos.y - Size;
		rt.right = Pos.x + Size;
		rt.bottom = Pos.y + Size;

		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Tile/Tile6.bmp", L"tile");
	};

private:
	bool collisionEnabled{};
	POINT pos{};
	int size{};
	RECT rt{};
	int index{};			//가로순으로 1,2,3.. 번째 블럭.
public:
	RECT GetRect() { return rt; }
	int GetSize() { return size; }
	POINT GetPos() { return pos; }
	int GetIndex() { return index; }
	bool GetCollisionEnable() { return collisionEnabled; }

public:
	void SetCollisionEnable(bool set) { collisionEnabled = set; }
public:
	void draw(HDC hdc);
};

