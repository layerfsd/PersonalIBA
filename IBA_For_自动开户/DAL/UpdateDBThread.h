#pragma once

namespace NS_DAL
{

class CUpdateDBThread : public CThread
{
public:
	CUpdateDBThread(void);
	virtual ~CUpdateDBThread(void);

protected :

	CADODBConnInfo* m_pDbConnInfo;

	virtual int Run();

	BOOL TableExists(LPCTSTR lpszTableName);

	BOOL FieldExists(LPCTSTR lpszTableName, LPCTSTR lpszFieldName);
	
	BOOL ExecuteSQL(CString strSQL);

	void DoUpdateDB();
	
};

}