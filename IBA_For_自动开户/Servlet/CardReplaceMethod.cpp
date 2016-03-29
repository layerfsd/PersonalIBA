#include "stdafx.h"
#include "CardReplaceMethod.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CCardReplaceMethod::CCardReplaceMethod(void)
: CServletMethod(_T("CardReplace")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCardReplaceMethod::~CCardReplaceMethod(void)
{
}

void CCardReplaceMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("cardId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("PersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("openNetbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("Lastpay"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("lastRefNo"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("endUsingTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
}

void CCardReplaceMethod::SetCardId(UINT newVal)
{
	m_Params.SetValue(_T("cardId"), newVal);
}

void CCardReplaceMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CCardReplaceMethod::SetOpenNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("openNetbarId"), newVal);
}

void CCardReplaceMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CCardReplaceMethod::SetPassword(CString newVal)
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

void CCardReplaceMethod::SetLastpay(UINT newVal)
{
	m_Params.SetValue(_T("Lastpay"), newVal);
}

void CCardReplaceMethod::SetLastRefNo(UINT newVal)
{
	m_Params.SetValue(_T("lastRefNo"), newVal);
}

void CCardReplaceMethod::SetEndUsingTime(CString newVal)
{
	m_Params.SetValue(_T("endUsingTime"), newVal);
}

void CCardReplaceMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCardReplaceMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCardReplaceMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCardReplaceMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("refNo1"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("refNo2"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("serialNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("tranTime"), EPT_STRING, 14);
}

UINT CCardReplaceMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

UINT CCardReplaceMethod::GetRefNo1()
{
	return m_RespFields.GetValueAsUINT(_T("refNo1"));
}

UINT CCardReplaceMethod::GetRefNo2()
{
	return m_RespFields.GetValueAsUINT(_T("refNo2"));
}

CString CCardReplaceMethod::GetSerialNum()
{
	return m_RespFields.GetValueAsString(_T("serialNum"));
}

CString CCardReplaceMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}

CString CCardReplaceMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}