#include "stdafx.h"
#include "CreditDirectMethod.h"

using namespace NS_SERVLET;

CCreditDirectMethod::CCreditDirectMethod(void)
: CServletMethod(_T("CreditDirect")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCreditDirectMethod::~CCreditDirectMethod(void)
{
}

void CCreditDirectMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！

	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);

	m_Params.AddParam(_T("accountType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("creditMode"), EPT_NUMBER, 1);

	m_Params.AddParam(_T("amount"), EPT_STRING, 6);
	m_Params.AddParam(_T("termTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCreditDirectMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CCreditDirectMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCreditDirectMethod::SetAccountType(UINT newVal)
{
	m_Params.SetValue(_T("accountType"), newVal);
}

void CCreditDirectMethod::SetCreditMode(INT newVal)
{
	m_Params.SetValue(_T("creditMode"), newVal);
}

void CCreditDirectMethod::SetAmount(INT newVal)
{
	m_Params.SetValue(_T("amount"), newVal);
}

void CCreditDirectMethod::SetTermTime(CString newVal)
{
	m_Params.SetValue(_T("termTime"), newVal);
}

void CCreditDirectMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CCreditDirectMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCreditDirectMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCreditDirectMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("availableBalance"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("tranTime"), EPT_NUMBER, 14);
}

INT CCreditDirectMethod::GetAvailableBalance()
{
	return m_RespFields.GetValueAsINT(_T("availableBalance"));
}

CString CCreditDirectMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString(_T("tranTime"));
}


