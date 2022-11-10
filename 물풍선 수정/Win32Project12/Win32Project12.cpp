// Win32Project12.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
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

// 전역 변수:
HWND hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

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
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT12));

    MSG msg;

    // 기본 메시지 루프입니다.
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
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//
//  목적:  주 창의 메시지를 처리합니다.
//
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

		// 맵 바닥 설치
		for (int i = 0; i < vertBlockCnt; ++i)
		{
			for (int j = 0; j < horzBlockCnt; ++j)
			{
				map.emplace_back(POINT{ 40 + j * 40, 60 + i * 40 }, blockSize, i * horzBlockCnt + j);
			}
		}

		// 부딪히는 블럭 설치
		for (int i = 30; i < 165; ++i)
		{
			//80%로 깐다.
			if (rand() % 10 <= 7)
			{
				if(rand()%10 <=1)
					obstacles.emplace_back(map[i].GetPos(), map[i].GetIndex(), map, true);
				else
					obstacles.emplace_back(map[i].GetPos(), map[i].GetIndex(), map, false);
			}
		}
			// 플레이어 추가
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
			// ballon충돌처리
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
			// ballon 삭제처리 및 연산처리
#pragma endregion

#pragma region item
			for (auto& Item : items)
			{
				Item.CheckCollisionPlayers(players);
				Item.CheckCollisionWaterStreams(waterstreams);

			}
#pragma endregion

#pragma region waterstream
			//물줄기 업데이트
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
			//물줄기 lateupdate(컨테이너 비우기)
#pragma endregion

#pragma region obstacle
			//장애물 처리 (컨테이너 비우기)
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

			if (players[0].Get_State() == CPlayer::DEAD || players[0].Get_State() == CPlayer::DIE)		//0이 죽었으면 0그리고 생존자 그려
			{
				players[0].Draw(hMemDC);
				players[1].Draw(hMemDC);
			}
			else if (players[1].Get_State() == CPlayer::DEAD || players[1].Get_State() == CPlayer::DIE)	//1이 죽었으면 1그리고 생존자 그려
			{
				players[1].Draw(hMemDC);
				players[0].Draw(hMemDC);
			}
			else																					// 둘다 아니면 아무렇게나
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
