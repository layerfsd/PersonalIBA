#include "stdafx.h"
#include ".\ibadal.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"
#include "..\Encrypt.h"
#include "..\LocalCenter\G2XCommonTool.h"
#include "..\GlobalFunctions.h"
#include "..\activemember.h"
#include "..\LocalCenter\LocalCenter.h"
using namespace IBAGlobal;

SINGLETON_IMPLEMENT(CIBADAL)

CIBADAL::CIBADAL(void)
: m_DBPool(2, _T("DSN=NETBAR;"))
, m_nConnectState(FALSE)
{
#ifdef _DEBUG
	//m_DBPool.SetConnStr(_T("DSN=NETBAR207;"));
#endif
}

CIBADAL::~CIBADAL(void)
{
}

BOOL CIBADAL::GetDBConnInfo(CADODBConnInfo* & pDb, BOOL bForceConnect /*= FALSE*/)
{
	if (m_nConnectState == 2 && !bForceConnect)
	{
		return FALSE;
	}

	CADODBConnInfo* pDbConnInfo = m_DBPool.Allocate();

	if (!pDbConnInfo->GetConn()->IsOpen())
	{
		CIBALog::GetInstance()->Write(_T("CIBADAL::GetDBConnInfo - Can't connect to DB.")); 

		pDbConnInfo->Disconnect();
		pDbConnInfo->Release();

		m_nConnectState = 2;
	}
	else
	{
		//判断是否真的连上了
		if (pDbConnInfo->GetConn()->Execute(_T("select now() as dbTime")))
		{
			m_nConnectState = 1;
			pDb = pDbConnInfo;

			return TRUE;
		}
		else
		{
			pDbConnInfo->Disconnect();
			pDbConnInfo->Release();
		}
	}

	return FALSE;
}

BOOL CIBADAL::ExecuteSQL(CString strSQL)
{
	BOOL bRet = FALSE;

	IBA_TRACE(strSQL);

	CADODBConnInfo* pDb = NULL;

	if (GetDBConnInfo(pDb))
	{
		bRet = pDb->GetConn()->Execute(strSQL);

		if (!bRet)
		{
			CIBALog::GetInstance()->WriteFormat(_T("CIBADAL::ExecuteSQL - %s"), 
				pDb->GetConn()->GetErrorDescription());
		}

		pDb->Release();
	}

	return bRet;
}

int NS_DAL::CIBADAL::ExecuteSQLEx( CString strSQL )
{
	int nAffected = -1;

	CADODBConnInfo* pDb = NULL;

	if (GetDBConnInfo(pDb))
	{
		CADODatabase* pDB = pDb->GetConn();
		BOOL bRet = pDB->Execute(strSQL);

		if (!bRet)
		{
			CIBALog::GetInstance()->WriteFormat(_T("CIBADAL::ExecuteSQL - %s"), 
				pDb->GetConn()->GetErrorDescription());
		}
		else 
		{
			nAffected = pDB->GetRecordsAffected();
		}

		pDb->Release();
	}

	IBA_TRACE(_T("ExecuteSQL -> %s 受影响行数为:%d"), strSQL, nAffected);

	return nAffected;
}


