#include "stdafx.h"
#include "Player.h"
#include "Ballon.h"
#include "Item.h"

extern std::vector<CPlayer> PLAYERS;
extern int myClientID;
extern HANDLE SendEvent;

CPlayer::CPlayer()
{
}

void CPlayer::Draw(HDC hdc)
{
	HDC hMemDC;

	if (eCurState == IDLE || eCurState == UP || eCurState == LEFT || eCurState == RIGHT || eCurState == DOWN)
	{
		if (clinetTeam == 0)
			hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"dao");
		else
			hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"bazzi");

		int adjustNumX{ -15 };
		int adjustNumY{ -37 };
		GdiTransparentBlt(hdc
			, rt.left + adjustNumX, rt.top + adjustNumY
			, 50, 60
			, hMemDC
			, frame.StartX * 50, frame.StateY * 60
			, 50, 60
			, RGB(100, 100, 100));
	}

	else
	{
		if (clinetTeam == 0)
			hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"daodead");
		else
			hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"bazzidead");

		int adjustNumX{ -23 };
		int adjustNumY{ -30 };

		if (eCurState == WIN)
		{
			adjustNumX = -26;
			adjustNumY = -40;
		}

		GdiTransparentBlt(hdc
			, rt.left + adjustNumX, rt.top + adjustNumY
			, 70, 70
			, hMemDC
			, frame.StartX * 70, frame.StateY * 70
			, 70, 70
			, RGB(0, 255, 0));
	}

	//Rectangle(hdc, pos.x - size, pos.y - size, pos.x + size, pos.y + size);
}

void CPlayer::DrawMainFrmUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"test2");

	GdiTransparentBlt(hdc
		, 0, 0
		, 800, 600
		, hMemDC
		, 0, 0
		, 800, 600
		, RGB(255, 255, 255));
}

void CPlayer::DrawWinnerUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"win");

	GdiTransparentBlt(hdc
		, 170, 170
		, 290 * 1.2, 50 * 1.2
		, hMemDC
		, 0, 0
		, 290, 50 
		, RGB(182, 185, 184));
}

void CPlayer::DrawLoserUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"lose");

	GdiTransparentBlt(hdc
		, 170, 170
		, 290 * 1.2, 50 * 1.2
		, hMemDC
		, 0, 0
		, 290, 50
		, RGB(182, 185, 184));
}

void CPlayer::DrawDrawUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"draw");

	GdiTransparentBlt(hdc
		, 170, 170
		, 290 * 1.2, 50 * 1.2
		, hMemDC
		, 0, 0
		, 290, 50
		, RGB(182, 185, 184));
}

void CPlayer::DrawStartUI(HDC hdc, int currentPlayerCnt, bool pressStart)
{
	if (pressStart)
		return;

	if (currentPlayerCnt >= 4)
		return;

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"start");

	GdiTransparentBlt(hdc
		, 0, 0
		, 800, 600
		, hMemDC
		, 0, 0
		, 800, 600
		, RGB(255, 245, 255));
}

void CPlayer::DrawLoadingUI(HDC hdc, int currentPlayerCnt, bool pressStart)
{
	return;

	if (!pressStart)
		return;			//start???? ?????? ?????? UI

	if (currentPlayerCnt >= 4)
		return;			//???????? 4?????????? ????X


	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"loading");

	GdiTransparentBlt(hdc
		, 200, 200
		, 150, 17
		, hMemDC
		, 0, 0
		, 150, 17
		, RGB(255, 245, 255));
}

void CPlayer::DrawItem(HDC hdc)
{
	if (!needle)
		return;
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"needle");


	if (clientNum == 0 || clientNum == 1 || clientNum == 2 || clientNum == 3)
	{
		GdiTransparentBlt(hdc
			, 664, 485
			, 38, 38
			, hMemDC
			, 0, 0
			, 38, 38
			, RGB(255, 255, 255));
	}
}

