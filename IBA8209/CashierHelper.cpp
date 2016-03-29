#include "stdafx.h"
#include ".\cashierhelper.h"
#include "Dal\IBADAL.h"
#include "OperationInfo.h"
#include "Dal\IDTypeInfo.h"
#include "ActiveMember.h"
#include "IBA.h"
#include "MainFrm.h"
#include "Cashier.h"
#include "Socket\LocalServer.h"
#include "ActiveMemberList.h"
#include "IBAHelpper.h"
#include "ComputerListView.h"
#include "Tts\IBATTS.h"
#include "DdnDataX.h"
#include "Encrypt.h"
#include "DdnPcpInc.h"

CCashierHelper::CCashierHelper(void)
{
}

CCashierHelper::~CCashierHelper(void)
{
}

void CCashierHelper::MakeLocalConsumeInfo(CActiveMember& ActiveMember, NS_DAL::CLocalConsumeInfo& consumeInfo)
{
	// 获取费用, 计算退款金额
	UINT nExpense = ActiveMember.GetLocalExpense();
	UINT nReturn = 0; 
	if (nExpense > (UINT)ActiveMember.GetAdvancePayment())
	{
		nReturn = 0;
		nExpense = (UINT)ActiveMember.GetAdvancePayment();
	}
	else
	{
		nReturn = ActiveMember.GetAdvancePayment() - nExpense;
	}

	consumeInfo.SetSubmitTime( CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()) );
	consumeInfo.SetRefNo( ActiveMember.GetMemberID() );

	if (!ActiveMember.GetTerminalID().IsEmpty())
	{
		if (!CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()))
		{
			// 上机时间不正常，使用当前时间
			consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( COleDateTime::GetCurrentTime() ));
			consumeInfo.SetCheckoutTime( consumeInfo.GetCheckinTime() );
		}
		else
		{
			consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( ActiveMember.GetCheckInTime()) );
			consumeInfo.SetCheckoutTime( CIBAHelpper::FormatTime( COleDateTime::GetCurrentTime() ) );
		}	
	}
	else
	{
		// 没有上过机，上机时间和下机和保持一致
		consumeInfo.SetCheckinTime( CIBAHelpper::FormatTime( ActiveMember.GetActivationTime()) );
		consumeInfo.SetCheckoutTime( CIBAHelpper::FormatTime( ActiveMember.GetActivationTime()) );
	}

	consumeInfo.SetConsumeAmount( nExpense );
	consumeInfo.SetCreditAmount( ActiveMember.GetAdvancePayment() );
	consumeInfo.SetReturnAmount( nReturn );
	consumeInfo.SetIdNumber( ActiveMember.GetPersonalID() );
	consumeInfo.SetMemberId( ActiveMember.GetMemberID() );
	consumeInfo.SetName( ActiveMember.GetUserName(TRUE) );
	consumeInfo.SetOperator( theApp.GetCurCashier()->GetName() );		
	consumeInfo.SetSerialNum( ActiveMember.GetNetId() );	
	consumeInfo.SetTermId( ActiveMember.GetTerminalID() );
	consumeInfo.SetClassId( ActiveMember.GetClassId() );
	consumeInfo.SetPCClass( ActiveMember.GetPCClassID() );
	consumeInfo.SetPayType( ActiveMember.GetPayType() );
	consumeInfo.MakeMD5();
}

