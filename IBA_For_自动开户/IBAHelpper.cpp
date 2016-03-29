#include "stdafx.h"
#include "resource.h"
#include "IBAConfig.h"
#include ".\ibahelpper.h"

#include <iphlpapi.h> 
#pragma comment(lib, "IPHLPAPI.lib") 
#include "Encrypt.h"
#include "IBA.h"
#include <complex>


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

//归化成标准10位数字，大于9位则原样返回
CString CIBAHelpper::FormatNetId(const CString& strNetId)
{
	INT nLength = strNetId.GetLength();

	if (nLength > 9)
	{
		return strNetId;
	}
	else
	{
		CString strRet = _T("0000000000");
		strRet = strRet.Mid(0, 10 - nLength);
		strRet.Append(strNetId);

		return strRet;
	}
}

// 用Shell执行一个路径
void CIBAHelpper::ExePath( LPCTSTR strPath, LPCTSTR strParam)
{ 
	// 初始化结构
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;	// 若fMask不设为SEE_MASK_NOCLOSEPROCESS则hProcess项值为NULL

	// 网址不能用runas，用runas会导致报告参数错误
	if(!IsWebUrl(strPath) && IsSysWin7())		// WIN7请求提升权限
	{
		sei.lpVerb = _T("runas");	// WIN7以管理员身份打开，其他系统以普通方式打开
	}
	else
	{
		sei.lpVerb = _T("open");	
	}	
	sei.lpFile = strPath;
	// 别忘了这个参数，否则，它会隐藏
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = strParam;

	if(ShellExecuteEx(&sei))
	{
		CloseHandle(sei.hProcess); 
	}
	else
	{
		DWORD dwStatus = GetLastError();
		if(dwStatus == ERROR_CANCELLED)
		{
			// 用户拒绝提升权限
		}
		else if(dwStatus == ERROR_ACCESS_DENIED)
		{
			// 其他的系统出现拒绝访问的情况

			// 程序拒绝访问，以管理员身份运行
			ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.lpVerb = _T("runas");
			sei.lpFile = strPath;
			// 别忘了这个参数，否则，它会隐藏
			sei.nShow = SW_SHOWNORMAL;
			if(ShellExecuteEx(&sei))
			{
				// 关闭子进程句柄 
				CloseHandle(sei.hProcess);
			}
		}
		else
		{
			// 
		}

	}
	return ;
}