BOOL CIBADAL::GetMaxKeyId(INT & nMaxKeyId)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL = _T("select max(keyId) + 1 from SystemConfig");

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		bRet = Rs.GetFieldValue(0, nMaxKeyId);
		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetAllSystemConfig(CSystemConfigArray & ConfigArray)
{
	BOOL bRet = FALSE;

	ConfigArray.RemoveAll();

	CString strSQL(_T("select * from systemconfig where keytype = 'others'"));

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		while (!Rs.IsEOF())
		{
			CConfigInfo configInfo;

			INT nTmp = 0; CString strTmp;

			if (Rs.GetFieldValue(_T("KeyID"), nTmp))
			{
				configInfo.SetKeyID(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue1"), nTmp))
			{
				configInfo.SetKeyValue1(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue2"), nTmp))
			{
				configInfo.SetKeyValue2(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue3"), strTmp))
			{
				configInfo.SetKeyValue3(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyType"), strTmp))
			{
				configInfo.SetKeyType(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyString"), strTmp))
			{
				configInfo.SetKeyString(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyName"), strTmp))
			{
				configInfo.SetKeyName(strTmp);
			}
			if (Rs.GetFieldValue(_T("Description"), strTmp))
			{
				configInfo.SetDescription(strTmp);
			}
			ConfigArray.Add(configInfo);

			Rs.MoveNext();
		}

		bRet = TRUE;

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::AddSystemConfig(CConfigInfo & configInfo)
{
	BOOL bRet = FALSE;

	INT nMaxKeyId = 0;
	GetMaxKeyId(nMaxKeyId);

	CString strSQL;
	strSQL.Format(_T("select * from systemconfig where keystring='%s'"), configInfo.GetKeyString());

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 0)
		{
			if (configInfo.GetKeyType().IsEmpty())
			{
				configInfo.SetKeyType(_T("others"));
			}

			if (configInfo.GetKeyName().IsEmpty())
			{
				configInfo.SetKeyName(configInfo.GetKeyString());
			}

			Rs.AddNew();
			Rs.SetFieldValue(_T("KeyID"), nMaxKeyId);
			Rs.SetFieldValue(_T("KeyName"), configInfo.GetKeyName());
			Rs.SetFieldValue(_T("KeyValue1"), configInfo.GetKeyValue1());
			Rs.SetFieldValue(_T("KeyValue2"), configInfo.GetKeyValue2());
			Rs.SetFieldValue(_T("KeyValue3"), configInfo.GetKeyValue3());
			Rs.SetFieldValue(_T("KeyType"), configInfo.GetKeyType());
			Rs.SetFieldValue(_T("KeyString"), configInfo.GetKeyString());
			Rs.SetFieldValue(_T("Description"), configInfo.GetDescription());

			bRet = Rs.Update();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetSystemConfig(CConfigInfo & configInfo)
{
	BOOL bRet = FALSE;

	CString strSQL;

	strSQL.Format(_T("select * from systemconfig where keystring='%s'"), configInfo.GetKeyString());

	configInfo.Clear();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			INT nTmp = 0; CString strTmp;

			if (Rs.GetFieldValue(_T("KeyID"), nTmp))
			{
				configInfo.SetKeyID(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue1"), nTmp))
			{
				configInfo.SetKeyValue1(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue2"), nTmp))
			{
				configInfo.SetKeyValue2(nTmp);
			}
			if (Rs.GetFieldValue(_T("KeyValue3"), strTmp))
			{
				configInfo.SetKeyValue3(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyType"), strTmp))
			{
				configInfo.SetKeyType(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyString"), strTmp))
			{
				configInfo.SetKeyString(strTmp);
			}
			if (Rs.GetFieldValue(_T("KeyName"), strTmp))
			{
				configInfo.SetKeyName(strTmp);
			}
			if (Rs.GetFieldValue(_T("Description"), strTmp))
			{
				configInfo.SetDescription(strTmp);
			}

			bRet = TRUE;
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::UpdateSystemConfig(CConfigInfo & configInfo, UINT whichKey /*= 1*/)
{
	BOOL bRet = FALSE;

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	BOOL bExists = FALSE;

	CString strSQL;
	
	strSQL.Format(_T("select * from systemconfig where keystring='%s'"), configInfo.GetKeyString());

	if (Rs.Open(strSQL))
	{
		bExists = Rs.GetRecordCount() > 0;
		Rs.Close();
	}
	else
	{
		pDb->Release();

		return bRet;
	}

	pDb->Release();

	if (bExists)
	{
		strSQL = _T("update systemconfig ");

		switch (whichKey)
		{
		case 1:
			strSQL.AppendFormat(_T("set keyvalue1=%d"), configInfo.GetKeyValue1());
			break;

		case 2:
			strSQL.AppendFormat(_T("set keyvalue2=%d"), configInfo.GetKeyValue2());
			break;

		case 3:
			strSQL.AppendFormat(_T("set keyvalue3='%s'"), configInfo.GetKeyValue3());
			break;

		case 9:
			strSQL.AppendFormat(_T("set keyvalue1=%d, keyvalue2=%d, keyvalue3='%s', description='%s'"), 
				configInfo.GetKeyValue1(), configInfo.GetKeyValue2(), configInfo.GetKeyValue3(), configInfo.GetDescription());
			break;

		default:
			strSQL.AppendFormat(_T("set keyvalue1=%d, keyvalue2=%d, keyvalue3='%s'"), 
				configInfo.GetKeyValue1(), configInfo.GetKeyValue2(), configInfo.GetKeyValue3());
			break;
		}

		strSQL.AppendFormat(_T(" where keystring='%s'"), configInfo.GetKeyString());

		bRet = ExecuteSQL(strSQL);
	}
	else
	{
		bRet = AddSystemConfig(configInfo);
	}

	return bRet;
}

BOOL CIBADAL::GetAllIDTypeInfo(CIDTypeInfoArray& IdTypeInfos)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("select * from idtype"));

	IdTypeInfos.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CIDTypeInfo IDTypeInfo;

			CString strTmp;

			if (Rs.GetFieldValue(_T("name"), strTmp))
			{
				IDTypeInfo.SetIDTypeName(strTmp);
			}

			INT nTmp = 0;

			if (Rs.GetFieldValue(_T("id"), nTmp))
			{
				IDTypeInfo.SetIDTypeID(nTmp);
			}

			IdTypeInfos.Add(IDTypeInfo);

			bRet = TRUE;

			Rs.MoveNext();
		}
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetAllNetBarInfo(CNetBarInfoArray& NetBarInfoArray)
{
	NetBarInfoArray.RemoveAll();

	BOOL bRet = FALSE;

	CString strSQL(_T("select * from netbarinfo"));

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		while (!Rs.IsEOF())
		{
			CNetBarInfo netbarInfo;

			CString strTmp; INT nTmp = 0;

			if (Rs.GetFieldValue(_T("netbarid"), nTmp))
			{
				netbarInfo.SetNetBarID(nTmp);
			}

			if (Rs.GetFieldValue(_T("netBarCaption"), strTmp))
			{
				netbarInfo.SetName(strTmp);
			}

			if (Rs.GetFieldValue(_T("netBarName"), strTmp))
			{
				netbarInfo.SetEnglistName(strTmp);
			}

			if (Rs.GetFieldValue(_T("address"), strTmp))
			{
				netbarInfo.SetAddress(strTmp);
			}

			if (Rs.GetFieldValue(_T("manager"), strTmp))
			{
				netbarInfo.SetManager(strTmp);
			}

			if (Rs.GetFieldValue(_T("openDate"), strTmp))
			{
				netbarInfo.SetOpenDate(strTmp);
			}

			if (Rs.GetFieldValue(_T("parentNetBar"), strTmp))
			{
				netbarInfo.SetParentNetBar(strTmp);
			}

			if (Rs.GetFieldValue(_T("pcNum"), nTmp))
			{
				netbarInfo.SetPCNumber(nTmp);
			}

			if (Rs.GetFieldValue(_T("tel"), strTmp))
			{
				netbarInfo.SetTel(strTmp);
			}

			if (Rs.GetFieldValue(_T("netBarType"), nTmp))
			{
				netbarInfo.SetNetBarType(nTmp);
			}

			NetBarInfoArray.Add(netbarInfo);

			bRet = TRUE;

			Rs.MoveNext();
		}
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetAllPCClassInfo(CIDNameRecordArray& IDNameRecords)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("select * from pcclass"));

	IDNameRecords.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CIDNameRecord IDNameRecord;

			CString strTmp;

			if (Rs.GetFieldValue(_T("name"), strTmp))
			{
				IDNameRecord.SetName(strTmp);
			}

			INT nTmp = 0;

			if (Rs.GetFieldValue(_T("id"), nTmp))
			{
				IDNameRecord.SetID(nTmp);
			}

			IDNameRecords.Add(IDNameRecord);

			bRet = TRUE;

			Rs.MoveNext();
		}
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::GetAllUserClassInfos(CUserClassInfoArray & UserClassInfoRecords)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("select * from memberclass"));

	UserClassInfoRecords.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CUserClassInfo UserClassInfo;

			CString strTmp;

			INT nTmp = 0;

			if (Rs.GetFieldValue(_T("classId"), nTmp))
			{
				UserClassInfo.SetClassID(nTmp);
			}

			if (Rs.GetFieldValue(_T("className"), strTmp))
			{
				UserClassInfo.SetClassName(strTmp);
			}

			if (Rs.GetFieldValue(_T("costExpense"), nTmp))
			{
				UserClassInfo.SetCostExpense(nTmp);
			}

			if (Rs.GetFieldValue(_T("deposit"), nTmp))
			{
				UserClassInfo.SetDeposit(nTmp);
			}

			if (Rs.GetFieldValue(_T("isMember"), nTmp))
			{
				UserClassInfo.SetIsMember(nTmp);
			}

			UserClassInfoRecords.Add(UserClassInfo);

			bRet = TRUE;

			Rs.MoveNext();
		}
	}

	pDb->Release();

	return bRet;
}
BOOL CIBADAL::GetShiftCash(CShiftCashArray& ShiftCashArray, CString strStartTime, CString strEndTime, CString strCashier)
{
	BOOL bRet = FALSE;

	ShiftCashArray.RemoveAll();

	strStartTime.Trim();
	strEndTime.Trim();
	strCashier.Trim();

	CString strSQL;

	strSQL.Format(_T("select * from shiftcash where EndDate between '%s' and '%s'"), strStartTime, strEndTime);

	if (!strCashier.IsEmpty())
	{
		strSQL.AppendFormat(_T(" and operator = '%s'"), strCashier);
	}

	strSQL.Append(_T(" order by id desc"));

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		while (!Rs.IsEof())
		{
			INT nTmp = 0;
			CString strTmp;

			CShiftCash ShiftCash;

			Rs.GetFieldValue(_T("ID"), nTmp);
			ShiftCash.SetID(nTmp);
			Rs.GetFieldValue(_T("StartDate"), strTmp);
			ShiftCash.SetStartDate(strTmp);
			Rs.GetFieldValue(_T("StandbyAmount"), nTmp);
			ShiftCash.SetStandbyAmount(nTmp);
			Rs.GetFieldValue(_T("Deposit"), nTmp);
			ShiftCash.SetDeposit(nTmp);
			Rs.GetFieldValue(_T("CreditAmount"), nTmp);
			ShiftCash.SetCreditAmount(nTmp);
			Rs.GetFieldValue(_T("PresentAmount"), nTmp);
			ShiftCash.SetPresentAmount(nTmp);
			Rs.GetFieldValue(_T("ReturnAmount"), nTmp);
			ShiftCash.SetReturnAmount(nTmp);
			Rs.GetFieldValue(_T("CardSoldAmount"), nTmp);
			ShiftCash.SetCardSoldAmount(nTmp);
			Rs.GetFieldValue(_T("CashSoldAmount"), nTmp);
			ShiftCash.SetCashSoldAmount(nTmp);
			Rs.GetFieldValue(_T("CardCreditedAmount"), nTmp);
			ShiftCash.SetCardCreditedAmount(nTmp);
			Rs.GetFieldValue(_T("TotalAmount"), nTmp);
			ShiftCash.SetTotalAmount(nTmp);
			Rs.GetFieldValue(_T("CostExpense"), nTmp);
			ShiftCash.SetCostExpense(nTmp);
			Rs.GetFieldValue(_T("Operator"), strTmp);
			ShiftCash.SetOperator(strTmp);
			Rs.GetFieldValue(_T("NextOperator"), strTmp);
			ShiftCash.SetNextOperator(strTmp);
			Rs.GetFieldValue(_T("Description"), strTmp);
			ShiftCash.SetDescription(strTmp);
			Rs.GetFieldValue(_T("EndDate"), strTmp);
			ShiftCash.SetEndDate(strTmp);
			Rs.GetFieldValue(_T("CashRegisterID"), nTmp);
			ShiftCash.SetCashRegisterID(nTmp);
			//Rs.GetFieldValue(_T("fetchAmount"), nTmp);
			//ShiftCash.SetFetchAmount(nTmp);
			Rs.GetFieldValue(_T("remainAmount"), nTmp);
			ShiftCash.SetRemainAmount(nTmp);
			Rs.GetFieldValue(_T("Payout"), nTmp);
			ShiftCash.SetPayout(nTmp);
			Rs.GetFieldValue(_T("CountAmount"), nTmp);
			ShiftCash.SetCountAmount(nTmp);

			ShiftCashArray.Add(ShiftCash);
			bRet = TRUE;
			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL CIBADAL::AddCashierToDB()
{
	BOOL bRet = FALSE;

	CString strIP, strMAC;
	CIBAHelpper::GetIPAndMAC(strIP, strMAC);

	CString strSQL;
	strSQL.Format(_T("select * from cashregister where ID = %d"), CIBAConfig::GetInstance()->GetCashRegisterID());

	//try
	//{
	CADODBConnInfo* pDb = NULL;

	if (!CIBADAL::GetInstance()->GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) )
	{
		if (Rs.IsEOF())
		{
			bRet = Rs.AddNew();
			if (bRet)
			{
				Rs.SetFieldValue(_T("ID"), CIBAConfig::GetInstance()->GetCashRegisterID());
				Rs.SetFieldValue(_T("NetIdentityManage"), 1);
				bRet = Rs.Update();
			}
			if (!bRet)
			{
				Rs.Close();
				pDb->Release();
				return FALSE;
			}
		}
		Rs.Close();
		pDb->Release();
	}
	else
	{
		pDb->Release();
		return FALSE;
	}

	CString strPwd = theApp.GetCurCashier()->GetPassword();
	CString strPwdMD5;
	CEncrypt::CalcMD5(strPwd, strPwdMD5);
	Rs.SetFieldValue(_T("password"), strPwdMD5);

	strSQL.Format(_T("update cashregister set ComputerIP='%s',Name='%s',dutyID=%d,password='%s' where ID=%d"),
		strIP,theApp.GetCurCashier()->GetName(),theApp.GetCurCashier()->GetDutyID(),strPwdMD5,
		CIBAConfig::GetInstance()->GetCashRegisterID());

	return CIBADAL::ExecuteSQL(strSQL);
	//}
	//catch (...)
	//{
	//	IBA_LOG(_T(" CIBADAL::AddCashierToDB exception!"));
	//	return FALSE;
	//}
}

BOOL CIBADAL::UpdateInternetCafeInfo()
{
	//查询是否有网咖记录，有则更新,无则添加
	BOOL bRet = FALSE;

	//查询是否有记录
	BOOL bExist=FALSE;
	CString sql;
	sql.Format(_T("select * from systemConfig where keyString='InternetCafeAddress'"));

	CADODBConnInfo *pDb = NULL;
	if(!GetDBConnInfo(pDb)){return bRet;}
	CADORecordset Rs(pDb->GetConn());
	if(Rs.Open(sql))
	{
		if(Rs.GetRecordCount()>0)
		{
			bExist = TRUE;
		}
	}
	Rs.Close();
	pDb->Release();

	//有记录,更新,无记录,添加
	CString strSQL;
	CString strKeyType(_T("others"));
	CString strKeyString(_T("InternetCafeAddress"));
	CString strDescription(_T("网咖地址(Keyvalue1=是否有网咖,Keyvalue3=网咖地址)"));
	int nMaxKeyId;
	CIBADAL::GetInstance()->GetMaxKeyId(nMaxKeyId);

	if(bExist)
	{
		strSQL.Format(_T("update systemConfig set KeyValue1 = %d,KeyValue3='%s'where KeyString = '%s'"), 
			CNetBarConfig::GetInstance()->GetIsInternetCafe(),CNetBarConfig::GetInstance()->GetAddressInternetCafe(),strKeyString);
	}
	else
	{
		strSQL.Format(_T("insert into systemConfig (KeyId,KeyType,KeyName,KeyString,KeyValue1,KeyValue3,description) values(%d,'%s','%s','%s',%d,'%s','%s')"), 
			nMaxKeyId,strKeyType,strKeyString,strKeyString,CNetBarConfig::GetInstance()->GetIsInternetCafe(),CNetBarConfig::GetInstance()->GetAddressInternetCafe(),strDescription);
	}

	return CIBADAL::ExecuteSQL(strSQL);
}

//往LocalConsume表中插入新纪录
BOOL NS_DAL::CIBADAL::AddLocalConsume(CLocalConsumeInfo& ConsumeInfo)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select * from LocalConsume where memberId=%d"), (INT)ConsumeInfo.GetMemberId());

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return FALSE;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 0)
		{
			bRet = Rs.AddNew();
		}
		else
		{
			Rs.Edit();
		}
		if (!bRet)           // liyajun note: 如果LocalConsume中存在对应记录，直接关闭数据库Recordset？？？
		{
			Rs.Close();
			pDb->Release();
			return FALSE;
		}

		Rs.SetFieldValue(_T("SubmitTime"), ConsumeInfo.GetSubmitTime());
		Rs.SetFieldValue(_T("NetbarId"), ConsumeInfo.GetNetbarId());
		Rs.SetFieldValue(_T("RefNo"), ConsumeInfo.GetRefNo());
		Rs.SetFieldValue(_T("SerialNo"), ConsumeInfo.GetSerialNo());
		Rs.SetFieldValue(_T("SerialNum"), ConsumeInfo.GetSerialNum());
		Rs.SetFieldValue(_T("Name"), ConsumeInfo.GetName());
		Rs.SetFieldValue(_T("IdNumber"), ConsumeInfo.GetIdNumber());
		Rs.SetFieldValue(_T("MemberId"), ConsumeInfo.GetMemberId());
		Rs.SetFieldValue(_T("CreditAmount"), ConsumeInfo.GetCreditAmount());
		Rs.SetFieldValue(_T("ConsumeAmount"), ConsumeInfo.GetConsumeAmount());
		Rs.SetFieldValue(_T("ReturnAmount"), ConsumeInfo.GetReturnAmount());
		Rs.SetFieldValue(_T("CheckinTime"), ConsumeInfo.GetCheckinTime());
		Rs.SetFieldValue(_T("CheckoutTime"), ConsumeInfo.GetCheckoutTime());
		Rs.SetFieldValue(_T("TermId"), ConsumeInfo.GetTermId());
		Rs.SetFieldValue(_T("Operator"), ConsumeInfo.GetOperator());
		Rs.SetFieldValue(_T("ClassId"), ConsumeInfo.GetClassId());
		Rs.SetFieldValue(_T("PayType"), ConsumeInfo.GetPayType());
		Rs.SetFieldValue(_T("PCClass"), ConsumeInfo.GetPCClass());
		Rs.SetFieldValue(_T("dataStatus"), ConsumeInfo.GetDataStatus());

		Rs.SetFieldValue(_T("CheckData"), ConsumeInfo.GetCheckData());
		
		bRet = Rs.Update();

		Rs.Close();
	}

	pDb->Release();

	return bRet;
}

BOOL NS_DAL::CIBADAL::DeleteLocalConsume(UINT nMemberID)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("delete from LocalConsume where memberId=%d"), (INT)nMemberID);

	return ExecuteSQL(strSQL);
}

void NS_DAL::CIBADAL::GetLocalConsumes(CArray<CLocalConsumeInfo,CLocalConsumeInfo&>& ConsumeInfoArray)
{
	BOOL bRet = FALSE;

	CString strSQL(_T("select * from LocalConsume"));

	ConsumeInfoArray.RemoveAll();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return ;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		for (UINT i = 0; i < Rs.GetRecordCount(); i++)
		{
			CLocalConsumeInfo ConsumeInfo;

			CString strTmp;

			INT nTmp = 0;

			Rs.GetFieldValue(_T("SubmitTime"), ConsumeInfo.SET(SubmitTime));
			Rs.GetFieldValue(_T("NetbarId"), ConsumeInfo.SET(NetbarId));
			Rs.GetFieldValue(_T("RefNo"), ConsumeInfo.SET(RefNo));
			Rs.GetFieldValue(_T("SerialNo"), ConsumeInfo.SET(SerialNo));
			Rs.GetFieldValue(_T("SerialNum"), ConsumeInfo.SET(SerialNum));
			Rs.GetFieldValue(_T("Name"), ConsumeInfo.SET(Name));
			
			Rs.GetFieldValue(_T("IdNumber"), ConsumeInfo.SET(IdNumber));
			Rs.GetFieldValue(_T("MemberId"), ConsumeInfo.SET(MemberId));
			Rs.GetFieldValue(_T("CreditAmount"), ConsumeInfo.SET(CreditAmount));
			Rs.GetFieldValue(_T("ConsumeAmount"), ConsumeInfo.SET(ConsumeAmount));
			Rs.GetFieldValue(_T("ReturnAmount"), ConsumeInfo.SET(ReturnAmount));
			Rs.GetFieldValue(_T("CheckinTime"), ConsumeInfo.SET(CheckinTime));
			Rs.GetFieldValue(_T("CheckoutTime"), ConsumeInfo.SET(CheckoutTime));
			Rs.GetFieldValue(_T("TermId"), ConsumeInfo.SET(TermId));
			Rs.GetFieldValue(_T("TimeConsume"), ConsumeInfo.SET(TimeConsume));
			Rs.GetFieldValue(_T("Operator"), ConsumeInfo.SET(Operator));
			Rs.GetFieldValue(_T("ClassId"), ConsumeInfo.SetClassId());
			Rs.GetFieldValue(_T("PayType"), ConsumeInfo.SetPayType());
			Rs.GetFieldValue(_T("PCClass"), ConsumeInfo.SetPCClass());
			
			CString strCheckData;
			if (!Rs.GetFieldValue(_T("CheckData"), strCheckData))
			{
				Rs.MoveNext();
				continue;
			}

			ConsumeInfo.MakeMD5();
			if (ConsumeInfo.GetCheckData().CompareNoCase(strCheckData) == 0)
			{
				ConsumeInfo.SetDataStatus(0); // 数据正常
			}
			else
			{
				ConsumeInfo.SetDataStatus(1); // 数据被篡改
			}

			ConsumeInfoArray.Add(ConsumeInfo);

			Rs.MoveNext();
		}

		Rs.Close();
	}

	pDb->Release();
}

BOOL NS_DAL::CIBADAL::GetRoomsComputerCount(CRoomInfo& roomInfo)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select count(*) from SM_TermInfo where pcClass=%d"), roomInfo.GetRoomID());

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, roomInfo.SetComputerCount());
		}
		Rs.Close();
	}
	if (!bRead)
	{
		IBA_LOG0(_T("读取包房的终端数量和终端信息失败!"));
		pDb->Release();
		return FALSE;
	}

	return bRead;

	strSQL.Format(_T("select termID from SM_TermInfo where pcClass=%d limit 1"), roomInfo.GetRoomID());

	bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, roomInfo.SetTermID());
			Rs.Close();
		}
	}
	pDb->Release();
	return bRead;
}

BOOL NS_DAL::CIBADAL::GetCostRate( int nWeek, int nHour, int nClassId, int nPCClass , int& nCostRate )
{
	CString strHour;
	strHour.Format(_T("t%d"), nHour);

	CString strSQL;
	if(-1 == nPCClass)
	{
		strSQL.Format(_T("SELECT %s FROM netbarbasepolicy WHERE classId = %d AND  week = %d ORDER BY %s DESC"), 
			strHour, nClassId, nWeek, strHour );
	}
	else
	{
		strSQL.Format(_T("SELECT %s FROM netbarbasepolicy WHERE classId = %d AND  week = %d AND pcClass = %d ORDER BY %s DESC"), 
			strHour, nClassId, nWeek, nPCClass, strHour );
	}

	BOOL bFlag = FALSE;
	//try
	//{
	CADODBConnInfo* pDb = NULL;

	if (CIBADAL::GetInstance()->GetDBConnInfo(pDb)) 
	{
		CADORecordset Rs(pDb->GetConn());

		if (Rs.Open(strSQL) )
		{
			if (0 != Rs.GetRecordCount())
			{				
				Rs.GetFieldValue(strHour, nCostRate);
				bFlag = TRUE;
			}
		}		
		Rs.Close();
		pDb->Release();
	}
	//}
	//catch(...)
	//{
	//	IBA_LOG0(_T("Query deduction rate abnormalities"));
	//	nCostRate = 0;
	//}
	return bFlag;
}

//从costRate表获取扣率信息
CString NS_DAL::CIBADAL::GetCostRateFromCostRate(int nClassId, int nPCClass)
{

	CString strSQL;
	strSQL.Format(_T("select * from CostRate where pcClass = %d and classId = %d"),nPCClass,nClassId);  

	CString strRate;
	//try
	//{

	CADODBConnInfo* pDb = NULL;
	GetDBConnInfo(pDb);
	CADORecordset Rs(pDb->GetConn());

	if (CIBADAL::GetInstance()->GetDBConnInfo(pDb)) 
	{
		CADORecordset Rs(pDb->GetConn());

		if (Rs.Open(strSQL) )
		{
			if (0 != Rs.GetRecordCount())
			{				
				Rs.GetFieldValue(_T("rate"), strRate);
			}
		}		
		Rs.Close();
		pDb->Release();
	}
	//}
	//catch(...)
	//{
	//	IBA_LOG0(_T("从CostRate中获取扣率信息失败"));
	//}
	return strRate;
}

UINT CIBADAL::GetRoundType(UINT nNetbarId)
{
	UINT nRoundType = 0;

	CString strSQL(_T("SELECT netbarInfo.roundType, netbarInfo.netbarId from netbarInfo "));
	strSQL.AppendFormat(_T("where netbarInfo.netbarId = '%d' "), nNetbarId);

	//CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetRoundType:%s"), strSQL);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return nRoundType;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("roundType"), nRoundType);
		}

		Rs.Close();
	}

	pDb->Release();

	return nRoundType;
}

ULONG CIBADAL::GetTableRecordCount(LPCTSTR szTable, LPCTSTR szWhere/* = ""*/)
{
	BOOL bRet = FALSE;

	CString sqlStr,strCount;

	if( NULL != szWhere)
	{
		sqlStr.Format( _T("SELECT count(*) as COUNT FROM %s where %s"),
			szTable,
			szWhere);
	}
	else
	{
		sqlStr.Format( _T("SELECT count(*) as COUNT FROM %s"),
			szTable);
	}

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return 0;

	CADORecordset Rs(pDb->GetConn());
	//try
	//{
	if (Rs.Open(sqlStr))
	{
		while( !Rs.IsEOF() )
		{

			Rs.GetFieldValue( _T("COUNT"), strCount );

			Rs.MoveNext();

			Rs.Close();

			return _ttol(strCount);
		}

		Rs.Close();
	}
	//}
	//catch(...)
	//{
	//	IBA_LOG(_T("数据库异常QueryIP_TermID(): %d"), GetLastError());
	//}
	return 0;
}

BOOL NS_DAL::CIBADAL::GetIPFromTermId(LPCTSTR szTermId, CString &strIP)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select staticIp from SM_TermInfo where termID='%s' limit 1"), szTermId);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, strIP);
		}
		Rs.Close();
	}

	if (!bRead)
	{
		IBA_LOG(_T("读取终端ID%s的IP失败"), szTermId);
		pDb->Release();
		return FALSE;
	}
	pDb->Release();

	return bRead;
	
}

