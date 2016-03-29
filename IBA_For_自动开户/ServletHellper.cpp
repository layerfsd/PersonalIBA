#include "stdafx.h"
#include "IBA.h"
#include "MainFrm.h"
#include "IBAHelpper.h"
#include "DAL\IBADAL.h"
#include "TTS\IBATTS.h"
#include "Socket\LocalServer.h"
#include "RealName\IBARealName.h"
#include "Servlet\WinHttpRequest.h"
#include ".\ServletHellper.h"
#include "CashierHelper.h"


UINT CServletHellper::GetMemberId(LPCTSTR lpszNetId)
{
	UINT nCardId = 0;

	return GetMemberId(lpszNetId, nCardId);
}

UINT CServletHellper::GetMemberId(LPCTSTR lpszNetId, UINT& nCardId)
{
	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(lpszNetId);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	if (checkCard.GetStatusCode() == 0)
	{
		nCardId = checkCard.GetCardId();

		return checkCard.GetMemberId();
	}

	return 0;
}

BOOL CServletHellper::ActivationMember(UINT nMemberID)
{
	//中心是否开启激活设置
	if (CNetBarConfig::GetInstance()->GetNeedActivation() == 0)
	{
		return FALSE;
	}

	//实名是否允许上机激活
	if (!theApp.GetRealName()->IsCheckInEnabled())
	{
		return FALSE;
	}

	CActivationMethod Activation;

	Activation.SetMemberId(nMemberID);
	Activation.SetMode( CActivationMethod::ACTIVATION_YES );

	theApp.GetCurCashier()->DoActivation(Activation);

	if (Activation.GetStatusCode() == 0)
	{	
		return TRUE;
	}
	else
	{
		theApp.IBAMsgBox(Activation.GetStatusMessage());
	}

	return FALSE;
}

BOOL CServletHellper::Synchronize()
{
	CString strNetBarKey = CNetBarConfig::GetInstance()->GetNetBarKey();
	CString strCenterIP = CNetBarConfig::GetInstance()->GetMainCenterIP();
	CString strPath = CNetBarConfig::GetInstance()->GetMainCenterPath();
	
	UINT nNetBarID = CNetBarConfig::GetInstance()->GetNetBarId();
	UINT nCenterPort = CNetBarConfig::GetInstance()->GetMainCenterPort();

	CString strURL;
	strURL.Format(_T("http://%s:8080/netbar/synchronize/cashregister.jsp?mode=%d&psk=%s&netBarId=%d&path=http://%s:%d/%s"),
		CNetBarConfig::GetInstance()->GetLocalServerIP(), 1, strNetBarKey,
		nNetBarID, strCenterIP, nCenterPort, strPath);

	CIBALog::GetInstance()->WriteFormat(_T("Synchronize URL = %s"), strURL);

	CWinHttpRequest WinHttpRequest; 

	if (WinHttpRequest.GetReadyState() == 0)
	{
		return FALSE;
	}

	WinHttpRequest.SetTimeouts(5000, 5000, 5000, 60000);
	WinHttpRequest.Open(strURL);
	WinHttpRequest.Send();

	if (WinHttpRequest.GetReadyState() == 4) //执行成功
	{
		CString strTmp = WinHttpRequest.GetResponseText().Trim();

		CIBALog::GetInstance()->Write(strTmp);

		INT nIdx = strTmp.Find(_T("失败"));

		if (nIdx >= 0 && nIdx < 20)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}	
	}
	else
	{
		return FALSE;
	}
}

//******************************************************************************


CCreditHelpper::CCreditHelpper(void)
: m_nCreditMoney(0), m_nMemberId(0)
{
}

CCreditHelpper::~CCreditHelpper(void)
{
}

void CCreditHelpper::SetNetId(CString newVal)
{
	m_strNetId = newVal;
}

void CCreditHelpper::SetCreditMoney(INT newVal)
{
	m_nCreditMoney = newVal;
}

