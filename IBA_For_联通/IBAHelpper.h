#pragma once


class CIBAHelpper
{
private:
	CIBAHelpper(void);
	~CIBAHelpper(void);

public:

	static BOOL IsCtrlPressed();

	static BOOL IsAltPressed();

	static BOOL IsShiftPressed();

public:

	static void CreateFont(CFont& font, LONG lHeight , LONG lWeight = 500,BOOL bUnderLine=FALSE);
	static void CreateFont(CFont& font);

	static BOOL GetIPAndMAC(CString& strIP, CString& strMAC);

	static BOOL PutTextToClipboard(LPCTSTR pTxtData);

	static void SetDllDirectory(LPCTSTR lpPathName);

	static void AddEnvironmentVariable(LPCTSTR lpszEnvVar);

	static void WaitFor(DWORD dwMilliseconds);

public:

	static BOOL MakeSurePathExists(LPCTSTR lpszPath);

	static BOOL ReadFile(CString strFileName, CString& strFileContent);

	static BOOL WriteFile(CString strFileName, CString& strFileContent);

	static BOOL AppendWriteFile(CString strFileName, CString& strFileContent);

	static void SplitLine(CString& strLine, CStringArray& strArray, TCHAR ch = '|');

public:

	static BOOL AdjustPrivileges(LPCTSTR lpPrivilegeName);

	static BOOL SyncLocalTime(COleDateTime dt);

	static CString GetCurrentTimeAsString(BOOL bWithFlag = FALSE);

	static COleDateTime CenterTimeToOleDateTime(CString strServerTime);

	static CString FormatCenterTime(CString strServerTime);
	
	static BOOL IsValidTime(const COleDateTime dt);

	static CString FormatTime(COleDateTime dt); // YYYY-MM-DD hh:mm:ss

	static CString TranformTime(CString strTimeFlags); //from "YYYY-MM-DD hh:mm:ss" to "YYYYMMDDhhmmss"
	
public:

	static void FillString(CString& strDest, UINT nLen, TCHAR ch);

	static void AppendString(CString& strDest, CString aParam, UINT nLen);

	static void AppendString(CString& strDest, INT nParam, UINT nLen);

	static void AppendString(CString& strDest, UINT nParam, UINT nLen);
	
	static UINT RoundMoney(UINT nMoney, UINT nType);
	
	static CString GetSexAsString(UINT nSex);

public:
	// 2011-4-1-gxx-弹出式的消息提示
	static void PopupWindowMsg(CString strCaption,CString strText);

	// 2011/08/05-8201-gxx: 加密密码,DES
	static CString EncodePassword(CString strPwd);
	
	
};
