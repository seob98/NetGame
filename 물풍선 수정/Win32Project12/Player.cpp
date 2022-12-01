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
	if (player0)
	{
		if (eCurState == WIN)
		{
			HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"win0");

			GdiTransparentBlt(hdc
				, 200, 200
				, 260, 90
				, hMemDC
				, 0, 0
				, 260, 90
				, RGB(100, 100, 100));
		}
	}

	else
	{
		if (eCurState == WIN)
		{
			HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"win1");

			GdiTransparentBlt(hdc
				, 200, 200
				, 260, 90
				, hMemDC
				, 0, 0
				, 260, 90
				, RGB(100, 100, 100));
		}
	}
}

void CPlayer::DrawDrawUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"draw");

	GdiTransparentBlt(hdc
		, 200, 200
		, 443, 167
		, hMemDC
		, 0, 0
		, 443, 167
		, RGB(255, 0, 255));
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
		return;			//start버튼 눌러야 나오는 UI

	if (currentPlayerCnt >= 4)
		return;			//플레이어 4명이상이면 실행X


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


	if (clientNum == 0)
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

	if (PLAYERS[myClientID].eCurState != IDLE && PLAYERS[myClientID].eCurState != LEFT &&
		PLAYERS[myClientID].eCurState != UP && PLAYERS[myClientID].eCurState != RIGHT && PLAYERS[myClientID].eCurState != DOWN)
		return;

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
		//PLAYERS[myClientID].pos.y -= 0;
		PLAYERS[myClientID].playerDir = 1;
		PLAYERS[myClientID].eCurState = UP;
		PLAYERS[myClientID].moving = true;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		//PLAYERS[myClientID].pos.y += 0;
		PLAYERS[myClientID].playerDir = 3;
		PLAYERS[myClientID].eCurState = DOWN;
		PLAYERS[myClientID].moving = true;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		//PLAYERS[myClientID].pos.x -= 0;
		PLAYERS[myClientID].playerDir = 0;
		PLAYERS[myClientID].eCurState = LEFT;
		PLAYERS[myClientID].moving = true;
		SetEvent(SendEvent);
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		//PLAYERS[myClientID].pos.x += 0;
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
			PLAYERS[myClientID].moving = true;
		}
		else
			PLAYERS[myClientID].frame.StartX = 1;
		SetEvent(SendEvent);
	}

	UpdateRect();
	index = GetCurrentIndex(map);
}




void CPlayer::MoveTrapped(bool playerA, std::vector<CBlock>& map)
{
	int trappedSpeed = 1;
	if (PLAYERS[myClientID].eCurState != TRAPPED)
		return;

	if (!GetActiveWindow())
		return;

	PLAYERS[myClientID].eCurState = TRAPPED;
	PLAYERS[myClientID].moving = false;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 1;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 3;
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 0;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		PLAYERS[myClientID].playerDir = 2;
	}
	else
	{
		PLAYERS[myClientID].playerDir = playerDir;
	}

	SetEvent(SendEvent);

	UpdateRect();
	index = GetCurrentIndex(map);

}

void CPlayer::StatusElse()
{
	//move, moveTrapped제외의 상태에는 상시로 해당 함수를 호출한다.
	//서버의 업데이트를 위해서는 클라이언트가 계속 메시지를 보내고 있어야한다.
	if (PLAYERS[myClientID].eCurState != SAVED && PLAYERS[myClientID].eCurState != DIE &&
		PLAYERS[myClientID].eCurState != DEAD && PLAYERS[myClientID].eCurState != WIN)
		return;


	PLAYERS[myClientID].moving = eCurState;
	PLAYERS[myClientID].moving = false;
	PLAYERS[myClientID].playerDir = 3;	//승리포즈, 탈출모션, 죽는 모션, 시체 모션 등..
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

bool CPlayer::SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, std::vector<CPlayer>& players, bool player0, int ballonID)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE)
		return false;						//물풍선 설치 실패

	int index = GetCurrentIndex(map);
	for (auto ballon : ballons)
	{
		if (ballon.GetIndex() == index)
			return false;					//물풍선 설치 실패
	}
	POINT pos = map[index].GetPos();
	int size = map[index].GetSize();

	if (ballonCurCnt < ballonMaxCnt)
	{														//인자 보내봤자 암것도 안함. 지우면 뭔가 에러뜨는거같아서 냅둠.
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

			eCurState = SAVED;
			frame.StartX = 0;
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
}

