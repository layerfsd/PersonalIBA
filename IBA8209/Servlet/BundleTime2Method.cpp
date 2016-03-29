#include "stdafx.h"
#include "BundleTime2Method.h"

using namespace NS_SERVLET;

CBundleTime2Method::CBundleTime2Method(void)
: CServletMethod(_T("BundleTime2")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBundleTime2Method::~CBundleTime2Method(void)
{
}

void CBundleTime2Method::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("timeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("termId"), EPT_STRING, 8);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CBundleTime2Method::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CBundleTime2Method::SetNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("netbarId"), newVal);
}

void CBundleTime2Method::SetTimeId(UINT newVal)
{
	m_Params.SetValue(_T("timeId"), newVal);
}

void CBundleTime2Method::SetTermId(CString newVal)
{
	m_Params.SetValue(_T("termId"), newVal);
}

void CBundleTime2Method::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CBundleTime2Method::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("amount"), EPT_NUMBER, 6);
}

UINT CBundleTime2Method::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CBundleTime2Method::GetAmount()
{
	return m_RespFields.GetValueAsUINT(_T("amount"));
}

