#include "stdafx.h"
#include "BmpMgr.h"
#include "BitMap.h"

CBmpMgr* CBmpMgr::m_pInstance = nullptr;
CBmpMgr::CBmpMgr()
{
}


CBmpMgr::~CBmpMgr()
{
	Release();
}

void CBmpMgr::Insert_Bmp(const TCHAR * _pFilePath, const TCHAR * _pImageKey)
{
	auto iter = std::find_if(m_mapBit.begin(), m_mapBit.end(), CStrCmp(_pImageKey));

	if (m_mapBit.end() == iter)
	{
		CBitMap* pBmp = new CBitMap;
		pBmp->Load_Bmp(_pFilePath);

		m_mapBit.emplace(_pImageKey, pBmp);
	}
}

HDC CBmpMgr::Find_Image(const TCHAR * _pImageKey)
{
	auto iter = std::find_if(m_mapBit.begin(), m_mapBit.end(), CStrCmp(_pImageKey));

	if (m_mapBit.end() == iter)
		return nullptr;

	return iter->second->Get_MemDC();
}

void CBmpMgr::Release()
{
	for (auto& pair : m_mapBit)
		SAFE_DELETE(pair.second);

	m_mapBit.clear();
}
