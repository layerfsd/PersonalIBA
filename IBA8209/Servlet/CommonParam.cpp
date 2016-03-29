#include "StdAfx.h"
#include "..\Servlet\CommonParam.h"

using namespace NS_SERVLET;

CCommonParam::CCommonParam(void)
: CServletMethod(_T("CommonParam")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCommonParam::~CCommonParam(void)
{

}

void CCommonParam::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("param"), EPT_STRING, 20);	// 要获取的参数/字段
}

void CCommonParam::SetNetbarId(int nNetbarId)
{
	m_Params.SetValue(_T("netBarId"), nNetbarId);
}

void CCommonParam::SetParam(CString newVal)
{
	m_Params.SetValue(_T("param"), newVal);
}

void CCommonParam::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("commonRequest"), EPT_STRING, 20);
}

CString CCommonParam::GetCommonRequest()
{
	return m_RespFields.GetValueAsString(_T("commonRequest"));
}

BOOL NS_SERVLET::CCommonParam::VerifyResp( CString strResp )
{
	return TRUE;
}
