#include "stdafx.h"
#include "WaterStream.h"

CWaterStream::CWaterStream(POINT _pos, int _index, int _size, int _summonTime, int _dir)
	:pos(_pos), index(_index), size(_size), summonTime(_summonTime), dir(_dir)
{
	float delayTimeDir = 150.f;

	//나중에 switch(dir)따라 frame 다르게
	switch (dir)
	{
	case 0:
		frame.StartX = 0;
		frame.EndX = 7;
		frame.Time = 0;
		frame.DelayTime = delayTimeDir;
		frame.RepeatCnt = 0;
		frame.MaxRepeat = 0;
		break;
	case 1:
		frame.StartX = 0;
		frame.EndX = 7;
		frame.Time = 0;
		frame.DelayTime = delayTimeDir;
		frame.RepeatCnt = 0;
		frame.MaxRepeat = 0;
		break;
	case 2:
		frame.StartX = 0;
		frame.EndX = 7;
		frame.Time = 0;
		frame.DelayTime = delayTimeDir;
		frame.RepeatCnt = 0;
		frame.MaxRepeat = 0;
		break;
	case 3:
		frame.StartX = 0;
		frame.EndX = 7;
		frame.Time = 0;
		frame.DelayTime = delayTimeDir;
		frame.RepeatCnt = 0;
		frame.MaxRepeat = 0;
		break;
	case 4:
		frame.StartX = 0;
		frame.EndX = 2;
		frame.Time = 0;
		frame.DelayTime = 70;
		frame.MaxRepeat = 2;
		break;
		
	}
	rt.left = pos.x - size;
	rt.top = pos.y - size;
	rt.right = pos.x + size;
	rt.bottom = pos.y + size;

	dead = false;
}

void CWaterStream::Draw(HDC hdc)
{
	if (summonTime > 0)
		return;

	HDC hMemDC{};

	int i;
	if (frame.StartX == 5)
		i = 1 + 1;
}

void CWaterStream::Update()
{
	UpdateSummonTime();

}

void CWaterStream::UpdateSummonTime()
{
	int frameSpeed = 70.f;
	if (frame.StartX >= 4)
		frameSpeed = 100.f;

	if (summonTime > 0)
		summonTime -= 0.1f;
	//summonTime이 끝나면 그림 처리 (frame.time, delaytime)
	else
	{
		if (dir == 4)
			frame.Time += frameSpeed;//50.f;
		else
			frame.Time += frameSpeed;// 50.f;
		if (frame.Time > frame.DelayTime)
		{
			frame.Time = 0.f;
			frame.StartX += 1;
		}
	}

	if (frame.StartX > frame.EndX)
	{
		frame.RepeatCnt += 1;
		frame.StartX = 0;
	}

	if (frame.RepeatCnt > frame.MaxRepeat)
		dead = true;
}
