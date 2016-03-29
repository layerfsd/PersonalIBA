#pragma once

namespace NS_SERVLET
{

class CURLEncode
{
private:

	BOOL	 IsUnsafe(char compareChar);
	CStringA DecToHex(char num, int radix);
	CStringA Convert(char val);
	CStringA URLEncode(CStringA vData);

private:

	CURLEncode(const CURLEncode&) throw();
	CURLEncode& operator=(const CURLEncode&) throw();

private:

	CString m_strURL;

public:

	CURLEncode(LPCTSTR lpszURL);

	operator LPCTSTR() const throw()
	{
		return m_strURL;
	}
};

}
