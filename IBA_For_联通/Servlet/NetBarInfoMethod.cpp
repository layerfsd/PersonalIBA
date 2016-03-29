#include "stdafx.h"
#include ".\netbarinfomethod.h"


using namespace NS_SERVLET;

CNetBarConfigMethod::CNetBarConfigMethod(void)
: CServletMethod(_T("NetBarInfo"))
{
	AddParams();
	AddRespFields();
}

CNetBarConfigMethod::~CNetBarConfigMethod(void)
{
}

void CNetBarConfigMethod::AddParams()
{
	CServletMethod::AddParams();

	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
}

void CNetBarConfigMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CNetBarConfigMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("netBarCount"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("netBarName"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("netBarCaption"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("ParentNetBar"), EPT_STRING, 3, FALSE);
	m_RespFields.AddRespField(_T("netBarType"), EPT_NUMBER, 1);
}

UINT CNetBarConfigMethod::GetNetBarCount()
{
	return m_RespFields.GetValueAsUINT(_T("netBarCount"));
}

UINT CNetBarConfigMethod::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

CString CNetBarConfigMethod::GetNetBarName()
{
	return m_RespFields.GetValueAsString(_T("netBarName"));
}

CString CNetBarConfigMethod::GetNetBarCaption()
{
	return m_RespFields.GetValueAsString(_T("netBarCaption"));
}

CString CNetBarConfigMethod::GetParentNetBar()
{
	return m_RespFields.GetValueAsString(_T("ParentNetBar"));
}

UINT CNetBarConfigMethod::GetNetBarType()
{
	return m_RespFields.GetValueAsUINT(_T("netBarType"));
}

