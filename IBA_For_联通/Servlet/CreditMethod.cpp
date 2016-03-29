#include "stdafx.h"
#include ".\creditmethod.h"
#include "LastTranInfo.h"


using namespace NS_SERVLET;

CCreditMethod::CCreditMethod(void)
: CServletMethod(_T("Credit"))
{
	AddParams();
	AddRespFields();
}

CCreditMethod::~CCreditMethod(void)
{
}

void CCreditMethod::AddParams()
{
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("amount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("noPresent"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("termTime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("activityId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCreditMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

UINT CCreditMethod::GetMemberId()
{
	return _ttoi(m_Params.GetValue(_T("memberId")));
}

void CCreditMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCreditMethod::SetAmount(UINT newVal)
{
	m_Params.SetValue(_T("amount"), newVal);
}

UINT CCreditMethod::GetAmount()
{
	return _ttoi(m_Params.GetValue(_T("amount")));
}

void CCreditMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCreditMethod::SetNoPresent(UINT newVal)
{
	m_Params.SetValue(_T("noPresent"), newVal);
}

void CCreditMethod::SetTermTime(CString newVal)
{
	m_Params.SetValue(_T("termTime"), newVal);
}

void CCreditMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCreditMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCreditMethod::SetActivityId(UINT newVal)
{
	m_Params.SetValue(_T("activityId"), newVal);
}

void CCreditMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("availableBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("usePresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Trantime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("increasedPoint"), EPT_NUMBER, 6);
}

UINT CCreditMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CCreditMethod::GetBalance()
{
	return m_RespFields.GetValueAsUINT(_T("balance"));
}

UINT CCreditMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsUINT(_T("creditPresent"));
}

UINT CCreditMethod::GetAvailableBalance()
{
	return m_RespFields.GetValueAsUINT(_T("availableBalance"));
}

UINT CCreditMethod::GetUsePresent()
{
	return m_RespFields.GetValueAsUINT(_T("usePresent"));
}

CString CCreditMethod::GetTrantime()
{
	return m_RespFields.GetValueAsString(_T("Trantime"));
}

UINT CCreditMethod::GetIncreasedPoint()
{
	return m_RespFields.GetValueAsUINT(_T("increasedPoint"));
}

CString CCreditMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CCreditMethod::SaveTranInfo()
{
	if (GetStatusCode() == 0)
	{
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(GetRefNo());
		TranInfo.SetLastTranTime(GetTrantime());
		TranInfo.SetLastTranType(2);
		TranInfo.Save();
	}
}
