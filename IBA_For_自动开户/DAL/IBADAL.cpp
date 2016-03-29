#include "stdafx.h"
#include ".\ibadal.h"
#include "..\IBA.h"
#include "..\IBAHelpper.h"
#include "..\Cashier.h"
#include "..\Encrypt.h"

SINGLETON_IMPLEMENT(CIBADAL)

CIBADAL::CIBADAL(void)
//: m_DBPool(2, _T("DSN=NETBAR;"))
: m_DBPool(2, _T("DSN=netbar;"))
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
	return FALSE;//	QSCTT
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
	return 1;
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
	return 1;
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
	return 1;
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
	return 1;
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

	try
	{
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
	}
	catch (...)
	{
		return FALSE;
	}
}

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
		if (!bRet)
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

UINT CIBADAL::GetRoundType(UINT nNetbarId)
{
	UINT nRoundType = 0;

	CString strSQL(_T("SELECT netbarInfo.roundType, netbarInfo.netbarId from netbarInfo "));
	strSQL.AppendFormat(_T("where netbarInfo.netbarId = '%d' "), nNetbarId);

	CIBALog::GetInstance()->WriteFormat(CIBALog::ELT_SQL, _T("GetRoundType:%s"), strSQL);

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

