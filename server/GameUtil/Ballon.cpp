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
	frame.StateY = 0;					// 스프라이트 이미지 세로 수 
	frame.EndX = 2;						// 스프라이트 이미지 가로 수 
	frame.Time = 0;						// 현재 진행 시간
	frame.DelayTime = 200;				// 프레임 딜레이 (높을수록 느립니다.)

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
		if (listOfIntersected[i] != -1)								//아직 겹치는 영역 살아있으면 충돌패스
			continue;
		else
		{
			RECT temp2 = players[i].GetRect();
			if (IntersectRect(&temp, &temp2, &rt))
			{
				int playerDir = players[i].GetDir();
				int playerSize = players[i].GetSize();

				if (playerDir == 0)		//플레이어가 왼쪽방향
					players[i].SetPosX(rt.right + playerSize);
				if (playerDir == 2)		//플레이어가 오른쪽방향
					players[i].SetPosX(rt.left - playerSize);
				if (playerDir == 1)		//플레이어가 위쪽방향
					players[i].SetPosY(rt.bottom + playerSize);
				if (playerDir == 3)		//플레이어가 아래방향
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
							//temp는 밑 함수의 goDir. 즉 더 갈 수 있느냐?
void CBallon::CheckDeadEnd(int dir, int& temp, int& suggestedIndex, std::vector<CObstacle>& obstacles, int obstacleTimeSet, int horzBlockCnt, int vertBlockCnt)
{
	// 맵 끝에 있는지를 판단하여 물줄기를 끊는 코드
	switch (dir)		//물줄기 방향
	{
	case 0:				//왼쪽
		if (suggestedIndex % horzBlockCnt == 0)
			temp = 0;
		break;
	case 1:				//위
		if (suggestedIndex < horzBlockCnt)
			temp = 0;
		break;
	case 2:				//오른쪽
		if ((suggestedIndex + 1) % horzBlockCnt == 0)
			temp = 0;
		break;
	case 3:				//아래
		if (horzBlockCnt*(vertBlockCnt-1) <= index)
			temp = 0;
		break;
	}


	// 장애물과 부딪히는지 판단하는 코드
	//std::vector<int> obstacleLocation{};
	for (auto& obstacle : obstacles)
	{
		switch (dir)
		{
		case 0: //좌
			if (suggestedIndex == obstacle.GetIndex() + 1)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 1: //상
			if (suggestedIndex == obstacle.GetIndex() + horzBlockCnt)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 2: //우
			if (suggestedIndex == obstacle.GetIndex() - 1)
			{
				obstacle.SetInsideExplosionRange();
				temp = 0;
			}
			break;
		case 3: //하
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

	//중간은 처음부터 만듬
												//물줄기 시간 및 블럭 깨지는 시간
	CheckDeadEnd(0, goLeft, LeftLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//왼쪽
	CheckDeadEnd(1, goUp, UpLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);				//위
	CheckDeadEnd(2, goRight, RightLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//오른쪽
	CheckDeadEnd(3, goDown, DownLocation, obstacles, 0, horzBlockCnt, vertBlockCnt);			//아래
	

	waterStreams.emplace_back(pos, index, 20, 0, 4);						// 중앙 물줄기

	while (goLeft)
	{
		//index에 물줄기 소환 (왼)
		LeftDelayTime += 1;
		goLeft -= 1;
		LeftLocation -= 1;

		// 물줄기 범위내 폭탄있으면 같이 터져라
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
		//index에 물줄기 소환 (위)
		UpDelayTime += 1;
		goUp -= 1;
		UpLocation -= horzBlockCnt;

		// 물줄기 범위내 폭탄있으면 같이 터져라
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
		//index에 물줄기 소환 (오)
		RightDelayTime += 1;
		goRight -= 1;
		RightLocation += 1;

		// 물줄기 범위내 폭탄있으면 같이 터져라
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
		//index에 물줄기 소환 (아래)
		DownDelayTime += 1;
		goDown -= 1;
		DownLocation += horzBlockCnt;
		// 물줄기 범위내 폭탄있으면 같이 터져라
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

