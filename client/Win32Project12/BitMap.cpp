#include "stdafx.h"
#include "BitMap.h"


CBitMap::CBitMap()
{
}


CBitMap::~CBitMap()
{
	Release();
}

void CBitMap::Load_Bmp(const TCHAR* _pFilePath)
{
	HDC hDC = GetDC(hWnd);
	m_hMemDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);

	m_hBitmap = (HBITMAP)LoadImage(NULL, _pFilePath
		, IMAGE_BITMAP
		, 0, 0
		, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);
}

void CBitMap::Release()
{
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hMemDC);
}