BOOL NS_DAL::CIBADAL::AddZdRnInfo( LPCTSTR szSN, LPCTSTR szTermId, LPCTSTR szUserName )
{
	DelZdRnInfo(szSN);	// 删除原有数据

	CString strSQL;
	strSQL = _T("insert into ZdRealName(serialNum, terminalId, updatetime, UserName");
	strSQL.Append(_T(") values("));

	strSQL.AppendFormat(_T("'%s',"), szSN);
	strSQL.AppendFormat(_T("'%s',"), szTermId);

	CTime tm = CTime::GetCurrentTime();
	CString strTime; 
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
		tm.GetYear(),tm.GetMonth(), tm.GetDay(), 
		tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	strSQL.AppendFormat(_T("'%s',"), strTime);

	strSQL.AppendFormat(_T("'%s'"), szUserName);

	strSQL.Append(_T(")"));
	
	return ExecuteSQL(strSQL);
}

BOOL NS_DAL::CIBADAL::DelZdRnInfo( LPCTSTR szSN )
{
	CString strSQL;

	//先删除同一个账号的记录，可能是以前退款没有成功删除掉的
	strSQL.Format(_T("delete from ZdRealName where serialNum = '%s'"), szSN);
	return ExecuteSQL(strSQL);
}	

BOOL NS_DAL::CIBADAL::GetZdRnInfo( LPCTSTR szSN, CString &strTermId, COleDateTime& updateTime )
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select terminalId, updatetime from ZdRealName where serialNum='%s' limit 1"), szSN);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, strTermId);
			bRead = Rs.GetFieldValue(1, updateTime);
		}
		Rs.Close();
	}

	if (!bRead)
	{
		pDb->Release();
		return FALSE;
	}
	pDb->Release();

	return bRead;
}

