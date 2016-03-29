#include "stdafx.h"
#include ".\lasttraninfo.h"

using namespace NS_SERVLET;

CLastTranInfo::CLastTranInfo(void)
: m_nLastTranType(0), m_nLastRefNo(0)
{
}

CLastTranInfo::~CLastTranInfo(void)
{
}

void CLastTranInfo::Load()
{
	m_nLastTranType = AfxGetApp()->GetProfileInt(_T("NetBarInfo\\LastTran"), _T("LastTranType"), 0);
	m_nLastRefNo = AfxGetApp()->GetProfileInt(_T("NetBarInfo\\LastTran"), _T("LastRefNo"), 0);
	m_strLastTranTime = AfxGetApp()->GetProfileString(_T("NetBarInfo\\LastTran"), _T("LastTranTime"));
}

void CLastTranInfo::Save()
{
	AfxGetApp()->WriteProfileInt(_T("NetBarInfo\\LastTran"), _T("LastTranType"), m_nLastTranType);
	AfxGetApp()->WriteProfileInt(_T("NetBarInfo\\LastTran"), _T("LastRefNo"), m_nLastRefNo);
	AfxGetApp()->WriteProfileString(_T("NetBarInfo\\LastTran"), _T("LastTranTime"), m_strLastTranTime);
}