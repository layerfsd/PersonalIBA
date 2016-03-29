#pragma once

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
#include "WX_base.h"

//#pragma warning(disable:4146)
//#import "C:/Program Files/Common Files/System/ADO/msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")
//#pragma warning(default:4146)
//using namespace ADODB;

class GxxADORecordSet;
class GxxADO
{
public:
	GxxADO();
	~GxxADO();

	bool Connect(const char* connection);
	bool Close();
	bool IsConnect();

	GxxADORecordSet GetRecords(const char* sql);
	int ExcuteSql(const char* sql);
	GxxADORecordSet ExcuteStoredProc(_CommandPtr command);

	/**
	* 获取记录的个数
	* @sqlFrom sql语句里from块的内容
	* @sqlWhere sql语句里where块的内容
	*/
	int CountOfRecords(const char* sqlFrom, const char* sqlWhere);

	_ConnectionPtr GetConnetionPtr();
	void BeginTrans();
	void CommitTrans();
	void RollbackTrans();

	_CommandPtr GetTempCommandPtr();

private:
	std::string m_strConnection;
	_ConnectionPtr m_ptrConnection;
	bool m_isConnected;
};

class GxxADORecordSet
{
public:
	GxxADORecordSet(_RecordsetPtr ptrRecordSet);

	bool IsEmpty();

	GxxArrayPtr GetFields();


	void SeekToBegin();
	void SeekToEnd();
	bool IsBOF();
	bool IsEOF();
	void Next();
	void Previous();
	

	GxxDictionaryPtr GetCurrentRecord();

private:
	_RecordsetPtr m_ptrRecordsSet;
	GxxArrayPtr m_ptrFields;
};


class GxxADOInsert 
{
public:
	GxxADOInsert(const char* tableName):m_strTableName(tableName){}

	void setValue(const char* key, const char* value);

	bool Execute(GxxADO* pConn);
private:
	std::string m_strTableName;
	GxxArrayPtr m_ptrValues;
};

class GxxADOUpdate 
{
public:
	GxxADOUpdate(const char* tableName):m_strTableName(tableName){}

	void setKey(const char* key, const char* value, const char* opt="=");
	void setValue(const char* key, const char* value);

	bool Execute(GxxADO* pConn, bool isNoInsert=true);
private:
	std::string m_strTableName;
	GxxArrayPtr m_ptrCondition;
	GxxArrayPtr m_ptrValues;
};