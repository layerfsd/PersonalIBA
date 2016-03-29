
#include "stdafx.h"
#include "WinHttpRequest.h"

CWinHttpRequest::CWinHttpRequest()
: m_hDllInst(NULL)
{
	m_lReadyState = 4;
}

CWinHttpRequest::~CWinHttpRequest()
{
	ReleaseDispatch();

	if (m_hDllInst != NULL)
	{
		FreeLibrary(m_hDllInst);
		m_hDllInst = NULL;
	}
}

BOOL CWinHttpRequest::CreateByReg()
{
	m_lReadyState = CreateDispatch(_T("WinHttp.WinHttpRequest.5.1"));

	return m_lReadyState;
}

BOOL CWinHttpRequest::CreateByDll(LPCTSTR lpszDllPath /*= _T("winhttp.dll")*/)
{
	static const GUID CLSID_WinHttpRequest = 
	{0x2087c2f4, 0x2cef, 0x4953, {0xa8, 0xab, 0x66, 0x77, 0x9b, 0x67, 0x04, 0x95}};

	static const GUID IID_IWinHttpRequest = 
	{0x016fe2ec, 0xb2c8, 0x45f8, {0xb2, 0x3b, 0x39, 0xe5, 0x3a, 0x75, 0x39, 0x6b}};

	typedef HRESULT (__stdcall* pfnDllGetClassObject)(REFCLSID, REFIID, void**); 

	m_hDllInst = LoadLibrary(lpszDllPath); 

	ASSERT(m_hDllInst);

	if (m_hDllInst == NULL)
	{
		return FALSE;
	}

	pfnDllGetClassObject fnDllGetClassObject = (pfnDllGetClassObject)GetProcAddress(m_hDllInst, "DllGetClassObject"); 

	ASSERT(fnDllGetClassObject);

	if (fnDllGetClassObject == NULL) 
	{ 
		return FALSE;
	}

	IClassFactory* lpClassFactory = NULL; 

	HRESULT hr = (fnDllGetClassObject)(CLSID_WinHttpRequest, IID_IClassFactory, (void**)&lpClassFactory); 

	ASSERT(lpClassFactory);

	if (SUCCEEDED(hr) && (lpClassFactory != NULL)) 
	{ 
		hr = lpClassFactory->CreateInstance(NULL, IID_IWinHttpRequest, (void**)&m_lpDispatch); 

		ASSERT(m_lpDispatch);

		if (SUCCEEDED(hr) && (m_lpDispatch != NULL)) 
		{
			//
		}

		lpClassFactory->Release(); 
	} 

	return m_lpDispatch != NULL;
}

void CWinHttpRequest::Open(LPCTSTR Method, LPCTSTR Url)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BOOL ;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Method, Url, FALSE);
}

void CWinHttpRequest::Send()
{
	if (!m_lReadyState)
	{
		return;
	}

	m_lReadyState = 1;
	//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Open = %d"), m_lReadyState);
	try
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, _T(""));

		/*if (m_bAsync)
		{
			m_lReadyState = 2;
		}
		else*/
		{ 
			m_lReadyState = 4;
		}

		//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Send = %d"), m_lReadyState);

	}
	catch (COleDispatchException * e) 
	{
		//CIBALog::GetInstance()->WriteFormat(_T("DealWithException(e)"));
	}
	//static BYTE parms[] = VTS_BSTR ;
	//InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, _T(""));
}

void CWinHttpRequest::Send(LPCTSTR Body)
{
	static BYTE parms[] = VTS_BSTR ;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Body);
}

long CWinHttpRequest::GetStatus()
{
	long result = 0;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString CWinHttpRequest::GetStatusText()
{
	CString result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CWinHttpRequest::GetResponseText()
{
	CString result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}


IStream* CWinHttpRequest::GetResponseStream()
{
	VARIANT varResponse;

	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&varResponse, NULL);

	if (VT_UNKNOWN == V_VT(&varResponse) || VT_STREAM == V_VT(&varResponse)) 
	{ 
		IStream* pStream = NULL;

		HRESULT hr = V_UNKNOWN(&varResponse)->QueryInterface(IID_IStream, reinterpret_cast<void**>(&pStream)); 

		if (SUCCEEDED(hr))
		{
			return pStream;
		}
	}

	return NULL;
}

void CWinHttpRequest::SetTimeouts(long ResolveTimeout, long ConnectTimeout, long SendTimeout, long ReceiveTimeout)
{
	static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ResolveTimeout, ConnectTimeout, SendTimeout, ReceiveTimeout);
}

void CWinHttpRequest::SetOption(WinHttpRequestOption Option, COleVariant newValue)
{
	static BYTE parms[] = VTS_I4 VTS_VARIANT ;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Option, &newValue);
}

BOOL CWinHttpRequest::DownloadFile( LPCTSTR lpszURL, LPCTSTR lpszFileName )
{
	BOOL bRet = FALSE;

	Open(_T("GET"), lpszURL);

	Send();

	if (GetStatus() != 200)
	{
		return FALSE;
	}

	CString strResp = GetResponseText();

	if (0 == strResp.Left(6).CompareNoCase(_T("<html>")))
	{
		return FALSE;
	}

	IStream* pStream = GetResponseStream();

	if (pStream != NULL)
	{
		CFile file;

		if (file.Open(lpszFileName, CFile::modeWrite | CFile::modeCreate))
		{
			BYTE bBuffer[8192] = {0}; 
			DWORD cbRead = 0; 

			while (TRUE) 
			{ 								
				if (SUCCEEDED(pStream->Read(bBuffer, 8192, &cbRead)) && 0 != cbRead)
				{				
					file.Write(bBuffer, cbRead);

					bRet = TRUE;
				}	
				else
				{
					break;
				}
			} 

			file.Close();
		}
	}

	return bRet;
}

COleVariant CWinHttpRequest::GetOption(WinHttpRequestOption Option)
{
	VARIANT result;
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms, Option);
	return result;
}


CString CWinHttpRequest::GetResponseHeader(LPCTSTR Header)
{
	CString result;
	static BYTE parms[] = VTS_BSTR ;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, Header);
	return result;
}

CString CWinHttpRequest::GetAllResponseHeaders()
{
	CString result;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return result;
}

LONG CWinHttpRequest::GetReadyState() const
{
	return m_lReadyState;
}