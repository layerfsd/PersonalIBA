#include "stdafx.h"
#include "BundleTimeMethod.h"


using namespace NS_SERVLET;

CBundleTimeMethod::CBundleTimeMethod(void)
: CServletMethod(_T("NewBundleTime")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBundleTimeMethod::~CBundleTimeMethod(void)
{
}

void CBundleTimeMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netbarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("reserve"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("timeId"), EPT_NUMBER, 4);
	m_Params.AddParam(_T("pcClass"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("beginTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("timePass"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("bundleAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("accountType"), EPT_NUMBER, 1);

	m_Params.AddParam(_T("creditAmount"), EPT_NUMBER, 6);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("cashRegisterId"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("dutyId"), EPT_NUMBER, 12);

}

void CBundleTimeMethod::SetBeginTime(CString newVal)
{
	m_Params.SetValue(_T("beginTime"), newVal);
}

void CBundleTimeMethod::SetTimePass(UINT newVal)
{
	m_Params.SetValue(_T("timePass"), newVal);
}

void CBundleTimeMethod::SetBundleAmount(UINT newVal)
{
	m_Params.SetValue(_T("bundleAmount"), newVal);
}

void CBundleTimeMethod::SetAccountType(UINT newVal)
{
	m_Params.SetValue(_T("accountType"), newVal);
}

void CBundleTimeMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CBundleTimeMethod::SetCreditAmount(UINT newVal)
{
	m_Params.SetValue(_T("creditAmount"), newVal);
}

void CBundleTimeMethod::SetNetbarId(UINT newVal)
{
	m_Params.SetValue(_T("netbarId"), newVal);
}

void CBundleTimeMethod::SetDutyId(UINT newVal)
{
	m_Params.SetValue(_T("dutyId"), newVal);
}

void CBundleTimeMethod::SetTimeId(UINT newVal)
{
	m_Params.SetValue(_T("timeId"), newVal);
}

void CBundleTimeMethod::SetPcClass(UINT newVal)
{
	m_Params.SetValue(_T("pcClass"), newVal);
}

void CBundleTimeMethod::SetCashRegisterId(UINT newVal)
{
	m_Params.SetValue(_T("cashRegisterId"), newVal);
}

void CBundleTimeMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CBundleTimeMethod::SetReserve(UINT newVal)
{
	m_Params.SetValue(_T("reserve"), newVal);
}

void CBundleTimeMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("startTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("endTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("amount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("timeId"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("needRelogin"), EPT_NUMBER, 1);
}

UINT CBundleTimeMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

CString CBundleTimeMethod::GetStartTime()
{
	return m_RespFields.GetValueAsString(_T("startTime"));
}

CString CBundleTimeMethod::GetEndTime()
{
	return m_RespFields.GetValueAsString(_T("endTime"));
}

UINT CBundleTimeMethod::GetAmount()
{
	return m_RespFields.GetValueAsUINT(_T("amount"));
}

UINT CBundleTimeMethod::GetTimeId()
{
	return m_RespFields.GetValueAsUINT(_T("timeId"));
}

BOOL CBundleTimeMethod::GetNeedRelogin()
{
	return m_RespFields.GetValueAsINT(_T("needRelogin"));
}
