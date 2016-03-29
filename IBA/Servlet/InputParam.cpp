#include "stdafx.h"
#include "inputparam.h"
#include "..\IBALog.h"
#include "..\IBA.h"

using namespace NS_SERVLET::NS_SERVLET_HELPPER;

CInputParam::CInputParam()
: m_nType(EPT_NUMBER), m_nLength(0)
, m_nLengthType(EPLT_FIXED)
{
}

CInputParam::~CInputParam(void)
{
}

void CInputParam::SetValue(CString newVal)
{
	m_strValue = newVal;
}

CString CInputParam::GetValue()
{
	CString strTmp;

	if (EPLT_VARIABLE == m_nLengthType)
	{
		CString strLen, strFormat;
		strLen.Format(_T("%u"), m_strValue.GetLength());
		
		if ((UINT)strLen.GetLength() <= m_nLength) //实际超过最大长度了
		{
			strFormat.Format(_T("%s%u%s"), _T("%0"), m_nLength, _T("u"));
			strTmp.Format(strFormat, m_strValue.GetLength());
			strTmp.Append(m_strValue);			
		}
		else
		{
			for (UINT i = 0; i < m_nLength; i++)
			{
				strFormat.AppendChar('9');
			}

			strTmp.Append(strFormat);
			strTmp.Append(m_strValue.Left(_ttoi(strFormat)));		
		}
	}
	else if (EPLT_OWN == m_nLengthType)
	{
		strTmp = m_strValue;
	}
	else if (EPLT_FIXED == m_nLengthType)
	{
		if (m_nLength > 0)
		{
			INT nSpaceLen = m_nLength - m_strValue.GetLength();

			if (nSpaceLen >= 0)
			{
				for (; nSpaceLen > 0; nSpaceLen--)
				{
					if (EPT_NUMBER == m_nType)
					{
						strTmp.AppendChar(_T('0')); //填充0
					}
					else if (EPT_STRING == m_nType)
					{
						strTmp.AppendChar(_T(' ')); //填充空格
					}			
				}

				strTmp.Append(m_strValue);
			}
			else
			{
				strTmp = m_strValue.Left(m_nLength);
			}
		}
		else
		{
			strTmp = m_strValue;
		}		
	}

	return strTmp;
}

UINT CInputParam::GetLength() const
{
	return m_nLength;
}

void CInputParam::SetLength(UINT newVal)
{
	m_nLength = newVal;
}

EParamLengthType CInputParam::GetLengthType() const
{
	return m_nLengthType;
}

void CInputParam::SetLengthType(EParamLengthType newVal)
{
	m_nLengthType = newVal;
}

EParamType CInputParam::GetType() const
{
	return m_nType;
}

void CInputParam::SetType(EParamType newVal)
{
	m_nType = newVal;
}


//////////////////////////////////////////////////////////////////////////

void CInputParamList::HandleError(CString& strError)
{
	ASSERT(FALSE);

	CIBALog::GetInstance()->Write(strError);

	AfxMessageBox(strError);

	TerminateProcess(GetCurrentProcess(), 0);
}

CInputParamList::CInputParamList(void)
{

}

CInputParamList::~CInputParamList(void)
{
	RemoveAll();
}

BOOL CInputParamList::AddParam(CString strKey, CInputParam& inputParam)
{	
	if (GetIndexByKey(strKey) >= 0)
	{
		CString strError;

		strError.Format(_T("CInputParamList::AddParam Error:%s not Found!"), strKey);

		CInputParamList::HandleError(strError);
		
		return FALSE;
	}

	m_KeyList.Add(strKey);
	m_ParamList.Add(inputParam);

	return TRUE;
}

BOOL CInputParamList::AddParam(CString strKey, EParamType nType, UINT nLength, EParamLengthType nLengthType)
{
	CInputParam inputParam;

	inputParam.SetLengthType(nLengthType);
	inputParam.SetType(nType);
	inputParam.SetLength(nLength);

	return AddParam(strKey, inputParam);
}

void CInputParamList::SetValue(CString strKey, CString newVal)
{
	INT nIndex = GetIndexByKey(strKey);

	if (nIndex < 0)
	{
		CString strError;

		strError.Format(_T("CInputParamList::SetValue Error:%s not Found!"), strKey);

		CInputParamList::HandleError(strError);

		return;
	}

	m_ParamList.GetAt(nIndex).SetValue(newVal);
}

void CInputParamList::SetValue(CString strKey, UINT newVal)
{
	CString strTmp;

	strTmp.Format(_T("%u"), newVal);

	SetValue(strKey, strTmp);
}

void CInputParamList::SetValue(CString strKey, INT newVal)
{
	CString strTmp;

	strTmp.Format(_T("%d"), newVal);

	SetValue(strKey, strTmp);
}

CString CInputParamList::GetValue(CString strKey)
{
	INT nIndex = GetIndexByKey(strKey);

	if (nIndex < 0)
	{
		CString strError;

		strError.Format(_T("CInputParamList::GetValue Error:%s not Found!"), strKey);

		CInputParamList::HandleError(strError);

		return _T("");
	}

	return m_ParamList.GetAt(nIndex).GetValue();
}

void CInputParamList::RemoveAll()
{
	m_KeyList.RemoveAll();
	m_ParamList.RemoveAll();
}

CString CInputParamList::GetAllValue()
{
	CString strTmp;

	for (INT i = 0 ; i < m_ParamList.GetCount(); i++)
	{
		if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
		{
			IBA_LOG( CIBALog ::ELT_PARAM , _T("P-%s:%s"), m_KeyList .GetAt ( i), m_ParamList .GetAt ( i). GetValue ());
		}		

		if (m_KeyList.GetAt(i) == _T("password"))
		{
			CString strPwd = m_ParamList.GetAt(i).GetValue();
			strPwd = strTmp;
		}
		strTmp.Append(m_ParamList.GetAt(i).GetValue());
	}

	return strTmp;
}

INT CInputParamList::GetIndexByKey(CString strKey)
{
	for (INT i = 0 ; i < m_KeyList.GetCount(); i++)
	{
		if (0 == m_KeyList.GetAt(i).CompareNoCase(strKey)) //已经存在了
		{
			return i;
		}
	}

	return -1;
}


