#include "stdafx.h"
#include "LocalConsumeMethod.h"

using namespace NS_SERVLET;

CLocalConsumeMethod::CLocalConsumeMethod(void)
: CServletMethod(_T("LocalConsume")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CLocalConsumeMethod::~CLocalConsumeMethod(void)
{
}

void CLocalConsumeMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("classId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("serialNum"), EPT_STRING, 20);
	m_Params.AddParam(_T("userName"), EPT_STRING, 10);
	m_Params.AddParam(_T("idNumber"), EPT_STRING, 18);
	m_Params.AddParam(_T("creditAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("consumeAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("returnAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("checkinTime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("checkoutTime"), EPT_NUMBER, 14);
	m_Params.AddParam(_T("termId"), EPT_STRING, 8);
	m_Params.AddParam(_T("pcClass"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("payType"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("dataStatus"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CLocalConsumeMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CLocalConsumeMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CLocalConsumeMethod::SetClassId(UINT newVal)
{
	m_Params.SetValue(_T("classId"), newVal);
}

void CLocalConsumeMethod::SetSerialNum(CString newVal)
{
	m_Params.SetValue(_T("serialNum"), newVal);
}

void CLocalConsumeMethod::SetUserName(CString newVal)
{
	m_Params.SetValue(_T("userName"), newVal);
}

void CLocalConsumeMethod::SetIdNumber(CString newVal)
{
	m_Params.SetValue(_T("idNumber"), newVal);
}

void CLocalConsumeMethod::SetCreditAmount(UINT newVal)
{
	m_Params.SetValue(_T("creditAmount"), newVal);
}

void CLocalConsumeMethod::SetConsumeAmount(UINT newVal)
{
	m_Params.SetValue(_T("consumeAmount"), newVal);
}

void CLocalConsumeMethod::SetReturnAmount(UINT newVal)
{
	m_Params.SetValue(_T("returnAmount"), newVal);
}

void CLocalConsumeMethod::SetCheckinTime(CString newVal)
{
	m_Params.SetValue(_T("checkinTime"), newVal);
}

void CLocalConsumeMethod::SetCheckoutTime(CString newVal)
{
	m_Params.SetValue(_T("checkoutTime"), newVal);
}

void CLocalConsumeMethod::SetTermId(CString newVal)
{
	m_Params.SetValue(_T("termId"), newVal);
}

void CLocalConsumeMethod::SetPcClass(UINT newVal)
{
	m_Params.SetValue(_T("pcClass"), newVal);
}

void CLocalConsumeMethod::SetPayType(UINT newVal)
{
	m_Params.SetValue(_T("payType"), newVal);
}

void CLocalConsumeMethod::SetDataStatus(UINT newVal)
{
	m_Params.SetValue(_T("dataStatus"), newVal);
}

void CLocalConsumeMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CLocalConsumeMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CLocalConsumeMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CLocalConsumeMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNO"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("tranTime"), EPT_NUMBER, 14);
	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
}

UINT CLocalConsumeMethod::GetRefNO()
{
	return m_RespFields.GetValueAsUINT(_T("refNO"));
}

CString CLocalConsumeMethod::GetTranTime()
{
	return m_RespFields.GetValueAsString("tranTime");
}
UINT CLocalConsumeMethod::GetMemberID()
{
	return m_RespFields.GetValueAsUINT("memberId");
}

