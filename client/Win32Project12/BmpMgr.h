#pragma once

#ifndef __BMPMGR_H__
#define __BMPMGR_H__

class CBitMap;
class CBmpMgr
{
private:
	CBmpMgr();
	~CBmpMgr();

public:
	void Insert_Bmp(const TCHAR* _pFilePath, const TCHAR* _pImageKey);
	HDC Find_Image(const TCHAR* _pImageKey);
	void Release();

public:
	static CBmpMgr* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CBmpMgr;
		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		SAFE_DELETE(m_pInstance);
	}

private:
	static CBmpMgr*		m_pInstance;

	std::map<const TCHAR*, CBitMap*>	m_mapBit;
};


#endif // !__BMPMGR_H__
