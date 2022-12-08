#include "stdafx.h"
#include "Block.h"



void CBlock::draw(HDC hdc)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"tile");

	BitBlt(hdc, rt.left, rt.top, 41, 40, hMemDC, 0, 0, SRCCOPY);
}
