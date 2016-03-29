#include "stdafx.h"
#include "IDCheckCardMethod.h"

using namespace NS_SERVLET;

CIDCheckCardMethod::CIDCheckCardMethod(void)
: CServletMethod(_T("IDCheckCard")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CIDCheckCardMethod::~CIDCheckCardMethod(void)
{
}

void CIDCheckCardMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("serialNum"), EPT_STRING, 20);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("serialNumType"), EPT_NUMBER, 1);
	
}

void CIDCheckCardMethod::SetSerialNum(CString newVal)
{
	m_Params.SetValue(_T("serialNum"), newVal);
}

void CIDCheckCardMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CIDCheckCardMethod::SetSerialNumType(UINT newVal)
{
	m_Params.SetValue(_T("serialNumType"), newVal);
}


BOOL CIDCheckCardMethod::VerifyResp(CString strResp)
{
	return TRUE;
}

void CIDCheckCardMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	m_RespFields.AddRespField(_T("MAC"), EPT_STRING, 16);
	m_RespFields.AddRespField(_T("serialNo"), EPT_NUMBER, 6);
	
	m_RespFields.AddRespField(_T("ErrCode"), EPT_NUMBER, 2);
	m_RespFields.SetValue(_T("ErrCode"), _T("-1"));

	m_RespFields.AddRespField(_T("Status"), EPT_NUMBER, 2);
	m_RespFields.SetValue(_T("Status"), _T("-1"));

	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("cardId"), EPT_NUMBER, 12);
}

UINT CIDCheckCardMethod::GetErrCode()
{
	return m_RespFields.GetValueAsUINT(_T("ErrCode"));
}

UINT CIDCheckCardMethod::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

CString CIDCheckCardMethod::GetMemberIdAsString()
{
	return m_RespFields.GetValueAsString(_T("memberId"));
}

UINT CIDCheckCardMethod::GetCardId()
{
	return m_RespFields.GetValueAsUINT(_T("cardId"));
}

CString CIDCheckCardMethod::GetCardIdAsString()
{
	return m_RespFields.GetValueAsString(_T("cardId"));
}