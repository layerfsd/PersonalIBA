#include "stdafx.h"
#include "ActivationMethod.h"


using namespace NS_SERVLET;

CActivationMethod::CActivationMethod(void)
: CServletMethod(_T("Activation")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CActivationMethod::~CActivationMethod(void)
{
}

void CActivationMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("Mode"), EPT_NUMBER, 1);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CActivationMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CActivationMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CActivationMethod::SetMode(UINT newVal)
{
	m_Params.SetValue(_T("Mode"), newVal);
}

void CActivationMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

