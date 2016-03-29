#include "stdafx.h"
#include "BundleTimeList2Method.h"

using namespace NS_SERVLET;

CBundleTimeInfo2::CBundleTimeInfo2()
{
	m_RespFields.AddRespField(_T("bundleId"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("startTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("endTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("mode"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("days"), EPT_NUMBER, 3);
	m_RespFields.AddRespField(_T("price"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Description"), EPT_STRING, 3, FALSE);
}

UINT CBundleTimeInfo2::GetBundleId()
{
	return m_RespFields.GetValueAsUINT(_T("bundleId"));
}

UINT CBundleTimeInfo2::GetMode()
{
	return m_RespFields.GetValueAsUINT(_T("mode"));
}

CString CBundleTimeInfo2::GetStartTime()
{
	return FormatCenterTime(m_RespFields.GetValueAsString(_T("startTime")));
}

CString CBundleTimeInfo2::GetEndTime()
{
	return FormatCenterTime(m_RespFields.GetValueAsString(_T("endTime")));
}

UINT CBundleTimeInfo2::GetDays()
{
	return m_RespFields.GetValueAsUINT(_T("days"));
}

UINT CBundleTimeInfo2::GetPrice()
{
	return m_RespFields.GetValueAsUINT(_T("price"));
}

CString CBundleTimeInfo2::GetDescription()
{
	return m_RespFields.GetValueAsString(_T("Description"));
}

CString CBundleTimeInfo2::FormatCenterTime(CString strServerTime)
{
	strServerTime.Trim();

	if (!strServerTime.IsEmpty())
	{
		strServerTime.Insert(2, ':');
		strServerTime.Insert(5, ':');
	}

	return strServerTime;
}


CBundleTimeList2Method::CBundleTimeList2Method(void)
: CServletMethod(_T("BundleTimeList2")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBundleTimeList2Method::~CBundleTimeList2Method(void)
{
}

void CBundleTimeList2Method::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);

	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CBundleTimeList2Method::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CBundleTimeList2Method::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

void CBundleTimeList2Method::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 2);
}

UINT CBundleTimeList2Method::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}

UINT CBundleTimeList2Method::GetBundleId(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetBundleId();
}

CString CBundleTimeList2Method::GetStartTime(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetStartTime();
}

CString CBundleTimeList2Method::GetEndTime(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetEndTime();
}

UINT CBundleTimeList2Method::GetMode(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetMode();
}

UINT CBundleTimeList2Method::GetDays(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetDays();
}

UINT CBundleTimeList2Method::GetPrice(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetPrice();
}

CString CBundleTimeList2Method::GetDescription(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetDescription();
}

BOOL CBundleTimeList2Method::DecodeResp(CString strResp)
{
	BOOL bRet = CServletMethod::DecodeResp(strResp);

	UINT nCount = GetRecordCount();

	m_BundleTimeList.RemoveAll();

	if (nCount > 0)
	{
		m_BundleTimeList.SetSize(nCount);

		INT nIndex = m_RespFields.GetAllLength();

		for (INT i = 0; i < m_BundleTimeList.GetCount(); i++)
		{
			CString strTmp = strResp.Mid(nIndex);
			m_BundleTimeList.GetAt(i).m_RespFields.DecodeResp(strTmp);
			nIndex += m_BundleTimeList.GetAt(i).m_RespFields.GetAllLength();
		}
	}

	return bRet;
}
