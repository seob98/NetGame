// Win32Project12.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Win32Project12.h"

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"

int WINCX{ 816 };
int WINCY{ 659 };
int horzBlockCnt = 15;
int vertBlockCnt = 13;
int blockSize = 20;

SOCKET sock;
int myClientID{ -1 };
int currentPlayerCnt{ 0 };

CS_EVENT event;
#pragma region init

using namespace std;

vector<CBlock>			TILES{};
vector<CPlayer>			PLAYERS{};
vector<CBallon>			BALLONS{};
vector<CObstacle>		OBSTACLES{};
vector<CWaterStream>	WATERSTREAMS{};
vector<CItem>			ITEMS{};
CUI						UI{};



#define MAX_LOADSTRING 100

// 전역 변수:
HWND hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HANDLE SendEvent;
DWORD WINAPI RecvThread(LPVOID arg);

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT12, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT12));

	MSG msg;

	SendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}




//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT12));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT12);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0, WINCX, WINCY, nullptr, nullptr, hInstance, nullptr);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//  목적:  주 창의 메시지를 처리합니다.
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#pragma endregion

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, hMemDC, hMemDCUI;
	HBITMAP hBit;

	static bool pressStart = false;

#pragma region 최초연결

	static bool connection = false;

	// 윈속 초기화
	if (!connection)
	{
		WSADATA wsa;
		int retval{};
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			return 1;

		//	// connect() 호출에 사용할 변수
		//	struct sockaddr_in serveraddr;
		//	memset(&serveraddr, 0, sizeof(serveraddr));
		//	serveraddr.sin_family = AF_INET;
		//	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
		//	serveraddr.sin_port = htons(SERVERPORT);

		//	// 소켓 생성
		//	SOCKET sockConnect = socket(AF_INET, SOCK_STREAM, 0);
		//	if (sockConnect == INVALID_SOCKET) err_quit("socket()");

		//	// connect()
		//	retval = connect(sockConnect, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		//	if (retval == SOCKET_ERROR) err_quit("connect()");

		connection = true;
	}

