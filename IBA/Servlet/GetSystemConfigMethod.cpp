#include "StdAfx.h"
#include "GetSystemConfigMethod.h"

using namespace NS_SERVLET;

CGetSystemConfigMethod::CGetSystemConfigMethod(void)
: CServletMethod(_T("GetSystemConfig")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CGetSystemConfigMethod::~CGetSystemConfigMethod(void)
{

}

void CGetSystemConfigMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("stringField"), EPT_STRING, 50);	// 要获取的参数/字段
}

void CGetSystemConfigMethod::SetNetbarId(int nNetbarId)
{
	m_Params.SetValue(_T("netBarId"), nNetbarId);
}

void CGetSystemConfigMethod::SetStringField(CString newVal)
{
	m_Params.SetValue(_T("stringField"), newVal);
}

void CGetSystemConfigMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("value1"), EPT_STRING, 11);
	m_RespFields.AddRespField(_T("value2"), EPT_NUMBER, 11);
	m_RespFields.AddRespField(_T("stringField"), EPT_NUMBER, 100);
}

CString CGetSystemConfigMethod::GetStringFieldValue()
{
	return m_RespFields.GetValueAsString(_T("stringField"));
}

UINT CGetSystemConfigMethod::GetValue1()
{
	return m_RespFields.GetValueAsUINT(_T("value1"));
}

UINT CGetSystemConfigMethod::GetValue2()
{
	return m_RespFields.GetValueAsUINT(_T("value2"));
}


