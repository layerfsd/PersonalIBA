#include "stdafx.h"

#include "G2XCommonTool.h"
#include "GxxString.h"
using namespace G2XUtility;
using namespace std;


CG2XCommonTool::CG2XCommonTool(void)
{
}

CG2XCommonTool::~CG2XCommonTool(void)
{
}

void CG2XCommonTool::ParseStrParams(
				const wchar_t* lpwStrParams, 
				std::map<std::wstring,std::wstring>& mapParams,
				const wchar_t wchAssign,
				const wchar_t wchSplit)
{
	mapParams.clear();

	while(*lpwStrParams)
	{
		const wchar_t* lpwStr = lpwStrParams;
		int nCount = 0;
		while(*lpwStr != wchAssign && *lpwStr)
		{
			lpwStr++;
			nCount++;
		}
		if (nCount==0)
		{
			// key不能为空
			break;
		}
		wstring strKey(lpwStrParams,nCount);

		lpwStrParams += nCount+1;
		lpwStr = lpwStrParams;
		nCount = 0;
		while(*lpwStr != wchSplit && *lpwStr)
		{
			lpwStr++;
			nCount++;
		}
		// value可以为空
		wstring strValue(lpwStrParams,nCount);

		mapParams[strKey] = strValue;
		
		lpwStrParams += nCount+1;
	}
}

void CG2XCommonTool::ParseStrParams(
				const char* lpStrParams, 
				std::map<std::string,std::string>& mapParams,
				const char chAssign,
				const char chSplit)
{
	mapParams.clear();

	while(*lpStrParams)
	{
		const char* lpStr = lpStrParams;
		int nCount = 0;
		while(*lpStr != chAssign && *lpStr)
		{
			lpStr++;
			nCount++;
		}
		if (nCount==0)
		{
			// key不能为空
			break;
		}
		string strKey(lpStrParams,nCount);

		lpStrParams += nCount+1;
		lpStr = lpStrParams;
		nCount = 0;
		while(*lpStr != chSplit && *lpStr)
		{
			lpStr++;
			nCount++;
		}
		// value可以为空
		string strValue(lpStrParams,nCount);

		mapParams[strKey] = strValue;

		lpStrParams += nCount+1;
	}
}

int CG2XCommonTool::ParseInt(const wchar_t* lpwStr)
{
	if (!lpwStr)
		return 0;

	return _wtoi(lpwStr);
}

int CG2XCommonTool::ParseInt(const char* lpStr)
{
	if (!lpStr)
		return 0;

	return atoi(lpStr);
}


unsigned int CG2XCommonTool::ParseUInt(const wchar_t* lpwStr)
{
	return (unsigned int)ParseInt(lpwStr);
}

unsigned int CG2XCommonTool::ParseUInt(const char* lpStr)
{
	return (unsigned int)ParseInt(lpStr);
}

std::wstring CG2XCommonTool::ParseString(const unsigned int nValue, int nLen, wchar_t wchFill/*=L'0'*/, bool bFront/*=true*/)
{
	CGxxStringW str(nValue);
	
	if(nLen < 0)
	{
		return wstring(str.GetData());
	}
	else if(nLen < str.GetLength())
	{
		return wstring(str.Right(nLen).GetData());
	}
	else
	{
		if (bFront)
		{
			str.FillCharFromLeft(nLen, wchFill);
		}
		else
		{
			str.FillCharFromRight(nLen, wchFill);
		}
		return wstring(str.GetData());
	}
}

std::string CG2XCommonTool::ParseString(const unsigned int nValue, int nLen, char chFill/*='0'*/, bool bFront/*=true*/)
{
	CGxxStringA str(nValue);

	if(nLen < 0)
	{
		return string(str.GetData());
	}
	else if(nLen < str.GetLength())
	{
		return string(str.Right(nLen).GetData());
	}
	else
	{
		if (bFront)
		{
			str.FillCharFromLeft(nLen, chFill);
		}
		else
		{
			str.FillCharFromRight(nLen, chFill);
		}
		return string(str.GetData());
	}
}	

std::wstring CG2XCommonTool::ParseString(const wchar_t* lpwValue, int nLen, wchar_t wchFill/*=L' '*/, bool bFront/*=true*/)
{
	CGxxStringW str(lpwValue);
	if (nLen < str.GetLength())
	{
		return wstring(str.Left(nLen).GetData());
	}
	else if(nLen < str.GetLength())
	{
		return wstring(str.Right(nLen).GetData());
	}
	else
	{
		if (bFront)
			str.FillCharFromLeft(nLen,wchFill);	
		else
			str.FillCharFromRight(nLen,wchFill);

		return wstring(str.GetData());
	}
}

std::string CG2XCommonTool::ParseString(const char* lpValue, int nLen, char chFill/*=' '*/, bool bFront/*=true*/)
{
	CGxxStringA str(lpValue);
	if (nLen < str.GetLength())
	{
		return string(str.Left(nLen).GetData());
	}
	else if(nLen < str.GetLength())
	{
		return string(str.Right(nLen).GetData());
	}
	else
	{
		if (bFront)
			return string(str.FillCharFromLeft(nLen,chFill).GetData());
		else
			return string(str.FillCharFromRight(nLen,chFill).GetData());
	}
}

std::wstring CG2XCommonTool::Utf8_to_WideChar( std::string& strUtf)
{
	wchar_t *pWch = new wchar_t[strUtf.length()+1];
	int nLen = MultiByteToWideChar(CP_UTF8,0,strUtf.c_str(),
		(int)strUtf.size(),pWch,strUtf.length());
	if( nLen <= 0 )
	{
		delete pWch;
		return L"";
	}
	else {
		pWch[nLen] = L'\0';
	}
	std::wstring strRes(pWch);
	delete pWch;
	return strRes;
}
std::string CG2XCommonTool::WideChar_to_Utf8( std::wstring& strVar)
{
	char *pCh = new char[strVar.length()*3 + 1];
	memset(pCh, 0, strVar.length()*3+1);
	UINT nLen = WideCharToMultiByte(CP_UTF8,0,strVar.c_str(),(int)strVar.length(),pCh,strVar.length()*3,NULL,NULL);
	if( nLen <= 0 )
	{
		delete pCh;
		return "";
	}
	else
	{
		if( nLen > strVar.length()*3 )
		{
			pCh[strVar.length()*3] = '\0';
		}
		else
		{
			pCh[nLen] = '\0';
		}
	}
	std::string strRes(pCh);
	delete pCh;
	return strRes;
}