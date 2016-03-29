#include "stdafx.h"
#include ".\deregistermethod.h"

#include "..\Encrypt.h"
#include "..\IBAHelpper.h"
#include "LastTranInfo.h"

using namespace NS_SERVLET;

CDeregisterMethod::CDeregisterMethod(void)
: CServletMethod(_T("Deregister"))
{
	AddParams();
	AddRespFields();
}

CDeregisterMethod::~CDeregisterMethod(void)
{
}

void CDeregisterMethod::AddParams()
{
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("cardId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("personalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("openNetbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("lastTimeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("Lastpay"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("lastRefNo"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("endUsingTime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("returnMode"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CDeregisterMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

UINT CDeregisterMethod::GetMemberId()
{
	return _ttoi(m_Params.GetValue(_T("memberId")));
}

void CDeregisterMethod::SetCardId(UINT newVal)
{
	m_Params.SetValue(_T("cardId"), newVal);
}

void CDeregisterMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CDeregisterMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("personalId"), newVal);
}

void CDeregisterMethod::SetOpenNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("openNetbarId"), newVal);
}

void CDeregisterMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CDeregisterMethod::SetPassword(CString newVal)
{
	CString strTmp;

	if (newVal != _T("PASSWORDPASSWORD"))
	{
		CIBAHelpper::FillString(newVal, 8, '0');

		CEncrypt::CalcMac(newVal, strTmp, m_strPSK, FALSE);
	}
	else
	{
		strTmp = newVal;
	}

	m_Params.SetValue(_T("password"), strTmp);
}

void CDeregisterMethod::SetLastTimeId(UINT newVal)
{
	m_Params.SetValue(_T("lastTimeId"), newVal);
}

void CDeregisterMethod::SetLastpay(UINT newVal)
{
	m_Params.SetValue(_T("Lastpay"), newVal);
}

void CDeregisterMethod::SetLastRefNo(UINT newVal)
{
	m_Params.SetValue(_T("lastRefNo"), newVal);
}

void CDeregisterMethod::SetEndUsingTime(CString newVal)
{
	m_Params.SetValue(_T("endUsingTime"), newVal);
}

void CDeregisterMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CDeregisterMethod::SetReturnMode(UINT newVal)
{
	m_Params.SetValue(_T("returnMode"), newVal);
}

UINT CDeregisterMethod::GetReturnMode()
{
	return _ttoi(m_Params.GetValue(_T("returnMode")));
}

void CDeregisterMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CDeregisterMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CDeregisterMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("guarantyMoney"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("tranTime"), EPT_STRING, 14);
}

UINT CDeregisterMethod::GetGuarantyMoney()
{
	return m_RespFields.GetValueAsUINT(_T("guarantyMoney"));
}

INT CDeregisterMethod::GetBalance()
{
	return m_RespFields.GetValueAsINT(_T("balance"));
}

UINT CDeregisterMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

CString CDeregisterMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}

CString CDeregisterMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CDeregisterMethod::SaveTranInfo()
{
	if (GetStatusCode() == 0)
	{
		CLastTranInfo TranInfo;
		TranInfo.SetLastRefNo(GetRefNo());
		TranInfo.SetLastTranTime(GetTranTime());
		TranInfo.SetLastTranType(3);
		TranInfo.Save();
	}
}