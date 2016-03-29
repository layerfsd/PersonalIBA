#pragma once

#include "datax.h"

namespace NS_DataX
{

class CDataXMSender : public CDataXData,  public IDataXSend
{
public:
	CDataXMSender(void);
	virtual ~CDataXMSender(void);

public:

	virtual BOOL Create(INT nAppId, LPCTSTR lpszMemMapName, 
		size_t nMemMapLen = 1024 * 4, UINT nTimeout = 10 * 1000);

	virtual INT SendData(INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf = NULL, size_t cbLen = 0);

	virtual INT PostData(INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf = NULL, size_t cbLen = 0);

protected:

	BOOL PrepareData(bool bSync, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

	virtual INT SendToRev(bool bSync);

private :

	INT m_nAppId;

	HWND m_hRevWnd;

	CDataXMSender(const CDataXMSender &rhs);
	CDataXMSender &operator=(const CDataXMSender &rhs);
};

}