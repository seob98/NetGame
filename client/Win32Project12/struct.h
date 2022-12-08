#pragma once

typedef struct tagFrame
{
	int		StartX;
	int		EndX;
	int		StateY;
	float	DelayTime;
	float	Time;
	int		RepeatCnt;
	int		MaxRepeat;
}FRAME;