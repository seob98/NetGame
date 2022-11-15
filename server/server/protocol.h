#include "Common.h"

#pragma once

#define MAX_ITEM_CNT		15
#define VERTBLOCKCNT		15
#define HORZBLOCKCNT		13
#define BLOCKSIZE			20
#define INDEX_MAPSTART		30
#define INDEX_MAPEND		135

enum KEYEVENT {
	PRESS_LEFT, PRESS_RIGHT,
	PRESS_UP, PRESS_DOWN, PRESS_SPACE, PRESS_ITEM, PRESS_END
};

enum STATE { IDLE, UP, DOWN, LEFT, RIGHT, TRAPPED, SAVED, DIE, DEAD, WIN };

enum TEAM { DAO, BAZZI };

struct CLIENTINFO
{
	SOCKET sock;
	unsigned short ID;
	CPlayer player;
};

struct ObjectINFO
{
	unsigned short pos;
	unsigned short type;
};
#pragma pack(push, 1)


struct SC_GAMEINFO
{
	unsigned short ID;
	int currentPlayerCnt;
	int gameStart;
	TEAM teamId;

	int blockType[INDEX_MAPEND - INDEX_MAPSTART];
	ObjectINFO itemType[MAX_ITEM_CNT];
};

struct SC_PLAYERUPDATE
{
	unsigned short ID;
	POINT pt;
	STATE state;
	int   ballonLength;
	bool keydown;
};

struct CS_EVENT
{
	unsigned short ID;
	bool keyType[PRESS_END];
	unsigned short Index;
};

#pragma pack(pop)