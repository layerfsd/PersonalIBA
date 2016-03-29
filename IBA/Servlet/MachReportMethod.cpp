#include "stdafx.h"
#include "MachReportMethod.h"

using namespace NS_SERVLET;

CMachReportMethod::CMachReportMethod()
: CServletMethod(_T("NonSysService"))
{
	AddParams();
	AddRespFields();
}

CMachReportMethod::~CMachReportMethod()
{

}

void CMachReportMethod::SetData(CString strData)
{
	m_Params.AddParam(_T("data"), EPT_STRING, strData.GetLength());
	m_Params.SetValue(_T("data"), strData);
}

BOOL CMachReportMethod::DoReport()
{
	return Execute();
}

INT CMachReportMethod::GetAddNum()
{
	return m_RespFields.GetValueAsINT(_T("addNum"));
}

INT CMachReportMethod::GetMergeNum()
{
	return m_RespFields.GetValueAsINT(_T("mergeNum"));
}

INT CMachReportMethod::GetErrorNum()
{
	return m_RespFields.GetValueAsINT(_T("errorNum"));
}

INT CMachReportMethod::GetDelNum()
{
	return m_RespFields.GetValueAsINT(_T("delNum"));
}

CString CMachReportMethod::GetOperateTime()
{
	return m_RespFields.GetValueAsString(_T("operateTime"));
}

void CMachReportMethod::AddParams()
{
}

void CMachReportMethod::AddRespFields()
{
	CServletMethod::AddRespFields();

	m_RespFields.AddRespField(_T("addNum"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("mergeNum"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("errorNum"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("delNum"), EPT_NUMBER, 6);
	m_RespFields.AddRespField(_T("operateTime"), EPT_STRING, 14);
}