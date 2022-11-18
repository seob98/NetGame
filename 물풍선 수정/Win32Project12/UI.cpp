#include "stdafx.h"
#include "UI.h"

CUI::CUI()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/test4.bmp", L"test2");
	//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Player/bazzidead_old.bmp", L"bazzidead");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win1p.bmp", L"win0");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win2p.bmp", L"win1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/draw.bmp", L"draw");
	//CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Stage/win2p.bmp", L"draw");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/Start.bmp", L"start");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"CrazyArcadeImage/Image/number.bmp", L"loading");
}

void CUI::DrawStartUI(HDC hdc, bool pressStartButton, int currentPlayerCnt)
{
	//if (currentPlayerCnt >= MAX_PLAYER_CNT)
	//	return;
	if (pressStartButton)
		return;

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"start");

	GdiTransparentBlt(hdc
		, 0, 0
		, 800, 600
		, hMemDC
		, 0, 0
		, 800, 600
		, RGB(255, 245, 255));
}

void CUI::DrawLoadingUI(HDC hdc, bool pressStartButton, int currentPlayerCnt)
{
	//if (!pressStartButton && currentPlayerCnt >= MAX_PLAYER_CNT)
	//	return;

	return;

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"loading");

	GdiTransparentBlt(hdc
		, 200, 200
		, 150, 17
		, hMemDC
		, 0, 0
		, 150, 17
		, RGB(255, 245, 255));
}

void CUI::DrawMainframeUI(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"test2");

	GdiTransparentBlt(hdc
		, 0, 0
		, 800, 600
		, hMemDC
		, 0, 0
		, 800, 600
		, RGB(255, 255, 255));
}


void CUI::Update()
{
}

