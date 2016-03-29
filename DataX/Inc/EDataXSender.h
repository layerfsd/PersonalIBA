#pragma once

#include "mdataxsender.h"

class DDN_EXT_CLASS CDataXESender : public CDataXMSender
{
public:
	CDataXESender(void);
	virtual ~CDataXESender(void);

	virtual BOOL Create(INT nAppId, LPCTSTR lpszMemMapName, 
		size_t nMemMapLen = 1024 * 4, UINT nTimeout = 10 * 1000);

private:

	virtual INT SendToRev(bool bSync);

	BOOL SetRevEvent();

	CString m_strRevEventName;

	CString m_strWaitReturnEventName;

	HANDLE m_hWaitReturn;

private :

	CDataXESender(const CDataXESender &rhs);
	CDataXESender &operator=(const CDataXESender &rhs);
};
