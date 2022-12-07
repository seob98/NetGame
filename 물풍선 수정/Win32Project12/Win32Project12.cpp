// Win32Project12.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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

int ballonID{};

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

// ���� ����:
HWND hWnd;
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

HANDLE SendEvent;
DWORD WINAPI RecvThread(LPVOID arg);
DWORD WINAPI SendThread(LPVOID arg);

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT12, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT12));

	MSG msg;

	SendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);

	// �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//  ����:  �� â�� �޽����� ó���մϴ�.
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//

#pragma endregion

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, hMemDC, hMemDCUI;
	HBITMAP hBit;

	static bool pressStart = false;

#pragma region ���ʿ���

	static bool connection = false;

	// ���� �ʱ�ȭ
	if (!connection)
	{
		WSADATA wsa;
		int retval{};
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			return 1;

		connection = true;
	}

#pragma endregion

	switch (message)
	{
	case WM_CREATE:
#pragma endregion

		SetTimer(hWnd, 1, 15, NULL);

		srand((unsigned int)time(NULL));

		// �� �ٴ� ��ġ
		for (int i = 0; i < vertBlockCnt; ++i)
		{
			for (int j = 0; j < horzBlockCnt; ++j)
			{
				TILES.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, blockSize, i * horzBlockCnt + j);
			}
		}

		break;
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
		// �޴� ������ ���� �м��մϴ�.
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


#pragma region player
			for (auto& player : PLAYERS)
			{
				//�÷��̾� ����, ����, ������ Ű�� ������ ����.
				//player.MoveTrapped(true, TILES);					//STATE = TRAPPED	
				player.Move(true, TILES);							//STATE = IDLE, LEFT,RIGHT,UP,DOWN			
				
				//player.StatusElse();								//STATE = SAVED, DIE, DEAD, WIN
				//

				//player.CheckCollisionMap(TILES);
				//player.CheckCollisionWaterStreams(WATERSTREAMS);		//���ٱ� Ŭ�� ó��
				player.STATE_CHECK();									//��ǳ���� Ŭ��,�������� ���ÿ� ������. 

				player.Update_Frame();									//������ ����

				player.Update_Frame_Once();								// (�������� ������ �Լ�. Ŭ�󿡼� ���� ����.)


				//player.CheckCollisionPlayers(PLAYERS);					//���� pos�� ���� �������� Ŭ�󿡼� ���� ������ ����

				player.Update_DeadTime(PLAYERS);						// (�÷��̾ ���� �� �ð��� �����ϴ� �Լ�. ������ �ű� ����.)
			}

#pragma endregion

#pragma region ballon
			// ballon�浹ó��
			for (auto& ballon : BALLONS)
			{
				ballon.UpdateFrame();																//�ִϸ��̼�
			}
			for (auto& ballon : BALLONS)
				ballon.Update(BALLONS, WATERSTREAMS, TILES, OBSTACLES, horzBlockCnt, vertBlockCnt);	//������ ��ǳ���� deadTime�� �˷��ش�. ��ǳ�� ���� �Լ��� Ŭ�󿡼� �ʿ�.
																									//�ٸ� Ŭ���� ballon.Update()�� �浹������ ������� �ʴ´�.
			for (int i = 0; i < BALLONS.size();)
			{
				if (BALLONS[i].getExplode())
					BALLONS.erase(BALLONS.begin() + i);												//��ü ����ó��
				else
					++i;
			}
#pragma endregion

#pragma region item	
			//������ init�Լ����� �̹� ������ ��ġ, ������ ��������. �÷��̾���� �������� �޾ƿ��� �ִ� �̻� �������� �ؾ��� ������ ����.
			for (auto& Item : ITEMS)
			{
				Item.CheckCollisionPlayers(PLAYERS);							//�÷��̾��� ������ ȹ�濩�� üũ
				Item.CheckCollisionWaterStreams(WATERSTREAMS);					//���ٱ⿡ ���� �ǰݴ��ҽ� ������ �ʿ��� ����.

			}
#pragma endregion

#pragma region waterstream
			//���ٱ� ������Ʈ
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
			//���ٱ� lateupdate(�����̳� ����)
#pragma endregion

#pragma region obstacle
			//��ֹ� ó�� (�����̳� ����)
			for (int i = 0; i < OBSTACLES.size(); ++i)
			{
				OBSTACLES[i].CheckExplosionRange(TILES);		//���ٱ⿡ ���� �ǰݿ��� ����
				OBSTACLES[i].makeItem(ITEMS);					//���� �μ������� ������ ����
			}
			for (int i = 0; i < OBSTACLES.size();)
			{
				if (OBSTACLES[i].GetDead())
					OBSTACLES.erase(OBSTACLES.begin() + i);		//��ü ���� ó��
				else
					++i;
			}
