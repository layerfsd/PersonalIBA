#include "StdAfx.h"
#include "commandlineinfo.h"

CCommandLine::CCommandLine(void)
{

}

CCommandLine::~CCommandLine(void)
{

}

void CCommandLine::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	CString strArg(pszParam);
	strArg.Trim();

	if (bFlag)
	{
		if (strArg.Left(3).MakeUpper() == "DID")
		{
			m_nDomainId = _ttoi(strArg.Mid(3));
		}
		else if (strArg.Left(3).MakeUpper() == "RNT")
		{
			m_nRealNameType = _ttoi(strArg.Mid(3));
		}
		else if (strArg.Left(1).MakeUpper() == "I")
		{
			StartRZX();
		}
		else if (strArg.Left(1).MakeUpper() == "U")
		{
			StopRZX();
		}
	}

	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

void CCommandLine::StartRZX()
{
	CString strMyPath = AfxGetApp()->m_pszHelpFilePath;
	strMyPath = strMyPath.Left(strMyPath.ReverseFind('\\') + 1);

	CString strRzxPath(strMyPath);
	strRzxPath.Append(_T("rzxsurename.exe"));

	CL2Process rzx(strRzxPath);
	rzx.Execute();
}

void CCommandLine::StopRZX()
{

#define WM_USER_IDCARD_MANAGE    WM_USER + 2000		// 与任子行主程序通信
#define IDCARD_MANAGE_CLOSE 		5				// WPARM 参数定义 通知任子行关闭


	HWND hRZX = ::FindWindow(NULL, _T("任子行实名登记管理系统"));
	
	if (hRZX != NULL)
	{
		::PostMessage(hRZX, WM_USER_IDCARD_MANAGE, IDCARD_MANAGE_CLOSE, 0);
	}
}
