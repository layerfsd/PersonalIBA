#pragma once

class CMD5
{
private:
	CMD5(void);
	~CMD5(void);

public:

	static CString MD5Buffer(LPCVOID lpBuf, size_t Len);

	static CString MD5String(LPCTSTR lpszString);

	static CString MD5File(LPCTSTR lpszFileName);

};
