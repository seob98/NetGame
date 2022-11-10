#pragma once
class CWaterStream
{
public:
	CWaterStream(POINT _pos, int _index, int _size, int _summonTime, int _dir);

private:
	POINT pos{};
	int index{};
	int size{};
	RECT rt{};
	int summonTime{};
	int dir{};

	FRAME frame{};
	bool dead{};
public:
	void Draw(HDC hdc);
	void Update();

public:
	bool GetDead() { return dead; }
	int GetIndex() { return index; }
	RECT GetRect() { return rt; }
public:
	void UpdateSummonTime();
};