BOOL CCreditHelpper::Execute()
{
	m_nMemberId = CServletHellper::GetMemberId(m_strNetId);

	//未注册的账号
	if (m_nMemberId == 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_HASNOTUSED);
		strTmp.Insert(0, m_strNetId);
		theApp.IBAMsgBox(strTmp);

		return FALSE;
	}

	// 2011/06/29-gxx: 是否可以充值, 挂机中不允许充的
	if (!CLocalServer::GetInstance()->ActiveMemberList.IsAllowCredit(m_nMemberId))
	{
		return FALSE;
	}


	//钱不对
	if (m_nCreditMoney <= 0)
	{
		theApp.IBAMsgBox(IDS_MONEYERROR);
		return FALSE;
	}

	//激活
	CServletHellper::ActivationMember(m_nMemberId);

	CCreditMethod Credit;

	Credit.SetMemberId(m_nMemberId);
	Credit.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
	Credit.SetActivityId(0);
	Credit.SetNoPresent(FALSE);
	Credit.SetAmount(m_nCreditMoney);

	theApp.GetCurCashier()->DoCredit(Credit);

	if (Credit.GetStatusCode() == 0)
	{	
		CIBATTS::GetInstance()->SpeakCreditResult(m_nCreditMoney, Credit.GetCreditPresent(), Credit.GetAvailableBalance());

		OnCreditOK(Credit);

		CString strTmp;

		strTmp.Format(IDS_CREDITRESULT2, m_strNetId, m_nCreditMoney / 100.0, Credit.GetCreditPresent() / 100.0);

		theApp.IBAMsgBox(strTmp);

		return TRUE;
	}
	else
	{
		theApp.IBAMsgBox(Credit.GetStatusMessage());
	}

	return FALSE;
}

BOOL CCreditHelpper::Execute(CString strCmdLine)
{
	IBA_LOG(_T("便捷通道快速充值"));
	CStringArray sa;
	
	CIBAHelpper::SplitLine(strCmdLine, sa, ' ');

	if (sa.GetCount() < 2)
	{
		theApp.IBAMsgBox(IDS_PARAMERROR);

		return FALSE;
	}

	CString strNetId = sa.GetAt(0);
	INT nCreditMoney = _tstof(sa.GetAt(1)) * 100;

	// 2012/03/05-8238-gxx: 
	if (CIBAConfig::GetInstance()->GetMaxCreditMoney() != 0)
	{
		INT nMax = CIBAConfig::GetInstance()->GetMaxCreditMoney()*100;
		if (nCreditMoney > nMax)
		{
			theApp.IBAMsgBox(IDS_OVERFLOW_MAXMONEY);
			return FALSE;
		}
	}
	// --end

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
	{
		SetNetId(strNetId);
		SetCreditMoney(nCreditMoney);
		return Execute();
	}
	else 
	{
		CActiveMember ActivemMember;
		if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strNetId,ActivemMember))
		{
			return FALSE;
		}
		
		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
		{
			if (!ActivemMember.GetIsLocalUser())
			{
				SetNetId(strNetId);
				SetCreditMoney(nCreditMoney);
				return Execute();
			}
		}
		else
		{
			if (!ActivemMember.GetIsLocalUser())
			{
				return FALSE;
			}
		}

		if (nCreditMoney <= 0)
		{
			return FALSE;
		}

		return CCashierHelper::CreditLocalUser(ActivemMember, nCreditMoney);
	}
	
	return FALSE;	
}

void CCreditHelpper::OnCreditOK(CCreditMethod & Credit)
{
	CBalanceMethod Balance;
	Balance.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBalance(Balance);

	if (Balance.GetStatusCode() == 0)
	{
		CString strTmp;

		if (Balance.GetOnline() == 1) //发送充值消息
		{			
			strTmp.Format(IDS_ONLINECREDIT, m_nCreditMoney / 100.0, Credit.GetCreditPresent() / 100.0);
			CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(m_nMemberId, strTmp);
		}

		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(1);
		OperationInfo.SetIsBundTime(FALSE);
		OperationInfo.SetUserName(Balance.GetUserName());
		OperationInfo.SetNetId(Balance.GetSerailNum());
		OperationInfo.SetMemberId(m_nMemberId);
		OperationInfo.SetOperationAmount(m_nCreditMoney);
		OperationInfo.SetPresentAmount(Credit.GetCreditPresent());
		SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		//写入数据库
		CCreditInfoDB CreditInfo;

		CreditInfo.SetCardSerial(Balance.GetSerailNum());
		CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(Credit.GetTrantime()));
		CreditInfo.SetRefNo(Credit.GetRefNo());
		CreditInfo.SetCreditPresent(Credit.GetCreditPresent());
		CreditInfo.SetCreditAmount(m_nCreditMoney);
		CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
		CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
		CreditInfo.SetMemberId(m_nMemberId);
		CreditInfo.SetSerialNo(Credit.GetLocalSerialNo());
		CreditInfo.SetCassId(Balance.GetUserClass());

		CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);

	}
}

//******************************************************************************

