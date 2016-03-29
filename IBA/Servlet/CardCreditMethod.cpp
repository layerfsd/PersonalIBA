#include "stdafx.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"
#include "CardCreditMethod.h"

using namespace NS_SERVLET;

CCardCreditMethod::CCardCreditMethod(void)
: CServletMethod(_T("CardCredit")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCardCreditMethod::~CCardCreditMethod(void)
{
}

void CCardCreditMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("category"), EPT_STRING, 2);
	m_Params.AddParam(_T("cardSerial"), EPT_NUMBER, 8);
	m_Params.AddParam(_T("cardPwd"), EPT_STRING, 32);
	m_Params.AddParam(_T("amount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("businessType"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("name"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCardCreditMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CCardCreditMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCardCreditMethod::SetCategory(CString newVal)
{
	m_Params.SetValue(_T("category"), newVal.MakeUpper());
}

void CCardCreditMethod::SetCardSerial(CString newVal)
{
	m_Params.SetValue(_T("cardSerial"), newVal);
}

void CCardCreditMethod::SetCardPwd(CString newVal)
{
	CString strPwd;
	newVal.Trim();

	if (newVal.GetLength() > 8)
	{
		CString strTmp;

		strTmp = newVal.Left(8);
		CEncrypt::CalcMac(strTmp, strPwd, m_strPSK, FALSE);

		strTmp = newVal.Mid(8);
		CIBAHelpper::FillString(strTmp, 8, '\0');
		CEncrypt::CalcMac(strTmp, strTmp, m_strPSK, FALSE);
		strPwd.Append(strTmp);
	}
	else
	{
		CIBAHelpper::FillString(newVal, 8, '0');
		CEncrypt::CalcMac(newVal, strPwd, m_strPSK, FALSE);
	}

	m_Params.SetValue(_T("cardPwd"), strPwd);
}

void CCardCreditMethod::SetAmount(UINT newVal)
{
	m_Params.SetValue(_T("amount"), newVal);
}

void CCardCreditMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCardCreditMethod::SetBusinessType(UINT newVal)
{
	m_Params.SetValue(_T("businessType"), newVal);
}

void CCardCreditMethod::SetName(CString newVal)
{
	m_Params.SetValue(_T("name"), newVal);
}

void CCardCreditMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCardCreditMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCardCreditMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Balance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("creditPresent"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("availableBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Amount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("tranTime"), EPT_STRING, 14);
}

UINT CCardCreditMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CCardCreditMethod::GetBalance()
{
	return m_RespFields.GetValueAsUINT(_T("Balance"));
}

UINT CCardCreditMethod::GetCreditPresent()
{
	return m_RespFields.GetValueAsUINT(_T("creditPresent"));
}

UINT CCardCreditMethod::GetAvailableBalance()
{
	return m_RespFields.GetValueAsUINT(_T("availableBalance"));
}

UINT CCardCreditMethod::GetAmount()
{
	return m_RespFields.GetValueAsUINT(_T("Amount"));
}

CString CCardCreditMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}

