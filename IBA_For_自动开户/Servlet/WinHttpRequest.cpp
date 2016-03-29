
#include "stdafx.h"
#include "..\IBA.h"
#include "WinHttpRequest.h"

using namespace NS_SERVLET;

CWinHttpRequest::CWinHttpRequest(BOOL bAutoCreate)
: m_lReadyState(0), m_bAsync(FALSE), m_bComInit(FALSE)
{
	if (bAutoCreate)
	{
		Create();
	}
}

CWinHttpRequest::~CWinHttpRequest()
{
	if(m_bComInit)
	{
		::CoUninitialize();
	}
}

BOOL CWinHttpRequest::Create()
{
	if (m_lReadyState) //已经创建了
	{
		return TRUE;
	}

	BOOL bRet = FALSE;

	//先创建 WinHttp 5.1版本的组件

	bRet = CreateDispatch(_T("WinHttp.WinHttpRequest.5.1"));
	
	// 2014-4-3 - qsc 外面会出现不能注册成功的情况,找不到原因只能加::CoInitialize()
	if (!bRet)
	{
		::CoInitialize(NULL);
		m_bComInit = TRUE;

		bRet = CreateDispatch(_T("WinHttp.WinHttpRequest.5.1"));
	}

	if (!bRet)
	{
		AfxMessageBox(IDS_REGWINHTTPDLL);

		WriteLog(_T("Create WinHttp5.1 Instance Failed."));
	
		//自动注册

		//CL2Process RegSvr32(_T("regsvr32.exe"));
		//RegSvr32.Execute(_T("/s winhttp.dll"));
		//RegSvr32.Wait();
		
		//如果没有则创建 WinHttp 5.0版本的组件
		
		//bRet = CreateDispatch(_T("WinHttp.WinHttpRequest.5"));

		//if (!bRet)
		//{
		//	WriteLog(_T("Create WinHttp5.0 Instance Failed."));
		//}
	}		

	m_lReadyState = bRet;

	ASSERT(bRet);

	return bRet;
}


void CWinHttpRequest::DealWithException(COleDispatchException * e)
{
	CString strError;

	if (!e->m_strSource.IsEmpty())
	{
		strError = e->m_strSource + _T(" - ");
	}

	if (!e->m_strDescription.IsEmpty())
	{
		strError += e->m_strDescription;
	}
	else
	{
		strError += _T("Unknown Error!");
	}

#ifdef _DEBUG
	//AfxMessageBox(strError, MB_OK);
	IBA_TRACE(strError);
#endif

	e->Delete();

	WriteLog(strError);
}

void CWinHttpRequest::WriteLog(LPCTSTR lpszInfo)
{
	CIBALog::GetInstance()->Write(lpszInfo);
}

LONG CWinHttpRequest::GetReadyState() const
{
	return m_lReadyState;
}

BOOL CWinHttpRequest::GetAsync() const
{
	return m_bAsync;
}

void CWinHttpRequest::Open(LPCTSTR lpszMethod, LPCTSTR lpszURL, BOOL bAsync)
{
	//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Open = %d"), m_lReadyState);

	if (!m_lReadyState)
	{
		return;
	}

	m_bAsync = bAsync;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BOOL ;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lpszMethod, lpszURL, bAsync);
}

void CWinHttpRequest::Open(LPCTSTR lpszURL)
{
	//Open(_T("POST"), lpszURL, FALSE);
	Open(_T("GET"), lpszURL, FALSE);
}

void CWinHttpRequest::SetRequestHeader(LPCTSTR Header, LPCTSTR Value)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR ;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Header, Value);
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

void CWinHttpRequest::Send(LPCTSTR Body)
{
	//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Send = %d"), m_lReadyState);

	if (!m_lReadyState)
	{
		return;
	}

	m_lReadyState = 1;
	//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Open = %d"), m_lReadyState);
	try
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Body);
		
		if (m_bAsync)
		{
			m_lReadyState = 2;
		}
		else
		{ 
			m_lReadyState = 4;
		}

		//CIBALog::GetInstance()->WriteFormat(_T("CWinHttpRequest::Send = %d"), m_lReadyState);

	}
	catch (COleDispatchException * e) 
	{
		//CIBALog::GetInstance()->WriteFormat(_T("DealWithException(e)"));
		DealWithException(e);
	}
}

LONG CWinHttpRequest::GetStatus()
{
	long result;
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

BOOL CWinHttpRequest::WaitForResponse(LONG Timeout)
{
	BOOL result = FALSE;

	try
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xf, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Timeout);
		
		if (result)
		{
			m_lReadyState = 4;
		}	
	}
	catch (COleDispatchException * e) 
	{
		DealWithException(e);
	}

	return result;
}

void CWinHttpRequest::Abort()
{
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CWinHttpRequest::SetTimeouts(LONG ResolveTimeout, LONG ConnectTimeout, LONG SendTimeout, LONG ReceiveTimeout)
{
	static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ResolveTimeout, ConnectTimeout, SendTimeout, ReceiveTimeout);
}

void CWinHttpRequest::SetOption(LONG Option, VARIANT newValue)
{
	static BYTE parms[] = VTS_I4 VTS_VARIANT ;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Option, &newValue);
}

VARIANT CWinHttpRequest::GetOption(LONG Option)
{
	VARIANT result;
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, parms, Option);
	return result;
}

VARIANT CWinHttpRequest::GetResponseBody()
{
	VARIANT result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
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

BOOL CWinHttpRequest::DownloadFile(LPCTSTR lpszURL, LPCTSTR lpszFileName)
{
	BOOL bRet = FALSE;

	Open(_T("GET"), lpszURL, FALSE);
	Send(_T(""));

	if (GetReadyState() != 4)
	{
		return FALSE;
	}

	CString strResp = GetResponseText();

	if (0 == strResp.Left(6).CompareNoCase(_T("<html>"))) //正在升级
	{
		WriteLog(_T("Center is being upgraded!"));

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