BOOL NS_DAL::CIBADAL::GetHaveChange( LPCTSTR szSN, int& HaveChange )
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select HaveChange from ZdRealName where serialNum='%s' limit 1"), szSN);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, HaveChange);
		}
		Rs.Close();
	}

	if (!bRead)
	{
		pDb->Release();
		return FALSE;
	}
	pDb->Release();

	return bRead;
}

BOOL NS_DAL::CIBADAL::SetHaveChange( LPCTSTR szSN, int HaveChange )
{
	CString strSQL;
	strSQL.Format(_T("UPDATE ZdRealName  SET HaveChange = %d WHERE serialNum='%s'"),
		HaveChange, szSN);

	return ExecuteSQL(strSQL);
	
}
BOOL NS_DAL::CIBADAL::GetZdRnInfoByTermId( LPCTSTR szTermId, CString &strSN, CString &strName )
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select serialNum,UserName from ZdRealName where terminalId='%s' limit 1"), szTermId);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(strSQL))
	{
		if (Rs.GetRecordCount() == 1)
		{
			bRead = Rs.GetFieldValue(0, strSN);
			bRead = Rs.GetFieldValue(1, strName);
		}
		Rs.Close();
	}

	if (!bRead)
	{
		pDb->Release();
		return FALSE;
	}
	pDb->Release();

	return bRead;
}