#pragma endregion
			InvalidateRect(hWnd, NULL, false);	// ������ ȯ��
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


		if (PLAYERS.size() == 4)
		{
			if (PLAYERS[0].GetState() == 8 && PLAYERS[1].GetState() == 8 &&
				PLAYERS[2].GetState() == 8 && PLAYERS[3].GetState() == 8)
			{
				PLAYERS[0].DrawDrawUI(hMemDC);
			}

			if (PLAYERS[0].GetState() == 9 || PLAYERS[2].GetState() == 9)
			{
				if (myClientID == 0 || myClientID == 2)
					PLAYERS[0].DrawWinnerUI(hMemDC);
				else
					PLAYERS[0].DrawLoserUI(hMemDC);
			}

			else if (PLAYERS[1].GetState() == 9 || PLAYERS[3].GetState() == 9)
			{
				if (myClientID == 1 || myClientID == 3)
					PLAYERS[0].DrawWinnerUI(hMemDC);
				else
					PLAYERS[0].DrawLoserUI(hMemDC);
			}
		}


		hMemDCUI = CBmpMgr::Get_Instance()->Find_Image(L"test");

		GdiTransparentBlt(hMemDC
			, 0, 0
			, 800, 600
			, hMemDCUI
			, 0, 0
			, 800, 600
			, RGB(182, 185, 183));

		//for (auto& player : PLAYERS)
		//	player.DrawWinnerUI(hMemDC);

		UI.DrawStartUI(hMemDC, pressStart, currentPlayerCnt);
		UI.DrawLoadingUI(hMemDC, pressStart, currentPlayerCnt);


		//if (PLAYERS[0].GetState() == 8 && (PLAYERS[1].GetState() == 8))
		//	PLAYERS[0].DrawDrawUI(hMemDC);

		
		if(currentPlayerCnt >= 4)
			PLAYERS[myClientID].DrawItem(hMemDC);
		
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
void Player_Update(SC_PLAYERUPDATE* in);
void Waterbomb_Update(SC_BALLONBOMBEVENT* in);

DWORD WINAPI RecvThread(LPVOID arg)
{
	int retval;
	char buf[1024];
	SC_GAMEINFO* data;

	// ���� ����
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

	// �� �÷��̾� ����
	Add_Player(myClientID);

	//�� ��ġ
	for (int i = 0; i < INDEX_MAPEND - INDEX_MAPSTART; ++i)
	{
		if (data->Blockinfo[i].blocktype != -1)		// Obstacle ������ ���� -> itemtype�� ���� ���ڷ� ����
			OBSTACLES.emplace_back(TILES[i + 30].GetPos(), TILES[i + 30].GetIndex(), TILES, 
				data->Blockinfo[i].blocktype, data->Blockinfo[i].itemtype);
	}

	// ������ ������ ���
	while (1) {
		if (currentPlayerCnt >= 4)
			break;
		retval = recv(sock, buf, sizeof(SC_GAMEINFO), MSG_WAITALL);
		data = (SC_GAMEINFO*)buf;
		currentPlayerCnt = data->currentPlayerCnt;
		// data->blockType[0];		������ map_init���� �迭�� ù��° ���� -1�� �ϰ� ���� ������ 0�� ��. ���� �迭�� ��簪�� 0���� ���޵Ǵ� ����.

//���� ������ �÷��̾� ����
		Add_Player(data->ID);
	}
	CreateThread(NULL, 0, SendThread, NULL, 0, NULL);
	while (1)
	{
		//SC_PLAYERUPDATE* u_data;
		retval = recv(sock, buf, sizeof(SC_PLAYERUPDATE) * 4, MSG_WAITALL);
		//if (retval == SOCKET_ERROR)
		//	err_quit("recv()");
		Player_Update((SC_PLAYERUPDATE*)buf);

		///================================================================================================================================
		retval = recv(sock, buf, sizeof(SC_BALLONBOMBEVENT), MSG_WAITALL);
		//if (retval == SOCKET_ERROR)
		//	err_quit("recv()");
		SC_BALLONBOMBEVENT* BB_EVENT = (SC_BALLONBOMBEVENT*)buf;
		if (BB_EVENT->explodedBomb[0] >= 0)
			Waterbomb_Update(BB_EVENT);

	}

	return 0;
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int retval;
	while (1)
	{
		WaitForSingleObject(SendEvent, INFINITE);
		event.ID = myClientID;
		event.Index = PLAYERS[myClientID].GetCurrentIndex(TILES);
		event.State = PLAYERS[myClientID].GetState();
		event.moving = PLAYERS[myClientID].isMoving();
		event.Dir = PLAYERS[myClientID].GetDir();
		event.setBallon = PLAYERS[myClientID].spaceButton();
		event.ballonLength = PLAYERS[myClientID].GetballonLength();
		event.speed = PLAYERS[myClientID].GetSpeed();
		event.usedNeedle = PLAYERS[myClientID].GetUseNeedle();
		event.ballonMaxCnt = PLAYERS[myClientID].GetballonCnt();

		retval = send(sock, (char*)&event, sizeof(CS_EVENT), 0);
		//if (retval == SOCKET_ERROR) err_quit("send()");
	}
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
		PLAYERS[i].clientNum = i;
		if (i % 2 == 0)
			PLAYERS[i].clinetTeam = 1;
		else
			PLAYERS[i].clinetTeam = 0;
		PLAYERS[i].SetPlayer0();
	}
}

// �׽�Ʈ
void Player_Update(SC_PLAYERUPDATE* in)
{
	int placed{};
	for (int i = 0; i < 4; i++) 
	{
		int ID = in[i].ID;
		PLAYERS[ID].SetDir(in[i].playerDir);
		PLAYERS[ID].SetPosX(in[i].pt.x);
		PLAYERS[ID].SetPosY(in[i].pt.y);
		PLAYERS[ID].SetMoving(in[i].moving);
		PLAYERS[ID].SetState(in[i].state);
		PLAYERS[ID].CheckAnimationFrame1();
		if (in[i].setBallon)
		{
			placed = PLAYERS[ID].SetupBallon(TILES, BALLONS, PLAYERS, true, ballonID);
			if (placed)
			{
				++ballonID;
			}
		}
	}
}

void Waterbomb_Update(SC_BALLONBOMBEVENT* in)
{
	for (auto i : in->explodedBomb)
	{
		if (i == -1)
			continue;

		for (auto& b : BALLONS)
		{
			if (b.GetID() == i)
				b.SetRemainingTimeToZero();
		}
	}
}