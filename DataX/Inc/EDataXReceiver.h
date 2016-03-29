#pragma once

#include "mdataxreceiver.h"

class DDN_EXT_CLASS CDataXEReceiver : public CDataXMReceiver
{
public:
	CDataXEReceiver(BOOL bIsComThread = FALSE);
	virtual ~CDataXEReceiver(void);

	virtual BOOL Create(LPCTSTR lpszMemMapName, 
		size_t nMemMapLen = 1024 * 4, UINT nTimeout = 10 * 1000);

protected:

	virtual BOOL CreateRevMechanism();

private:

	HANDLE m_hRevEvent;	

	CString m_strRevEventName;

	CString m_strWaitReturnEventName;

	BOOL SetWaitReturnEvent();

private:
	
	BOOL m_bIsComThread;

	INT Run();

	virtual void OnRevTimeout();

private :

	CDataXEReceiver(const CDataXEReceiver &rhs);
	CDataXEReceiver &operator=(const CDataXEReceiver &rhs);
};


template<class TClass>
class CDataXEReceiverT : public CDataXEReceiver
{
public :

	typedef INT (TClass::*pfnOnDataArrival)(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

	CDataXEReceiverT(TClass *pObj, pfnOnDataArrival pOnDataArrival)
		:m_pObj(pObj)
		,m_pfnOnDataArrival(pOnDataArrival)
	{

	}

protected :

	virtual INT OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen)
	{
		return (m_pObj->*m_pfnOnDataArrival)(nAppId, nMsg, nParam1, nParam2, lpBuf, cbLen);
	}

	TClass* m_pObj;

	pfnOnDataArrival m_pfnOnDataArrival;

private :

	CDataXEReceiverT(const CDataXEReceiverT &rhs);
	CDataXEReceiverT &operator=(const CDataXEReceiverT &rhs);
};