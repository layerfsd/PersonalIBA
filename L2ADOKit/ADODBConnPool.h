
#pragma once

#include "ado2.h"

#include <vector>
#include <afxmt.h>


//////////////////////////////////////////////////////////////////////////

class CADODBConnPool;

class L2_EXT_CLASS CADODBConnInfo
{
	friend class CADODBConnPool;
private:
	
	CADODatabase m_dbConn;
	CADODBConnPool& m_DBConnPool;
	bool m_isConnected;

private:

	CADODBConnInfo(CADODBConnPool& Pool);

public:
	
	CADODatabase* GetConn();		

	void Release();				

	void Disconnect();

	bool IsConnected();

private:
	
	CADODBConnInfo(const CADODBConnInfo& rhs);
	CADODBConnInfo& operator=(const CADODBConnInfo& rhs);
};

class L2_EXT_CLASS CADODBConnPool  
{

	friend class CADODBConnInfo;
public:

	CADODBConnPool(size_t maxFreeConnNum, LPCTSTR dbConnStr = NULL);
	
	virtual ~CADODBConnPool();
	
	CADODBConnInfo* Allocate();					

	void SetConnStr(LPCTSTR dbConnStr);

private:

	const size_t m_maxFreeConnNum;
	
	CString m_dbConnStr;

	typedef std::vector<CADODBConnInfo*> ConnList;

	ConnList m_activeList;
	ConnList m_freeList;

	CCriticalSection m_criticalSection;

	void Release(CADODBConnInfo* pConnInfo);
	
	void DestroyDBConn(CADODBConnInfo* pConnInfo);
	
	void Flush();

private:
	
	CADODBConnPool(const CADODBConnPool& rhs);
	CADODBConnPool& operator=(const CADODBConnPool& rhs);
	
};

