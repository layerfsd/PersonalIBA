// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__F858142C_DC7A_47CF_8424_0CDFE2F30CC3__INCLUDED_)
#define AFX_LOG_H__F858142C_DC7A_47CF_8424_0CDFE2F30CC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

class CLog  
{
public:
	int Write(const char * format, ...);
	void Close(void);
	bool IsOpen();
	BOOL Open(const BOOL bLogTime = TRUE, const BOOL bAutoNewline = TRUE, const char *pFileName = NULL);
	CLog();
	virtual ~CLog();

private:
	BOOL m_bAutoNewline;
	BOOL m_bLogTime;
	FILE *m_fpLog;
};

#endif // !defined(AFX_LOG_H__F858142C_DC7A_47CF_8424_0CDFE2F30CC3__INCLUDED_)