void NS_DAL::CIBADAL::DelZdRnOutDateData()
{
	// 删除 超过两天的数据

	CString strSql, strInvalidTime;

	__int64 nTime = CTime::GetCurrentTime().GetTime();
	nTime -= 60*60*24*2;
	CTime tm(nTime);
	strInvalidTime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"), 
		tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	strSql.Format(_T("delete from ZdRealName ")
		_T("where updatetime < '%s'"), strInvalidTime);

	ExecuteSQL(strSql);
}

/*************************************************
Function:       // CountOfRecordsWithSQL
Description:    // 获取结果的数目
Input:          // 查询语句strSQL
Return:         // 结果数目
*************************************************/
int NS_DAL::CIBADAL::CountOfRecordsWithSQL( CString strSQL )
{
	int iCount = -1;	

	CADODBConnInfo* pDb = NULL;
	if (!GetDBConnInfo(pDb)) 
		return FALSE;
	CADORecordset Rs(pDb->GetConn());

	BOOL bRead = FALSE;

	if (Rs.Open(strSQL))
	{
		iCount = 0;
		if (Rs.GetRecordCount() == 1) 
		{
			unsigned int cc = 0;
			Rs.GetFieldValue((int)0, cc);//获取第一个字段的值存入到iCount中
			iCount = (int)cc;
		}
		
		Rs.Close();
	}

	if (!bRead)        //一定是等于FALSE. 为何这么写？？
	{
		pDb->Release();
		return iCount;
	}
	pDb->Release();

	return iCount;
}

int NS_DAL::CIBADAL::GetPCClassIDWithTermID( LPCTSTR lpszTermID )
{
	CString sql;
	sql.Format(_T("select pcClass from sm_terminfo where termId='%s'"), lpszTermID);
	int nPcClass = 0;


	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return FALSE;

	CADORecordset Rs(pDb->GetConn());
	BOOL bRead = FALSE;
	if (Rs.Open(sql))
	{
		if (Rs.GetRecordCount() > 0) {
			Rs.GetFieldValue((int)0, nPcClass);
		}

		Rs.Close();
	}

	if (!bRead)
	{
		pDb->Release();
		return nPcClass;
	}
	pDb->Release();

	return nPcClass;
}

GxxArrayPtr NS_DAL::CIBADAL::ReadRecordsFromDB( LPCTSTR lpszSql)
{
	GxxArrayPtr recordsPtr = GxxArray::create();

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) 
		return recordsPtr;

	CADORecordset Rs(pDb->GetConn());
	if (Rs.Open(lpszSql))
	{
		if (Rs.GetRecordCount() > 0) {
			long nFieldCount = Rs.GetFieldCount();
			CArray<CString> arr;
			for (long i = 0; i < nFieldCount; i++) {
				CADOFieldInfo finfo;
				Rs.GetFieldInfo(i, &finfo);
				arr.Add(finfo.m_strName);
			}
			long nRecordCount = Rs.GetRecordCount();
			for (long i = 0; i < nRecordCount; i++) {
				GxxDictionaryPtr record = GxxDictionary::create();
				for (long k = 0; k < arr.GetCount(); k++) {
					CString& key = arr.GetAt(k);
					CT2A taKey(key);
					CString strValue;
					Rs.GetFieldValue(key, strValue);
					std::string taValue = G2XUtility::CG2XCommonTool::WideChar_to_Utf8( std::wstring(strValue.GetBuffer()));
					strValue.ReleaseBuffer();
					record->setValueForKey(GxxValue::create(taValue.c_str()), (const char*)taKey);
				}
				recordsPtr->addValue(GxxValue::create(record));
				Rs.MoveNext();
			}
		}
		Rs.Close();
	}

	pDb->Release();
	return recordsPtr;
}

GxxDictionaryPtr NS_DAL::CIBADAL::ReadRecordFromDB( LPCTSTR lpszSql )
{
	GxxArrayPtr tmpArrayPtr = ReadRecordsFromDB(lpszSql);
	if (tmpArrayPtr->count() > 0) 
	{
		return tmpArrayPtr->valueAtIndex(0)->getDictionary();
	}
	return GxxDictionaryPtr();
}
/*************************************************
Function:       //CreateNewLocalMemberFromActiveMember
Description:    //
Calls:          //
Table Accessed: //
Table Updated:  //
Input:          //
                //
Output:         //
Return:         //
Others:         //
*************************************************/
GxxDictionaryPtr NS_DAL::CIBADAL::CreateNewLocalMemberFromActiveMember(CActiveMember &ActiveMember, LPCTSTR lpszTermID/* = NULL*/)
{
	UINT nMemberID = ActiveMember.GetMemberID();
	if(nMemberID == 0)
	{
		IBA_LOG(_T("Abnormal. CIBADAL::CreateNewLocalMemberFromActiveMember will return NULL, because MemberId = 0"));
		return NULL;
	}
	IBA_LOG(_T("CIBADAL::CreateNewLocalMemberFromActiveMember Start - For MemberId = %d"),nMemberID);
	GxxDictionaryPtr memberPtr;
	memberPtr = GxxDictionary::create();	

	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetMemberID()), "memberId");	//memberId = memberId
	CStringA AstrNetId(ActiveMember.GetNetId());                                                //netId = NetId 
	memberPtr->setValueForKey(GxxValue::create(AstrNetId.GetBuffer()), "netId");	
	CStringA APersonalID(ActiveMember.GetPersonalID());
	memberPtr->setValueForKey(GxxValue::create(APersonalID.GetBuffer()), "idNumber");	        //idNumber = PersonalID
	CStringA AstrMemberName(ActiveMember.GetUserName());
	memberPtr->setValueForKey(GxxValue::create(AstrMemberName.GetBuffer()), "memberName");		//memberName = UserName
	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetAvailavleBalance()), "balance");	//balance = AvailavleBalance	
	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetAmount()), "beforeUsed");	//beforeUsed = Amount

	COleDateTime CheckinTime = ActiveMember.GetCheckInTime();
	CTime Cintime(CheckinTime.GetYear(),CheckinTime.GetMonth(),CheckinTime.GetDay(),CheckinTime.GetHour(),CheckinTime.GetMinute(),CheckinTime.GetSecond());
	time_t tmCheckin = Cintime.GetTime();
	memberPtr->setValueForKey(GxxValue::create((long)tmCheckin), "checkinTime");	            //checkinTime = CheckInTime

	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetNextCheckinTime()), "nextCheckinTime"); //nextCheckinTime = NextCheckinTime	

	time_t tmNow = time(NULL);
	memberPtr->setValueForKey(GxxValue::create((long)tmNow), "lastUpdateTime");	                 //lastUpdateTime = 当前时间

	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetAvailavleBalance()), "totalMoney");	//totalMoney = 	AvailavleBalance
	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetPCClassID()), "pcClass");	//pcClass = PCClassID	
	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetClassId()), "classId");	//classId = ClassId

	if (NULL == lpszTermID)
	{
		CStringA AstrTerminalID(ActiveMember.GetTerminalID());
		memberPtr->setValueForKey(GxxValue::create(AstrTerminalID.GetBuffer()),"termId");	   //termId = 传进来的termId(如果没有就等于TerminalID)
	}
	else
	{
		CStringA strTermId = CT2A(lpszTermID);
		memberPtr->setValueForKey(GxxValue::create(strTermId.GetBuffer()), "termId");
		strTermId.ReleaseBuffer();			
	}

	memberPtr->setValueForKey(GxxValue::create((long)0), "isReturned");                         //isReturned = 0


	//2015-0910 liyajun 从CostRate中取扣率信息
	CString strRate;
	CString strSQL;
	int nPCClass = ActiveMember.GetPCClassID();
	int nclassId = ActiveMember.GetClassId();
	strSQL.Format(_T("select * from CostRate where pcClass = %d and classId = %d"),nPCClass,nclassId);  

	CADODBConnInfo* pDb = NULL;
	GetDBConnInfo(pDb);
	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL) && Rs.GetRecordCount() > 0)
	{
		Rs.GetFieldValue(_T("rate"), strRate);
		Rs.Close();
	}

	pDb->Release();		

	CStringA strRateA = CT2A(strRate);
	memberPtr->setValueForKey(GxxValue::create(strRateA.GetBuffer()), "rate");                //rate = 用pcclass和classId到CostRate表中查找的结果

	strRateA.ReleaseBuffer();
	//memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("policyInfo")), "rate");
	memberPtr->setValueForKey(GxxValue::create((long)ActiveMember.GetPayType()), "payType");  //payType = PayType

	BOOL bRelt = FALSE;

	if(memberPtr)
	{
		bRelt = TRUE;
	}
	IBA_LOG(_T("CIBADAL::CreateNewLocalMemberFromActiveMember Result - %d , For MemberId = %d"),(int)bRelt,nMemberID);

	return memberPtr;
}
GxxDictionaryPtr NS_DAL::CIBADAL::CreateNewLocalMemberWithMemberID( UINT nMemberID, LPCTSTR lpszTermID /* = NULL */ )
{
	GxxDictionaryPtr memberPtr;

	CString sql;
	sql.Format(_T("select * from activemember where memberId=%d"), nMemberID);
	GxxDictionaryPtr tmpDic = CIBADAL::GetInstance()->ReadRecordFromDB(sql.GetBuffer());
		if(tmpDic == NULL){IBA_LOG(_T("CreateNewLocalMemberWithMemberID: tmpDic is Invalid"));}
	sql.ReleaseBuffer();
	
	if (tmpDic) 
	{
		memberPtr = GxxDictionary::create();		
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after create"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("memberId")), "memberId");//memberId = memberId	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set memberId"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("netId")), "netId");	//netId = NetId 
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set netId"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("idNumber")), "idNumber");//idNumber = PersonalID	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set idNumber"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("name")), "memberName");	//memberName = name	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set memberName"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("lastRemainAmount")), "balance");	//balance = lastRemainAmount	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set balance"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("amount")), "beforeUsed");	//beforeUsed = Amount
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set beforeused"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("checkinTime")), "checkinTime");	 //checkinTime = CheckInTime	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set checkinTime"));}
		memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("nextCheckinTime")), "nextCheckinTime");//nextCheckinTime = NextCheckinTime
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set nextCheckinTime"));}
		
		time_t tmNow = time(NULL);
		memberPtr->setValueForKey(GxxValue::create((long)tmNow), "lastUpdateTime");	 //lastUpdateTime = 当前时间
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set lastUpdateTime"));}
		memberPtr->setValueForKey(GxxValue::create((long)tmpDic->valueForKey("lastRemainAmount")->intValue()), "totalMoney");//totalMoney = 	AvailavleBalance
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set totalMoney"));}
		memberPtr->setValueForKey(GxxValue::create((long)tmpDic->valueForKey("pcClass")->intValue()), "pcClass");	//pcClass = PCClassID	
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set pcClass"));}
		memberPtr->setValueForKey(GxxValue::create((long)tmpDic->valueForKey("classId")->intValue()), "classId");	//classId = ClassId
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set classId"));}
		
		if (NULL == lpszTermID)
		{
			memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("termId")), "termId");//termId = 传进来的termId(如果没有就等于TerminalID)
		}
		else
		{
			CStringA strTermId = CT2A(lpszTermID);
			memberPtr->setValueForKey(GxxValue::create(strTermId.GetBuffer()), "termId");
			strTermId.ReleaseBuffer();			
		}
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set termId"));}
        memberPtr->setValueForKey(GxxValue::create((long)0), "isReturned");
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set isReturned"));}		

		//2015-0910 liyajun 从CostRate中取扣率信息
		int nPCClass = tmpDic->valueForKey("pcClass")->intValue();
		int nclassId = tmpDic->valueForKey("classId")->intValue();
		CString strRate;
		//当PCClass为0时，表示未上机，Rate应设置为空
		if(0 == nPCClass)
		{
			IBA_LOG(_T("用户未上机,无扣率信息"));
			//strRate = _T("");
		}
		//else
		//{
		strRate = GetCostRateFromCostRate(nclassId,nPCClass);//从表中取扣率，没有则取到空
		IBA_LOG(_T("Rate from costrate:%s"),strRate);
		//}		

		if(strRate.GetLength() != 96)
		{
			if(0 != nPCClass)
			{
				//由于无法创建记录，就无法进行本地充值等操作，先改为允许无效的扣率
				IBA_LOG(_T("rate中读取扣率信息有误(ClassId:%d,PCClass:%d))"),nclassId,nPCClass);
			}
			//return NULL;
		}

		CStringA strRateA = CT2A(strRate);
		memberPtr->setValueForKey(GxxValue::create(strRateA.GetBuffer()), "rate");
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set rate"));}
		strRateA.ReleaseBuffer();
		//memberPtr->setValueForKey(GxxValuePtr(tmpDic->valueForKey("policyInfo")), "rate");
		memberPtr->setValueForKey(GxxValue::create((long)tmpDic->valueForKey("payType")->intValue()), "payType");
			if(memberPtr == NULL){IBA_LOG(_T("memberPtr is invalid after set payType"));}

	}
	BOOL bRelt = FALSE;
	if(memberPtr == NULL)
	{
		IBA_LOG(_T("Error!创建LocalMember出错,memberPtr is NULL"));
	}
	else
	{
		bRelt = TRUE;
	}
	return memberPtr;
}

