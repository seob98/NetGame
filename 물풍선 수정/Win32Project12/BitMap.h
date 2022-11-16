#pragma once

#ifndef __MYBITMAP_H__
#define __MYBITMAP_H__


class CBitMap
{
public:
	CBitMap();
	~CBitMap();

public:
	HDC Get_MemDC() { return m_hMemDC; }
	void Load_Bmp(const TCHAR* _pFilePath);
	void Release();

private:
	HDC			m_hMemDC;
	HBITMAP		m_hBitmap;
	HBITMAP		m_hOldBmp;
};


#endif // !__MYBITMAP_H__
