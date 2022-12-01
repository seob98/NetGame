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
	int index{};			//���μ����� 1,2,3.. ��° ��.
	bool inExplosionRange{};
	bool dead{};
	bool Invincible{};
	int itemtype{};	// �������� �޾Ƽ� �����ϴ� ���� ������Ÿ��
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