void CPlayer::Move(bool playerA, std::vector<CBlock>& map)
{
	//if (PLAYERS[myClientID].eCurState == TRAPPED || PLAYERS[myClientID].eCurState == SAVED ||
	//	PLAYERS[myClientID].eCurState == DEAD || PLAYERS[myClientID].eCurState == DIE ||
	//	PLAYERS[myClientID].eCurState == WIN)
	//	return;
	int trappedSpeed = 1;
	



	if (PLAYERS[myClientID].eCurState == IDLE || PLAYERS[myClientID].eCurState == LEFT ||
		PLAYERS[myClientID].eCurState == UP || PLAYERS[myClientID].eCurState == RIGHT || PLAYERS[myClientID].eCurState == DOWN)
	{
		if (!GetActiveWindow())
			return;

		if (GetAsyncKeyState(VK_SPACE))
		{
			PLAYERS[myClientID].pressSpace = true;
		}
		else
			PLAYERS[myClientID].pressSpace = false;

		if (GetAsyncKeyState('W') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 1;
			PLAYERS[myClientID].eCurState = UP;
			PLAYERS[myClientID].moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 3;
			PLAYERS[myClientID].eCurState = DOWN;
			PLAYERS[myClientID].moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 0;
			PLAYERS[myClientID].eCurState = LEFT;
			PLAYERS[myClientID].moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 2;
			PLAYERS[myClientID].eCurState = RIGHT;
			PLAYERS[myClientID].moving = true;
			SetEvent(SendEvent);
		}
		else
		{
			PLAYERS[myClientID].moving = false;

			if (PLAYERS[myClientID].eCurState == DOWN || PLAYERS[myClientID].eCurState == IDLE)
			{
				PLAYERS[myClientID].eCurState = IDLE;
				PLAYERS[myClientID].moving = false;
			}
			else
				PLAYERS[myClientID].frame.StartX = 1;
			SetEvent(SendEvent);
		}
	}
	else if (PLAYERS[myClientID].eCurState == TRAPPED)
	{
		if (!GetActiveWindow())
			return;

		if (GetAsyncKeyState('W') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 1;
			moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 3;
			moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 0;
			moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			PLAYERS[myClientID].playerDir = 2;
			moving = true;
			SetEvent(SendEvent);
		}
		else if (GetAsyncKeyState('Q') & 0x8000)
		{
			PLAYERS[myClientID].useNeedle();
			//PLAYERS[myClientID].useneedle = true;
			SetEvent(SendEvent);
		}
		else {
			PLAYERS[myClientID].playerDir = -1;
			moving = false;
			PLAYERS[myClientID].useneedle = false;
			SetEvent(SendEvent);
		}
	}

	UpdateRect();
	index = GetCurrentIndex(map);
}



/*
void CPlayer::MoveTrapped(bool playerA, std::vector<CBlock>& map)
{
	int trappedSpeed = 1;
	if (eCurState != TRAPPED)
		return;

	if (!GetActiveWindow())
		return;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 1;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 3;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 0;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 2;
		SetEvent(SendEvent);
	}

	UpdateRect();
	index = GetCurrentIndex(map);

}
*/

void CPlayer::StatusElse()
{
	//move, moveTrapped?????? ???????? ?????? ???? ?????? ????????.
	//?????? ?????????? ???????? ???????????? ???? ???????? ?????? ??????????.
	if (eCurState != SAVED && eCurState != DIE &&
		eCurState != DEAD && eCurState != WIN)
		return;

	PLAYERS[myClientID].playerDir = 3;	//????????, ????????, ???? ????, ???? ???? ??..
	SetEvent(SendEvent);
}



void CPlayer::UpdateRect()
{
	rt.left = pos.x - size;
	rt.top = pos.y - size;
	rt.right = pos.x + size;
	rt.bottom = pos.y + size;
}

