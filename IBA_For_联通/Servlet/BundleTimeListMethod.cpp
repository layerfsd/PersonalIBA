#include "stdafx.h"
#include "BundleTimeListMethod.h"


using namespace NS_SERVLET;

CBundleTimeListMethod::CBundleTimeListMethod(void)
: CServletMethod(_T("BundleTimeList")) //中心方法名称
{
	AddParams(); //初始化参数
	AddRespFields(); //初始化返回字段
}

CBundleTimeListMethod::~CBundleTimeListMethod(void)
{
}

void CBundleTimeListMethod::AddParams()
{
	//请不要修改本函数代码，参数字段顺序正确才能正确合并！
	
	m_Params.AddParam(_T("memberId"), EPT_NUMBER, 12);
	m_Params.AddParam(_T("netBarId"), EPT_NUMBER, 5);
	m_Params.AddParam(_T("position"), EPT_NUMBER, 1);
	m_Params.AddParam(_T("operator"), EPT_STRING, 10);
	m_Params.AddParam(_T("classId"), EPT_NUMBER, 2);
	m_Params.SetValue(_T("netbarId"), m_nNetbarId);
}

void CBundleTimeListMethod::SetMemberId(UINT newVal)
{
	m_Params.SetValue(_T("memberId"), newVal);
}

void CBundleTimeListMethod::SetClassId(UINT newVal)
{
	m_Params.SetValue(_T("classId"), newVal);
}

void CBundleTimeListMethod::SetNetBarId(UINT newVal)
{
	m_Params.SetValue(_T("netBarId"), newVal);
}

void CBundleTimeListMethod::SetPosition(UINT newVal)
{
	m_Params.SetValue(_T("position"), newVal);
}

void CBundleTimeListMethod::SetOperator(CString newVal)
{
	m_Params.SetValue(_T("operator"), newVal);
}

BOOL CBundleTimeListMethod::DecodeResp(CString strResp)
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

void CBundleTimeListMethod::AddRespFields()
{
	//请不要修改本函数代码，返回字段顺序正确才能正确解析！
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("recordCount"), EPT_NUMBER, 2);

}

UINT CBundleTimeListMethod::GetRecordCount()
{
	return m_RespFields.GetValueAsUINT(_T("recordCount"));
}


UINT CBundleTimeListMethod::GetBundleId(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetBundleId();
}

UINT CBundleTimeListMethod::GetBundleType(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetBundleType();
}

CString CBundleTimeListMethod::GetStartTime(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetStartTime();
}

CString CBundleTimeListMethod::GetEndTime(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetEndTime();
}

UINT CBundleTimeListMethod::GetTimePass(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetTimePass();
}

UINT CBundleTimeListMethod::GetPrice(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetPrice();
}

CString CBundleTimeListMethod::GetDescription(INT nIndex)
{
	return m_BundleTimeList.GetAt(nIndex).GetDescription();
}

//******************************************************************************

CBundleTimeInfo::CBundleTimeInfo()
{
	m_RespFields.AddRespField(_T("bundleId"), EPT_NUMBER, 4);
	m_RespFields.AddRespField(_T("bundleType"), EPT_NUMBER, 1);
	m_RespFields.AddRespField(_T("startTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("endTime"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("timePass"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("price"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("Description"), EPT_STRING, 3, FALSE);
}

UINT CBundleTimeInfo::GetBundleId()
{
	return m_RespFields.GetValueAsUINT(_T("bundleId"));
}

UINT CBundleTimeInfo::GetBundleType()
{
	return m_RespFields.GetValueAsUINT(_T("bundleType"));
}

CString CBundleTimeInfo::GetStartTime()
{
	return FormatCenterTime(m_RespFields.GetValueAsString(_T("startTime")));
}

CString CBundleTimeInfo::GetEndTime()
{
	return FormatCenterTime(m_RespFields.GetValueAsString(_T("endTime")));
}

UINT CBundleTimeInfo::GetTimePass()
{
	return m_RespFields.GetValueAsUINT(_T("timePass"));
}

UINT CBundleTimeInfo::GetPrice()
{
	return m_RespFields.GetValueAsUINT(_T("price"));
}

CString CBundleTimeInfo::GetDescription()
{
	return m_RespFields.GetValueAsString(_T("Description"));
}

CString CBundleTimeInfo::FormatCenterTime(CString strServerTime)
{
	strServerTime.Trim();

	if (!strServerTime.IsEmpty())
	{
		strServerTime.Insert(2, ':');
		strServerTime.Insert(5, ':');
	}

	return strServerTime;
}