void CPlayer::STATE_CHECK()
{
	if (ePreState != eCurState)
	{
		switch (eCurState)
		{
		case IDLE:
			frame.StartX = 0;
			frame.StateY = 0;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case UP:
			frame.StartX = 0;
			frame.StateY = 1;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 55;// 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case DOWN:
			frame.StartX = 0;
			frame.StateY = 2;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 55; //100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case LEFT:
			frame.StartX = 0;
			frame.StateY = 3;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 55; //100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case RIGHT:
			frame.StartX = 0;
			frame.StateY = 4;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 55; //100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case WIN:
			frame.StartX = 0;
			frame.StateY = 3;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 200;				// 프레임 딜레이 (높을수록 느립니다.)
			break;

		case TRAPPED: //trapped,dead,saved는 다른 비트맵 사용
			frame.StartX = 0;
			frame.StateY = 0;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 400;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case SAVED:
			frame.StartX = 0;
			frame.StateY = 1;					// 스프라이트 이미지 세로 수 
			frame.EndX = 2;
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case DIE:
			frame.StartX = 0;
			frame.StateY = 2;					// 스프라이트 이미지 세로 수 
			frame.EndX = 6;
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 50; //;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case DEAD:
			frame.StartX = 3;
			frame.StateY = 2;					// 스프라이트 이미지 세로 수 
			frame.EndX = 0;						// 단일이미지라 상관없다.
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 1000;				// 프레임 딜레이 (높을수록 느립니다.)
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
		{
			frame.StartX = frame.EndX;
		}
		//if (frame.StartX >= frame.EndX)
		//{
		//	if (eCurState == TRAPPED)
		//		eCurState = DIE;			//너무 오래 갖혀있으면 죽고

		//	else if (eCurState == SAVED)
		//		eCurState = IDLE;			//탈출 애니메이션 끝나면 IDLE로

		//	else if (eCurState == DIE)
		//		eCurState = DEAD;			//죽는 애니메이션 끝나면 시체

		//	frame.StartX = 0;
		//}
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
		DeadTime += 10.f;			// 죽으면 시간 카운트
	else
		return;

	//if (DeadTime > 1000)
	//{
	//	if (player0)				//죽은놈이 0이고 1이 생존해있다면 승리
	//	{
	//		STATE opponentState = _players[1].Get_State();
	//		if (opponentState != DEAD && opponentState != SAVED && opponentState != TRAPPED && opponentState != DIE)
	//			_players[1].Set_Winner();
	//	}

	//	else						//죽은놈이 1이고 0이 생존해있다면 승리
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
		return;						//내가 물방울상태아니면 안함.

	RECT temp{};
	//if (player0)					// 0번 플레이어의 경우 1번과 연산
	//{
	//	STATE opponentState = _players[1].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//상대방도 죽었거나 살어나는 모션중이거나 물방울 갇혔거나 죽어있으면 충돌처리 안한다.
	//	if (IntersectRect(&temp, &rt, &_players[1].GetRect()))
	//		eCurState = DIE;		//상대방이 멀쩡한 상태에서 부딪히면 펑
	//}

	//else							// 1번 플레이어의 경우 0번과 연산
	//{
	//	STATE opponentState = _players[0].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//상대방도 죽었거나 살어나는 모션중이거나 물방울 갇혔거나 죽어있으면 충돌처리 안한다.
	//	if (IntersectRect(&temp, &rt, &_players[0].GetRect()))
	//		eCurState = DIE;		//상대방이 멀쩡한 상태에서 부딪히면 펑
	//}

	for (auto player : _players)
	{
		if (player.clientNum == clientNum)
			continue;						//자기자신과 충돌은 스킵한다.

		STATE opponentState = player.Get_State();
		if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
			continue;		//상대방도 죽었거나 살어나는 모션중이거나 물방울 갇혔거나 죽어있으면 충돌처리 안한다.
		if (IntersectRect(&temp, &rt, &player.GetRect()))
			eCurState = DIE;		//상대방이 멀쩡한 상태에서 부딪히면 펑
	}
}




