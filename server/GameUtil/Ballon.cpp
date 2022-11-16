#include "stdafx.h"
#include "Ballon.h"
#include "Obstacle.h"
#include "Block.h"
#include "WaterStream.h"

CBallon::CBallon(POINT Pos, int Size, int Number, int _explosionLength, bool _player0, int _summonerClientNum, CPlayer* _summoner)
	: pos(Pos), size(Size), index(Number), explosionLength(_explosionLength), summonerClientNum(_summonerClientNum), summoner(_summoner)/*, player0(_player0)*/
{
	rt.left = Pos.x - Size;
	rt.top = Pos.y - Size;
	rt.right = Pos.x + Size;
	rt.bottom = Pos.y + Size;

	frame.StartX = 0;
	frame.StateY = 0;					// ��������Ʈ �̹��� ���� �� 
	frame.EndX = 2;						// ��������Ʈ �̹��� ���� �� 
	frame.Time = 0;						// ���� ���� �ð�
	frame.DelayTime = 200;				// ������ ������ (�������� �����ϴ�.)

	exploded = false;

	goLeft	= _explosionLength;
	goRight	= _explosionLength;
	goUp	= _explosionLength;
	goDown	= _explosionLength;

	LeftLocation	= index;
	RightLocation	= index;
	UpLocation		= index;
	DownLocation	= index;

	for (int i = 0; i < 4; ++i)
	{
		listOfIntersected[i] = i;
	}
}

void CBallon::Draw(HDC hdc)
{
}

void CBallon::CheckPlayerOut(std::vector<CPlayer>& players)
{
	RECT temp{};

 	for (int i = 0; i < MAX_PLAYER_CNT; ++i)
	{
		if (listOfIntersected[i] < 0)
			continue;

		RECT temp2 = players[listOfIntersected[i]].GetRect();
		if (!IntersectRect(&temp, &rt, &temp2))
			listOfIntersected[i] = -1;
	}
}

void CBallon::CheckCollision(std::vector<CPlayer>& players)
{
	RECT temp{};

	for (int i = 0; i < MAX_PLAYER_CNT; ++i)
	{
		if (listOfIntersected[i] != -1)								//���� ��ġ�� ���� ��������� �浹�н�
			continue;
		else
		{
			RECT temp2 = players[i].GetRect();
			if (IntersectRect(&temp, &temp2, &rt))
			{
				int playerDir = players[i].GetDir();
				int playerSize = players[i].GetSize();

				if (playerDir == 0)		//�÷��̾ ���ʹ���
					players[i].SetPosX(rt.right + playerSize);
				if (playerDir == 2)		//�÷��̾ �����ʹ���
					players[i].SetPosX(rt.left - playerSize);
				if (playerDir == 1)		//�÷��̾ ���ʹ���
					players[i].SetPosY(rt.bottom + playerSize);
				if (playerDir == 3)		//�÷��̾ �Ʒ�����
					players[i].SetPosY(rt.top - playerSize);
			}
		}
	}
}

void CBallon::UpdateFrame()
{
	frame.Time += 10.f;
	if (frame.Time > frame.DelayTime)
	{
		frame.Time = 0.f;
		frame.StartX += 1;
	}

	if (frame.StartX >= frame.EndX)
		frame.StartX = 0;
}

void CBallon::Update(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt)
{
	BallonTimeCheck();
	if(explosionTime <= 0)
		CheckChainExplosion(ballons, waterStreams, blocks, obstacles, horzBlockCnt, vertBlockCnt);
}

void CBallon::BallonTimeCheck()
{
	if (explosionTime <= 0)
		explosionTime = 0.f;

	else
		explosionTime -= explosionSpeed;
}
							//temp�� �� �Լ��� goDir. �� �� �� �� �ִ���?