BOOL CCashierHelper::ReturnLocalUser(CActiveMember& ActiveMember,NS_DAL::CLocalConsumeInfo* lpLocalInfo)
{
	IBA_ASSERT2(ActiveMember.GetIsLocalUser(), "不需为本地用户才能执行此操作");

	CLocalConsumeInfo consumeInfoTmp;
	if (lpLocalInfo == NULL)
	{
		MakeLocalConsumeInfo(ActiveMember, consumeInfoTmp);
		lpLocalInfo = &consumeInfoTmp;
	}
	CLocalConsumeInfo& consumeInfo = *lpLocalInfo;

	if (CIBADAL::GetInstance()->AddLocalConsume( consumeInfo ))
	{
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeLocalReturn);

		OperationInfo.SetIsBundTime( FALSE );
		OperationInfo.SetUserName( consumeInfo.GetName() );
		OperationInfo.SetNetId( consumeInfo.GetSerialNum() );
		OperationInfo.SetMemberId( consumeInfo.GetMemberId() );
		OperationInfo.SetOperationAmount( consumeInfo.GetReturnAmount() );
		OperationInfo.SetAllAmount( consumeInfo.GetCreditAmount() );
		OperationInfo.SetOperationTime( COleDateTime::GetCurrentTime() );
		OperationInfo.SetDescription(_T("本地用户"));
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);

		CIBADAL::GetInstance()->DeleteLastUserInfo(consumeInfo.GetTermId());

		// 通知客户端结账锁屏
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(consumeInfo.GetMemberId());
		if (nIndex >= 0)
		{	
			// 处于上机状态,通知客户端
			CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, consumeInfo.GetMemberId());
		}

		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(consumeInfo.GetMemberId());
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

		IBA_LOG(_T("断网本地用户退款: netId=%s, memberId=%d"), consumeInfo.GetSerialNum(), consumeInfo.GetMemberId());

		return TRUE;
	}

	return FALSE;
}

void CCashierHelper::MakeCheckOutInfo(CActiveMember& ActiveMember, NS_DAL::CCheckOutinfo& CheckOutinfo)
{
	CheckOutinfo.SetCheckOutTime(CIBAHelpper::GetCurrentTimeAsString(TRUE));

	//各个顺序不能变化，必须和写入的时候一样

	CheckOutinfo.SetTerminalId(ActiveMember.GetTerminalID());

	CheckOutinfo.SetSerialNum(ActiveMember.GetNetId());

	CheckOutinfo.SetMemberId(ActiveMember.GetMemberID());

	CheckOutinfo.SetUserName(ActiveMember.GetUserName());

	CheckOutinfo.SetPersonalId(ActiveMember.GetPersonalID());

	CheckOutinfo.SetClassName(ActiveMember.GetUserClassName());

	CheckOutinfo.SetClassId(ActiveMember.GetClassId());

	CheckOutinfo.SetClassState(ActiveMember.IsMember());

	CheckOutinfo.SetBalanceMoney(ActiveMember.GetAvailavleBalance());
}

