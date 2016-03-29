#include "stdafx.h"
#include "resource.h"
#include "IBAConfig.h"
#include ".\ibahelpper.h"

#include <iphlpapi.h> 
#pragma comment(lib, "IPHLPAPI.lib") 
#include "Encrypt.h"


CIBAHelpper::CIBAHelpper(void)
{
}

CIBAHelpper::~CIBAHelpper(void)
{
}

BOOL CIBAHelpper::IsCtrlPressed() 
{
	return ((GetKeyState(VK_CONTROL) & (1<<(sizeof(SHORT)*8-1))) != 0);
}

BOOL CIBAHelpper::IsAltPressed() 
{
	return ((GetKeyState(VK_MENU) & (1<<(sizeof(SHORT)*8-1))) != 0);
}

BOOL CIBAHelpper::IsShiftPressed() 
{
	return ((GetKeyState(VK_SHIFT) & (1<<(sizeof(SHORT)*8-1))) != 0);
}

void CIBAHelpper::SetDllDirectory(LPCTSTR lpPathName)
{
	HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));

	typedef VOID (WINAPI *pfnSetDllDirectory)(LPCTSTR lpPathName);

	if (hKernel32 != NULL)
	{

#ifdef _UNICODE
		pfnSetDllDirectory pSetDllDirectory = (pfnSetDllDirectory)GetProcAddress(hKernel32, "SetDllDirectoryW");
#else
		pfnSetDllDirectory pSetDllDirectory = (pfnSetDllDirectory)GetProcAddress(hKernel32, "SetDllDirectoryA");
#endif

		if (pSetDllDirectory != NULL)
		{
			pSetDllDirectory(lpPathName);
		}
		else
		{
			AfxMessageBox(_T("Can't call SetDllDirectory!"));
		}
	}
}

void CIBAHelpper::AddEnvironmentVariable(LPCTSTR lpszEnvVar)
{
	CString strTmp;
	
	strTmp.GetEnvironmentVariable(_T("Path"));

	if (strTmp.Right(1) != _T(";"))
	{
		strTmp.AppendChar(';');
	}

	strTmp.Append(lpszEnvVar);

	SetEnvironmentVariable(_T("Path"), strTmp);
};

COleDateTime CIBAHelpper::CenterTimeToOleDateTime(CString strServerTime)
{
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	year = _ttoi(strServerTime.Mid(0, 4));
	month = _ttoi(strServerTime.Mid(4, 2));
	day = _ttoi(strServerTime.Mid(6, 2));
	hour = _ttoi(strServerTime.Mid(8, 2));
	minute = _ttoi(strServerTime.Mid(10, 2));
	second = _ttoi(strServerTime.Mid(12, 2));

	return COleDateTime(year, month, day, hour, minute, second);
}

CString CIBAHelpper::FormatCenterTime(CString strServerTime)
{
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	year = _ttoi(strServerTime.Mid(0, 4));
	month = _ttoi(strServerTime.Mid(4, 2));
	day = _ttoi(strServerTime.Mid(6, 2));
	hour = _ttoi(strServerTime.Mid(8, 2));
	minute = _ttoi(strServerTime.Mid(10, 2));
	second = _ttoi(strServerTime.Mid(12, 2));

	return COleDateTime(year, month, day, hour, minute, second).Format(_T("%Y-%m-%d %H:%M:%S"));
}

BOOL CIBAHelpper::MakeSurePathExists(LPCTSTR lpszPath)
{
	BOOL bRet = FALSE;

	bRet = PathFileExists(lpszPath);

	if (!bRet)
	{
		bRet = (ERROR_SUCCESS == SHCreateDirectoryEx(NULL, lpszPath, NULL)) ;
	}

	return bRet;
}


