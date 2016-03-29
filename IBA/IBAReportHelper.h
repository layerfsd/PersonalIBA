#pragma once

#include <string>

class CIBAReportHelper
{
protected:
	CIBAReportHelper(void);

public:
	~CIBAReportHelper(void);

	static CIBAReportHelper* GetInstance();

	void DoReport();

private:
	CString pathToLocalPathURL(LPCTSTR lpPath);
	CString workPath();
	std::wstring Utf8_to_WideChar( std::string& strUtf);
	std::string WideChar_to_Utf8( std::wstring& strVar);
public:
	CString m_strNetbarName;

	CString m_strValue1;
	CString m_strValue2;
	CString m_strValue3;
	CString m_strValue4;
	CString m_strValue5;
	CString m_strValue6;
	CString m_strValue7;
	CString m_strValue8;
	CString m_strValue9;
	CString m_strValue10;
	CString m_strValue11;
	CString m_strValue12;
	CString m_strValue13;
	CString m_strValue14;
	CString m_strValue15;
	CString m_strValue16;
	CString m_strValue17;
	CString m_strValue18;
	CString m_strValue19;
	CString m_strValue20;
	//CString m_strValue21;	// 改为m_strValue23，这样才能配对
	CString m_strValue22;
	CString m_strValue23;
};
