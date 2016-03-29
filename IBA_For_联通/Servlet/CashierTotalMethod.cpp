#include "stdafx.h"
#include "CashierTotalMethod.h"

using namespace NS_SERVLET;

CCashierTotalMethod::CCashierTotalMethod(void)
: CServletMethod(_T("CashierTotal")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCashierTotalMethod::~CCashierTotalMethod(void)
{
}

void CCashierTotalMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("beginTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("endTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
}

void CCashierTotalMethod::SetBeginTime(CString newVal)
{
	m_Params.SetValue(_T("beginTime"), newVal);
}

void CCashierTotalMethod::SetEndTime(CString newVal)
{
	m_Params.SetValue(_T("endTime"), newVal);
}

void CCashierTotalMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CCashierTotalMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("CardCount"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("Amount"), EPT_NUMBER, 12);
}

UINT CCashierTotalMethod::GetCardCount()
{
	return m_RespFields.GetValueAsUINT(_T("CardCount"));
}

UINT CCashierTotalMethod::GetAmount()
{
	return m_RespFields.GetValueAsUINT(_T("Amount"));
}

