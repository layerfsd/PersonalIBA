#pragma once
#include <map>
class CStringContainer
{
public:
	CStringContainer(void);
	~CStringContainer(void);

	BOOL LoadGrcFile(CString strFilePath);

public:
	void GetStringA_(UINT nID, char* pCh,const UINT nSize)const;
	void GetStringW_(UINT nID, wchar_t* pCh,const UINT nSize)const;
	CString GetString_(UINT nID)const;

	std::map<UINT,CString> m_mapStrs;
};
