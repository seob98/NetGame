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

	bool pressSpace{};

	float DeadTime{};
	bool needle{ false };
	bool useneedle{ false };
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

		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/test4.bmp", L"test2");
		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/dao.bmp", L"dao");
		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/daodead_new.bmp", L"daodead");
		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/bazzi.bmp", L"bazzi");
		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/bazzidead_old.bmp", L"bazzidead");


		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/Win.bmp", L"win");
		CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/Lose.bmp", L"lose");
		 
		////CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/bazzidead_old.bmp", L"bazzidead");
		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win1p.bmp", L"win0");
		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win2p.bmp", L"win1");
		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/draw.bmp", L"draw");
		////CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win2p.bmp", L"draw");

		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Start.bmp", L"start");
		//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/number.bmp", L"loading");
	}

public:
	void Draw(HDC hdc);
	void DrawMainFrmUI(HDC hdc);
	void DrawWinnerUI(HDC hdc);
	void DrawLoserUI(HDC hdc);
	void DrawDrawUI(HDC hdc);
	void DrawStartUI(HDC hdc, int currentPlayerCnt, bool pressStart);
	void DrawLoadingUI(HDC hdc, int currentPlayerCnt, bool pressStart);
	void DrawItem(HDC hdc);
	void Move(bool playerA, std::vector<CBlock>& map);
	void MoveTrapped(bool playerA, std::vector<CBlock>& map);
	void StatusElse();

	void UpdateRect();
	void CheckCollisionMap(std::vector<CBlock>& map);

public:
	void SetPosX(int x);
	void SetPosY(int y);

public:
	int GetCurrentIndex(std::vector<CBlock>& map);
	int GetDir() { return playerDir; }
	void SetDir(int in) { playerDir = in; }
	int GetSize() { return size; }
	int GetState() { return (int)eCurState; }
	bool GetUseNeedle() { return useneedle; }
	void SetState(STATE in) { eCurState = in; }
	void CheckAnimationFrame1();

	RECT GetRect() { return rt; }
	POINT GetPos() { return pos; }
	int GetSpeed() { return speed; }
	bool isMoving() { return moving; }
	bool spaceButton() { return pressSpace; }
	void SetMoving(bool in) { moving = in; }
	int GetballonCnt() { return ballonMaxCnt; }
	int GetballonLength() { return ballonLength; }
	float GetDeadTime() { return DeadTime; }
public:
	bool SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, std::vector<CPlayer>& players, bool player0, int ballonID);
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

	//void CheckWin();

	void CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams);
	void CheckCollisionPlayers(std::vector<CPlayer>& _players);
	STATE Get_State() { return eCurState; };
};