BOOL CCashierHelper::ReturnLocalCenterUser(CActiveMember& ActiveMember, NS_DAL::CCheckOutinfo* lpCheckOut)
{
	IBA_ASSERT2(!ActiveMember.GetIsLocalUser(), "必须为中心用户才能执行此操作");

	CCheckOutinfo CheckOutInfoTemp;
	if (lpCheckOut == NULL)
	{
		MakeCheckOutInfo(ActiveMember, CheckOutInfoTemp);
		lpCheckOut = &CheckOutInfoTemp;
	}
	CCheckOutinfo& CheckOutinfo = *lpCheckOut;

	CheckOutinfo.SetReturnState(1); // 设置状态为已退款

	BOOL bRes = CIBADAL::GetInstance()->AddCheckOutInfo(CheckOutinfo);
	if (!bRes)
	{
		IBA_ASSERT(bRes);
		CIBALog::GetInstance()->Write(_T("断网结账时，数据库连接失败"));
		theApp.IBAMsgBox(_T("数据库连接失败"));
		return FALSE;
	}

	////刷新
	//CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);

	// 通知客户端结账锁屏
	INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(CheckOutinfo.GetMemberId());
	if (nIndex >= 0)
	{	
		// 处于上机状态,通知客户端
		CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, CheckOutinfo.GetMemberId());

		CIBADAL::GetInstance()->DeleteLastUserInfo(CheckOutinfo.GetTerminalId());
	}

	{//写操作日志
		COperationInfo OperationInfo;
		if (ActiveMember.IsMember())
		{
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturnVip); // 会用断网结账
		}
		else
		{
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturn); // 中心临时用户断网退款
		}

		OperationInfo.SetIsBundTime(FALSE);
		OperationInfo.SetUserName(ActiveMember.GetUserName());
		OperationInfo.SetNetId(ActiveMember.GetNetId());
		OperationInfo.SetMemberId(ActiveMember.GetMemberID());
		OperationInfo.SetOperationAmount(ActiveMember.GetAvailavleBalance());
		OperationInfo.SetAllAmount(ActiveMember.GetAdvancePayment());
		OperationInfo.SetOperationTime(COleDateTime::GetCurrentTime());
		OperationInfo.SetDescription(_T("网络连接中断时"));
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);

		IBA_LOG(_T("断网中心用户退款: netId=%s, memberId=%d"), ActiveMember.GetNetId(), ActiveMember.GetMemberID());
	}
	
	if ( !CLocalServer::GetInstance()
		->GetActiveMemberView()->GetActiveMemberList()
		->RemoveActiveMember(ActiveMember.GetMemberID()))
	{
		return FALSE;
	}

	CLocalServer::GetInstance()->GetComputerListView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);
	CLocalServer::GetInstance()->GetActiveMemberView()->SendMessage(WM_COMMAND,IDM_MYREFRESH,0);

	//CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

	return TRUE;
}

BOOL CCashierHelper::CreditLocalUser(CActiveMember& ActiveMember, UINT nCreditMoeny)
{
	IBA_ASSERT2(ActiveMember.GetIsLocalUser(), "必须为本地用户");

	if (ActiveMember.GetMemberID() == 0)
	{
		IBA_ASSERT2(FALSE,"不是有效用户");
		return FALSE;
	}

	if (nCreditMoeny == 0)
	{
		return FALSE;
	}


	ActiveMember.SetAdvancePayment() += nCreditMoeny;
	ActiveMember.SetAvailavleBalance() += nCreditMoeny;
	if (nCreditMoeny %100 == 0)
	{
		ActiveMember.SetCreditProcess().AppendFormat(_T("+%d"), nCreditMoeny/100);
	}
	else if (nCreditMoeny %10 == 0)
	{
		ActiveMember.SetCreditProcess().AppendFormat(_T("+%.1f"), nCreditMoeny/100.0);
	}
	else
	{
		ActiveMember.SetCreditProcess().AppendFormat(_T("+%.2f"), nCreditMoeny/100.0);
	}


	//充值成功后设置各种状态
	if (CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMemberDB(ActiveMember))
	{	
		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember,FALSE);
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);


		{//写到充值信息日志记录里
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeLocalCredit);
			OperationInfo.SetUserName(ActiveMember.GetUserName(TRUE));
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetOperationAmount(nCreditMoeny);
			OperationInfo.SetDeposit(0 * 100);
			OperationInfo.SetPassWord(_T(""));
			OperationInfo.SetPresentAmount(0);
			OperationInfo.SetDescription(_T("本地用户充值  ") + ActiveMember.GetTerminalID());
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
		}

		{//写入数据库
			CCreditInfoDB CreditInfo;
			CreditInfo.SetCardSerial(ActiveMember.GetNetId());
			CreditInfo.SetCreditDate(CIBAHelpper::GetCurrentTimeAsString(TRUE));
			CreditInfo.SetRefNo(0);
			CreditInfo.SetCreditPresent(0);
			CreditInfo.SetCreditAmount(nCreditMoeny);
			CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
			CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
			CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
			CreditInfo.SetMemberId(ActiveMember.GetMemberID());
			CreditInfo.SetSerialNo(CIBAConfig::GetInstance()->GetLocalSerialNo());
			CreditInfo.SetCassId(ActiveMember.GetClassId());
			CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);
		}

		if (ActiveMember.GetIsOnLine()) //发送充值消息
		{			
			CString strTmp;
			strTmp.Format(IDS_ONLINECREDIT, nCreditMoeny / 100.0, 0 / 100.0);
			CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(ActiveMember.GetMemberID(), strTmp);
		}

		CIBATTS::GetInstance()->SpeakCreditResult(nCreditMoeny, 0, ActiveMember.GetAvailavleBalance());
		return TRUE;
	}
	return FALSE;
}

