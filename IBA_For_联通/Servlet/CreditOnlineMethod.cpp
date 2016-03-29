#include "stdafx.h"
#include ".\creditonlinemethod.h"

#include "LastTranInfo.h"

using namespace NS_SERVLET;

CCreditOnlineMethod::CCreditOnlineMethod(void)
: CServletMethod(_T("CreditOnline"))
{
	AddParams();
	AddRespFields();
}

CCreditOnlineMethod::~CCreditOnlineMethod(void)
{
}

void CCreditOnlineMethod::AddParams()
{
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("amount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("noPresent"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("termTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("termId"), EPT_STRING, 8);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCreditOnlineMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CCreditOnlineMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCreditOnlineMethod::SetAmount(UINT newVal)
{
	m_Params.SetValue(_T("amount"), newVal);
}

void CCreditOnlineMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCreditOnlineMethod::SetNoPresent(UINT newVal)
{
	m_Params.SetValue(_T("noPresent"), newVal);
}

void CCreditOnlineMethod::SetTermTime(CString newVal)
{
	m_Params.SetValue(_T("termTime"), newVal);
}

void CCreditOnlineMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCreditOnlineMethod::SetTermId(CString newVal)
{
	m_Params.SetValue(_T("termId"), newVal);
}

void CCreditOnlineMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCreditOnlineMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("availableBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("usePresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("tranTime"), EPT_STRING, 14);
}

UINT CCreditOnlineMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CCreditOnlineMethod::GetBalance()
{
	return m_RespFields.GetValueAsUINT(_T("balance"));
}

UINT CCreditOnlineMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsUINT(_T("creditPresent"));
}

UINT CCreditOnlineMethod::GetAvailableBalance()
{
	return m_RespFields.GetValueAsUINT(_T("availableBalance"));
}

UINT CCreditOnlineMethod::GetUsePresent()
{
	return m_RespFields.GetValueAsUINT(_T("usePresent"));
}

CString CCreditOnlineMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}

void CCreditOnlineMethod::SaveTranInfo()
{
	if (GetStatusCode() == 0)
	{
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(GetRefNo());
		TranInfo.SetLastTranTime(GetTranTime());
		TranInfo.SetLastTranType(2);
		TranInfo.Save();
	}
}
