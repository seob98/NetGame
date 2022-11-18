#include "GameUtil/stdafx.h"
#include "Common.h"
#include "GameUtil/Player.h"

#pragma once

#define MAX_ITEM_CNT		15
#define VERTBLOCKCNT		15
#define HORZBLOCKCNT		13
#define BLOCKSIZE			20
#define INDEX_MAPSTART		30
#define INDEX_MAPEND		165

enum KEYEVENT {
	PRESS_LEFT, PRESS_RIGHT,
	PRESS_UP, PRESS_DOWN, PRESS_SPACE, PRESS_ITEM, PRESS_END
};

//enum STATE { IDLE, UP, DOWN, LEFT, RIGHT, TRAPPED, SAVED, DIE, DEAD, WIN };

enum TEAM { DAO, BAZZI };

struct CLIENTINFO
{
	SOCKET sock{};
	unsigned short ID = -1;
	CPlayer player{};
};

struct ObjectINFO
{
	unsigned short pos;
	unsigned short type;
};
#pragma pack(push, 1)


struct SC_GAMEINFO
{
	unsigned short ID = -1;
	int currentPlayerCnt = -1;
	int gameStart{};
	TEAM teamId;

	int blockType[INDEX_MAPEND - INDEX_MAPSTART];
	ObjectINFO itemType[MAX_ITEM_CNT];
};

struct SC_PLAYERUPDATE
{
	unsigned short ID = -1;
	POINT pt;
	CPlayer::STATE state;
	int   ballonLength = -1;
	bool keydown;
};

struct CS_EVENT
{
	unsigned short ID = -1;
	int moveType = -1;
	bool setBallon = false;
	bool usedNeedle = false;
	unsigned short Index = -1;
};

#pragma pack(pop)