#include "stdafx.h"
#include ".\respfield.h"
#include "..\IBALog.h"

using namespace NS_SERVLET::NS_SERVLET_HELPPER;


CRespField::CRespField(void)
: m_nType(EPT_NUMBER), m_nLength(0), m_bFixedLength(TRUE)
, m_bLoopBody(FALSE)
{
}

CRespField::~CRespField(void)
{
}

void CRespField::SetValue(CString newVal)
{
	m_strValue = newVal;
}

CString CRespField::GetValue()
{
	return m_strValue;
}

UINT CRespField::GetLength() const
{
	return m_nLength;
}

void CRespField::SetLength(UINT newVal)
{
	m_nLength = newVal;
}

BOOL CRespField::GetFixedLength() const
{
	return m_bFixedLength;
}

void CRespField::SetFixedLength(BOOL newVal)
{
	m_bFixedLength = newVal;
}

EParamType CRespField::GetType() const
{
	return m_nType;
}

void CRespField::SetType(EParamType newVal)
{
	m_nType = newVal;
}

BOOL CRespField::GetLoopBody() const
{
	return m_bLoopBody;
}

void CRespField::SetLoopBody(BOOL newVal)
{
	m_bLoopBody = newVal;
}

//////////////////////////////////////////////////////////////////////////


CRespFieldList::CRespFieldList(void)
: m_nAllLength(0)
{
}

CRespFieldList::~CRespFieldList(void)
{
	RemoveAll();
}

BOOL CRespFieldList::AddRespField(CString strKey, CRespField& RespField)
{	
	if (GetIndexByKey(strKey) >= 0)
	{
		CString strError;

		strError.Format(_T("CRespFieldList::AddRespField Error:%s Found!"), strKey);

		CInputParamList::HandleError(strError);

		return FALSE;
	}

	m_KeyList.Add(strKey);
	m_RespFieldList.Add(RespField);

	return TRUE;
}

BOOL CRespFieldList::AddRespField(CString strKey, EParamType nType, UINT nLength, BOOL bFixedLength, BOOL bLoopBody)
{
	CRespField RespField;

	RespField.SetFixedLength(bFixedLength);
	RespField.SetType(nType);
	RespField.SetLength(nLength);
	RespField.SetLoopBody(bLoopBody);

	return AddRespField(strKey, RespField);
}

void CRespFieldList::SetValue(CString strKey, CString newVal)
{
	INT nIndex = GetIndexByKey(strKey);

	if (nIndex < 0)
	{
		CString strError;

		strError.Format(_T("CRespFieldList::SetValue Error:%s not Found!"), strKey);

		CInputParamList::HandleError(strError);

		return;
	}

	m_RespFieldList.GetAt(nIndex).SetValue(newVal);
}


CString CRespFieldList::GetValueAsString(CString strKey)
{
	INT nIndex = GetIndexByKey(strKey);

	if (nIndex < 0)
	{
		CString strError;

		strError.Format(_T("CRespFieldList::GetValueAsString Error:%s not Found!"), strKey);

		CInputParamList::HandleError(strError);

		return _T("");
	}

	return m_RespFieldList.GetAt(nIndex).GetValue();
}

UINT CRespFieldList::GetValueAsUINT(CString strKey)
{
	return _ttoi(GetValueAsString(strKey));
}

INT CRespFieldList::GetValueAsINT(CString strKey)
{
	return _ttoi(GetValueAsString(strKey));
}

void CRespFieldList::RemoveAll()
{
	m_KeyList.RemoveAll();
	m_RespFieldList.RemoveAll();
}

BOOL CRespFieldList::DecodeResp(CString strResp)
{
	BOOL bRet = FALSE;

	INT nIdx = 0; 

	CString strTmp;

	try
	{
		for (INT i = 0 ; i < m_RespFieldList.GetCount(); i++)
		{
			strTmp = strResp.Mid(nIdx, m_RespFieldList.GetAt(i).GetLength());
			nIdx += m_RespFieldList.GetAt(i).GetLength();

			if (m_RespFieldList.GetAt(i).GetFixedLength()) //定长的
			{	
				if (m_RespFieldList.GetAt(i).GetLoopBody()) //下个字段是循环体
				{
					CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_RESPFIELD, _T("R-%s:%s"), m_KeyList.GetAt(i), strTmp);
					m_RespFieldList.GetAt(i).SetValue(strTmp.Trim());
					
					INT nCount = _ttoi(strTmp);

					i++; //给下一个字段赋值，同时循环也要跳过

					if (nCount > 0)
					{
						nCount = nCount * m_RespFieldList.GetAt(i).GetLength(); //计算实际的长度
						strTmp = strResp.Mid(nIdx, nCount);
						m_RespFieldList.GetAt(i).SetValue(strTmp); //这里不能TRIM，因为空格对后面的操作很重要
						nIdx += nCount;
					}
				}
				else
				{
					m_RespFieldList.GetAt(i).SetValue(strTmp.Trim());
				}
			}
			else
			{	
				INT nTmp = _ttoi(strTmp);

				strTmp = strResp.Mid(nIdx, nTmp);

				m_RespFieldList.GetAt(i).SetValue(strTmp.Trim());

				nIdx += nTmp;				
			}
			
			CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_RESPFIELD, _T("R-%s:%s"), m_KeyList.GetAt(i), strTmp);
		}

		m_nAllLength = nIdx;

		bRet = TRUE;
	}
	catch (...)
	{
		
		CIBALog::GetInstance()->Write(_T("CRespFieldList::DecodeResp Error."));

		bRet = FALSE;
	}

	return bRet;
}

INT CRespFieldList::GetIndexByKey(CString strKey)
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

void CRespFieldList::ResetAllValue()
{
	for (INT i = 0 ; i < m_RespFieldList.GetCount(); i++)
	{
		m_RespFieldList.GetAt(i).SetValue(_T(""));
	}
}

