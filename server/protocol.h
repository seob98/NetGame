#include "GameUtil/stdafx.h"
#include "Common.h"
#include "GameUtil/Player.h"

#include <iostream>

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
	short ID = -1;
	CPlayer player{};
};

struct BlockINFO
{
	// short index 멤버가 클라이언트에서 사용되지 않음을 확인하여 삭제함.
	short blocktype = -1;
	short itemtype = -1;
};
#pragma pack(push, 1)


struct SC_GAMEINFO
{
	short ID = -1;
	int currentPlayerCnt = -1;
	int gameStart{};
	TEAM teamId;

	//int blockType[INDEX_MAPEND - INDEX_MAPSTART];
	BlockINFO Blockinfo[INDEX_MAPEND - INDEX_MAPSTART];
};

struct SC_PLAYERUPDATE
{
	short ID = -1;
	POINT pt;
	CPlayer::STATE state;
	short   ballonLength = -1;
	bool keydown;
	bool moving;
	short playerDir = -1;

	//서버에서 물풍선을 관리. 물풍선 설치 여부를 알려주는 bool변수 추가.
	//플레이어가 물풍선 설치를 성공하는 경우만 체크한다.
	bool setBallon = false;
};

struct CS_EVENT
{
	short ID = -1;
	int State = -1;
	bool moving = false;
	bool setBallon = false;
	bool usedNeedle = false;
	short Dir = -1;
	short Index = -1;
	short ballonLength = -1;
	short speed = -1;
	short ballonMaxCnt = -1;
};


//물풍선 폭발 정보를 넘기기 위한 구조체. 
//후에 아이템 관련 정보를 담을때도 사용 가능.

struct SC_BALLONBOMBEVENT
{
	int explodedBomb[20]{};
};

#pragma pack(pop)