BOOL NS_DAL::CIBADAL::InsertNewLocalMember( GxxDictionaryPtr memberPtr )
{
	CString sql;
	sql = _T("insert into localMember (memberId,netId,balance,beforeUsed,checkinTime,lastUpdateTime,nextCheckinTime,totalMoney,pcClass,classId,termId,memberName,idNumber,rate,payType,isReturned) values(");
	sql.Append(CA2T(memberPtr->valueForKey("memberId")->stringValue()));sql.Append(_T(",'"));
	sql.Append(StringValueForKey("netId", memberPtr));sql.Append(_T("',"));
	sql.Append(CA2T(memberPtr->valueForKey("balance")->stringValue()));sql.Append(_T(","));
	sql.Append(CA2T(memberPtr->valueForKey("beforeUsed")->stringValue()));sql.Append(_T(",'"));
	sql.Append(CA2T(memberPtr->valueForKey("checkinTime")->stringValue()));sql.Append(_T("','"));
	sql.Append(CA2T(memberPtr->valueForKey("lastUpdateTime")->stringValue()));sql.Append(_T("',"));
	sql.Append(CA2T(memberPtr->valueForKey("nextCheckinTime")->stringValue()));sql.Append(_T(","));
	sql.Append(CA2T(memberPtr->valueForKey("totalMoney")->stringValue()));sql.Append(_T(","));
	sql.Append(CA2T(memberPtr->valueForKey("pcClass")->stringValue()));sql.Append(_T(","));
	sql.Append(CA2T(memberPtr->valueForKey("classId")->stringValue()));sql.Append(_T(",'"));
	sql.Append(StringValueForKey("termId", memberPtr));sql.Append(_T("','"));
	sql.Append(StringValueForKey("memberName", memberPtr));sql.Append(_T("','"));
	sql.Append(CA2T(memberPtr->valueForKey("idNumber")->stringValue()));sql.Append(_T("','"));
	sql.Append(CA2T(memberPtr->valueForKey("rate")->stringValue()));sql.Append(_T("',"));
	sql.Append(CA2T(memberPtr->valueForKey("payType")->stringValue()));sql.Append(_T(","));
	sql.Append(CA2T(memberPtr->valueForKey("isReturned")->stringValue()));sql.Append(_T(")"));
	
	BOOL bRet;
	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) > 0)
	{
		bRet = TRUE;
	}
	else
	{

		IBA_LOG(_T("Error!插入LocalMember出错"));
	}

	return bRet;
}