void CBallon::CheckDeadEnd(int dir, int& temp, int& suggestedIndex, std::vector<CObstacle>& obstacles, int obstacleTimeSet, int horzBlockCnt, int vertBlockCnt)
{
	// �� ���� �ִ����� �Ǵ��Ͽ� ���ٱ⸦ ���� �ڵ�
	switch (dir)		//���ٱ� ����
	{
	case 0:				//����
		if (suggestedIndex % horzBlockCnt == 0)
			temp = 0;
		break;
	case 1:				//��
		if (suggestedIndex < horzBlockCnt)
			temp = 0;
		break;
	case 2:				//������
		if ((suggestedIndex + 1) % horzBlockCnt == 0)
			temp = 0;
		break;
	case 3:				//�Ʒ�
		if (horzBlockCnt*(vertBlockCnt-1) <= index)
			temp = 0;
		break;
	}


	// ��ֹ��� �ε������� �Ǵ��ϴ� �ڵ�
	//std::vector<int> obstacleLocation{};
	for (auto& obstacle : obstacles)
	{
		switch (dir)
		{
		case 0: //��
			if (suggestedIndex == obstacle.GetIndex() + 1)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 1: //��
			if (suggestedIndex == obstacle.GetIndex() + horzBlockCnt)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 2: //��
			if (suggestedIndex == obstacle.GetIndex() - 1)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 3: //��
			if (suggestedIndex == obstacle.GetIndex() - horzBlockCnt)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		}
		//obstacleLocation.emplace_back(obstacle.GetIndex());
	}

}


void CBallon::CheckChainExplosion(std::vector<CBallon>& ballons, std::vector<CWaterStream>& waterStreams, std::vector<CBlock>& blocks, std::vector<CObstacle>& obstacles, int horzBlockCnt, int vertBlockCnt)
{
	if (exploded)
		return;

	//�߰��� ó������ ����
												//���ٱ� �ð� �� �� ������ �ð�
	CheckDeadEnd(0, goLeft, LeftLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//����
	CheckDeadEnd(1, goUp, UpLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);				//��
	CheckDeadEnd(2, goRight, RightLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//������
	CheckDeadEnd(3, goDown, DownLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//�Ʒ�
	

	waterStreams.emplace_back(pos, index, 20, 0, 4);						// �߾� ���ٱ�

	while (goLeft)
	{
		//index�� ���ٱ� ��ȯ (��)
		LeftDelayTime += 1;
		goLeft -= 1;
		LeftLocation -= 1;

		// ���ٱ� ������ ��ź������ ���� ������
		for (auto& ballon : ballons)
		{
			if (ballon.GetIndex() == index)
				continue;
			if (ballon.GetIndex() == LeftLocation)
				ballon.explosionTime = -1;
		}

		waterStreams.emplace_back(blocks[LeftLocation].GetPos(), goLeft, 20, LeftDelayTime * streamDelayTime, 0);
		CheckDeadEnd(0, goLeft, LeftLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);
	}
	while (goUp)
	{
		//index�� ���ٱ� ��ȯ (��)
		UpDelayTime += 1;
		goUp -= 1;
		UpLocation -= horzBlockCnt;

		// ���ٱ� ������ ��ź������ ���� ������
		for (auto& ballon : ballons)
		{
			if (ballon.GetIndex() == index)
				continue;
			if (ballon.GetIndex() == UpLocation)
				ballon.explosionTime = -1;
		}	

		waterStreams.emplace_back(blocks[UpLocation].GetPos(), goUp, 20, UpDelayTime * streamDelayTime, 1);
		CheckDeadEnd(1, goUp, UpLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);
	}
	while (goRight)
	{
		//index�� ���ٱ� ��ȯ (��)
		RightDelayTime += 1;
		goRight -= 1;
		RightLocation += 1;

		// ���ٱ� ������ ��ź������ ���� ������
		for (auto& ballon : ballons)
		{
			if (ballon.GetIndex() == index)
				continue;
			if (ballon.GetIndex() == RightLocation)
				ballon.explosionTime = -1;
		}

		waterStreams.emplace_back(blocks[RightLocation].GetPos(), goRight, 20, RightDelayTime * streamDelayTime, 2);
		CheckDeadEnd(2, goRight, RightLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);
	}
	while (goDown)
	{
		//index�� ���ٱ� ��ȯ (�Ʒ�)
		DownDelayTime += 1;
		goDown -= 1;
		DownLocation += horzBlockCnt;
		// ���ٱ� ������ ��ź������ ���� ������
		for (auto& ballon : ballons)
		{
			if (ballon.GetIndex() == index)
				continue;
			if (ballon.GetIndex() == DownLocation)
				ballon.explosionTime = -1;
		}

		if (DownLocation - 1 > horzBlockCnt * vertBlockCnt)
		{
			goDown = 0;
			break;
		}
		waterStreams.emplace_back(blocks[DownLocation].GetPos(), goDown, 20, DownDelayTime * streamDelayTime, 3);
		CheckDeadEnd(3, goDown, DownLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);
	}

	exploded = true;
	
	
	summoner->BallonCntUpdate();



	blocks[index].SetCollisionEnable(false);
}