#pragma endregion

	switch (message)
	{
	case WM_CREATE:
#pragma endregion

		SetTimer(hWnd, 1, 10, NULL);

		srand((unsigned int)time(NULL));

		// 맵 바닥 설치
		for (int i = 0; i < vertBlockCnt; ++i)
		{
			for (int j = 0; j < horzBlockCnt; ++j)
			{
				TILES.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, blockSize, i * horzBlockCnt + j);
			}
		}

		// 부딪히는 블럭 설치
		//for (int i = 30; i < 165; ++i)
		//{
		//	//80%로 깐다.
		//	if (rand() % 10 <= 7)
		//	{
		//		if(rand()%10 <=1)
		//			OBSTACLES.emplace_back(TILES[i].GetPos(), TILES[i].GetIndex(), TILES, true);
		//		else
		//			OBSTACLES.emplace_back(TILES[i].GetPos(), TILES[i].GetIndex(), TILES, false);
		//	}
		//}

		// 플레이어 추가
		//PLAYERS.emplace_back(TILES[170].GetPos());
		//PLAYERS[0].clientNum = 0;
		//PLAYERS[0].clinetTeam = 0;
		//PLAYERS[0].SetPlayer0();
		//PLAYERS.emplace_back(TILES[170].GetPos());
		//PLAYERS[1].clientNum = 1;
		//PLAYERS[1].clinetTeam = 1;

		break;

	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			//PLAYERS[0].SetupBallon(TILES, BALLONS, PLAYERS, true);
			event.setBallon = true;
		}
		if (wParam == VK_SHIFT)
		{
			PLAYERS[1].SetupBallon(TILES, BALLONS, PLAYERS, false);
		}

		if (wParam == 'Q')
		{
			PLAYERS[0].useNeedle();
		}
		if (wParam == 'P')
		{
			PLAYERS[1].useNeedle();
		}
	case WM_LBUTTONDOWN:
	{
		RECT rtStart{ 155, 432, 155 + 78, 432 + 97 };
		RECT rtQuit{ 571, 432, 571 + 78, 432 + 97 };
		RECT rtQuit2{ 647, 565, 647 + 140, 565 + 34 };
		POINT pos{ LOWORD(lParam),HIWORD(lParam) };
		if (PtInRect(&rtStart, pos))
			pressStart = true;
		else if (PtInRect(&rtQuit, pos))
			PostQuitMessage(0);

		if (pressStart)
		{
			if (PtInRect(&rtQuit2, pos))
				PostQuitMessage(0);
		}
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (!pressStart)
				break;

			//static bool Once = true;
			//static bool Once2 = true;
			//if (Once)
			//{
			//	Once = false;
			//}
			//if (Once2)
			//{
			//	if (PLAYERS[0].GetState() == 9 || PLAYERS[1].GetState() == 9 || (PLAYERS[0].GetState() == 8 && (PLAYERS[1].GetState() == 8)))
			//	{
			//		Once2 = false;
			//	}
			//}


#pragma region player
			//PLAYERS[0].Move(true, TILES);
			//PLAYERS[0].MoveTrapped(true, TILES);
			//PLAYERS[0].CheckCollisionMap(TILES);
			//PLAYERS[1].Move(false, TILES);
			//PLAYERS[1].MoveTrapped(false, TILES);
			//PLAYERS[1].CheckCollisionMap(TILES);
			//PLAYERS[0].CheckCollisionWaterStreams(WATERSTREAMS);
			//PLAYERS[1].CheckCollisionWaterStreams(WATERSTREAMS);
			//PLAYERS[0].STATE_CHECK();
			//PLAYERS[1].STATE_CHECK();
			//PLAYERS[myClientID].Move(true, TILES);
			for (auto& player : PLAYERS)
			{
				player.Move(true, TILES);
				player.MoveTrapped(true, TILES);
				player.CheckCollisionMap(TILES);
				player.CheckCollisionWaterStreams(WATERSTREAMS);
				player.STATE_CHECK();
				//player.CheckCollisionWaterStreams(waterstreams);
				//player.STATE_CHECK();
				player.Update_Frame();
				player.Update_Frame_Once();
				player.CheckCollisionPlayers(PLAYERS);
				player.Update_DeadTime(PLAYERS);
			}
			//players[1].Update_Frame_Once();
#pragma endregion

#pragma region ballon
			// ballon충돌처리
			for (auto& ballon : BALLONS)
			{
				ballon.CheckPlayerOut(PLAYERS);
				ballon.CheckCollision(PLAYERS);
				ballon.UpdateFrame();
			}
			for (auto& ballon : BALLONS)
				ballon.Update(BALLONS, WATERSTREAMS, TILES, OBSTACLES, horzBlockCnt, vertBlockCnt);
			for (int i = 0; i < BALLONS.size();)
			{
				if (BALLONS[i].getExplode())
					BALLONS.erase(BALLONS.begin() + i);
				else
					++i;
			}
			// ballon 삭제처리 및 연산처리
#pragma endregion

#pragma region item
			for (auto& Item : ITEMS)
			{
				Item.CheckCollisionPlayers(PLAYERS);
				Item.CheckCollisionWaterStreams(WATERSTREAMS);

			}
#pragma endregion

#pragma region waterstream
			//물줄기 업데이트
			for (int i = 0; i < WATERSTREAMS.size(); ++i)
			{
				WATERSTREAMS[i].Update();
			}
			for (int i = 0; i < WATERSTREAMS.size();)
			{
				if (WATERSTREAMS[i].GetDead())
					WATERSTREAMS.erase(WATERSTREAMS.begin() + i);
				else
					++i;
			}
			//물줄기 lateupdate(컨테이너 비우기)
#pragma endregion

#pragma region obstacle
			//장애물 처리 (컨테이너 비우기)
			for (int i = 0; i < OBSTACLES.size(); ++i)
			{
				OBSTACLES[i].CheckExplosionRange(TILES);
				OBSTACLES[i].makeItem(ITEMS);
			}
			for (int i = 0; i < OBSTACLES.size();)
			{
				if (OBSTACLES[i].GetDead())
					OBSTACLES.erase(OBSTACLES.begin() + i);
				else
					++i;
			}
#pragma endregion
			InvalidateRect(hWnd, NULL, false);	// 프레임 환기
			break;
		}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		hMemDC = CreateCompatibleDC(hdc);
		hBit = CreateCompatibleBitmap(hdc, WINCX, WINCY);
		SelectObject(hMemDC, hBit);

		//

		Rectangle(hMemDC, 0, 0, WINCX, WINCY);

		//PLAYERS[0].DrawMainFrmUI(hMemDC);
		UI.DrawMainframeUI(hMemDC);

		for (auto& Block : TILES)
			Block.draw(hMemDC);
		for (auto& Obstacle : OBSTACLES)
			Obstacle.draw(hMemDC);

		for (auto& ballon : BALLONS)
			ballon.Draw(hMemDC);

		for (auto& WaterStream : WATERSTREAMS)
			WaterStream.Draw(hMemDC);

		for (auto& Item : ITEMS)
			Item.Draw(hMemDC);

		for (auto& Player : PLAYERS)
			Player.Draw(hMemDC);

		//승패조건 코드. 이후에 수정
		//if (PLAYERS[0].Get_State() == CPlayer::DEAD || PLAYERS[0].Get_State() == CPlayer::DIE)		//0이 죽었으면 0그리고 생존자 그려
		//{
		//	PLAYERS[0].Draw(hMemDC);
		//	PLAYERS[1].Draw(hMemDC);
		//}
		//else if (PLAYERS[1].Get_State() == CPlayer::DEAD || PLAYERS[1].Get_State() == CPlayer::DIE)	//1이 죽었으면 1그리고 생존자 그려
		//{
		//	PLAYERS[1].Draw(hMemDC);
		//	PLAYERS[0].Draw(hMemDC);
		//}
		//else																					// 둘다 아니면 아무렇게나
		//{
		//	if (PLAYERS[0].GetPos().y > PLAYERS[1].GetPos().y)
		//	{
		//		PLAYERS[1].Draw(hMemDC);
		//		PLAYERS[0].Draw(hMemDC);
		//	}
		//	else
		//	{
		//		PLAYERS[0].Draw(hMemDC);
		//		PLAYERS[1].Draw(hMemDC);
		//	}
		//}

		hMemDCUI = CBmpMgr::Get_Instance()->Find_Image(L"test");

		GdiTransparentBlt(hMemDC
			, 0, 0
			, 800, 600
			, hMemDCUI
			, 0, 0
			, 800, 600
			, RGB(182, 185, 183));

		for (auto& player : PLAYERS)
			player.DrawWinnerUI(hMemDC);

		UI.DrawStartUI(hMemDC, pressStart, currentPlayerCnt);
		UI.DrawLoadingUI(hMemDC, pressStart, currentPlayerCnt);


		//if (PLAYERS[0].GetState() == 8 && (PLAYERS[1].GetState() == 8))
		//	PLAYERS[0].DrawDrawUI(hMemDC);

		/*
		if(currentPlayerCnt >= 4)
			PLAYERS[myClientID].DrawItem(hMemDC);
		*/
		BitBlt(hdc, 0, 0, WINCX, WINCY, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
		DeleteDC(hMemDCUI);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Add_Player(int cnt);
void Player_Update(int cnt, int x, int y);

DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	char buf[1024];
	SC_GAMEINFO* data;

	// 소켓 생성
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	//if (retval == SOCKET_ERROR) err_quit("connect()");


	retval = recv(sock, buf, sizeof(SC_GAMEINFO), MSG_WAITALL);
	data = (SC_GAMEINFO*)buf;
	myClientID = data->ID;
	currentPlayerCnt = data->currentPlayerCnt;

	// 내 플레이어 생성
	Add_Player(myClientID);

	//맵 설치
	for (int i = 0; i < 135; ++i)
	{
		if (data->blockType[i] != -1)
			OBSTACLES.emplace_back(TILES[i + 30].GetPos(), TILES[i + 30].GetIndex(), TILES, data->blockType[i]);
	}

	// 서버와 데이터 통신
	while (1) {
		if (currentPlayerCnt >= 4)
			break;
		retval = recv(sock, buf, sizeof(SC_GAMEINFO), MSG_WAITALL);
		data = (SC_GAMEINFO*)buf;
		currentPlayerCnt = data->currentPlayerCnt;
		// data->blockType[0];		서버의 map_init에서 배열의 첫번째 수를 -1로 하고 값을 받으면 0이 뜸. 현재 배열의 모든값이 0으로 전달되는 상태.

//새로 접속한 플레이어 생성
		Add_Player(data->ID);
	}
	while (1)
	{
		WaitForSingleObject(SendEvent, INFINITE);
		event.ID = myClientID;
		event.Index = PLAYERS[myClientID].GetCurrentIndex(TILES);
		event.State = PLAYERS[myClientID].GetState();
		event.moving = PLAYERS[myClientID].isMoving();
		retval = send(sock, (char*)&event, sizeof(CS_EVENT), 0);
		if (retval == SOCKET_ERROR) err_quit("send()");
		event.setBallon = false;

		//SC_PLAYERUPDATE* u_data;
		//retval = recv(sock, buf, sizeof(SC_PLAYERUPDATE), MSG_WAITALL);
		//if (retval == SOCKET_ERROR)
		//	err_quit("recv()");
		//u_data = (SC_PLAYERUPDATE*)buf;
		//Player_Update(u_data->ID, u_data->pt.x, u_data->pt.y);
	}

	return 0;
}

void Add_Player(int cnt) {
	for (int i = 0; i <= cnt; i++) {
		PLAYERS.reserve(i);
		PLAYERS.resize(i);
		if (i == 0)
			PLAYERS.emplace_back(TILES[0].GetPos());
		else if (i == 1)
			PLAYERS.emplace_back(TILES[14].GetPos());
		else if (i == 2)
			PLAYERS.emplace_back(TILES[180].GetPos());
		else if (i == 3)
			PLAYERS.emplace_back(TILES[194].GetPos());
		PLAYERS[i].clientNum = 0;
		if (i % 2 == 0)
			PLAYERS[i].clinetTeam = 1;
		else
			PLAYERS[i].clinetTeam = 0;
		PLAYERS[i].SetPlayer0();
	}
}

// 테스트
void Player_Update(int cnt, int x, int y)
{
	PLAYERS[cnt].SetPosX(x);
	PLAYERS[cnt].SetPosY(y);
}