BOOL CIBAHelpper::IsSysWin7()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	if ( VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CIBAHelpper::IsWebUrl( const CString& strPath )
{
	CString strTmp = strPath;
	strTmp.MakeLower();
	if(_T("http") == strTmp.Left(4) || _T("www.") == strTmp.Left(3))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// 从sfreg获取证件照地址
void CIBAHelpper::GetSfregZJImgPath(CString& strZJ)
{
	CString strSfregPath, strIBAPath, strConfingPath;
	strIBAPath = theApp.GetWorkPath() + _T("\\IBABin\\");		// IBA当前路径
	strSfregPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\");		// SFREG路径

	strConfingPath  = strSfregPath + _T("SCONFIG.INI");					// SFREG配置文件路径
	if(!PathFileExists(strConfingPath))				// 如果配置文件不存在，则证件和注册照都是保存在sfreg的当前目录
	{
		strZJ = strSfregPath;
	}
	else
	{
		GetPrivateProfileString(_T("PATH"), _T("SFZ_IMAGE_PATH"), NULL, strZJ.GetBuffer(MAX_PATH), MAX_PATH, strConfingPath);
		strZJ.ReleaseBuffer();
		strZJ.Replace(_T("..\\"), strIBAPath);		// 注意这两个的顺序不要调换因为"..\"包含".\"
		strZJ.Replace(_T(".\\"), strSfregPath);
	}
	/*if(strZJ.Right(1) != '\\')
		strZJ += _T("\\");*/
}

// 从sfreg获取注册照地址
void CIBAHelpper::GetSfregOrgImgPath(CString& strOrg)
{
	CString strSfregPath, strIBAPath, strConfingPath;
	strIBAPath = theApp.GetWorkPath() + _T("\\IBABin\\");		// IBA当前路径
	strSfregPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\");		// SFREG路径

	strConfingPath  = strSfregPath + _T("SCONFIG.INI");					// SFREG配置文件路径
	if(!PathFileExists(strConfingPath))				// 如果配置文件不存在，则证件和注册照都是保存在sfreg的当前目录
	{
		strOrg = strSfregPath;
	}
	else
	{
		GetPrivateProfileString(_T("PATH"), _T("ORG_IMAGE_PATH"), NULL, strOrg.GetBuffer(MAX_PATH), MAX_PATH, strConfingPath);
		strOrg.ReleaseBuffer();
		strOrg.Replace(_T("..\\"), strIBAPath);		// 注意这两个的顺序不要调换因为"..\"包含".\"
		strOrg.Replace(_T(".\\"), strSfregPath);
	}
	/*if(strOrg.Right(1) != '\\')
		strOrg += _T("\\");*/
}

CString CIBAHelpper::GetUserZJImagePath(
						   int idType, 
						   const CString& zjh )
{
	CString strZJImgPath;
	GetSfregZJImgPath(strZJImgPath);
	if ( idType == 11 || idType == 12)
	{
		CString sm = zjh.Left(2);
		CString dfm = zjh.Mid(2, 4);

		CString path = strZJImgPath + sm + _T("\\") 
			+ dfm + _T("\\") + zjh + _T(".jpg");

		return path;
	}
	else
	{
		CString temp;
		temp.Format( _T("%d"), idType );

		CString path = strZJImgPath + "Other" + _T("\\") + temp + _T("\\") + zjh + _T(".jpg");

		return path;
	}
}

//////////////////////////////////////////////////////////////////////////

CString CIBAHelpper::GetUserOrgImagePath(
							int idType, 
							const CString& zjh )
{
	CString strOrgImgPath;
	GetSfregOrgImgPath(strOrgImgPath);
	if ( idType == 11  || idType == 12) 
	{
		CString sm = zjh.Left(2);
		CString dfm = zjh.Mid(2, 4);

		CString path = strOrgImgPath + sm + _T("\\")
			+ dfm + _T("\\") + zjh + _T(".jpg");

		return path;
	}
	else
	{
		CString temp;
		temp.Format( _T("%d"), idType );

		CString path = strOrgImgPath + "Other" + _T("\\") + temp + _T("\\") + zjh + _T(".jpg");

		return path;
	}
}

// 使用这个函数的原因是浮点数表示是不准确的
// 例如2.3的double就会变成2.29999....
// 将一个小数字符串转换位整数，后面的nMulNumSize是取小数的位数
// szNum = "15.02000", nMulNumSize = 2
// return 1502
int CIBAHelpper::DecT2Int( LPCTSTR szNum, int nMulSize /*= 2*/ )
{
	CString strInt, strDec;
	CString strNum(szNum);
	int nPos = -1;
	nPos = strNum.Find('.');
	if(nPos < 0 )
	{
		strInt = strNum;
		strDec.Empty();
	}
	else
	{
		strInt = strNum.Left(nPos);
		nPos = strNum.ReverseFind('.');

		// 以下两步超过了截取，不足补0
		// 假设nMulNumSize == 2
		// "01" -> "01"
		// "1" -> "10"
		strDec = strNum.Mid(nPos + 1, nMulSize);
		while(strDec.GetLength() < nMulSize)
		{
			strDec.AppendChar('0');
		}
	}
	int nInt = _ttoi(strInt);
	int nDec = _ttoi(strDec);
	int nRet = nInt * pow(10, nMulSize) + nDec;
	return nRet;
}
