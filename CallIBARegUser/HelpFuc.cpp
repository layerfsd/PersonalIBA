#include "stdafx.h"
#include "HelpFuc.h"

// T转多字节
CStringA CDllHelper::TC2AC(LPCTSTR szTrans)
{
	CStringA strTmp;
#ifdef _UNICODE
	DWORD dwLen = WideCharToMultiByte(CP_OEMCP,NULL,szTrans,-1,NULL,0,NULL,FALSE);		// 先获取字符数组的长度
	char* pArr = new char[dwLen];
	WideCharToMultiByte (CP_ACP,NULL,szTrans,-1,pArr,dwLen,NULL,FALSE);
	strTmp = pArr;
	delete pArr;
#else
	strTmp = szTrans;
#endif
	return strTmp;
}

// 多字节转T
CString CDllHelper::AC2TC(LPCTSTR szTrans)
{
	CString strTmp;
#ifdef _UNICODE
	DWORD dwLen = MultiByteToWideChar(CP_OEMCP,NULL,szTrans,-1,NULL,0,NULL,FALSE);		// 先获取字符数组的长度
	wchar_t *pArr = new wchar_t[dwLen];
	MultiByteToWideChar (CP_ACP,NULL,szTrans,-1,pArr,dwLen,NULL,FALSE);
	strTmp = pArr;
	delete pArr;
#else
	strTmp = szTrans;
#endif
	return strTmp;
}