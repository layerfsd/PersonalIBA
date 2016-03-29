#pragma once

class CIBAString : public CString
{
public:
	CString LoadString(UINT ID)
	{
		CString str = LOAD_STRING(ID);
		CString::operator =(str);
		return str;
	}
	CString& operator=(const CString& str)
	{
		return CString::operator =(str);
	}
};