#include "stdafx.h"
#include "WX_ado.h"

GxxADO::GxxADO() : m_isConnected(false)
{
CoInitialize(NULL);
}

GxxADO::~GxxADO()
{
	CoUninitialize();
}


bool GxxADO::Connect( const char* connection )
{
	m_strConnection = connection;

	if (m_isConnected) {
		return true;
	}
	HRESULT hr = m_ptrConnection.CreateInstance("ADODB.Connection"); //创建Connection对象
	if(!SUCCEEDED(hr))  {
		return false;
	}

	hr = m_ptrConnection->Open(m_strConnection.c_str(),"","",adModeUnknown);
	if (!SUCCEEDED(hr)) {
		return false;
	}

	m_isConnected = true;

	return true;
}

bool GxxADO::IsConnect()//判断当前的连接状态
{
	try    
	{   
		//状态判断   
		if(m_ptrConnection==NULL)   {
			m_isConnected = false;
			return  false;   
		}
		if(m_ptrConnection->GetState()==adStateClosed)   {
			m_isConnected = false;
			return  false;
		}
		GetRecords("select   NOW() ");
		m_isConnected = true;
		return true;   
	}   
	catch(_com_error e)   
	{   
		m_isConnected = false;
		return false;
	}   

	return false;   

}

bool GxxADO::Close()
{

	HRESULT hr = m_ptrConnection->Close();
	if(!SUCCEEDED(hr))  {
		return false;
	}
	m_isConnected = false;
	return true;
}


GxxADORecordSet GxxADO::GetRecords( const char* sql )
{
	if (!m_isConnected) {
		if(!this->Connect(m_strConnection.c_str())) 
			return GxxADORecordSet(NULL);
	}

	_RecordsetPtr ptrRecordset = m_ptrConnection->Execute(_bstr_t(sql),NULL,adCmdText);///执行命令，取得记录集
	return GxxADORecordSet(ptrRecordset);
}


int GxxADO::ExcuteSql( const char* sql )
{
	if (!m_isConnected) {
		if (!this->Connect(m_strConnection.c_str()))
			return 0;
	}
	VARIANT vrAffected;
	vrAffected.vt = VT_INT;
	_bstr_t bst(sql);
	m_ptrConnection->Execute(bst,&vrAffected,adCmdText);///执行命令
	return vrAffected.intVal;
}

_ConnectionPtr GxxADO::GetConnetionPtr()
{
	return m_ptrConnection;
}


_CommandPtr GxxADO::GetTempCommandPtr()
{
	_CommandPtr ptrCommand;
	ptrCommand.CreateInstance("ADODB.Command");
	if (ptrCommand) {
		ptrCommand->ActiveConnection = m_ptrConnection;
	}
	return ptrCommand;
}


void GxxADO::BeginTrans()
{
	m_ptrConnection->BeginTrans();
}

void GxxADO::CommitTrans()
{
	m_ptrConnection->CommitTrans();
}

void GxxADO::RollbackTrans()
{
	m_ptrConnection->RollbackTrans();
}

GxxADORecordSet GxxADO::ExcuteStoredProc(_CommandPtr command)
{
	command->CommandType = adCmdStoredProc;
	_RecordsetPtr retPtr = command->Execute(NULL,NULL,adCmdStoredProc);
	return GxxADORecordSet(retPtr);
}

int GxxADO::CountOfRecords( const char* sqlFrom, const char* sqlWhere )
{
	char ch[1024];
	sprintf(ch, "select count(*) as r_cc from %s where %s", sqlFrom, sqlWhere);

	GxxADORecordSet recordSet = GetRecords(ch);
	if (recordSet.IsEmpty()) {
		return 0;
	}
	recordSet.SeekToBegin();
	if (recordSet.IsEOF()) {
		return 0;
	}
	GxxDictionaryPtr dic = recordSet.GetCurrentRecord();
	if (dic) {
		return dic->valueForKey("r_cc")->intValue();
	}
	return 0;
}



//------------------------------------------------------------------------------------------------

GxxADORecordSet::GxxADORecordSet( _RecordsetPtr ptrRecordSet )
{
	m_ptrRecordsSet = ptrRecordSet;
}


bool GxxADORecordSet::IsEmpty()
{
	return m_ptrRecordsSet == NULL;

}

GxxArrayPtr GxxADORecordSet::GetFields()
{
	if (!m_ptrFields) {
		m_ptrFields = GxxArray::create();

		if (m_ptrRecordsSet) {
			FieldsPtr ptrFields = m_ptrRecordsSet->GetFields();
			long colCount = ptrFields->GetCount();

			for (long i = 0; i < colCount ;i++) {
				FieldPtr fieldPtr = ptrFields->GetItem(i);
				m_ptrFields->addValue(GxxValue::create( (const char*)fieldPtr->GetName() ));
			}
		}
	}
	return m_ptrFields;
}

void GxxADORecordSet::SeekToBegin()
{
	m_ptrRecordsSet->MoveFirst();
}

void GxxADORecordSet::SeekToEnd()
{
	m_ptrRecordsSet->MoveLast();
}

bool GxxADORecordSet::IsBOF()
{
	return m_ptrRecordsSet->BOF == VARIANT_TRUE; 
}

bool GxxADORecordSet::IsEOF()
{
	return m_ptrRecordsSet->adoEOF == VARIANT_TRUE; 
}

void GxxADORecordSet::Next()
{
	m_ptrRecordsSet->MoveNext();
}

void GxxADORecordSet::Previous()
{
	m_ptrRecordsSet->MovePrevious();
}

