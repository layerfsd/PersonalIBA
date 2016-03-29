#include "stdafx.h"
#include "..\Encrypt.h"
#include "MergeMemberMethod.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CMergeMemberMethod::CMergeMemberMethod(void)
: CServletMethod(_T("IDMergeMember")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CMergeMemberMethod::~CMergeMemberMethod(void)
{
}

void CMergeMemberMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("destMemberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("Password"), EPT_STRING, 16);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CMergeMemberMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CMergeMemberMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CMergeMemberMethod::SetDestMemberId(UINT newVal)
{
	m_Params.SetValue(_T("destMemberId"), newVal);
}

void CMergeMemberMethod::SetPassword(CString newVal)
{
	CString strTmp;

	if (newVal != _T("PASSWORDPASSWORD"))
	{
		CIBAHelpper::FillString(newVal, 8, '0');

		CEncrypt::CalcMac(newVal, strTmp, m_strPSK, FALSE);
	}
	else
	{
		strTmp = newVal;
	}

	m_Params.SetValue(_T("password"), strTmp);
}

void CMergeMemberMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CMergeMemberMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CMergeMemberMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CMergeMemberMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();
}

