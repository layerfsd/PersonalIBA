#pragma once

// CWinHttpRequest 包装类

class CWinHttpRequest : public COleDispatchDriver
{
public:
	CWinHttpRequest();
	~CWinHttpRequest();

	enum WinHttpRequestOption
	{
		WinHttpRequestOption_UserAgentString = 0,
		WinHttpRequestOption_URL = 1,
		WinHttpRequestOption_URLCodePage = 2,
		WinHttpRequestOption_EscapePercentInURL = 3,
		WinHttpRequestOption_SslErrorIgnoreFlags = 4,
		WinHttpRequestOption_SelectCertificate = 5,
		WinHttpRequestOption_EnableRedirects = 6,
		WinHttpRequestOption_UrlEscapeDisable = 7,
		WinHttpRequestOption_UrlEscapeDisableQuery = 8,
		WinHttpRequestOption_SecureProtocols = 9,
		WinHttpRequestOption_EnableTracing = 10,
		WinHttpRequestOption_RevertImpersonationOverSsl = 11,
		WinHttpRequestOption_EnableHttpsToHttpRedirects = 12,
		WinHttpRequestOption_EnablePassportAuthentication = 13,
		WinHttpRequestOption_MaxAutomaticRedirects = 14,
		WinHttpRequestOption_MaxResponseHeaderSize = 15,
		WinHttpRequestOption_MaxResponseDrainSize = 16,
		WinHttpRequestOption_EnableHttp1_1 = 17,
		WinHttpRequestOption_EnableCertificateRevocationCheck = 18
	};

	static const LONG CodePage_GB2312 = 936; // 简体中文 (GB2312)

	//这个方法创建实例需要注册动态库
	BOOL CreateByReg();

	//这个方法创建实例需要知道动态库的路径
	BOOL CreateByDll(LPCTSTR lpszDllPath = _T("winhttp.dll"));

private:

	CWinHttpRequest(const CWinHttpRequest &rhs);
	CWinHttpRequest &operator=(const CWinHttpRequest &rhs);

	HINSTANCE m_hDllInst;

	INT m_lReadyState;
public:

	void Open(LPCTSTR Method, LPCTSTR Url);

	void Send();
	void Send(LPCTSTR Body);

	long GetStatus();

	LONG GetReadyState() const;

	CString GetStatusText();

	CString GetResponseText();

	CString GetResponseHeader(LPCTSTR Header);

	CString GetAllResponseHeaders();

	IStream* GetResponseStream();

	void SetTimeouts(long ResolveTimeout, long ConnectTimeout, long SendTimeout, long ReceiveTimeout);

	void SetOption(WinHttpRequestOption Option, COleVariant newValue);

	COleVariant GetOption(WinHttpRequestOption Option);

	BOOL DownloadFile(LPCTSTR lpszURL, LPCTSTR lpszFileName);
	
};
