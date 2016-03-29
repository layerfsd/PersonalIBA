#include "stdafx.h"
#include "TermConsumeMethod.h"

using namespace NS_SERVLET;

CTermConsumeMethod::CTermConsumeMethod(void)
: CServletMethod(_T("TermConsume")) //中心方法名称
{
	m_bUseQueryHost = TRUE; //使用查询机
	m_nRevTimeout = 120;	//超时2分钟

	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CTermConsumeMethod::~CTermConsumeMethod(void)
{
}

void CTermConsumeMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("termed"), EPT_STRING, 8);
	m_Params.AddParam(_T("beginTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("endTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
}

void CTermConsumeMethod::SetTermed(CString newVal)
{
	m_Params.SetValue(_T("termed"), newVal);
}

void CTermConsumeMethod::SetBeginTime(CString newVal)
{
	m_Params.SetValue(_T("beginTime"), newVal);
}

void CTermConsumeMethod::SetEndTime(CString newVal)
{
	m_Params.SetValue(_T("endTime"), newVal);
}

void CTermConsumeMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CTermConsumeMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 4);

}

BOOL CTermConsumeMethod::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetRecordCount();

	m_TermConsumeList.RemoveAll();

	if (bRet && nCount > 0)
	{
		m_TermConsumeList.SetSize(nCount);

		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_TermConsumeList.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_TermConsumeList.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_TermConsumeList.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

UINT CTermConsumeMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

UINT CTermConsumeMethod::GetMemberId(INT nIndex)
{
	return m_TermConsumeList.GetAt(nIndex).GetMemberId();
}

CString CTermConsumeMethod::GetBeginTime(INT nIndex)
{
	return m_TermConsumeList.GetAt(nIndex).GetBeginTime();
}

CString CTermConsumeMethod::GetEndTime(INT nIndex)
{
	return m_TermConsumeList.GetAt(nIndex).GetEndTime();
}

UINT CTermConsumeMethod::GetAmount(INT nIndex)
{
	return m_TermConsumeList.GetAt(nIndex).GetAmount();
}


//******************************************************************************

CTermConsumeInfo::CTermConsumeInfo(void)
{
	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("beginTime"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("endTime"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("amount"), EPT_NUMBER, 6);
}

UINT CTermConsumeInfo::GetMemberId()
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

CString CTermConsumeInfo::GetBeginTime()
{
	return m_RespFields.GetValueAsString(_T("beginTime"));
}

CString CTermConsumeInfo::GetEndTime()
{
	return m_RespFields.GetValueAsString(_T("endTime"));
}

UINT CTermConsumeInfo::GetAmount()
{
	return m_RespFields.GetValueAsUINT(_T("amount"));
}