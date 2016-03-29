#include "stdafx.h"
#include "ResetPwdMethod.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CResetPwdMethod::CResetPwdMethod(void)
: CServletMethod(_T("ResetPwd")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CResetPwdMethod::~CResetPwdMethod(void)
{
}

void CResetPwdMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	CServletMethod::AddParams();

	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("personalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);

}

void CResetPwdMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CResetPwdMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CResetPwdMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("personalId"), newVal);
}

void CResetPwdMethod::SetPassword(CString newVal)
{
	CString strTmp;

	CIBAHelpper::FillString(newVal, 8, '0');

	CEncrypt::CalcMac(newVal, strTmp, m_strPSK, FALSE);
	//CEncrypt::CalcMac(newVal, strTmp, _T("944FF0E934B33DCC"), FALSE);
	

	m_Params.SetValue(_T("password"), strTmp);
}

void CResetPwdMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CResetPwdMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

