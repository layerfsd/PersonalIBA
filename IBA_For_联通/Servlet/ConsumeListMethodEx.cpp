#include "stdafx.h"
#include "ConsumeListMethodEx.h"

using namespace NS_SERVLET;

CConsumeInfoEx::CConsumeInfoEx()
{
	m_RespFields.AddRespField(_T("memberId"), EPT_NUMBER, 12);
	m_RespFields.AddRespField(_T("costTime"), EPT_NUMBER, 10);
	m_RespFields.AddRespField(_T("amount"), EPT_STRING, 6);
	m_RespFields.AddRespField(_T("businessName"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("description"), EPT_STRING, 3, FALSE);
}

UINT CConsumeInfoEx::GetMemberID() 
{
	return m_RespFields.GetValueAsUINT(_T("memberId"));
}

CString CConsumeInfoEx::GetCostTime() 
{
	return m_RespFields.GetValueAsString(_T("costTime"));
}

INT CConsumeInfoEx::GetAmount() 
{
	return m_RespFields.GetValueAsINT(_T("amount"));
}

CString CConsumeInfoEx::GetBusinessName() 
{
	return m_RespFields.GetValueAsString(_T("businessName"));
}

CString CConsumeInfoEx::GetDescription() 
{
	return m_RespFields.GetValueAsString(_T("description"));
}

//******************************************************************************

CConsumeListMethodEx::CConsumeListMethodEx(void)
: CServletMethod(_T("ThirdPartPayList")) //中心方法名称
{
#ifndef _DEBUG
	m_bUseQueryHost = TRUE; //使用查询机
	m_nRevTimeout = 120;	//超时2分钟
#endif // _DEBUG


	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CConsumeListMethodEx::~CConsumeListMethodEx(void)
{
}

void CConsumeListMethodEx::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
}

BOOL CConsumeListMethodEx::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetRecordCount();

	m_ConsumeInfoList.RemoveAll();

	if (bRet && nCount > 0)
	{
		m_ConsumeInfoList.SetSize(nCount);

		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_ConsumeInfoList.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_ConsumeInfoList.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_ConsumeInfoList.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}

void CConsumeListMethodEx::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CConsumeListMethodEx::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 4);
}

UINT CConsumeListMethodEx::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

UINT CConsumeListMethodEx::GetMemberID(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetMemberID();
}

CString CConsumeListMethodEx::GetCostTime(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetCostTime();
}

INT CConsumeListMethodEx::GetAmount(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetAmount();
}

CString CConsumeListMethodEx::GetBusinessName(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetBusinessName();
}

CString CConsumeListMethodEx::GetDescription(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetDescription();
}