BOOL CDeregisterHelpper::DeregisterUser(CBalanceMethod & Balance)
{
	CDeregisterMethod Deregister;

	Deregister.SetPersonalId(Balance.GetPersonalId());
	Deregister.SetCardId(Balance.GetCardId());
	Deregister.SetMemberName(Balance.GetMemberName());
	Deregister.SetMemberId(Balance.GetMemberId());
	Deregister.SetReturnMode(0);
	Deregister.SetPassword(_T("PASSWORDPASSWORD"));

	theApp.GetCurCashier()->DoDeregister(Deregister);

	if (Deregister.GetStatusCode() == 0)
	{	
		UINT nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计

		if (CNetBarConfig::GetInstance()->GetReminderAutoReturn())
		{
			CIBATTS::GetInstance()->SpeakReturnResult(nAll);//报声
		}

		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeAutoReturn);
		OperationInfo.SetIsBundTime(FALSE);
		OperationInfo.SetUserName(Balance.GetUserName());
		OperationInfo.SetNetId(Balance.GetSerailNum());
		OperationInfo.SetMemberId(Balance.GetMemberId());
		OperationInfo.SetAllAmount(Balance.GetBalance());
		OperationInfo.SetOperationAmount(Deregister.GetBalance());
		OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
		OperationInfo.SetClassId(Balance.GetUserClass());
		OperationInfo.SetIdNum(Balance.GetPersonalId());
		OperationInfo.SetTermId(Balance.GetTermId());

		//{ 2011/10/21-8208-gxx: 
		CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(Balance.GetMemberId(),ActiveMember))
		{
			OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
			OperationInfo.SetConsumeMoney(ActiveMember.GetAdvancePayment()-nAll);
			
		}
		//}
		
		SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);

		CReturnedInfo ReturnedInfo;
		ReturnedInfo.SetReturnDate(CIBAHelpper::FormatCenterTime(Deregister.GetTranTime()));
		ReturnedInfo.SetRefNo(Deregister.GetRefNo());
		ReturnedInfo.SetDeposit(Deregister.GetGuarantyMoney());
		ReturnedInfo.SetOperator(theApp.GetCurCashier()->GetName());
		ReturnedInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		ReturnedInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
		ReturnedInfo.SetMemberId(Balance.GetMemberId());
		ReturnedInfo.SetSerialNo(Deregister.GetLocalSerialNo());
		ReturnedInfo.SetReturnAmount(nAll);
		ReturnedInfo.SetClassId(Balance.GetUserClass());
		CIBADAL::GetInstance()->AddReturnedRecord(ReturnedInfo);
	
		//实名退临时卡
		if (CNetBarConfig::GetInstance()->GetReturnOnlyClassId() != Balance.GetUserClass())
		{
			theApp.GetRealName()->InvokeRealName(100, (LPVOID)Balance.GetSerailNum().GetString());
		}

		return TRUE;
	}
	else
	{
		CIBALog::GetInstance()->WriteFormat(_T("CDeregisterHelpper::DeregisterUser:%s"), Deregister.GetStatusMessage());
	}

	return FALSE;
}

//自动退款功能
BOOL CDeregisterHelpper::AllowAutoRefund(CBalanceMethod & Balance)
{		
	//if (!CIBAConfig::GetInstance()->GetAutoRefundment()) //是否启用了刷卡自动退款
	//{
	//	return FALSE;
	//}

	if (Balance.IsNetbarMember()) //会员用户或者不可退款
	{
		return FALSE;
	}

	if (Balance.GetAllowReturn() == 1) //可以退款
	{
		if (Balance.GetReturnAmount() > 0) //可退账户还有钱，不能退
		{
			return FALSE;
		}
	}
	else if (Balance.GetAllowReturn() == 2) //只退押金
	{
		if (Balance.GetTotalBalacne() > 0) //合计账户还有钱，不能退
		{
			return FALSE;
		}
	}
	else //不允许退款
	{
		return FALSE;
	}

	if (Balance.GetGuarantyMoney() > 0) //有押金的
	{
		return FALSE;
	}

	if (Balance.GetOnline() == 1) //在线 
	{
		return FALSE;
	}
	
	if (Balance.GetPayType() > 0) //包时
	{
		return FALSE;
	}

	if (Balance.GetLogoutTime().CollateNoCase(_T("19700101080000")) == 0) //新开的账号不退
	{
		return FALSE;
	}
	
	COleDateTime dtLogoutTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetLogoutTime());
	
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	
	COleDateTimeSpan dtSpan = dtNow - dtLogoutTime; //下机离现在多长时间了

	if (dtSpan.GetTotalMinutes() < CActiveMember::GetRefundmentTime())
	{
		return FALSE;
	}
	
	return TRUE;
}