BOOL CCashierHelper::CashSellDianka(LPCTSTR lpURL)
{
	CDataXMSender xmSender;
	if (xmSender.Create(0,_T("DdnPcp-CA591950-AA6A-495a-9ABE-C02374F479FE")))
	{
		CStringA strURL = CT2W(lpURL);
		INT nRes = xmSender.SendData(1,0,0,(LPVOID)strURL.GetString(),strURL.GetLength());
		IBA_LOG0(_T("弹出客户端请求现金购点卡的tip"));
		return nRes >= 0;
	}	
	else
	{
		IBA_LOG(_T("处理客户端发送过来的现金购买点卡的请求失败"));
		return FALSE;
	}
}

BOOL CCashierHelper::InitDiankaPlatform()
{
	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		IBA_LOG0(_T("error: 初始化点卡平台失败, 收银员处于未上班状态"));
		return FALSE;
	}


	// 2012/02/29-8237-gxx: 
	{
		CFileVersionInfo fvi;
		CString strPath = theApp.GetWorkPath() +  + _T("\\IBABin\\");
		strPath.TrimRight(_T('\\'));
		strPath += _T("\\DdnPcp.dll");
		if (!fvi.Create(strPath))
		{
			IBA_LOG0(_T("error: 读取DdnPcp.dll的版本号失败"));
		}
		else
		{
			CString strVer = fvi.GetProductVersion();

			strVer.Replace(_T(", "), _T("."));

			IBA_LOG(_T("DdnPcp.dll (version: %s)"), strVer);

			if (strVer.CompareNoCase(_T("2.0.0.3")) < 0)
			{
				CString strInfo;
				strInfo.Format(_T("IBA.exe %s 只能支持DdnPcp.dll %s 以上版本"), 
					CIBALog::GetInstance()->GetAppVer(), strVer);
				IBA_LOG0(strInfo);
				theApp.IBAMsgBox(strInfo,MB_OK|MB_ICONWARNING);
				return FALSE;
			}
		}
	}
	
	IBA_LOG0(_T("初始化点卡平台参数"));
	


	//ShowClientDianKa(m_hWnd,_T("http://www.baidu.com/"),WM_CLIENT_DIANKA,0,0);
	//	1.地址格式：
	//http://计费中心:8080/netbar/common/pointcard.jsp?
	//	userId=zhoufeng&netBarId=4&dutyId=12348709&cashRegisterId=1
	//		&serial=1311144479&check=150c5c46996ca7726fe4e73c28cbae0a 
	//		2.参数含义：
	//userId: 当前收银员的登录账号
	//netBarId: 当前网吧id
	//dutyId: 当前班次id，此字段为以后扩展
	//cashRegisterId: 收银台id，此字段为以后扩展
	//serial: 以整数表示的时间戳，即time()的返回值，中心允许10分钟的时间差。
	//check: 校验串，check=md5(userId+netBarId+dutyId+cashRegisterId+serial+md5(userPwd))，其中userPwd为收银员的密码

	UINT nNetbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strCasherID = theApp.GetCurCashier()->GetName();
	CString strPwd = theApp.GetCurCashier()->GetPassword();
	CString strTime;
	CTime tm = CTime::GetCurrentTime();
	strTime.Format(_T("%lld"), tm.GetTime());
	UINT nDutyId = theApp.GetCurCashier()->GetDutyID();
	UINT nCashRegisterId = CIBAConfig::GetInstance()->GetCashRegisterID();

	CString strOrgCode;
	strOrgCode.AppendFormat(_T("%s"),strCasherID);
	strOrgCode.AppendFormat(_T("%d"), nNetbarID);
	strOrgCode.AppendFormat(_T("%d"), nDutyId);
	strOrgCode.AppendFormat(_T("%d"), nCashRegisterId);
	strOrgCode.AppendFormat(_T("%lld"), tm.GetTime());
	CString strPwdMd5;
	CEncrypt::CalcMD5(theApp.GetCurCashier()->GetPassword(),strPwdMd5);
	strOrgCode.AppendFormat(_T("%s"), strPwdMd5);
	CString strNewCode;
	CEncrypt::CalcMD5(strOrgCode,strNewCode);

	CString strParam;
	strParam.AppendFormat(_T("userId=%s"),strCasherID);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("netBarId=%d"), nNetbarID);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("dutyId=%d"), nDutyId);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("cashRegisterId=%d"), nCashRegisterId);
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("serial=%lld"), tm.GetTime());
	strParam += _T("&amp;");
	strParam.AppendFormat(_T("check=%s"), strNewCode);


	CString strUrlMain = _T("http://");
	strUrlMain += CNetBarConfig::GetInstance()->GetMainCenterIP();
	strUrlMain.AppendFormat(_T(":%d"), CNetBarConfig::GetInstance()->GetMainCenterPort());
	strUrlMain += _T("/netbar/common/pointcard.jsp");

	CString strUrlBakup = _T("http://");
	strUrlBakup += CNetBarConfig::GetInstance()->GetBackupCenterIP();
	strUrlBakup.AppendFormat(_T(":%d"), CNetBarConfig::GetInstance()->GetBackupCenterPort());
	strUrlBakup += _T("/netbar/common/pointcard.jsp");

	CString strUrls;
	strUrls += strUrlMain;
	strUrls += _T("|");
	strUrls += strUrlBakup;

	//strUrl += strParam;

	//strUrl = _T("http://msdn.microsoft.com/library");

	/*<DdnPayCard Version= "1" DomainId="99" 
		NetbarId="4" Cashier="pm" DdnServer="192.168.10.253:8080">URL</DdnPayCard>*/

	

	CString strDDNPro;
	strDDNPro = _T("<DdnPayCard Version=\"1\" ");   
	strDDNPro.AppendFormat(_T("CashId=\"%d\" "), nCashRegisterId); // 收银台ID
	strDDNPro.AppendFormat(_T("DomainId=\"%d\" "), CNetBarConfig::GetInstance()->GetDomainId());
	strDDNPro.AppendFormat(_T("NetbarId=\"%d\" "), nNetbarID);
	strDDNPro.AppendFormat(_T("Cashier=\"%s\" "), strCasherID);// 收银员名称
	strDDNPro.AppendFormat(_T("DutyId=\"%d\" "), nDutyId);
	strDDNPro.AppendFormat(_T("CasherPwd=\"%s\" "), theApp.GetCurCashier()->GetPassword());

	strDDNPro.AppendFormat(_T("DdnServer=\"%s\">"), CNetBarConfig::GetInstance()->GetDodoNewHost());
	strDDNPro.AppendFormat(_T("%s</DdnPayCard>"),strUrls);


	if (!DdnInitializePcp(strDDNPro))
	{
		IBA_LOG0(_T("error: 调用  DdnPcp.dll 的导出函数DdnInitializePcp失败"));
		return FALSE;
	}

	
	return TRUE;
}

BOOL CCashierHelper::UninitDiankaPlatform()
{
	DdnTerminatePcp();
	IBA_LOG0(_T("卸载点卡平台"));
	return TRUE;
}

BOOL CCashierHelper::SellDianka()
{
	IBA_LOG0(_T("点击点卡销售"));
	DdnShowPcp();
	return TRUE;
}