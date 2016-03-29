#include "stdafx.h"
#include "AuditMethod.h"

using namespace NS_SERVLET;

CAuditMethod::CAuditMethod(void)
: CServletMethod(_T("Audit")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CAuditMethod::~CAuditMethod(void)
{
}

CString CAuditMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CAuditMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("MemberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("NetBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("AuditType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("AuditId"), EPT_STRING, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CAuditMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("MemberId"), newVal);
}

void CAuditMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("NetBarId"), newVal);
}

void CAuditMethod::SetAuditType(UINT newVal)
{
	m_Params.SetValue(_T("AuditType"), newVal);
}

void CAuditMethod::SetAuditId(CString newVal)
{
	m_Params.SetValue(_T("AuditId"), newVal);
}

void CAuditMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

