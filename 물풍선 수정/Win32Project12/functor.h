#pragma once

class CStrCmp
{
public:
	CStrCmp(const TCHAR* _pString) : m_pString(_pString) {}

public:
	template <typename T>
	bool operator()(T& _Dst)
	{
		return !lstrcmp(_Dst.first, m_pString);
	}

private:
	const TCHAR* m_pString;
};
