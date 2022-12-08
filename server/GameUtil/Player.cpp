#include "stdafx.h"
#include "Player.h"
#include "Ballon.h"
#include "Item.h"
#include <iostream>
using namespace std;

CPlayer::CPlayer()
{
}

void CPlayer::Draw(HDC hdc)
{
	HDC hMemDC;

	if (eCurState == IDLE || eCurState == UP || eCurState == LEFT || eCurState == RIGHT || eCurState == DOWN)
	{
		int adjustNumX{ -15 };
		int adjustNumY{ -37 };
	}

	else
	{
		int adjustNumX{ -23 };
		int adjustNumY{ -30 };

		if (eCurState == WIN)
		{
			adjustNumX = -26;
			adjustNumY = -40;
		}
	}

	//Rectangle(hdc, pos.x - size, pos.y - size, pos.x + size, pos.y + size);
}

void CPlayer::DrawMainFrmUI(HDC hdc)
{

}

void CPlayer::DrawWinnerUI(HDC hdc)
{

}

void CPlayer::DrawDrawUI(HDC hdc)
{
}

void CPlayer::DrawStartUI(HDC hdc, bool Start)
{
}

void CPlayer::DrawItem(HDC hdc)
{
}

void CPlayer::Move(std::vector<CBlock>& map, int pressedButton)
{
	int trappedSpeed = 1;
	//if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE || eCurState == WIN)
	//	return;

	if (eCurState == IDLE || eCurState == LEFT || eCurState == UP || eCurState == RIGHT || eCurState == DOWN) {

		if (pressedButton == 1)
		{
			pos.y -= speed;
			playerDir = 1;
			eCurState = UP;
			moving = true;
		}
		else if (pressedButton == 3)
		{
			pos.y += speed;
			playerDir = 3;
			eCurState = DOWN;
			moving = true;
		}
		else if (pressedButton == 0)
		{
			pos.x -= speed;
			playerDir = 0;
			eCurState = LEFT;
			moving = true;
		}
		else if (pressedButton == 2)
		{
			pos.x += speed;
			playerDir = 2;
			eCurState = RIGHT;
			moving = true;
		}
		else
		{
			moving = false;
			if (eCurState == DOWN || eCurState == IDLE)
			{
				moving = true;
				eCurState = IDLE;
			}
			else
				frame.StartX = 1;
		}
	}
	else if (eCurState == TRAPPED)
	{
		if (pressedButton == 1)
		{
			pos.y -= trappedSpeed;
			eCurState = TRAPPED;
			playerDir = 1;
			moving = true;
		}
		else if (pressedButton == 3)
		{
			pos.y += trappedSpeed;
			eCurState = TRAPPED;
			playerDir = 3;
			moving = true;
		}
		else if (pressedButton == 0)
		{
			pos.x -= trappedSpeed;
			eCurState = TRAPPED;
			playerDir = 0;
			moving = true;
		}
		else if (pressedButton == 2)
		{
			pos.x += trappedSpeed;
			eCurState = TRAPPED;
			playerDir = 2;
			moving = true;
		}
	}

	UpdateRect();
	index = GetCurrentIndex(map);
}

void CPlayer::MoveTrapped( std::vector<CBlock>& map, int pressedButton)
{
	int trappedSpeed = 1;

	if (eCurState != TRAPPED)
		return;

	if (pressedButton == 1)
	{
		pos.y -= trappedSpeed;
		playerDir = 1;
	}
	else if (pressedButton == 2)
	{
		pos.y += trappedSpeed;
		playerDir = 3;

	}
	else if (pressedButton == 3)
	{
		pos.x -= trappedSpeed;
		playerDir = 0;
	}
	else if (pressedButton == 4)
	{
		pos.x += trappedSpeed;
		playerDir = 2;
	}

	UpdateRect();
	index = GetCurrentIndex(map);
	//}
}