BOOL CIBAHelpper::ReadFile(CString strFileName, CString& strFileContent)
{
	BOOL bRet = FALSE;

	CFile File;

	if (File.Open(strFileName, CFile::modeRead))
	{
		CStringA strTmp;

		UINT nLen = (UINT)File.GetLength();

		File.Read(strTmp.GetBuffer(nLen), nLen);
		strTmp.ReleaseBuffer(nLen);

		File.Close();

#ifdef _UNICODE
		strFileContent = CA2WEX<2048>(strTmp);
#else
		strFileContent = strTmp;
#endif

		bRet = TRUE;
	}

	return bRet;
}

BOOL CIBAHelpper::WriteFile(CString strFileName, CString& strFileContent)
{
	BOOL bRet = FALSE;

	CStringA strTmp;

#ifdef _UNICODE
	strTmp = CW2AEX<2048>(strFileContent);
#else
	strTmp = strFileContent;
#endif

	CFile File;

	if (File.Open(strFileName, CFile::modeWrite | CFile::modeCreate))
	{
		File.Write(strTmp.GetBuffer(), strTmp.GetLength());

		File.Close();

		bRet = TRUE;
	}

	return bRet;
}

BOOL CIBAHelpper::AppendWriteFile(CString strFileName, CString& strFileContent)
{
	BOOL bRet = FALSE;

	CStringA strTmp;

#ifdef _UNICODE
	strTmp = CW2AEX<2048>(strFileContent);
#else
	strTmp = strFileContent;
#endif

	UINT uMode = CFile::modeWrite;
	if( !::PathFileExists(strFileName) )
	{
		uMode |= CFile::modeCreate;
	}
	CFile File;
	if (File.Open(strFileName, uMode))
	{
		File.SeekToEnd();
		File.Write(strTmp.GetBuffer(), strTmp.GetLength());

		File.Close();

		bRet = TRUE;
	}

	return bRet;
}


void CIBAHelpper::SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch /*= '|'*/)
{
	strArray.RemoveAll();

	strLine.Trim();
	
	if (strLine.IsEmpty())
	{
		return;
	}

	INT nPos = 0;

	INT nIndex = strLine.Find(ch);
	
	CString strTmp;
	
	while (nIndex >= 0)
	{
		strTmp.Empty();
		
		strTmp = strLine.Mid(nPos, nIndex - nPos);

		strArray.Add(strTmp);

		nPos = nIndex + 1;

		nIndex = strLine.Find(ch, nPos);
	}

	if (nPos == 0) //没有找到分割符合
	{
		strArray.Add(strLine);
	}
	else
	{
		if ((strLine.GetLength() - 1) >= nPos)
		{
			strTmp.Empty();
			strTmp = strLine.Mid(nPos, strLine.GetLength() - nPos);
			strArray.Add(strTmp);
		}
	}
}

void CIBAHelpper::CreateFont(CFont& font, LONG lHeight, LONG lWeight,BOOL bUnderLine)
{
	font.DeleteObject();

	LOGFONT lf = {0};
	font.CreateStockObject(DEFAULT_GUI_FONT);
	font.GetLogFont(&lf);
	font.DeleteObject();
	lf.lfWidth = 0;
	lf.lfHeight = 0 - lHeight;
	lf.lfWeight = lWeight;
	if( bUnderLine )
		lf.lfUnderline = 1;

	font.CreateFontIndirect(&lf);
}

void CIBAHelpper::CreateFont(CFont& font)
{
	switch (CIBAConfig::GetInstance()->GetFontType())
	{
	case 1:
		CreateFont(font, 14);
		break;
	case 2:
		CreateFont(font, 18);
		break;
	}
}

