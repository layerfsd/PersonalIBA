#include "stdafx.h"
#include "Commonmethod.h"
#include "..\IBAHelpper.h"
#include "..\IBA.h"
using namespace NS_SERVLET;

CStatusMessageMethod::CStatusMessageMethod(void)
: CServletMethod(_T("StatusMessage"))
, m_nStatusCode(0)
{
	AddParams();
	AddRespFields();
}

CStatusMessageMethod::~CStatusMessageMethod(void)
{
}

BOOL CStatusMessageMethod::Execute()
{
	BOOL bRet = TRUE;

	if (m_nStatusCode > 0)
	{
		bRet = CServletMethod::Execute();
	}

	return bRet;
}

void CStatusMessageMethod::AddParams()
{
	CServletMethod::AddParams();
	m_Params.AddParam(_T("statusCode"), EPT_NUMBER, 2);
}

void CStatusMessageMethod::AddRespFields()
{
	CServletMethod::AddRespFields();
	m_RespFields.AddRespField(_T("Message"), EPT_STRING, 2, FALSE);
}

CString CStatusMessageMethod::MakeMacString(CString strParams)
{
	return _T("");
}

BOOL CStatusMessageMethod::VerifyResp(CString strResp)
{
	return TRUE;
}

void CStatusMessageMethod::SetStatusCode(INT newVal)
{
	m_nStatusCode = newVal;
	m_Params.SetValue(_T("statusCode"), newVal);
}

CString CStatusMessageMethod::GetMessage()
{
	CIBAString strRet;

	if (m_nStatusCode < 0)
	{
		strRet.LoadString(IDS_NETFAILED);
	}
	else if (m_nStatusCode > 0)
	{
		strRet.LoadString(IDS_OPREATEFAILED);
		strRet = m_RespFields.GetValueAsString(_T("Message"));
	}
	else
	{
		strRet.LoadString(IDS_OPERATEOK);
	}

	return strRet;
}

///////////////////////////////////////////////////////////////////////////////

CGetTimeMethod::CGetTimeMethod(void)
: CServletMethod(_T("GetTime"))
{
	AddParams();
	AddRespFields();
}

CGetTimeMethod::~CGetTimeMethod(void)
{
}

void CGetTimeMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("serverTime"), EPT_STRING, 14);
}

CString CGetTimeMethod::MakeMacString(CString strParams)
{
	return _T("");
}

BOOL CGetTimeMethod::VerifyResp(CString strResp)
{
	return TRUE;
}

CString CGetTimeMethod::GetServerTimeAsString()
{
	return m_RespFields.GetValueAsString(_T("serverTime"));
}

BOOL CGetTimeMethod::SyncLocalTime()
{
	if (GetStatusCode() != 0)
	{
		return FALSE;
	}
	
	return CIBAHelpper::SyncLocalTime(CIBAHelpper::CenterTimeToOleDateTime(GetServerTimeAsString()));
}

