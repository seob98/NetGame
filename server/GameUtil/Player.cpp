#include "stdafx.h"
#include "Player.h"
#include "Ballon.h"
#include "Item.h"

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

void CPlayer::Move(bool playerA, std::vector<CBlock>& map)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE || eCurState == WIN)
		return;

	if (playerA)
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			pos.y -= speed;
			playerDir = 1;
			eCurState = UP;
			moving = true;
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			pos.y += speed;
			playerDir = 3;
			eCurState = DOWN;
			moving = true;
		}
		else if (GetAsyncKeyState('A') & 0x8000)
		{
			pos.x -= speed;
			playerDir = 0;
			eCurState = LEFT;
			moving = true;
		}
		else if (GetAsyncKeyState('D') & 0x8000)
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

	else
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			pos.y -= speed;
			playerDir = 1;
			eCurState = UP;
			moving = true;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			pos.y += speed;
			playerDir = 3;
			eCurState = DOWN;
			moving = true;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			pos.x -= speed;
			playerDir = 0;
			eCurState = LEFT;
			moving = true;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
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

	UpdateRect();
	index = GetCurrentIndex(map);
}

void CPlayer::MoveTrapped(bool playerA, std::vector<CBlock>& map)
{
	int trappedSpeed = 1;
	if (eCurState == WIN)
		return;



	if (eCurState == TRAPPED)
	{
		if (playerA)
		{
			if (GetAsyncKeyState('W') & 0x8000)
			{
				pos.y -= trappedSpeed;
				playerDir = 1;
			}
			else if (GetAsyncKeyState('S') & 0x8000)
			{
				pos.y += trappedSpeed;
				playerDir = 3;
			}
			else if (GetAsyncKeyState('A') & 0x8000)
			{
				pos.x -= trappedSpeed;
				playerDir = 0;
			}
			else if (GetAsyncKeyState('D') & 0x8000)
			{
				pos.x += trappedSpeed;
				playerDir = 2;
			}
		}

		else
		{
			if (GetAsyncKeyState(VK_UP) & 0x8000)
			{
				pos.y -= trappedSpeed;
				playerDir = 1;
			}
			else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			{
				pos.y += trappedSpeed;
				playerDir = 3;
			}
			else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			{
				pos.x -= trappedSpeed;
				playerDir = 0;
			}
			else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			{
				pos.x += trappedSpeed;
				playerDir = 2;
			}
		}

		UpdateRect();
		index = GetCurrentIndex(map);
	}
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
	UpdateRect();
}

void CPlayer::SetPosY(int y)
{
	pos.y = y;
	UpdateRect();
}

