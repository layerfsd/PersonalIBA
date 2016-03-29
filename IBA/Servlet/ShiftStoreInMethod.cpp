
#include "stdafx.h"
#include ".\ShiftStoreInMethod.h"


using namespace NS_SERVLET;

CShiftStoreInMethod::CShiftStoreInMethod(void)
: CServletMethod(_T("ShiftStoreIn"))
{
	AddParams();
	AddRespFields();
}

CShiftStoreInMethod::~CShiftStoreInMethod(void)
{
}

void CShiftStoreInMethod::AddParams()
{
	CServletMethod::AddParams();
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
}

void CShiftStoreInMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CShiftStoreInMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CShiftStoreInMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CShiftStoreInMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("ondutyTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("storeInCount"), EPT_NUMBER, 5);
	
}

CShiftStoreInRes& CShiftStoreInMethod::GetStoreRecord(INT nIndex)
{
	return m_arrayRes.GetAt(nIndex);
}

CString CShiftStoreInMethod::GetOndutyTime()
{
	return m_RespFields.GetValueAsString(_T("ondutyTime"));
}

BOOL CShiftStoreInMethod::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetRecordCount();

	m_arrayRes.RemoveAll();

	if (bRet && nCount > 0)
	{
		m_arrayRes.SetSize(nCount);

		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_arrayRes.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_arrayRes.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_arrayRes.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

UINT CShiftStoreInMethod::GetRecordCount()
{
	UINT nVal = m_RespFields.GetValueAsUINT(_T("storeInCount"));
	IBA_CHECK_UINT(nVal);
	return nVal;
}
