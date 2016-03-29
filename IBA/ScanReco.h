#pragma once

#include "..\Lib\ScanRecoInc.h"

class CIBAScanReco
{
public:
	CIBAScanReco(void);
	~CIBAScanReco(void);

private:

	BOOL m_bOpen;

	INT m_nSex;
	CString m_strName;
	CString m_strIDNumber;
	CString m_strNation;
	CString m_strBirthDay;
	CString m_strAddress;
	CString m_strSignDate;
	CString m_strValidterm;
	CString m_strDepartment;

public:

	INT GetSex() const { return m_nSex; }
	CString GetName() const { return m_strName; }
	CString GetIDNumber() const { return m_strIDNumber; }
	CString GetNation() const { return m_strNation; }
	CString GetBirthDay() const { return m_strBirthDay; }
	CString GetAddress() const { return m_strAddress; }
	CString GetSignDate() const { return m_strSignDate; }
	CString GetValidterm() const { return m_strValidterm; }
	CString GetDepartment() const { return m_strDepartment; }

private:

	void ClearData();

public:

	BOOL Open();
	BOOL Close();

	BOOL ScanImage(INT nIdType, LPCTSTR lpszImageFile);
	BOOL Recognise(INT nIdType, LPCTSTR lpszHeadImageFile);

	BOOL ReadCard(LPCTSTR lpszHeadImageFile);
};
