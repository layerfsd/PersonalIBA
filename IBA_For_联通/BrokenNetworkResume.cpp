#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "dal\IBADAL.h"
#include "RealName\IBARealName.h"
#include "MainFrm.h"
#include "IBAHelpper.h"
#include ".\brokennetworkresume.h"


CBrokenNetworkResume::CBrokenNetworkResume(void)
{
	m_bShouldResume = FALSE;
}

CBrokenNetworkResume::~CBrokenNetworkResume(void)
{
}

BOOL CBrokenNetworkResume::Init()
{
	return (m_bShouldResume = CheckShouldResume());
}

void CBrokenNetworkResume::SetShouldResume(BOOL bResume)
{
	m_bShouldResume = bResume;
}

BOOL CBrokenNetworkResume::GetShouldResume()
{
	return m_bShouldResume;
}

BOOL CBrokenNetworkResume::CheckShouldResume()
{
	BOOL bRet = FALSE;

	CCheckOutInfoArray CheckOutArray;
	
	CIBADAL::GetInstance()->GetCheckOutInfo(CheckOutArray);
	
	if (CheckOutArray.GetCount() > 0)
	{
		bRet = TRUE;
	}
	
	return bRet;
}


void CBrokenNetworkResume::StartResume(INT nConnectionState)
{
	if (nConnectionState == -1)
		return;

	if (!m_bShouldResume)
		return;

	CIBALog::GetInstance()->Write(_T("CBrokenNetworkResume::StartResume"));

	m_bShouldResume = FALSE;

	Start();
}


INT CBrokenNetworkResume::Run()
{
	CoInitialize(NULL);

	CSingleLock lock(&m_TestUploadLock, TRUE);
	//
	// 向中心提交断网数据
	//

	//查询数据库
	CCheckOutInfoArray CheckOutArray;

	CIBADAL::GetInstance()->GetCheckOutInfo(CheckOutArray);

	INT i = 0;

	for (i = 0; i < CheckOutArray.GetCount(); i++)
	{
		if (!theApp.GetCurCashier()->IsOnDuty())
		{
			theApp.IBAMsgBox(IDS_STRING326);//只能在上班状态进行断网恢复
			CoUninitialize();
			return 0;
		}

		CCheckOutinfo & CheckOutinfo = CheckOutArray.GetAt(i);
		CIBALog::GetInstance()->WriteFormat(_T("补传断网结账的用户 %s "),CheckOutinfo.GetSerialNum());
		//统一执行登出
		CCheckout2Method Checkout2;
		Checkout2.SetSafeLogout(0); //登出，为1不登出
		Checkout2.SetMemberId(CheckOutinfo.GetMemberId());

		COleDateTime OleCheckOutTime;
		OleCheckOutTime.ParseDateTime(CheckOutinfo.GetCheckOutTime());
		Checkout2.SetEndUsingTime(OleCheckOutTime.Format(_T("%Y%m%d%H%M%S")));
		
		theApp.GetCurCashier()->DoCheckout2(Checkout2);		

		int nStatus = Checkout2.GetStatusCode();

		if (CheckOutinfo.GetClassState() == 0 && CheckOutinfo.GetReturnState() == 1)//普通用户
		{
			SetShouldResume(FALSE); // 防止递归式的恢复
			DeregisterUser(CheckOutinfo);
		}
		else
		{
			//写退款信息日志
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeVipReturn);
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(CheckOutinfo.GetUserName());
			OperationInfo.SetNetId(CheckOutinfo.GetSerialNum());
			OperationInfo.SetMemberId(CheckOutinfo.GetMemberId());
			OperationInfo.SetDescription(_T("断网后续传"));
			OperationInfo.SetIsResumeData(TRUE);
			OperationInfo.SetClassId(CheckOutinfo.GetClassId());

			//必须等待直到返回，否则容易造成线程不安全
			SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
		}

		

		Sleep(50);
	}
	
	IBA_TRACE("============================i=%d",i);

	if (i >= CheckOutArray.GetCount())
	{
		//清空数据库
		CIBADAL::GetInstance()->DeleteAllCheckOutInfo();
	}

	CoUninitialize();

	return 0;
}


BOOL CBrokenNetworkResume::DeregisterUser(CCheckOutinfo& CheckOutinfo)
{
	//检验卡号
	CIDCheckCardMethod IDCheckCard;
	IDCheckCard.SetSerialNum(CheckOutinfo.GetSerialNum());
	theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

	if (IDCheckCard.GetStatusCode() != 0)
	{
		return FALSE;
	}

	//注销操作
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(IDCheckCard.GetMemberId());
	Deregister.SetPersonalId(CheckOutinfo.GetPersonalId());
	Deregister.SetMemberName(CheckOutinfo.GetClassName());
	Deregister.SetCardId(IDCheckCard.GetCardId());
	Deregister.SetPassword(_T("PASSWORDPASSWORD"));
	Deregister.SetReturnMode(0);

	theApp.GetCurCashier()->DoDeregister(Deregister);

	if (Deregister.GetStatusCode() == 0)
	{	
		UINT nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计

		CString strNetId;
		strNetId.Format(_T("%s"), CheckOutinfo.GetSerialNum());

		//写退款信息日志
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeReturn);
		OperationInfo.SetIsBundTime(FALSE);
		OperationInfo.SetUserName(CheckOutinfo.GetUserName());
		OperationInfo.SetNetId(CheckOutinfo.GetSerialNum());
		OperationInfo.SetMemberId(CheckOutinfo.GetMemberId());
		OperationInfo.SetOperationAmount(Deregister.GetBalance());
		OperationInfo.SetAllAmount(nAll);
		OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
		OperationInfo.SetDescription(_T("断网后续传"));
		//必须等待直到返回，否则容易造成线程不安全
		SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		//回馈信息写入数据库
		{//写数据库
			CReturnedInfo ReturnedInfo;

			ReturnedInfo.SetCardId(IDCheckCard.GetCardId());
			ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(Deregister.GetTranTime()));
			ReturnedInfo.SetRefNo(Deregister.GetRefNo());
			ReturnedInfo.SetDeposit(Deregister.GetGuarantyMoney());
			ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());
			ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			ReturnedInfo.SetMemberId(CheckOutinfo.GetMemberId());
			ReturnedInfo.SetSerialNo(Deregister.GetLocalSerialNo());
			ReturnedInfo.SetReturnAmount(nAll);
			ReturnedInfo.SetClassId(1);
			CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);
		}

		//实名退临时卡
		if (CNetBarConfig::GetInstance()->GetReturnOnlyClassId() != CheckOutinfo.GetClassId())
		{
			theApp.GetRealName()->InvokeRealName(100, (LPVOID)strNetId.GetString());
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
