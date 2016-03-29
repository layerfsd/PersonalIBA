#pragma once

#include "datax.h"
namespace NS_DataX
{

class CMemMapFile;

class  CDataXMReceiver : public CDataXData, public IDataXReceive
{
public:
	CDataXMReceiver(void);
	virtual ~CDataXMReceiver(void);

	virtual BOOL Create(LPCTSTR lpszMemMapName, 
		size_t nMemMapLen = 1024 * 4, UINT nTimeout = 10 * 1000);

protected:

	virtual INT OnDataArrival();

	virtual INT OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

	virtual BOOL CreateRevMechanism();

private:
	
	BOOL CreateMemMap();

	CMemMapFile* m_lpMemMapFile;

	CWnd* m_lpRevWnd;

private :

	CDataXMReceiver(const CDataXMReceiver &rhs);
	CDataXMReceiver &operator=(const CDataXMReceiver &rhs);

};


template<class TClass>
class CDataXMReceiverT : public CDataXMReceiver
{
public :

	typedef INT (TClass::*pfnOnDataArrival)(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

	CDataXMReceiverT(TClass *pObj, pfnOnDataArrival pOnDataArrival)
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

	CDataXMReceiverT(const CDataXMReceiverT &rhs);
	CDataXMReceiverT &operator=(const CDataXMReceiverT &rhs);
};
}