BOOL CIBAHelpper::GetIPAndMAC(CString& strIP, CString& strMAC)
{
	BOOL bRet = FALSE, bFind = FALSE;

	DWORD AdapterInfoSize = 0;   

	bRet = (GetAdaptersInfo(0, &AdapterInfoSize) == ERROR_BUFFER_OVERFLOW); //获取长度  

	if (bRet)  
	{   
		PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[AdapterInfoSize]; //分配内存   

		bRet = (GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize) == ERROR_SUCCESS); //获取信息   

		if (bRet) 
		{   
			PIP_ADAPTER_INFO pAdapter = pAdapterInfo;   

			while (pAdapter != NULL)   
			{    
				strIP = CA2W(pAdapter->IpAddressList.IpAddress.String);

				if (strIP.Left(3) == _T("192") || strIP.Left(3) == _T("172") || strIP.Left(3) == "10.")
				{
					strMAC.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),  				 
						pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2],   
						pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);

					bFind = TRUE;

					break;
				}

				pAdapter = pAdapter->Next; //下一个区域  
			}   

			//DWORD dwCount = AdapterInfoSize / sizeof(_IP_ADAPTER_INFO)

			if (!bFind) //没找到，就用第一个
			{
				pAdapter = pAdapterInfo;  

				strIP = CA2W(pAdapter->IpAddressList.IpAddress.String);

				strMAC.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),  				 
					pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2],   
					pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
			}
		}   

		delete[] pAdapterInfo; //释放内存   
	}   

	return bRet;
}

BOOL CIBAHelpper::PutTextToClipboard(LPCTSTR pTxtData)
{
	BOOL bRet = FALSE;

	if (OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()))
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (lstrlen(pTxtData) + 1) * sizeof(TCHAR));
		
		if (hData != NULL)
		{
			LPTSTR pszData = (LPTSTR)::GlobalLock(hData);
			lstrcpy(pszData, pTxtData);
			GlobalUnlock(hData);
			
#ifdef _UNICODE
			bRet = (SetClipboardData(CF_UNICODETEXT, hData) != NULL);
#else
			bRet = (SetClipboardData(CF_TEXT, hData) != NULL);
#endif // _UNICODE			
		}

		CloseClipboard();
	}

	return bRet; 
}

BOOL CIBAHelpper::AdjustPrivileges(LPCTSTR lpPrivilegeName)
{
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		LUID luid = {0};   
		TOKEN_PRIVILEGES tp ={0};

		if (LookupPrivilegeValue(NULL, lpPrivilegeName, &luid))
		{
			tp.PrivilegeCount = 1;   
			tp.Privileges[0].Luid = luid;   
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   

			if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
			{
				return TRUE;
			}
		}

		CloseHandle(hToken);
	}

	return FALSE;
}