void CPlayer::SetState(STATE State)
{
	eCurState = State;
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

void CPlayer::SetupBallon(std::vector<CBlock>& map, std::vector<CBallon>& ballons, std::vector<CPlayer>& players, bool player0)
{
	if (eCurState == TRAPPED || eCurState == SAVED || eCurState == DEAD || eCurState == DIE)
		return;

	int index = GetCurrentIndex(map);
	for (auto ballon : ballons)
	{
		if (ballon.GetIndex() == index)
			return;
	}
	POINT pos = map[index].GetPos();
	int size = map[index].GetSize();

	if (ballonCurCnt < ballonMaxCnt)
	{														//���� �������� �ϰ͵� ����. ����� ���� �����ߴ°Ű��Ƽ� ����.
		ballons.emplace_back(pos, size, index, ballonLength, player0,  clientNum, this);
		ballonCurCnt += 1;
	}
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
			frame.StateY = 0;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 400;				// ������ ������ (�������� �����ϴ�.)
			break;
		case UP:
			frame.StartX = 0;
			frame.StateY = 1;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case DOWN:
			frame.StartX = 0;
			frame.StateY = 2;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case LEFT:
			frame.StartX = 0;
			frame.StateY = 3;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case RIGHT:
			frame.StartX = 0;
			frame.StateY = 4;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case WIN:
			frame.StartX = 0;
			frame.StateY = 3;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						// ��������Ʈ �̹��� ���� �� 
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 200;				// ������ ������ (�������� �����ϴ�.)
			break;

		case TRAPPED: //trapped,dead,saved�� �ٸ� ��Ʈ�� ���
			frame.StartX = 0;
			frame.StateY = 0;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 4;						
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 400;				// ������ ������ (�������� �����ϴ�.)
			 break;
		case SAVED:
			frame.StartX = 0;
			frame.StateY = 1;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 2;					
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case DIE:
			frame.StartX = 0;
			frame.StateY = 2;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 6;						
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 100;				// ������ ������ (�������� �����ϴ�.)
			break;
		case DEAD:
			frame.StartX = 3;
			frame.StateY = 2;					// ��������Ʈ �̹��� ���� �� 
			frame.EndX = 0;						// �����̹����� �������.
			frame.Time = 0;						// ���� ���� �ð�
			frame.DelayTime = 1000;				// ������ ������ (�������� �����ϴ�.)
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
				eCurState = DIE;			//�ʹ� ���� ���������� �װ�

			else if (eCurState == SAVED)
				eCurState = IDLE;			//Ż�� �ִϸ��̼� ������ IDLE��

			else if (eCurState == DIE)
				eCurState = DEAD;			//�״� �ִϸ��̼� ������ ��ü

			frame.StartX = 0;
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
		DeadTime += 10.f;			// ������ �ð� ī��Ʈ
	else
		return;

	if (DeadTime > 1000)
	{
		if (player0)				//�������� 0�̰� 1�� �������ִٸ� �¸�
		{
			STATE opponentState = _players[1].Get_State();
			if (opponentState != DEAD && opponentState != SAVED && opponentState != TRAPPED && opponentState != DIE)
				_players[1].Set_Winner();
		}

		else						//�������� 1�̰� 0�� �������ִٸ� �¸�
		{
			STATE opponentState = _players[0].Get_State();
			if (opponentState != DEAD && opponentState != SAVED && opponentState != TRAPPED && opponentState != DIE)
				_players[0].Set_Winner();
		}
	}
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

void CPlayer::CheckCollisionPlayers(std::vector<CPlayer>& _players)
{
	if (eCurState != TRAPPED)
		return;						//���� �������¾ƴϸ� ����.

	RECT temp{};
	//if (player0)					// 0�� �÷��̾��� ��� 1���� ����
	//{
	//	STATE opponentState = _players[1].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//���浵 �׾��ų� ���� ������̰ų� ����� �����ų� �׾������� �浹ó�� ���Ѵ�.
	//	if (IntersectRect(&temp, &rt, &_players[1].GetRect()))
	//		eCurState = DIE;		//������ ������ ���¿��� �ε����� ��
	//}

	//else							// 1�� �÷��̾��� ��� 0���� ����
	//{
	//	STATE opponentState = _players[0].Get_State();
	//	if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
	//		return;		//���浵 �׾��ų� ���� ������̰ų� ����� �����ų� �׾������� �浹ó�� ���Ѵ�.
	//	if (IntersectRect(&temp, &rt, &_players[0].GetRect()))
	//		eCurState = DIE;		//������ ������ ���¿��� �ε����� ��
	//}

	for (auto player : _players)
	{
		if (player.clientNum == clientNum)
			continue;						//�ڱ��ڽŰ� �浹�� ��ŵ�Ѵ�.

		STATE opponentState = player.Get_State();
		if (opponentState == DEAD || opponentState == SAVED || opponentState == TRAPPED || opponentState == DIE)
			continue;		//���浵 �׾��ų� ���� ������̰ų� ����� �����ų� �׾������� �浹ó�� ���Ѵ�.

		RECT temp2 = player.GetRect();
		if (IntersectRect(&temp, &rt, &temp2))
			eCurState = DIE;		//������ ������ ���¿��� �ε����� ��
	}
}