/*************************************************
Function:       //GetLocalMember
Description:    //从LocalMember表中获取表内容
Table Accessed: //LocalMember
Input:          //nMemberId - 待查询的用户ID
Output:         //LocalMember - 输出的表内容
Return:         //查询到结果返回true.
*************************************************/
BOOL NS_DAL::CIBADAL::GetLocalMemberByMemberID(int nMemberId, CLocalMember& LocalMember)
{
	BOOL bRet = false;

	CString strSQL;
	strSQL.Format(_T("select * from localMember where memberId = %d"),nMemberId);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("memberId"),LocalMember.nMemberId);
			Rs.GetFieldValue(_T("Balance"),LocalMember.nBalance);
			Rs.GetFieldValue(_T("totalMoney"),LocalMember.ntotalMoney);
			Rs.GetFieldValue(_T("BeforeUsed"),LocalMember.nBeforeUsed);
			Rs.GetFieldValue(_T("MemberName"),LocalMember.strMemberName);
			Rs.GetFieldValue(_T("NetId"),LocalMember.strNetId);
			Rs.GetFieldValue(_T("IdNumber"),LocalMember.strIdNumber);
			Rs.GetFieldValue(_T("checkinTime"),LocalMember.checkinTime);
			Rs.GetFieldValue(_T("NextCheckinTime"),LocalMember.checkinTime);
			Rs.GetFieldValue(_T("TermId"),LocalMember.strTermId);
			Rs.GetFieldValue(_T("PcClass"),LocalMember.nPcClass);
			Rs.GetFieldValue(_T("ClassId"),LocalMember.nClassId);
			Rs.GetFieldValue(_T("PayType"),LocalMember.nPayType);
			Rs.GetFieldValue(_T("localCheckinTime"),LocalMember.localCheckinTime);
			Rs.GetFieldValue(_T("IsLocalCheckin"),LocalMember.nIsLocalCheckin);
			Rs.GetFieldValue(_T("LastUpdateTime"),LocalMember.nLastUpdateTime);
			Rs.GetFieldValue(_T("checkoutTime"),LocalMember.checkoutTime);
			Rs.GetFieldValue(_T("IsReturned"),LocalMember.nIsReturned);
			Rs.GetFieldValue(_T("Rate"),LocalMember.strRate);

			bRet = true;
		}
		Rs.Close();
	}
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetLocalMember
Description:    //从LocalMember表中获取表内容
Table Accessed: //LocalMember
Input:          //strTermID - 待查询的用户的TermID
Output:         //LocalMember - 输出的表内容
Return:         //查询到结果返回true.
*************************************************/
BOOL NS_DAL::CIBADAL::GetLocalMemberByTermID(CString strTermID,CLocalMember& LocalMember)
{
	BOOL bRet = false;

	CString strSQL;
	strSQL.Format(_T("select * from localMember where termId = '%s'"),strTermID);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("memberId"),LocalMember.nMemberId);
			Rs.GetFieldValue(_T("Balance"),LocalMember.nBalance);
			Rs.GetFieldValue(_T("totalMoney"),LocalMember.ntotalMoney);
			Rs.GetFieldValue(_T("BeforeUsed"),LocalMember.nBeforeUsed);
			Rs.GetFieldValue(_T("MemberName"),LocalMember.strMemberName);
			Rs.GetFieldValue(_T("NetId"),LocalMember.strNetId);
			Rs.GetFieldValue(_T("IdNumber"),LocalMember.strIdNumber);
			Rs.GetFieldValue(_T("checkinTime"),LocalMember.checkinTime);
			Rs.GetFieldValue(_T("NextCheckinTime"),LocalMember.checkinTime);
			Rs.GetFieldValue(_T("TermId"),LocalMember.strTermId);
			Rs.GetFieldValue(_T("PcClass"),LocalMember.nPcClass);
			Rs.GetFieldValue(_T("ClassId"),LocalMember.nClassId);
			Rs.GetFieldValue(_T("PayType"),LocalMember.nPayType);
			Rs.GetFieldValue(_T("localCheckinTime"),LocalMember.localCheckinTime);
			Rs.GetFieldValue(_T("IsLocalCheckin"),LocalMember.nIsLocalCheckin);
			Rs.GetFieldValue(_T("LastUpdateTime"),LocalMember.nLastUpdateTime);
			Rs.GetFieldValue(_T("checkoutTime"),LocalMember.checkoutTime);
			Rs.GetFieldValue(_T("IsReturned"),LocalMember.nIsReturned);
			Rs.GetFieldValue(_T("Rate"),LocalMember.strRate);

			bRet = true;
		}
		Rs.Close();
	}
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetAllLocalMember
Description:    //从LocalMember表中获取所有元素
Table Accessed: //LocalMember
Output:         //lMembers - LocalMember的vector
Return:         //查询到结果且数量>0返回true.
*************************************************/
BOOL CIBADAL::GetAllLocalMember(LocalMembers& lMembers)
{
	BOOL bRet = FALSE;
	lMembers.clear();

	CADODBConnInfo *pDb = NULL;
	if(!GetDBConnInfo(pDb)){return bRet;}
	CADORecordset Rs(pDb->GetConn());
	CString sql;
	sql.Format(_T("select * from localMember"));
	if(Rs.Open(sql))
	{
		if(Rs.GetRecordCount()>0)
		{
			while(!Rs.IsEOF())
			{
				CLocalMember LocalMember;
				Rs.GetFieldValue(_T("memberId"),LocalMember.nMemberId);
				Rs.GetFieldValue(_T("Balance"),LocalMember.nBalance);
				Rs.GetFieldValue(_T("totalMoney"),LocalMember.ntotalMoney);
				Rs.GetFieldValue(_T("BeforeUsed"),LocalMember.nBeforeUsed);
				Rs.GetFieldValue(_T("MemberName"),LocalMember.strMemberName);
				Rs.GetFieldValue(_T("NetId"),LocalMember.strNetId);
				Rs.GetFieldValue(_T("IdNumber"),LocalMember.strIdNumber);
				Rs.GetFieldValue(_T("checkinTime"),LocalMember.checkinTime);
				Rs.GetFieldValue(_T("NextCheckinTime"),LocalMember.checkinTime);
				Rs.GetFieldValue(_T("TermId"),LocalMember.strTermId);
				Rs.GetFieldValue(_T("PcClass"),LocalMember.nPcClass);
				Rs.GetFieldValue(_T("ClassId"),LocalMember.nClassId);
				Rs.GetFieldValue(_T("PayType"),LocalMember.nPayType);
				Rs.GetFieldValue(_T("localCheckinTime"),LocalMember.localCheckinTime);
				Rs.GetFieldValue(_T("IsLocalCheckin"),LocalMember.nIsLocalCheckin);
				Rs.GetFieldValue(_T("LastUpdateTime"),LocalMember.nLastUpdateTime);
				Rs.GetFieldValue(_T("checkoutTime"),LocalMember.checkoutTime);
				Rs.GetFieldValue(_T("IsReturned"),LocalMember.nIsReturned);
				Rs.GetFieldValue(_T("Rate"),LocalMember.strRate);
				lMembers.push_back(LocalMember);
				Rs.MoveNext();
			}
			bRet = TRUE;
		}
		Rs.Close();
	}
	pDb->Release();
	return bRet;
}

/*************************************************
Function:       //RemoveLocalMember
Description:    //删除LocalMember指定记录
Table Updated:  //LocalMember
Input:          //nMemberId - 待删除记录用户号
Return:         //删除成功返回true
*************************************************/
BOOL CIBADAL::RemoveLocalMember(UINT nMemberId)
{
	BOOL bRet = FALSE;
	CString sql;
	sql.Format(_T("delete from localMember where memberId=%d"),nMemberId);
	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) == 1)
	{
		bRet = TRUE;
	}
	else
	{
		IBA_LOG(_T("Error!CIBADAL::RemoveLocalMember 删除记录失败.sql=%s"),sql);
	}
	return bRet;
}

/*************************************************
Function:       //GetCountOfOnlineUser
Description:    //获取localMember中在线的用户
Table Updated:  //LocalMember
Return:         //失败返回-1，成功返回数目
*************************************************/
int CIBADAL::GetCountOfOnlineUser()
{
	int nRet = -1;
	CString sql;
	sql.Format(_T("select count(*) from localMember where date(checkintime)>'2015-01-01' and isReturned = 0"));
	nRet = CountOfRecordsWithSQL(sql);

	return nRet;
}
/*************************************************
Function:       //InsertNewLocalCredit
Description:    //向LocalCredit插入新充值记录,充值时间取当前时间
Table Updated:  //LocalCredit
Input:          //nMemberId - 充值用户ID
				//nCreditAmount - 充值金额
Output:			//CString& strCheck - 返回checkCode
Return:         //插入成功返回true
*************************************************/
BOOL CIBADAL::InsertNewLocalCredit(UINT nMemberId, UINT nCreditAmount, CString strCheck)
{
	BOOL bRet = FALSE;
	//计算checkcode
	CString strCreditTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);
	CString strCheckcode = CIBAHelpper::CalcLocalCreditCheckCode(nMemberId, strCreditTime, nCreditAmount);

	//插入新记录
	CString sql;
	sql.Format(_T("insert into localCredit (memberId,amount,creditTime,checkcode) values(%d,%d,'%s','%s')"), nMemberId, nCreditAmount, 
		strCreditTime, strCheckcode);
	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) == 1)
	{
		strCheck = strCheckcode;
		bRet = TRUE;

		IBA_LOG(_T("LocalCredit表 - 插入MemberId = %d本地充值记录:"),nMemberId);
		IBA_LOG(_T("amount - %d"),nCreditAmount);
		IBA_LOG(_T("creditTime - %s"),strCreditTime);
		IBA_LOG(_T("CheckCode - %s"),strCheck);
	}
	return bRet;
}

/*************************************************
Function:       //DeleteLocalCredit
Description:    //删除LocalCredit指定记录
Table Updated:  //LocalCredit
Input:          //strCheckCode - 按checkcode查找要删除记录
Return:         //删除成功返回true
*************************************************/
BOOL CIBADAL::DeleteLocalCredit(CString strCheckCode)
{
	BOOL bRet = FALSE;
	CString sql;
	sql.Format(_T("delete from localcredit where checkcode = '%s'"),strCheckCode);
	if(CIBADAL::GetInstance()->ExecuteSQLEx(sql) == 1)
	{
		bRet = TRUE;
	}
	else
	{
		IBA_LOG(_T("Error!CIBADAL::DeleteLocalCredit 删除localCredit记录失败.sql=%s"),sql);
	}
	return bRet;
}

