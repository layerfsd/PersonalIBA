#include "stdafx.h"
#include "DeclareLossMethod.h"
#include "..\Encrypt.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CDeclareLossMethod::CDeclareLossMethod(void)
: CServletMethod(_T("DeclareLoss")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CDeclareLossMethod::~CDeclareLossMethod(void)
{
}

CString CDeclareLossMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}
void CDeclareLossMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("PersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("openNetbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("memberName"), EPT_STRING, 20);
	m_Params.AddParam(_T("password"), EPT_STRING, 16);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);
}

void CDeclareLossMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CDeclareLossMethod::SetOpenNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("openNetbarId"), newVal);
}

void CDeclareLossMethod::SetMemberName(CString newVal)
{
	m_Params.SetValue(_T("memberName"), newVal);
}

void CDeclareLossMethod::SetPassword(CString newVal)
{
	CString strTmp;

	CIBAHelpper::FillString(newVal, 8, '0');

	CEncrypt::CalcMac(newVal, strTmp, m_strPSK, FALSE);

	m_Params.SetValue(_T("password"), strTmp);
}

void CDeclareLossMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CDeclareLossMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CDeclareLossMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CDeclareLossMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
}

UINT CDeclareLossMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

