#include "stdafx.h"
#include "CardAssignMethod.h"

using namespace NS_SERVLET;

CCardAssignMethod::CCardAssignMethod(void)
: CServletMethod(_T("CardAssign")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCardAssignMethod::~CCardAssignMethod(void)
{
}

void CCardAssignMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("cardId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("costExpense"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("guarantyMoney"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("PersonalId"), EPT_NUMBER, 18);
	m_Params.AddParam(_T("openNetbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("hasCard"), EPT_NUMBER, 1);
}

void CCardAssignMethod::SetCardId(UINT newVal)
{
	m_Params.SetValue(_T("cardId"), newVal);
}

void CCardAssignMethod::SetCostExpense(UINT newVal)
{
	m_Params.SetValue(_T("costExpense"), newVal);
}

void CCardAssignMethod::SetGuarantyMoney(UINT newVal)
{
	m_Params.SetValue(_T("guarantyMoney"), newVal);
}

void CCardAssignMethod::SetPersonalId(UINT newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CCardAssignMethod::SetOpenNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("openNetbarId"), newVal);
}

void CCardAssignMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CCardAssignMethod::SetPassword(CString newVal)
{
	m_Params.SetValue(_T("password"), newVal);
}

void CCardAssignMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCardAssignMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCardAssignMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCardAssignMethod::SetHasCard(UINT newVal)
{
	m_Params.SetValue(_T("hasCard"), newVal);
}

void CCardAssignMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("serialNum"), EPT_STRING, 20);
	m_RespFields.AddRespField(_T("tranTime"), EPT_NUMBER, 14);
}

UINT CCardAssignMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

UINT CCardAssignMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

CString CCardAssignMethod::GetSerialNum()
{
	return m_RespFields.GetValueAsString(_T("serialNum"));
}

UINT CCardAssignMethod::GetTranTime()
{
	return m_RespFields.GetValueAsUINT(_T("tranTime"));
}

