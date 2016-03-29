#pragma once

class G2X_CLASSES CG2XUIException : public CSimpleException
{
public:
	CG2XUIException();

	CG2XUIException(BOOL bDeleteMem, LPCTSTR lpszError);

	void Delete();

	LPCTSTR GetErrorMessage() { return m_tcError; }
protected:
	TCHAR m_tcError[256];
	BOOL m_bAutoDelete;

};