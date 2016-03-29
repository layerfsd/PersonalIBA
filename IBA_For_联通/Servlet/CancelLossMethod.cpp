#include "stdafx.h"
#include "CancelLossMethod.h"

#include "..\Encrypt.h"
#include "..\IBAHelpper.h"

using namespace NS_SERVLET;

CCancelLossMethod::CCancelLossMethod(void)
: CServletMethod(_T("CancelLoss")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCancelLossMethod::~CCancelLossMethod(void)
{
}

CString CCancelLossMethod::GetMethodName()
{
	CString strTmp;

	strTmp = m_strMethodName;

	if (GetCardType() == ECT_ID)
	{
		strTmp.Insert(0, _T("ID"));
	}

	return strTmp;
}

void CCancelLossMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("MemberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("NetBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("PersonalId"), EPT_STRING, 18);
	m_Params.AddParam(_T("Password"), EPT_STRING, 16);
	//m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CCancelLossMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("MemberId"), newVal);
}

void CCancelLossMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("NetBarId"), newVal);
}

void CCancelLossMethod::SetPersonalId(CString newVal)
{
	m_Params.SetValue(_T("PersonalId"), newVal);
}

void CCancelLossMethod::SetPassword(CString newVal)
{
	CString strTmp;

	CIBAHelpper::FillString(newVal, 8, '0');

	BOOL bDes = TRUE;

	if (GetCardType() == ECT_ID)
	{
		bDes = FALSE;
	}

	CEncrypt::CalcMac(newVal, strTmp, m_strPSK, bDes);

	m_Params.SetValue(_T("password"), strTmp);
}

//void CCancelLossMethod::SetOperator(CString newVal)
//{
//	m_Params.SetValue(_T("Operator"), newVal);
//}

void CCancelLossMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CCancelLossMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CCancelLossMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

}