GxxDictionaryPtr GxxADORecordSet::GetCurrentRecord()
{
	GxxArrayPtr colsName = this->GetFields();
	GxxDictionaryPtr ptrResult = GxxDictionary::create();
	for (int i = 0; i < colsName->count(); i++) {
		_variant_t var;
		var = m_ptrRecordsSet->GetCollect(colsName->valueAtIndex(i)->stringValue());

		if(var.vt!=VT_NULL)
		{
			ptrResult->setValueForKey(GxxValue::create((const char*)_bstr_t(var)), colsName->valueAtIndex(i)->stringValue());
		}
		else {
			ptrResult->setValueForKey(GxxValue::create(""), colsName->valueAtIndex(i)->stringValue());
		}
	}
	return ptrResult;
}

void GxxADOUpdate::setKey( const char* key, const char* value, const char* opt/*="="*/ )
{
	if (!m_ptrCondition) {
		m_ptrCondition = GxxArray::create();
	}
	GxxDictionaryPtr ma = GxxDictionary::create();
	ma->setValueForKey(GxxValue::create(key), "key");
	ma->setValueForKey(GxxValue::create(value), "value");
	ma->setValueForKey(GxxValue::create(opt), "opt");
	m_ptrCondition->addValue( GxxValue::create(ma));
}

void GxxADOUpdate::setValue( const char* key, const char* value )
{
	if (!m_ptrValues) {
		m_ptrValues = GxxArray::create();
	}
	GxxDictionaryPtr ma = GxxDictionary::create();
	ma->setValueForKey(GxxValue::create(key), "key");
	ma->setValueForKey(GxxValue::create(value), "value");
	m_ptrValues->addValue( GxxValue::create(ma));
}

bool GxxADOUpdate::Execute( GxxADO* pConn, bool isNoInsert)
{
	if (!pConn) 
		return false;
	bool isInsert = 0;
	if (m_ptrCondition) {
		
		std::string strCon;
		for (int i = 0; i < m_ptrCondition->count(); i++) {
			GxxDictionaryPtr ma = m_ptrCondition->valueAtIndex(i)->getDictionary();

			if (i != 0) {
				strCon.append(" and ");
			}
			strCon.append(1,'`');
			strCon.append( ma->stringValueForKey("key"));
			strCon.append(1,'`');
			strCon.append( ma->stringValueForKey("opt"));
			strCon.append("'");
			strCon.append( ma->stringValueForKey("value"));
			strCon.append("'");
		}

		if (pConn->CountOfRecords(m_strTableName.c_str(), strCon.c_str()) > 0) {
			std::string sql = "update ";
			sql.append(m_strTableName);
			sql.append(" set ");
			if (!m_ptrValues) {
				return false;
			}
			for (int i = 0; i < m_ptrValues->count(); i++) {
				GxxDictionaryPtr ma = m_ptrValues->valueAtIndex(i)->getDictionary();

				if (i != 0) {
					sql.append(1,',');
				}
				sql.append("`");
				sql.append(ma->stringValueForKey("key"));
				sql.append("`='");
				sql.append(ma->stringValueForKey("value"));
				sql.append("'");
			}
			sql.append(" where ");
			sql.append(strCon);

			return pConn->ExcuteSql(sql.c_str()) > 0;
		}
		else {
			isInsert = true;
		}
	}
	else {
		if (m_ptrValues) {
			isInsert = true;
		}
		else {
			return false;
		}
	}

	if (isInsert) {
		std::string sql = "insert into ";
		sql.append(m_strTableName);
		sql.append("(");
		if (!m_ptrValues) {
			return false;
		}
		std::string name;
		std::string value2;
		for (int i = 0; i < m_ptrValues->count(); i++) {
			GxxDictionaryPtr ma = m_ptrValues->valueAtIndex(i)->getDictionary();

			if (i != 0) {
				name.append(1,',');
				value2.append(1,',');
			}
			name.append("`");
			name.append(ma->stringValueForKey("key"));
			name.append("`");
			value2.append("'");
			value2.append(ma->stringValueForKey("value"));
			value2.append("'");
		}
		sql.append(name);
		sql.append(")values(");
		sql.append(value2);
		sql.append(")");
		return pConn->ExcuteSql(sql.c_str()) > 0;
	}
	return false;
}

void GxxADOInsert::setValue( const char* key, const char* value )
{
	if (!m_ptrValues) {
		m_ptrValues = GxxArray::create();
	}
	GxxDictionaryPtr ma = GxxDictionary::create();
	ma->setValueForKey(GxxValue::create(key), "key");
	ma->setValueForKey(GxxValue::create(value), "value");
	m_ptrValues->addValue( GxxValue::create(ma));
}

bool GxxADOInsert::Execute( GxxADO* pConn )
{
	std::string sql = "insert into ";
	sql.append(m_strTableName);
	sql.append("(");
	if (!m_ptrValues) {
		return false;
	}
	std::string name;
	std::string value2;
	for (int i = 0; i < m_ptrValues->count(); i++) {
		GxxDictionaryPtr ma = m_ptrValues->valueAtIndex(i)->getDictionary();

		if (i != 0) {
			name.append(1,',');
			value2.append(1,',');
		}
		name.append("`");
		name.append(ma->stringValueForKey("key"));
		name.append("`");
		value2.append("'");
		value2.append(ma->stringValueForKey("value"));
		value2.append("'");
	}
	sql.append(name);
	sql.append(")values(");
	sql.append(value2);
	sql.append(")");
	return pConn->ExcuteSql(sql.c_str()) > 0;
}