BOOL CIBAHelpper::SyncLocalTime(COleDateTime dt)
{
	if (AdjustPrivileges(SE_SYSTEMTIME_NAME))
	{
		SYSTEMTIME sysTime = {0};

		if (dt.GetAsSystemTime(sysTime))
		{
			if (SetLocalTime(&sysTime))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

CString CIBAHelpper::GetCurrentTimeAsString(BOOL bWithFlag)
{
	CString strRet;

	if (!bWithFlag)
	{
		strRet = COleDateTime::GetCurrentTime().Format(_T("%Y%m%d%H%M%S"));
	}
	else
	{
		strRet = COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	}

	return strRet;
}

BOOL CIBAHelpper::IsValidTime(const COleDateTime dt)
{
	if (dt.GetStatus() == COleDateTime::invalid ||
		dt.GetStatus() == COleDateTime::null ||
		dt.m_dt == 0)
	{
		return FALSE;
	}
	return TRUE;
}

CString CIBAHelpper::FormatTime(COleDateTime dt)
{
	if (!IsValidTime(dt))
	{
		return _T("0000-00-00 00:00:00");
	}
	return dt.Format(_T("%Y-%m-%d %H:%M:%S"));
}


void CIBAHelpper::FillString(CString& strDest, UINT nLen, TCHAR ch)
{
	INT nFillLen = nLen - strDest.GetLength();

	if (nFillLen < 0)
	{
		strDest = strDest.Left(nLen);
	}

	for (; nFillLen > 0; nFillLen--)
	{
		strDest.AppendChar(ch); 
	}
}

void CIBAHelpper::AppendString(CString& strDest, CString aParam, UINT nLen)
{
	INT nFillLen = nLen - aParam.GetLength();

	for (; nFillLen > 0; nFillLen--)
	{
		strDest.AppendChar(' '); 
	}

	strDest.Append(aParam);
}

void CIBAHelpper::AppendString(CString& strDest, INT nParam, UINT nLen)
{
	CString aParam;
	aParam.Format(_T("%d"), nParam);

	AppendString(strDest, aParam, nLen);
}

void CIBAHelpper::AppendString(CString& strDest, UINT nParam, UINT nLen)
{
	CString aParam;
	aParam.Format(_T("%d"), nParam);

	AppendString(strDest, aParam, nLen);
}

UINT CIBAHelpper::RoundMoney(UINT nMoney, UINT nType)
{
	UINT nRetMoeny = 0;

	switch(nType)
	{
	case 0://无舍入
		{
			nRetMoeny = nMoney;
			break;
		}
	case 1://4舍5入
		{
			nRetMoeny  = (nMoney + 50) / 100 * 100;
			break;
		}
	case 2://2舍3入
		{
			nRetMoeny = (nMoney + 25) / 50 * 50;
			break;
		}
	case 3://取整到10
		{
			nRetMoeny = nMoney / 100 * 100;
			break;
		}
	case 4://取整到5
		{
			nRetMoeny = nMoney / 50 * 50;
			break;
		}
	case 5://四九进一
		{
			nRetMoeny = (nMoney + 10) / 50 * 50;
			break;
		}
	}

	return nRetMoeny;
}


CString CIBAHelpper::GetSexAsString(UINT nSex)
{
	CIBAString strTmp;

	strTmp.LoadString(nSex == 0 ? IDS_MAN : IDS_WOMAN);

	return strTmp;
}

void CIBAHelpper::PopupWindowMsg(CString strCaption,CString strText)
{
	CBCGPPopupWindow* pPopup = new CBCGPPopupWindow; //这里不需要删除，框架会自动删除的

	pPopup->SetAnimationType(CBCGPPopupMenu::ANIMATION_TYPE::NO_ANIMATION);
	pPopup->SetAnimationSpeed(100);
	pPopup->SetSmallCaption(FALSE);	//大标题
	pPopup->SetAutoCloseTime(0);	//不自动关闭

	// Create indirect:
	CBCGPPopupWndParams params;//设置窗口参数

	//接受客户端消息
	params.m_strText = strText;
	params.m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pPopup->Create(NULL, params);//创建窗体
	pPopup->SetWindowText(strCaption);//显示消息
}

void CIBAHelpper::WaitFor(DWORD dwMilliseconds)
{
	const DWORD PerSleep = 50;

	DWORD dwCount = dwMilliseconds / PerSleep;

	for (DWORD i = 0; i < dwCount; i++)
	{
		Sleep(PerSleep);

		AfxGetApp()->PumpMessage();
	}
}

CString CIBAHelpper::TranformTime(CString strTimeFlags)
{
	strTimeFlags.Remove(_T('-'));
	strTimeFlags.Remove(_T(':'));
	strTimeFlags.Remove(_T(' '));

	if (strTimeFlags.IsEmpty())
	{
		return _T("00000000000000");
	}

	IBA_ASSERT2(strTimeFlags.GetLength()==14, "时间格式不对");

	return strTimeFlags;
}

CString CIBAHelpper::EncodePassword(CString strPwd)
{
	const CString strKey(_T("5E0928C18071DC10")); //固定的密码KEY
	CIBAHelpper::FillString(strPwd, 8, '0');
	if (strPwd.GetLength() > 8)
	{
		strPwd = strPwd.Left(8);
	}
	return CEncrypt::EncryptString(strPwd, strKey);
}
