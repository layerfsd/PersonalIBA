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

	static COleDateTime StringToOleDateTime(CString strDataTime); // from "YYYY-MM-DD hh:mm:ss" to COleDateTime
	//COleDateTime StringToOleDateTime(CString strDataTime); // from "YYYY-MM-DD hh:mm:ss" to COleDateTime

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
	static CString EncodePassword(CString strPwd, bool bAppendZero = true);
	static CString FormatNetId(const CString& strNetId);
	static BOOL IsWebUrl( const CString& strPath );
	static BOOL IsSysWin7();
	static BOOL ExePath( LPCTSTR strPath, LPCTSTR strParam = NULL);
	static CString GetSfregUserOrgImagePath( int idType, const CString& zjh );
	static CString GetSfregUserZJImagePath( int idType, const CString& zjh );
	static void GetSfregZJImgPath(CString& strZJ);
	static void GetSfregOrgImgPath(CString& strOrg);
	static int DecT2Int(LPCTSTR szNum, int nMulSize = 2);

	// 2014-6-13 - qsc 用于发消息给恒信，做结账操作
	static BOOL AutoHXPayout(LPCTSTR szTermID);
	static BOOL CALLBACK EnumChildProcHX(HWND hwnd,LPARAM lParam);

	// 2014-6-20 - qsc 
	static INT_PTR MsgBox(CWnd* pParent, UINT nResId, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);
	static INT_PTR MsgBox(CWnd* pParent, LPCTSTR lpszText, UINT nType = MB_OK, LPCTSTR lpszCaption = NULL);
	static INT_PTR MsgBoxFormat(CWnd* pParent, UINT nType, UINT nFormatId, ...);

	// 2014-6-25 - qsc
	static void FillSuitableString(CString& strDest, UINT nMinLen, TCHAR chFilled, BOOL bInsertAtFront = TRUE);
	static CString GetZcImgPath();
	static CString GetZjImgPath();
	static CString GetUserZcImgFilePath( int idType, const CString& zjh );
	static CString GetUserZjImgFilePath( int idType, const CString& zjh );

	static CString GetUserZcImgPath( int idType, const CString& zjh );
	static CString GetUserZjImgPath( int idType, const CString& zjh );//获取用户照片目录，不存在则创建
	// 2014-6-30 - qsc 拍照
	static bool TakePhoto(LPCTSTR szPath, LPCTSTR szName);

	// 2014-7-21 - qsc 是否是运行在虚拟机中
	static bool IsVirMach();
	static bool IsInsideVMWare();
	static bool IsInsideVPC();
	static DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep);
	
	// 2014-10-30 - qsc
	static std::string DecodeBase64( const char* Data,int DataByte,int& OutByte );
	static std::string EncodeBase64( const char* Data,int DataByte);

	static BOOL GetSelfDefineResource(HMODULE hRes, INT ID, LPCTSTR szResType, PVOID pText, long* ltextSize);

	static CString CalcLocalCreditCheckCode(UINT nMemberID, LPCTSTR lpszCreditTime, UINT nAmount);
	static void WriteRegisterIsFN(BOOL bAdd /* = TRUE */);

	// 2015-3-31 - tmelody
	static BOOL ReadUtf8File(CString strFileName, CString& strFileContent);
	
	static std::wstring Utf8_to_WideChar( std::string& strUtf);
	static std::string WideChar_to_Utf8( const std::wstring& strVar);

	// 2015-0824 liyajun 为状态栏添加鼠标移动的提示框以显示所有状态
	static void AFXAPI DDX_Control_IfIDIsWnd(CDataExchange* pDX, int nIDC, CWnd& rControl);

	//2016-0307 liyajun 生成获取二维码接口的请求参数
	static CString GenQrRequestParam(const CString *pstrAccount,const CString *pstrPwd,const UINT nMemberId,const UINT nPresentAmount, const UINT nNeedPay,const UINT nAmount);
	//2016-0307 liyajun 请求二维码
	static BOOL GetQrData(const CString *pstrRequestedData, CString *strQr, UINT *nStatusCode);
	//2016-0307 liyajun 更改账号的显示方式
	static CString MaskNetID(const CString *pstrNetId);
	//2016-0308 liyajun 用于urlencode相关
	static unsigned char ToHex(unsigned char x);
	static unsigned char FromHex(unsigned char x);
	static std::string UrlEncode(const std::string& str);
	static std::string UrlDecode(const std::string& str);  
};
