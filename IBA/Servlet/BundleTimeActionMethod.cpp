#include "stdafx.h"
#include "BundleTimeActionMethod.h"


using namespace NS_SERVLET;

CBundleTimeActionMethod::CBundleTimeActionMethod(void)
: CServletMethod(_T("BundleTimeAction")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBundleTimeActionMethod::~CBundleTimeActionMethod(void)
{
}

void CBundleTimeActionMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CBundleTimeActionMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CBundleTimeActionMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CBundleTimeActionMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("BundleTimeSerialNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("payType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("pcClass"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("startTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("endTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("amount1"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("amount2"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("refNo"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("freezeMoney"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("timeId"), EPT_NUMBER, 4);
}

UINT CBundleTimeActionMethod::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

UINT CBundleTimeActionMethod::GetBundleTimeSerialNo()
{
	return m_RespFields.GetValueAsUINT(_T("serialNo"));
}

UINT CBundleTimeActionMethod::GetPayType()
{
	return m_RespFields.GetValueAsUINT(_T("payType"));
}

UINT CBundleTimeActionMethod::GetPcClass()
{
	return m_RespFields.GetValueAsUINT(_T("pcClass"));
}

CString CBundleTimeActionMethod::GetStartTime()
{
	return m_RespFields.GetValueAsString(_T("startTime"));
}

CString CBundleTimeActionMethod::GetEndTime()
{
	return m_RespFields.GetValueAsString(_T("endTime"));
}

UINT CBundleTimeActionMethod::GetAmount1()
{
	return m_RespFields.GetValueAsUINT(_T("amount1"));
}

UINT CBundleTimeActionMethod::GetAmount2()
{
	return m_RespFields.GetValueAsUINT(_T("amount2"));
}

UINT CBundleTimeActionMethod::GetRefNo()
{
	return m_RespFields.GetValueAsUINT(_T("refNo"));
}

UINT CBundleTimeActionMethod::GetFreezeMoney()
{
	return m_RespFields.GetValueAsUINT(_T("freezeMoney"));
}

UINT CBundleTimeActionMethod::GetTimeId()
{
	return m_RespFields.GetValueAsUINT(_T("timeId"));
}

