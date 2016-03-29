#include "StdAfx.h"
#include ".\iba.h"
#include "ODBCDBConnPool.h"

using namespace Win32Tools;

CDBConnPool g_DBConnPool(3, "DSN=netbar");

BOOL ExecuteSql(CString strSql)
{
	// 执行 Sql 语句

	CDBConnInfo* pDBConnInfo = g_DBConnPool.Allocate();	
	if(!pDBConnInfo->GetConn()->IsOpen())
	{
		g_DBConnPool.Release(pDBConnInfo);
		return FALSE;
	}
	try
	{
		pDBConnInfo->GetConn()->ExecuteSQL(strSql);
	}
	catch(CDBException* pErr) 
	{
		pDBConnInfo->Disconnect();
		g_DBConnPool.Release(pDBConnInfo);
		char ErrorMsg[256] = {0};
		pErr->GetErrorMessage(ErrorMsg, 255);
		pErr->Delete();
		return FALSE;
	}
	g_DBConnPool.Release(pDBConnInfo);
	return TRUE;
}

void InsertTable()
{
	CString strSQL;

	strSQL += "CREATE TABLE rzxIdInfo (\r\n";
	strSQL += "idNumber varchar(30) NOT NULL default '0',\r\n";
	strSQL += "folk int(11) default NULL,\r\n";
	strSQL += "PRIMARY KEY (idNumber)\r\n)";
	strSQL += "ENGINE=InnoDB DEFAULT CHARSET=gbk";

	ExecuteSql(strSQL);
}

void WriteFolkInfo(char *pszIdNumber, BYTE bFolk)
{
	CString strSql;
	strSql.Format("insert into rzxIdInfo (idNumber,folk) values('%s', %d) ", pszIdNumber, (int)bFolk);
	if (ExecuteSql(strSql)) {
		return;
	}

	strSql.Format("update rzxIdInfo set folk=%d where idNumber='%s'", (int)bFolk, pszIdNumber);
	ExecuteSql(strSql);
}



BOOL IsIBARunning()
{
	HWND hIBA = FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);

	return (hIBA != NULL);
}

BOOL SendMessageToIBA(UINT nType)
{
	BOOL bRet = FALSE;

	HWND hIBA = FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);

	if (hIBA != NULL)
	{
		bRet = SendMessage(hIBA, WM_REALNAME, nType, NULL);
	}
	else
	{
		AfxMessageBox(_T("请先运行嘟嘟牛收银台！"));
	}

	return bRet ? 1 : 0;
}

int IBA_IDUser(char *pszIDNo, char *pszUserNo, char *pszPassword, BYTE IDCardType, BOOL bValidate, const REQ_REGISTER_INFO& RegInfo)
{
	GXX_LOG_HIT("IBA_IDUser(...), 证件号:%s, 名族:%d", RegInfo.szCertificateID, (int)RegInfo.bFolk);

	CRegKey regIBA;

	if (ERROR_SUCCESS == regIBA.Create(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\RZX\\IDUser")))
	{
		regIBA.SetStringValue(_T("IDNo"), pszIDNo);
		regIBA.SetStringValue(_T("UserNo"), pszUserNo);
		regIBA.SetStringValue(_T("Password"), pszPassword);
		regIBA.SetDWORDValue(_T("IDCardType"), IDCardType);
		regIBA.SetDWORDValue(_T("Validate"), bValidate);

		regIBA.SetStringValue(_T("Name"), RegInfo.szName);
		regIBA.SetStringValue(_T("CertificateType"), RegInfo.szCertificateType);
		regIBA.SetStringValue(_T("CertificateID"), RegInfo.szCertificateID);
		regIBA.SetStringValue(_T("Company"), RegInfo.szCompany);
		regIBA.SetStringValue(_T("Country"), RegInfo.szCountry);
		regIBA.SetStringValue(_T("Detail"), RegInfo.szDetail);


		regIBA.Close();

		InsertTable();
		WriteFolkInfo((char*)RegInfo.szCertificateID, RegInfo.bFolk);

		GXX_LOG_HIT("姓名: %s 证件号码: %s", RegInfo.szName, RegInfo.szCertificateID);

		return SendMessageToIBA(RN_RZX_IDUSER);
	}

	return 0;
}

int IBA_AddToMemberRZX(char *pszIDNo, char *pszUserNo, char *pszPassword, BYTE IDCardType, const REQ_REGISTER_INFO& RegInfo)
{
	GXX_LOG_HIT("IBA_AddToMemberRZX(...) ,证件号:%s, 名族:%d", RegInfo.szCertificateID, (int)RegInfo.bFolk);

	CRegKey regIBA;

	if (ERROR_SUCCESS == regIBA.Create(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\RZX\\AddToMemberRZX")))
	{
		regIBA.SetStringValue(_T("IDNo"), pszIDNo);
		regIBA.SetStringValue(_T("UserNo"), pszUserNo);
		regIBA.SetStringValue(_T("Password"), pszPassword);
		regIBA.SetDWORDValue(_T("IDCardType"), IDCardType);

		regIBA.SetStringValue(_T("Name"), RegInfo.szName);
		regIBA.SetStringValue(_T("CertificateType"), RegInfo.szCertificateType);
		regIBA.SetStringValue(_T("CertificateID"), RegInfo.szCertificateID);
		regIBA.SetStringValue(_T("Company"), RegInfo.szCompany);
		regIBA.SetStringValue(_T("Country"), RegInfo.szCountry);
		regIBA.SetStringValue(_T("Detail"), RegInfo.szDetail);

		regIBA.Close();

		InsertTable();
		WriteFolkInfo((char *)RegInfo.szCertificateID, RegInfo.bFolk);

		GXX_LOG_HIT("姓名: %s 证件号码: %s", RegInfo.szName, RegInfo.szCertificateID);

		return SendMessageToIBA(RN_RZX_ADDMEMBER);
	}

	return 0;
}

int IBA_RzxNotiFy(int nNotifyType, const char * lpNotifyMsg)
{
	return 0;
}