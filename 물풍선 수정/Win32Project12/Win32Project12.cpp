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

		//	// connect() ȣ�⿡ ����� ����
		//	struct sockaddr_in serveraddr;
		//	memset(&serveraddr, 0, sizeof(serveraddr));
		//	serveraddr.sin_family = AF_INET;
		//	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
		//	serveraddr.sin_port = htons(SERVERPORT);

		//	// ���� ����
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

		// �� �ٴ� ��ġ
		for (int i = 0; i < vertBlockCnt; ++i)
		{
			for (int j = 0; j < horzBlockCnt; ++j)
			{
				TILES.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, blockSize, i * horzBlockCnt + j);
			}
		}

		// �ε����� �� ��ġ
		//for (int i = 30; i < 165; ++i)
		//{
		//	//80%�� ���.
		//	if (rand() % 10 <= 7)
		//	{
		//		if(rand()%10 <=1)
		//			OBSTACLES.emplace_back(TILES[i].GetPos(), TILES[i].GetIndex(), TILES, true);
		//		else
		//			OBSTACLES.emplace_back(TILES[i].GetPos(), TILES[i].GetIndex(), TILES, false);
		//	}
		//}

		// �÷��̾� �߰�
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
			// ballon�浹ó��
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
			// ballon ����ó�� �� ����ó��
#pragma endregion

#pragma region item
			for (auto& Item : ITEMS)
			{
				Item.CheckCollisionPlayers(PLAYERS);
				Item.CheckCollisionWaterStreams(WATERSTREAMS);

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

		//�������� �ڵ�. ���Ŀ� ����
		//if (PLAYERS[0].Get_State() == CPlayer::DEAD || PLAYERS[0].Get_State() == CPlayer::DIE)		//0�� �׾����� 0�׸��� ������ �׷�
		//{
		//	PLAYERS[0].Draw(hMemDC);
		//	PLAYERS[1].Draw(hMemDC);
		//}
		//else if (PLAYERS[1].Get_State() == CPlayer::DEAD || PLAYERS[1].Get_State() == CPlayer::DIE)	//1�� �׾����� 1�׸��� ������ �׷�
		//{
		//	PLAYERS[1].Draw(hMemDC);
		//	PLAYERS[0].Draw(hMemDC);
		//}
		//else																					// �Ѵ� �ƴϸ� �ƹ����Գ�
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
void Player_Update(int cnt, int x, int y);

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
	for (int i = 0; i < 135; ++i)
	{
		if (data->blockType[i] != -1)
			OBSTACLES.emplace_back(TILES[i + 30].GetPos(), TILES[i + 30].GetIndex(), TILES, data->blockType[i]);
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

// �׽�Ʈ
void Player_Update(int cnt, int x, int y)
{
	PLAYERS[cnt].SetPosX(x);
	PLAYERS[cnt].SetPosY(y);
}