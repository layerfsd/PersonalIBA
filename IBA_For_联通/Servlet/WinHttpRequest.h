#pragma once

namespace NS_SERVLET
{

class CWinHttpRequest : private COleDispatchDriver
{
private :

	CWinHttpRequest(const CWinHttpRequest &rhs);
	CWinHttpRequest &operator=(const CWinHttpRequest &rhs);

public :

	CWinHttpRequest(BOOL bAutoCreate = TRUE); 
	
	BOOL Create();

	BOOL GetAsync() const;

	LONG GetReadyState() const;

	BOOL DownloadFile(LPCTSTR lpszURL, LPCTSTR lpszFileName);

protected :

	LONG m_lReadyState;

	BOOL m_bAsync;
	
	void WriteLog(LPCTSTR lpszInfo);

	void DealWithException(COleDispatchException * e);

public :

	void Open(LPCTSTR lpszMethod, LPCTSTR lpszURL, BOOL bAsync = FALSE);
	void Open(LPCTSTR lpszURL);
	
	void Send(LPCTSTR Body = _T(""));
	
	void Abort();
	BOOL WaitForResponse(LONG Timeout);
	
	void SetTimeouts(LONG ResolveTimeout, LONG ConnectTimeout, LONG SendTimeout, LONG ReceiveTimeout);
	void SetRequestHeader(LPCTSTR Header, LPCTSTR Value);
	
	LONG GetStatus();
	CString GetStatusText();

	CString GetResponseHeader(LPCTSTR Header);
	CString GetAllResponseHeaders();

	CString GetResponseText();
	VARIANT GetResponseBody();
	IStream* GetResponseStream();

	VARIANT GetOption(LONG Option);
	void SetOption(LONG Option, VARIANT newValue);

};

}
