#include "stdafx.h"
#include ".\creditreversemethod.h"

#include "LastTranInfo.h"
#include "..\Encrypt.h"

using namespace NS_SERVLET;

CCreditReverseMethod::CCreditReverseMethod(void)
: CServletMethod(_T("CreditReverse"))
{
	AddParams();
	AddRespFields();
}

CCreditReverseMethod::~CCreditReverseMethod(void)
{
}

void CCreditReverseMethod::AddParams()
{
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("trantime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("lastRefNo"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);

	m_Params.AddParam(_T("cashRegisterId"), EPT_STRING, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCreditReverseMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCreditReverseMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCreditReverseMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CCreditReverseMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCreditReverseMethod::SetTrantime(CString newVal)
{
	m_Params.SetValue(_T("trantime"), newVal);
}

void CCreditReverseMethod::SetLastRefNo(UINT newVal)
{
	m_Params.SetValue(_T("lastRefNo"), newVal);
}

void CCreditReverseMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCreditReverseMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("available"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("presentBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
}

INT CCreditReverseMethod::GetBalance()
{
	return m_RespFields.GetValueAsINT(_T("balance"));
}

INT CCreditReverseMethod::GetAvailable()
{
	return m_RespFields.GetValueAsINT(_T("available"));
}

INT CCreditReverseMethod::GetPresentBalance()
{
	return m_RespFields.GetValueAsINT(_T("presentBalance"));
}


CString CCreditReverseMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

INT NS_SERVLET::CCreditReverseMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsINT(_T("creditPresent"));
}