#include "stdafx.h"
#include "CancelBundleTimeMethod.h"

using namespace NS_SERVLET;

CCancelBundleTimeMethod::CCancelBundleTimeMethod(void)
: CServletMethod(_T("CancelBundleTime")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCancelBundleTimeMethod::~CCancelBundleTimeMethod(void)
{
}

void CCancelBundleTimeMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
}

void CCancelBundleTimeMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CCancelBundleTimeMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CCancelBundleTimeMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CCancelBundleTimeMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

