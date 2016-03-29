#include "stdafx.h"
#include "CreditActivityPolicyMethod.h"

using namespace NS_SERVLET;

CCreditActivityPolicyMethod::CCreditActivityPolicyMethod(void)
: CServletMethod(_T("CreditActivityPolicy")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CCreditActivityPolicyMethod::~CCreditActivityPolicyMethod(void)
{
}

void CCreditActivityPolicyMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
}

void CCreditActivityPolicyMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CCreditActivityPolicyMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("activityId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("activityName"), EPT_STRING, 30);
	m_RespFields.AddRespField(_T("activityType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("classId"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("limitAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("displayAmount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("startTime"), EPT_STRING, 14);
	m_RespFields.AddRespField(_T("stopTime"), EPT_STRING, 14);
}

UINT CCreditActivityPolicyMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

UINT CCreditActivityPolicyMethod::GetActivityId()
{
	return m_RespFields.GetValueAsUINT(_T("activityId"));
}

CString CCreditActivityPolicyMethod::GetActivityName()
{
	return m_RespFields.GetValueAsString(_T("activityName"));
}

UINT CCreditActivityPolicyMethod::GetActivityType()
{
	return m_RespFields.GetValueAsUINT(_T("activityType"));
}

UINT CCreditActivityPolicyMethod::GetClassId()
{
	return m_RespFields.GetValueAsUINT(_T("classId"));
}

UINT CCreditActivityPolicyMethod::GetLimitAmount()
{
	return m_RespFields.GetValueAsUINT(_T("limitAmount"));
}

UINT CCreditActivityPolicyMethod::GetDisplayAmount()
{
	return m_RespFields.GetValueAsUINT(_T("displayAmount"));
}

CString CCreditActivityPolicyMethod::GetStartTime()
{
	return m_RespFields.GetValueAsString(_T("startTime"));
}

CString CCreditActivityPolicyMethod::GetStopTime()
{
	return m_RespFields.GetValueAsString(_T("stopTime"));
}

