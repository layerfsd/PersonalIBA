#include "stdafx.h"
#include "ConsumeListMethod.h"

using namespace NS_SERVLET;

CConsumeInfo::CConsumeInfo()
{
	m_RespFields.AddRespField(_T("netBarId"), EPT_NUMBER, 5);
	m_RespFields.AddRespField(_T("consumeType"), EPT_NUMBER, 2);
	m_RespFields.AddRespField(_T("beginTime"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("endTime"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("Amount"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("businessName"), EPT_STRING, 10);
	m_RespFields.AddRespField(_T("Description"), EPT_STRING, 3, FALSE);
}


UINT CConsumeInfo::GetNetBarId()
{
	return m_RespFields.GetValueAsUINT(_T("netBarId"));
}

UINT CConsumeInfo::GetConsumeType()
{
	return m_RespFields.GetValueAsUINT(_T("consumeType"));
}

CString CConsumeInfo::GetBeginTime()
{
	return m_RespFields.GetValueAsString(_T("beginTime"));
}

CString CConsumeInfo::GetEndTime()
{
	return m_RespFields.GetValueAsString(_T("endTime"));
}

INT CConsumeInfo::GetAmount()
{
	return m_RespFields.GetValueAsINT(_T("Amount"));
}

CString CConsumeInfo::GetBusinessName()
{
	return m_RespFields.GetValueAsString(_T("businessName"));
}

CString CConsumeInfo::GetDescription()
{
	return m_RespFields.GetValueAsString(_T("Description"));
}

//******************************************************************************

CConsumeListMethod::CConsumeListMethod(void)
: CServletMethod(_T("ConsumeList")) //中心方法名称
{
#ifndef _DEBUG
	m_bUseQueryHost = TRUE; //使用查询机
	m_nRevTimeout = 120;	//超时2分钟
#endif // _DEBUG


	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CConsumeListMethod::~CConsumeListMethod(void)
{
}

void CConsumeListMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	CServletMethod::AddParams();
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("consumeType"), EPT_NUMBER, 2);
	m_Params.AddParam(_T("beginTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("endTime"), EPT_STRING, 14);
	m_Params.AddParam(_T("Operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("needBusinessName"), EPT_NUMBER, 1);
}

BOOL CConsumeListMethod::DecodeResp(CString strResp)
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

void CConsumeListMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CConsumeListMethod::SetConsumeType(UINT newVal)
{
	m_Params.SetValue(_T("consumeType"), newVal);
}

void CConsumeListMethod::SetBeginTime(CString newVal)
{
	m_Params.SetValue(_T("beginTime"), newVal);
}

void CConsumeListMethod::SetEndTime(CString newVal)
{
	m_Params.SetValue(_T("endTime"), newVal);
}

void CConsumeListMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("Operator"), newVal);
}

void CConsumeListMethod::SetNeedBusinessName(UINT newVal)
{
	m_Params.SetValue(_T("needBusinessName"), newVal);
}

void CConsumeListMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 4);
}

UINT CConsumeListMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

UINT CConsumeListMethod::GetNetBarId(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetNetBarId();
}

UINT CConsumeListMethod::GetConsumeType(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetConsumeType();
}

CString CConsumeListMethod::GetBeginTime(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetBeginTime();
}

CString CConsumeListMethod::GetEndTime(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetEndTime();
}

INT CConsumeListMethod::GetAmount(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetAmount();
}

CString CConsumeListMethod::GetBusinessName(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetBusinessName();
}

CString CConsumeListMethod::GetDescription(INT nIndex)
{
	return m_ConsumeInfoList.GetAt(nIndex).GetDescription();
}