void CPlayer::CheckCollisionMap(std::vector<CBlock>& map)
{
	RECT temp{};
	for (auto& mapBlock : map)
	{
		if (mapBlock.GetCollisionEnable())
		{
			if (IntersectRect(&temp, &mapBlock.GetRect(), &rt))
			{
				if (playerDir == 0)
					pos.x = mapBlock.GetRect().right + size;
				else if (playerDir == 1)
					pos.y = mapBlock.GetRect().bottom + size;
				else if (playerDir == 2)
					pos.x = mapBlock.GetRect().left - size;
				else if (playerDir == 3)
					pos.y = mapBlock.GetRect().top - size;
			}
		}
	}

	if (pos.x - size < 20)
		pos.x = 20 + size;
	else if (pos.y - size < 40)
		pos.y = 40 + size;
	else if (pos.x + size > 620)
		pos.x = 620 - size;
	else if (pos.y + size > 560)
		pos.y = 560 - size;

	UpdateRect();
	index = GetCurrentIndex(map);
}

void CPlayer::SetPosX(int x)
{
	pos.x = x;
	UpdateRect();
}

void CPlayer::SetPosY(int y)
{
	pos.y = y;
	UpdateRect();
}

int CPlayer::GetCurrentIndex(std::vector<CBlock>& map)
{
	for (auto& block : map)
		if (PtInRect(&block.GetRect(), pos))
		{
			index = block.GetIndex();
			return index;
		}
}

void CPlayer::CheckAnimationFrame1()
{
	//if(eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState)
	if (eCurState == LEFT || eCurState == RIGHT || eCurState == UP || eCurState == DOWN)
	{
		if (!moving)
			frame.StartX = 1;
	}
}

bool CPlayer::SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, std::vector<CPlayer>& players, bool player0, int ballonID)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE)
		return false;						//?????? ???? ????

	int index = GetCurrentIndex(map);
	for (auto ballon : ballons)
	{
		if (ballon.GetIndex() == index)
			return false;					//?????? ???? ????
	}
	POINT pos = map[index].GetPos();
	int size = map[index].GetSize();

	if (ballonCurCnt < ballonMaxCnt)
	{														//???? ???????? ?????? ????. ?????? ???? ???????????????? ????.
		ballons.emplace_back(pos, size, index, ballonLength, player0, clientNum, this, ballonID);
		ballonCurCnt += 1;
		return true;
	}

	return false;
}

void CPlayer::useNeedle()
{
	if (needle)
	{
		if (eCurState == TRAPPED)
		{
			needle = false;
			PLAYERS[myClientID].useneedle = true;
			//eCurState = SAVED;
			//frame.StartX = 0;
		}
	}
}

void CPlayer::AcquireItem(int _eType)
{
	switch (_eType)
	{
	case 0:
		ballonMaxCnt += 1;
		if (ballonMaxCnt > 7)
			ballonMaxCnt = 7;
		break;
	case  1:
		needle = true;
		break;
	case  2:
		speed += 1;
		if (speed > maxSpeed)
			speed = maxSpeed;
		break;

	case 3:
		ballonLength += 1;
		if (ballonLength > maxBallonLength)
			ballonLength = maxBallonLength;
		break;
	case 4:
		ballonLength = maxBallonLength;
		break;
	}
	SetEvent(SendEvent);
}

void CPlayer::STATE_CHECK()
{
	if (ePreState != eCurState)
	{
		switch (eCurState)
		{
		case IDLE:
			frame.StartX = 0;
			frame.StateY = 0;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 400;				// ?????? ?????? (???????? ????????.)
			break;
		case UP:
			frame.StartX = 0;
			frame.StateY = 1;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case DOWN:
			frame.StartX = 0;
			frame.StateY = 2;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case LEFT:
			frame.StartX = 0;
			frame.StateY = 3;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case RIGHT:
			frame.StartX = 0;
			frame.StateY = 4;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case WIN:
			frame.StartX = 0;
			frame.StateY = 3;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;						// ?????????? ?????? ???? ?? 
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 200;				// ?????? ?????? (???????? ????????.)
			break;

		case TRAPPED: //trapped,dead,saved?? ???? ?????? ????
			frame.StartX = 0;
			frame.StateY = 0;					// ?????????? ?????? ???? ?? 
			frame.EndX = 4;
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 400;				// ?????? ?????? (???????? ????????.)
			break;
		case SAVED:
			frame.StartX = 0;
			frame.StateY = 1;					// ?????????? ?????? ???? ?? 
			frame.EndX = 2;
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case DIE:
			frame.StartX = 0;
			frame.StateY = 2;					// ?????????? ?????? ???? ?? 
			frame.EndX = 6;
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 100;				// ?????? ?????? (???????? ????????.)
			break;
		case DEAD:
			frame.StartX = 3;
			frame.StateY = 2;					// ?????????? ?????? ???? ?? 
			frame.EndX = 0;						// ???????????? ????????.
			frame.Time = 0;						// ???? ???? ????
			frame.DelayTime = 1000;				// ?????? ?????? (???????? ????????.)
			break;
		}
		ePreState = eCurState;
	}
}

