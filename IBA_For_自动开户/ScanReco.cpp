#include "stdafx.h"
#include "IBA.h"
#include "IdentityNumber.h"
#include ".\scanreco.h"

CIBAScanReco::CIBAScanReco(void)
: m_bOpen(FALSE), m_nSex(0)
{
}

CIBAScanReco::~CIBAScanReco(void)
{
	Close();
}

BOOL CIBAScanReco::Open()
{
	//if(!m_bOpen)
	m_bOpen = OpenDevice();

	return m_bOpen;
}

BOOL CIBAScanReco::Close()
{
	if (m_bOpen)
	{
		return CloseDevice();
	}

	m_bOpen = FALSE;

	return FALSE;
}

BOOL CIBAScanReco::ScanImage(INT nIdType, LPCTSTR lpszImageFile)
{
	CStringA strImageFile = CT2CA(lpszImageFile);

	BOOL bRet = FALSE;
	bRet = ::ScanImage(nIdType, (LPSTR)strImageFile.GetString());

	if (!bRet)
	{
		CIBALog::GetInstance()->Write(_T("Can't ScanImage."));
	}
	
	if(!PathFileExists(lpszImageFile))
	{
		IBA_LOG0(_T("’’∆¨…˙≥… ß∞‹£°"));
	}

	return bRet;
}

BOOL CIBAScanReco::Recognise(INT nIdType, LPCTSTR lpszHeadImageFile)
{
	if (!IsSupportRecognise(nIdType))
	{
		CIBALog::GetInstance()->WriteFormat(_T("Is not support recognise IdType = %d."), nIdType);

		return FALSE;
	}

	ClearData();

	BOOL bRet = FALSE;

	CStringA strImageFile = CT2CA(lpszHeadImageFile);

	IDENTITY_INFO identity_info = {0};

	bRet = RecogniseImage(nIdType, identity_info, (LPSTR)strImageFile.GetString());

	if (bRet)
	{
		m_strName = CA2T(identity_info.name);
		m_strIDNumber = CA2T(identity_info.number);
		m_strNation = CA2T(identity_info.nation);
		m_strBirthDay = CA2T(identity_info.birthday);
		m_strAddress = CA2T(identity_info.address);
		m_strSignDate = CA2T(identity_info.signdate);
		m_strValidterm = CA2T(identity_info.validterm);
		m_strDepartment = CA2T(identity_info.department);

		CIdentityNumber IdentityNumber(m_strIDNumber);
		m_nSex = IdentityNumber.GetSex();

	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Can't RecogniseImage."));
	}

	return bRet;
}

BOOL CIBAScanReco::ReadCard(LPCTSTR lpszHeadImageFile)
{
	if (!IsSupportReader())
	{
		CIBALog::GetInstance()->Write(_T("Is not support Reader IdType."));

		return FALSE;
	}

	ClearData();

	BOOL bRet = FALSE;

	CStringA strImageFile = CT2CA(lpszHeadImageFile);

	IDENTITY_INFO identity_info = {0};

	//IBA_LOG(_T("’’∆¨µÿ÷∑%s"),  lpszHeadImageFile);
	bRet = ::ReadCard(identity_info, (LPSTR)strImageFile.GetString());

	if (bRet)
	{
		m_strName = CA2T(identity_info.name);
		m_strIDNumber = CA2T(identity_info.number);
		m_strNation = CA2T(identity_info.nation);
		m_strBirthDay = CA2T(identity_info.birthday);
		m_strAddress = CA2T(identity_info.address);
		m_strSignDate = CA2T(identity_info.signdate);
		m_strValidterm = CA2T(identity_info.validterm);
		m_strDepartment = CA2T(identity_info.department);
		
		CIdentityNumber IdentityNumber(m_strIDNumber);
		m_nSex = IdentityNumber.GetSex();

	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Can't RecogniseImage."));
	}
	
	if(!PathFileExists(lpszHeadImageFile))
	{
		IBA_LOG0(_T("’’∆¨…˙≥… ß∞‹£°"));
	}
	return bRet;
}

void CIBAScanReco::ClearData()
{
	m_nSex = 0;
	m_strName.Empty();
	m_strIDNumber.Empty();
	m_strNation.Empty();
	m_strBirthDay.Empty();
	m_strAddress.Empty();
	m_strSignDate.Empty();
	m_strValidterm.Empty();
	m_strDepartment.Empty();
}