/*************************************************
Function:       //GetLocalReport
Description:    //获取符合条件的LocalReport记录
Table Updated:  //LocalReport
Input:          //strNetId - 账号
				//strPassword - 密码
Output:			//LocalReport对象
Return:         //成功返回TRUE
*************************************************/
BOOL CIBADAL::GetLocalReport(CString strNetId, CString strPassword, CLocalReport& lr)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select * from LocalReport where netId='%s' and password='%s'"), strNetId, strPassword);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("memberId"),lr.nMemberId);
			Rs.GetFieldValue(_T("reportTime"),lr.reportTime);
			lr.strNetId = strNetId;
			lr.strPassword = strPassword;
			bRet = true;
		}
	}

	Rs.Close();
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetUserNameFromMember
Description:    //从Member表获取UserName
Table Updated:  //Member
Input:          //strSerialNum - 账号
Output:			//strUserName - UserName
Return:         //成功返回TRUE
*************************************************/
BOOL CIBADAL::GetUserNameFromMember(CString strSerialNum, CString& strUserName)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select name from member where serialNum='%s'"), strSerialNum);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("name"),strUserName);
			bRet = true;
		}
	}

	Rs.Close();
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetCardIdFromMember
Description:    //从Member表获取CardId
Table Updated:  //Member
Input:          //strSerialNum - 账号
Output:			//nCardId
Return:         //成功返回TRUE
*************************************************/
BOOL CIBADAL::GetCardIdFromMember(CString strSerialNum,UINT& nCardId)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select cardId from member where serialNum='%s'"), strSerialNum);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("cardId"),nCardId);
			bRet = true;
		}
	}

	Rs.Close();
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetDepositFromMember
Description:    //从Member表获取Deposit
Table Updated:  //Member
Input:          //strSerialNum - 账号
Output:			//nDeposit
Return:         //成功返回TRUE
*************************************************/
BOOL CIBADAL::GetDepositFromMember(CString strSerialNum,UINT& nDeposit)
{
	BOOL bRet = FALSE;

	CString strSQL;
	strSQL.Format(_T("select deposit from member where serialNum='%s'"), strSerialNum);

	CADODBConnInfo* pDb = NULL;

	if (!GetDBConnInfo(pDb)) return bRet;

	CADORecordset Rs(pDb->GetConn());

	if (Rs.Open(strSQL))
	{
		if(Rs.GetRecordCount() > 0)
		{
			Rs.GetFieldValue(_T("deposit"),nDeposit);
			bRet = true;
		}
	}

	Rs.Close();
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //GetMemberFromMember
Description:    //从Member表获取用户信息
Table Updated:  //Member
Input:          //strSerialNum - 账号
				//NetbarId - 网吧ID
Return:         //返回会员字典
*************************************************/
GxxDictionaryPtr CIBADAL::GetMemberFromMember(CString strSerialNum,BOOL bUseNetbarId /*= FALSE*/)
{
	CString sql ;
	if(bUseNetbarId)
	{
		UINT uNetBarId = CNetBarConfig::GetInstance()->GetNetBarId();
		sql .Format ( _T("select * from member where serialNum='%s' and netBarId = %d and (available = 0 or available = 1)"), strSerialNum,uNetBarId);
	}
	else
	{
		sql .Format ( _T("select * from member where serialNum='%s' and (available = 0 or available = 1)"), strSerialNum);
	}
	
	//IBA_LOG(_T("Temp.CRealNameBase::CheckCardInLocalMode: Find userinfomation with NebBarId = %d and serialNum = %s and (available = 0 or available = 1)"),uNetBarId,lpszNetId);

	return ReadRecordFromDB(sql);
}
/*************************************************
Function:       //GetLocalCounting
Description:    //从SystemConfig中获取本地计费设置
Table Accessed: //SystemConfig
Output:			//pKeyvalue1 - keyvalue1字段值(代表收银台ID)
				//pKeyvalue2 - keyvalue2字段值(最近一次本地计费的time_t值)
Return:         //找到记录并取得结果返回TRUE
*************************************************/
BOOL CIBADAL::GetLocalCounting(int &Keyvalue1, int &Keyvalue2)
{
	BOOL bRet = FALSE;

	CString sql;
	sql.Format(_T("select * from systemConfig where keyString='localCounting'"));

	CADODBConnInfo *pDb = NULL;
	if(!GetDBConnInfo(pDb)){return bRet;}
	CADORecordset Rs(pDb->GetConn());
	if(Rs.Open(sql))
	{
		if(Rs.GetRecordCount()>0)
		{
			Rs.GetFieldValue(_T("keyValue1"),Keyvalue1);
			Rs.GetFieldValue(_T("KeyValue1"),Keyvalue2);
			bRet = TRUE;
		}
	}
	Rs.Close();
	pDb->Release();

	return bRet;
}

/*************************************************
Function:       //UpdateLastUpdateTime_LocalCounting
Description:    //更新本地计费的更新时间
Table Accessed: //SystemConfig
Return:         //找到记录并取得时间为当前时间,返回1.未找到记录,返回0.数据库操作失败,返回-1.
*************************************************/
int CIBADAL::UpdateLastUpdateTime_LocalCounting()
{
	int nRet = -1;

	CString sql;
	sql.Format(_T("select Count(*) from systemConfig where keyString='localCounting'"));
	nRet = CountOfRecordsWithSQL(sql);
	if(nRet < 1) {return nRet;}

	sql.Empty();
	time_t now = time(NULL);
	sql.Format(_T("update systemConfig set keyValue2=%d where keyString='localCounting'"),(int)now);
	nRet = ExecuteSQLEx(sql);
	if(nRet < 1)
	{
		DWORD dwErr = GetLastError();
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			dwErr, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		IBA_LOG(_T("Error.数据库更新localCounting时间失败.ErrCode:%d ErrorMsg:%s"),dwErr,(LPCTSTR)lpMsgBuf);
	}
	return nRet;
}

/*************************************************
Function:       //UpdateCashierandTime_LocalCounting
Description:    //更新本地计费的收银台ID和更新时间
Table Accessed: //SystemConfig
Return:         //找到记录并取得时间为当前时间和当前收银台ID,返回TRUE.
*************************************************/
BOOL CIBADAL::UpdateCashierandTime_LocalCounting()
{
	BOOL bRet = FALSE;

	CString sql;
	sql.Format(_T("select Count(*) from systemConfig where keyString='localCounting'"));
	int n = CountOfRecordsWithSQL(sql);
	if(n < 1) {return bRet;}

	sql.Empty();
	time_t now = time(NULL);
	int nCashierID =  CIBAConfig::GetInstance()->GetCashRegisterID();
	sql.Format(_T("update systemConfig set keyValue1=%d, keyValue2=%d where keyString='localCounting'"),nCashierID,(int)now);
	if(0 < ExecuteSQLEx(sql))
	{
		bRet = TRUE;
	}
	else
	{
		DWORD dwErr = GetLastError();
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			dwErr, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		IBA_LOG(_T("Error.数据库更新localCounting收银台ID和时间失败.ErrCode:%d ErrorMsg:%s"),dwErr,(LPCTSTR)lpMsgBuf);
	}
	return bRet;
}

/*************************************************
Function:       //AddLocalCounting
Description:    //增加本地计费设置.值分别为当前收银台ID(kevalue1)和当前时间(keyvalue2),以及'为本地同时只允许一个收银台处于本地计费中提供依据'(keyvalue3)
Table Accessed: //SystemConfig
Return:         //成功返回TRUE
*************************************************/
BOOL CIBADAL::AddLocalCounting()
{
	BOOL bRet = FALSE;

	int nMaxKeyId = 0;
	CIBADAL::GetInstance()->GetMaxKeyId(nMaxKeyId);
	int nCashierID =  CIBAConfig::GetInstance()->GetCashRegisterID();
	time_t now = time(NULL);
	CString strValue3 = _T("为本地同时只允许一个收银台处于本地计费中提供依据");

	CString sql;
	sql.Format(_T("insert into systemConfig (keyId,keyString,keyValue1,keyValue2,keyValue3) values(%d,'localCounting', %d, %d, '%s')"),nMaxKeyId,nCashierID,(int)now,strValue3);

	if(0 < ExecuteSQLEx(sql))
	{
		bRet = TRUE;
	}
	else
	{
		DWORD dwErr = GetLastError();
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			dwErr, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		IBA_LOG(_T("Error.数据库插入localCounting记录失败.ErrCode:%d ErrorMsg:%s"),dwErr,(LPCTSTR)lpMsgBuf);
	}
	return bRet;
}