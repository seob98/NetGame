#pragma once

class CBallon;
class CItem;
class CWaterStream;

class CPlayer
{
public:
	enum STATE {IDLE, UP , DOWN, LEFT, RIGHT, TRAPPED, SAVED, DIE, DEAD, WIN};
public:
	int clientNum{};
	int clinetTeam{};
private:
	POINT pos{};
	int size{ 10 };
	int speed{ 3 }; //3
	int playerDir{ 3 };
	RECT rt{};
	int index{};
private:
	int ballonLength = 2; //2
	int ballonMaxCnt{ 2 }; //2
	int ballonCurCnt{ 0 };

	int maxSpeed{ 7 };
	int maxBallonLength{ 7 };

	bool player0{};
	bool moving{};
	int DeadTime{};
	bool needle{ false };

private:
	STATE ePreState{ IDLE };
	STATE eCurState{ IDLE };
	FRAME frame{};

public:
	CPlayer();
	CPlayer(POINT Position)
		:pos(Position)
	{
		rt.left = pos.x - size;
		rt.top = pos.y - size;
		rt.right = pos.x + size;
		rt.bottom = pos.y + size;
		ballonLength = 2;
	}

public:
	void Draw(HDC hdc);
	void DrawMainFrmUI(HDC hdc);
	void DrawWinnerUI(HDC hdc);
	void DrawDrawUI(HDC hdc);
	void DrawStartUI(HDC hdc, bool Start);
	void DrawItem(HDC hdc);
	void Move(bool playerA, std::vector<CBlock>& map);
	void MoveTrapped(bool playerA, std::vector<CBlock>& map);
	void UpdateRect();
	void CheckCollisionMap(std::vector<CBlock>& map);

public:
	void SetPosX(int x);
	void SetPosY(int y);

public:
	int GetCurrentIndex(std::vector<CBlock>& map);
	int GetDir() { return playerDir; }
	int GetSize() { return size; }
	int GetState() { return (int)eCurState; }
	RECT GetRect() { return rt; }
	POINT GetPos() { return pos; }

public:
	void SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, std::vector<CPlayer>& players, bool player0);
	void BallonCntUpdate() { ballonCurCnt -= 1; }

	void useNeedle();
public:
	void AcquireItem(int _eType);

public:
	void SetPlayer0() { player0 = true; }
	void STATE_CHECK();
	void Update_Frame();
	void Update_Frame_Once();
	void Update_DeadTime(std::vector<CPlayer>& _players);
	void Set_Winner() { eCurState = WIN; }

	void CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams);
	void CheckCollisionPlayers(std::vector<CPlayer>& _players);
	STATE Get_State() { return eCurState; };
};

