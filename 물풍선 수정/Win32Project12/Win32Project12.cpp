// Win32Project12.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Win32Project12.h"


int WINCX{ 816 };
int WINCY{ 659 };
int horzBlockCnt = 15;
int vertBlockCnt = 13;
int blockSize = 20;

#pragma region init

using namespace std;

#define MAX_LOADSTRING 100

// ���� ����:
HWND hWnd;
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

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
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT12));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT12));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT12);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
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

	static vector<CBlock> map{};
	static vector<CPlayer> players{};
	static vector<CBallon> ballons{};
	static vector<CObstacle> obstacles{};
	static vector<CWaterStream> waterstreams{};
	static vector<CItem> items{};
	static bool GameStart = false;

	switch (message)
	{
	case WM_CREATE:
		
		SetTimer(hWnd, 1, 10, NULL);

		srand((unsigned int)time(NULL));

		// �� �ٴ� ��ġ
		for (int i = 0; i < vertBlockCnt; ++i)
		{
			for (int j = 0; j < horzBlockCnt; ++j)
			{
				map.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, blockSize, i * horzBlockCnt + j);
			}
		}

		// �ε����� �� ��ġ
		for (int i = 30; i < 165; ++i)
		{
			//80%�� ���.
			if (rand() % 10 <= 7)
			{
				if(rand()%10 <=1)
					obstacles.emplace_back(map[i].GetPos(), map[i].GetIndex(), map, true);
				else
					obstacles.emplace_back(map[i].GetPos(), map[i].GetIndex(), map, false);
			}
		}
			// �÷��̾� �߰�
		players.emplace_back(map[0].GetPos());
		players[0].clientNum = 0;
		players[0].clinetTeam = 0;
		players[0].SetPlayer0();
		players.emplace_back(map[horzBlockCnt* vertBlockCnt - 1].GetPos());
		players[1].clientNum = 1;
		players[1].clinetTeam = 1;

		break;

	case WM_KEYDOWN:
		if (wParam == VK_SPACE)
		{
			players[0].SetupBallon(map, ballons, players, true);
		}
		if (wParam == VK_SHIFT)
		{
			players[1].SetupBallon(map, ballons, players, false);
		}

		if (wParam == 'Q')
		{
			players[0].useNeedle();
		}
		if (wParam == 'P')
		{
			players[1].useNeedle();
		}
	case WM_LBUTTONDOWN:
	{
		RECT rtStart{155, 432, 155+78, 432 +97};
		RECT rtQuit{ 571, 432, 571 + 78, 432 + 97 };
		RECT rtQuit2{647, 565, 647 + 140, 565 + 34};
		POINT pos{ LOWORD(lParam),HIWORD(lParam) };
		if (PtInRect(&rtStart, pos))
			GameStart = true;
		else if(PtInRect(&rtQuit, pos))
			PostQuitMessage(0);

		if (GameStart)
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
			if (!GameStart)
				break;

			static bool Once = true;
			static bool Once2 = true;
			if (Once)
			{
				Once = false;
			}
			if (Once2)
			{
				if (players[0].GetState() == 9 || players[1].GetState() == 9 || (players[0].GetState() == 8 && (players[1].GetState() == 8)))
				{
					Once2 = false;
				}
			}


#pragma region player
			players[0].Move(true, map);
			players[0].MoveTrapped(true, map);
			players[0].CheckCollisionMap(map);
			players[1].Move(false, map);
			players[1].MoveTrapped(false, map);
			players[1].CheckCollisionMap(map);
			players[0].CheckCollisionWaterStreams(waterstreams);
			players[1].CheckCollisionWaterStreams(waterstreams);
			players[0].STATE_CHECK();
			players[1].STATE_CHECK();
			for (auto& player : players)
			{
				//player.CheckCollisionWaterStreams(waterstreams);
				//player.STATE_CHECK();
				player.Update_Frame();
				player.Update_Frame_Once();
				player.CheckCollisionPlayers(players);
				player.Update_DeadTime(players);
			}
			//players[1].Update_Frame_Once();
#pragma endregion

#pragma region ballon
			// ballon�浹ó��
			for (auto& ballon : ballons)
			{
				ballon.CheckPlayerOut(players);
				ballon.CheckCollision(players);
				ballon.UpdateFrame();		
			}
			for (auto& ballon : ballons)
				ballon.Update(ballons, waterstreams, map, obstacles, horzBlockCnt, vertBlockCnt);
			for (int i = 0; i < ballons.size();)
			{
				if (ballons[i].getExplode())
					ballons.erase(ballons.begin() + i);
				else
					++i;
			}
			// ballon ����ó�� �� ����ó��
#pragma endregion

#pragma region item
			for (auto& Item : items)
			{
				Item.CheckCollisionPlayers(players);
				Item.CheckCollisionWaterStreams(waterstreams);

			}
#pragma endregion

#pragma region waterstream
			//���ٱ� ������Ʈ
			for (int i = 0; i < waterstreams.size(); ++i)
			{
				waterstreams[i].Update();
			}		
			for (int i = 0; i < waterstreams.size();)
			{
				if (waterstreams[i].GetDead())
					waterstreams.erase(waterstreams.begin() + i);
				else
					++i;
			}			
			//���ٱ� lateupdate(�����̳� ����)
#pragma endregion

#pragma region obstacle
			//��ֹ� ó�� (�����̳� ����)
			for (int i = 0; i < obstacles.size(); ++i)
			{
				obstacles[i].CheckExplosionRange(map);
				obstacles[i].makeItem(items);
			}
			for (int i = 0; i < obstacles.size();)
			{
				if (obstacles[i].GetDead())
					obstacles.erase(obstacles.begin() + i);
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

			players[0].DrawMainFrmUI(hMemDC);
			for (auto& Block : map)
				Block.draw(hMemDC);
			for (auto& Obstacle : obstacles)
				Obstacle.draw(hMemDC);

			for (auto& ballon : ballons)
				ballon.Draw(hMemDC);

			for (auto& WaterStream : waterstreams)
				WaterStream.Draw(hMemDC);

			for (auto& Item : items)
				Item.Draw(hMemDC);

			if (players[0].Get_State() == CPlayer::DEAD || players[0].Get_State() == CPlayer::DIE)		//0�� �׾����� 0�׸��� ������ �׷�
			{
				players[0].Draw(hMemDC);
				players[1].Draw(hMemDC);
			}
			else if (players[1].Get_State() == CPlayer::DEAD || players[1].Get_State() == CPlayer::DIE)	//1�� �׾����� 1�׸��� ������ �׷�
			{
				players[1].Draw(hMemDC);
				players[0].Draw(hMemDC);
			}
			else																					// �Ѵ� �ƴϸ� �ƹ����Գ�
			{
				if (players[0].GetPos().y > players[1].GetPos().y)
				{
					players[1].Draw(hMemDC);
					players[0].Draw(hMemDC);
				}
				else
				{
					players[0].Draw(hMemDC);
					players[1].Draw(hMemDC);
				}

			}

			hMemDCUI = CBmpMgr::Get_Instance()->Find_Image(L"test");

			GdiTransparentBlt(hMemDC
				, 0, 0
				, 800,600
				, hMemDCUI
				, 0,0
				, 800,600
				, RGB(182, 185, 183));

			for (auto& player : players)
				player.DrawWinnerUI(hMemDC);

			players[0].DrawStartUI(hMemDC, GameStart);
			if (players[0].GetState() == 8 && (players[1].GetState() == 8))
				players[0].DrawDrawUI(hMemDC);

			players[0].DrawItem(hMemDC);
			players[1].DrawItem(hMemDC);
			//

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
