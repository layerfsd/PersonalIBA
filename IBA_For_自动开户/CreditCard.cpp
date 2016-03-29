#include "stdafx.h"
#include "IBAHelpper.h"
#include "IBA.h"
#include ".\creditcard.h"

CCreditCard::CCreditCard(void)
: m_nCurPos(0)
, m_nType(0), m_nType2(0), m_nMoney(0)
{
	LoadFile();
}

CCreditCard::~CCreditCard(void)
{
	SaveFile();
}

BOOL CCreditCard::LoadFile()
{
	CString strFileName = theApp.GetWorkPath() + _T("\\IBAConfig\\cards.txt");

	if (PathFileExists(strFileName))
	{
		m_nCurPos = 0;

		CString strTmp;

		CIBAHelpper::ReadFile(strFileName, strTmp);

		strTmp.Remove('\r');

		CIBAHelpper::SplitLine(strTmp, m_StringArray, '\n');
	}

	return m_StringArray.GetCount() > 1;
}

BOOL CCreditCard::NextCreditCard()
{
	if (m_StringArray.GetCount() > 1)
	{
		m_nCurPos++;
		//数组有效范围保护
		if (m_nCurPos == 0)
		{
			m_nCurPos = 1;
		}
		if (m_StringArray.GetCount() - 1 < m_nCurPos)
		{
			m_nCurPos = 1;
		}

		CString strTmp = m_StringArray.GetAt(m_nCurPos);
		
		if (strTmp.IsEmpty())
		{
			m_nCurPos++;

			strTmp = m_StringArray.GetAt(m_nCurPos);
		}

		CStringArray strArray;

		CIBAHelpper::SplitLine(strTmp, strArray, ',');

		if (0 == strArray.GetAt(0).CompareNoCase(_T("ZS")))
		{
			m_nType = 1;
		}
		else
		{
			m_nType = 0;
		}
		
		m_nType2 = _ttoi(strArray.GetAt(1));

		m_strId = strArray.GetAt(2);
		
		double dblMoney = _tstof(strArray.GetAt(3));
		m_nMoney = dblMoney * 100;

		m_strPwd = strArray.GetAt(4);

		m_Date.ParseDateTime(strArray.GetAt(5));

		return TRUE;

	}

	return FALSE;
}


BOOL CCreditCard::SaveFile()
{
	CString strFileName = theApp.GetWorkPath() + _T("\\IBAConfig\\cards.txt");

	if (PathFileExists(strFileName))
	{
		m_nCurPos = 0;

		CString strTmp;

		for (INT i = 0; i < m_StringArray.GetCount(); i++)
		{
			strTmp.Append(m_StringArray.GetAt(i) + _T("\r\n"));
		}
		
		strTmp.Trim(_T("\r\n"));

		return CIBAHelpper::WriteFile(strFileName, strTmp);
	}

	return FALSE;
}

UINT CCreditCard::GetCreditCardType()
{
	return m_nType;
}

UINT CCreditCard::GeCreditCardFun()
{
	return m_nType2;
}

CString CCreditCard::GetCreditCardID()
{
	return m_strId;
}

CString CCreditCard::GetCreditCardPassword()
{
	return m_strPwd;
}

UINT CCreditCard::GeCreditCardMoney()
{
	return m_nMoney;
}

BOOL CCreditCard::IsCreditCardValid()
{
	COleDateTime dtNow = COleDateTime::GetCurrentTime();

	return dtNow < m_Date;
}

BOOL CCreditCard::DeleteCreditCard()
{
	if (m_StringArray.GetCount() > 1 && m_nCurPos > 0)
	{
		m_StringArray.RemoveAt(m_nCurPos);
		m_nCurPos--;

		return TRUE;
	}

	return FALSE;
}

INT CCreditCard::GetCount()
{
	return m_StringArray.GetCount();
}