void CPlayer::StatusElse()
{
	if (eCurState != SAVED && eCurState != DIE &&
		eCurState != DEAD && eCurState != WIN)
		return;

	moving = false;
	// 클라이언트와 다르게 딱히 할 일은 없다.
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
	for (auto &mapBlock : map)
	{
		if (mapBlock.GetCollisionEnable())
		{
			RECT temp2 = mapBlock.GetRect();
			if (IntersectRect(&temp, &temp2, &rt))
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
	if (pos.x <= 40)
		pos.x = 40;
	if (pos.x >= 600)
		pos.x = 600;
	UpdateRect();
}

void CPlayer::SetPosY(int y)
{
	pos.y = y;
	if (pos.y <= 60)
		pos.y = 60;
	if (pos.y >= 540)
		pos.y = 540;
	UpdateRect();
}

void CPlayer::SetState(STATE State)
{
	eCurState = State;
}

void CPlayer::SetState2(int eState)
{
	eCurState = (STATE)eState;
}

int CPlayer::GetCurrentIndex(std::vector<CBlock>& map)
{
	for (auto& block : map)
	{
		RECT temp2 = block.GetRect();
		if (PtInRect(&temp2, pos))
		{
			index = block.GetIndex();
			return index;
		}
	}

	return -1;
}

bool CPlayer::SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, /*std::vector<CPlayer>& players,*/ bool player0, int ballonID)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE || eCurState == WIN)
		return false;						//물풍선 설치 실패

	int index = GetCurrentIndex(map);
	for (auto& ballon : ballons)
	{
		if (ballon.GetIndex() == index)
			return false;					//물풍선 설치 실패
	}
	POINT pos = map[index].GetPos();
	int size = map[index].GetSize();

	if (ballonCurCnt < ballonMaxCnt)
	{														//인자 보내봤자 암것도 안함. 지우면 뭔가 에러뜨는거같아서 냅둠.
		ballons.emplace_back(pos, size, index, ballonLength, player0,  clientNum, this, ballonID);
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
			frame.DelayTime = 400;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case UP:
			frame.StartX = 0;
			frame.StateY = 1;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case DOWN:
			frame.StartX = 0;
			frame.StateY = 2;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case LEFT:
			frame.StartX = 0;
			frame.StateY = 3;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
			break;
		case RIGHT:
			frame.StartX = 0;
			frame.StateY = 4;					// 스프라이트 이미지 세로 수 
			frame.EndX = 4;						// 스프라이트 이미지 가로 수 
			frame.Time = 0;						// 현재 진행 시간
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
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
			frame.DelayTime = 100;				// 프레임 딜레이 (높을수록 느립니다.)
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

	if(moving)
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
			if (eCurState == TRAPPED)
			{
				eCurState = DIE;			//너무 오래 갖혀있으면 죽고
			}

			else if (eCurState == SAVED)
				eCurState = IDLE;			//탈출 애니메이션 끝나면 IDLE로

			else if (eCurState == DIE)
			{
				eCurState = DEAD;			//죽는 애니메이션 끝나면 시체
			}

			frame.StartX = 0;
		}

	}

	if (eCurState == DEAD)
	{
		frame.StartX = 3;
		frame.StateY = 2;
	}
}

void CPlayer::Update_DeadTime()
{
	if (eCurState == DEAD)
		DeadTime += 10.f;			// 죽으면 시간 카운트
	else
		return;
}

void CPlayer::CheckCollisionWaterStreams(std::vector<CWaterStream>& _waterstreams)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE || eCurState == WIN)
		return;

	RECT temp{};

	for (auto& waterstream : _waterstreams)
	{
		RECT temp2 = waterstream.GetRect();
		if (IntersectRect(&temp, &rt, &temp2))
		{
			eCurState = TRAPPED;
			frame.StartX = 0;
			frame.EndX = 4;
		}
	}
}

void CPlayer::CheckCollisionPlayers(std::vector<CPlayer*> _players)
{
	if (eCurState != TRAPPED)
		return;						//내가 물방울상태아니면 안함.

	RECT temp{};
	for (int i = 0; i < MAX_PLAYER_CNT; i++)
	{
		
		if (clientNum == _players[i]->clientNum)
			continue;
		if (clinetTeam == _players[i]->clinetTeam) // 배찌 팀
		{
			RECT temp2 = _players[i]->GetRect();
			if (IntersectRect(&temp, &rt, &temp2))
			{
				eCurState = SAVED;
			}
		}
		else
		{
			RECT temp2 = _players[i]->GetRect();
			if (IntersectRect(&temp, &rt, &temp2))
			{
				eCurState = DIE;
			}
		}
	}
}