void CPlayer::Update_Frame()
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DIE || eCurState == DEAD)
		return;

	if (eCurState == WIN)
		moving = true;

	if (moving)
		frame.Time += 10.f;
	if (frame.Time > frame.DelayTime)
	{
		frame.Time = 0.f;
		frame.StartX += 1;
	}

	if (frame.StartX >= frame.EndX)
		frame.StartX = 0;
}

void CPlayer::Update_Frame_Once()
{
	if (eCurState == WIN)
		return;

	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DIE)
	{
		frame.Time += 10.f;
		if (frame.Time > frame.DelayTime)
		{
			frame.Time = 0.f;
			frame.StartX += 1;
		}

		if (frame.StartX >= frame.EndX)
		if (frame.StartX >= frame.EndX)
		{
			frame.StartX = frame.EndX;
		}
	}

	if (eCurState == DEAD)
	{
		frame.StartX = 3;
		frame.StateY = 2;
	}
}

void CPlayer::Update_DeadTime(std::vector<CPlayer>& _players)
{
	if (eCurState == DEAD)
		DeadTime += 10.f;			// ?????? ???? ??????
	else
		return;

	//if (DeadTime > 1000)
	//{
	//	if (player0)				//???????? 0???? 1?? ???????????? ????
	//	{
	//		STATE opponentState = _players[1].Get_State();
	//		if (opponentState != DEAD && opponentState != SAVED && opponentState != TRAPPED && opponentState != DIE)
	//			_players[1].Set_Winner();
	//	}

	//	else						//???????? 1???? 0?? ???????????? ????
	//	{
	//		STATE opponentState = _players[0].Get_State();
	//		if (opponentState != DEAD && opponentState != SAVED && opponentState != TRAPPED && opponentState != DIE)
	//			_players[0].Set_Winner();
	//	}
	//}
}

void CPlayer::CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE || eCurState == WIN)
		return;

	RECT temp{};

	for (auto& waterstream : _waterstreams)
		if (IntersectRect(&temp, &rt, &waterstream.GetRect()))
		{
			eCurState = TRAPPED;
			frame.StartX = 0;
			frame.EndX = 4;
		}
}

void CPlayer::CheckCollisionPlayers(std::vector<CPlayer>& _players)
{
	if (eCurState != TRAPPED)
		return;						//???? ???????????????? ????.

	RECT temp{};
	//if (player0)					// 0?? ?????????? ???? 1???? ????
	//{
	//	STATE opponentState = _players[1].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//???????? ???????? ???????? ???????????? ?????? ???????? ?????????? ???????? ??????.
	//	if (IntersectRect(&temp, &rt, &_players[1].GetRect()))
	//		eCurState = DIE;		//???????? ?????? ???????? ???????? ??
	//}

	//else							// 1?? ?????????? ???? 0???? ????
	//{
	//	STATE opponentState = _players[0].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//???????? ???????? ???????? ???????????? ?????? ???????? ?????????? ???????? ??????.
	//	if (IntersectRect(&temp, &rt, &_players[0].GetRect()))
	//		eCurState = DIE;		//???????? ?????? ???????? ???????? ??
	//}

	for (auto player : _players)
	{
		if (player.clientNum == clientNum)
			continue;						//?????????? ?????? ????????.

		STATE opponentState = player.Get_State();
		if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
			continue;		//???????? ???????? ???????? ???????????? ?????? ???????? ?????????? ???????? ??????.
		if (IntersectRect(&temp, &rt, &player.GetRect()))
			eCurState = DIE;		//???????? ?????? ???????? ???????? ??
	}
}




