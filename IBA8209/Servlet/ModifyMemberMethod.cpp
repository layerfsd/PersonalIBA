#include "stdafx.h"
#include "ModifyMemberMethod.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CModifyMemberMethod::CModifyMemberMethod(void)
: CServletMethod(_T("ModifyMember")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CModifyMemberMethod::~CModifyMemberMethod(void)
{
}

CString CModifyMemberMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CModifyMemberMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("Password"), EPT_STRING, 16);
	m_Params.AddParam(_T("oldPersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("username"), EPT_STRING, 20);
	m_Params.AddParam(_T("sex"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("idType"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("personalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("idDepart"), EPT_STRING, 30);
	m_Params.AddParam(_T("countryId"), EPT_STRING, 2);
	m_Params.AddParam(_T("Address"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Email"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Tel"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Job"), EPT_STRING, 2, EPLT_VARIABLE);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("openNetBarId"), EPT_NUMBER, 5);
}

void CModifyMemberMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CModifyMemberMethod::SetPassword(CString newVal)
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

void CModifyMemberMethod::SetOldPersonalId(CString newVal)
{
	m_Params.SetValue(_T("oldPersonalId"), newVal);
}

void CModifyMemberMethod::SetUsername(CString newVal)
{
	m_Params.SetValue(_T("username"), newVal);
}

void CModifyMemberMethod::SetSex(UINT newVal)
{
	m_Params.SetValue(_T("sex"), newVal);
}

void CModifyMemberMethod::SetIdType(UINT newVal)
{
	m_Params.SetValue(_T("idType"), newVal);
}

void CModifyMemberMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("personalId"), newVal);
}

void CModifyMemberMethod::SetIdDepart(CString newVal)
{
	m_Params.SetValue(_T("idDepart"), newVal);
}

void CModifyMemberMethod::SetCountryId(CString newVal)
{
	m_Params.SetValue(_T("countryId"), newVal);
}

void CModifyMemberMethod::SetAddress(CString newVal)
{
	m_Params.SetValue(_T("Address"), newVal);
}

void CModifyMemberMethod::SetEmail(CString newVal)
{
	m_Params.SetValue(_T("Email"), newVal);
}

void CModifyMemberMethod::SetTel(CString newVal)
{
	m_Params.SetValue(_T("Tel"), newVal);
}

void CModifyMemberMethod::SetJob(CString newVal)
{
	m_Params.SetValue(_T("Job"), newVal);
}

void CModifyMemberMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CModifyMemberMethod::SetOpenNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("openNetBarId"), newVal);
}

void CModifyMemberMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}


