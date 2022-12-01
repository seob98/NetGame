#pragma once

class CPlayer;
class CWaterStream;

class CItem
{
public:
	enum ItemType {ballon, needle, skate, wave, maxwave};

	CItem(POINT _pos, int _size, int _index, int type);
private:
	POINT pos;
	int size;
	RECT rt{};
	ItemType eType{};
	int index;
	float floatingValue{};
	bool taken{ false };
public:
	void Draw(HDC hdc);

	void CheckCollisionPlayers(std::vector<CPlayer>& _players);
	void CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams);
};

