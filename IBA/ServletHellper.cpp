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
#include "CurrentNetBarUser.h"
#include "RealName\AuditInterface.h"
#include "RealName\RZXRealName.h"
#include "GlobalFunctions.h"
#include "ComputerListView.h"

/*************************************************
Function:       // GetMemberId
Description:    // 使用NetId向中心CheckCard，成功返回MemberId，否则返回0
*************************************************/

UINT CServletHellper::GetMemberId(LPCTSTR lpszNetId)
{
	UINT nCardId = 0;

	return GetMemberId(lpszNetId, nCardId);
}

/*************************************************
Function:       // GetMemberId
Description:    // 使用NetId向中心CheckCard
Calls:          // DoCheckCard
Input:          // lpszNetId - 用来校验的账号
Output:         // nCardId - 返回的卡号
Return:         // 成功返回MemberId，否则返回0
Others:         // None
*************************************************/

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

/*************************************************
Function:       // ActivationMember
Description:    // 向中心进行激活。 （需要中心设置了以及实名程序允许激活）
Calls:          // CCashier::DoActivation
Input:          // nMemberId会员号，nActivationType激活类型(默认为0)
Return:         // 中心激活成功返回TRUE，其他返回FALSE
*************************************************/
BOOL CServletHellper::ActivationMember(UINT nMemberID, UINT nActivationType /*=0*/)
{
	IBA_LOG(_T("CServletHellper::ActivationMember Start - MemberId = %d, ActivationType = %d"),nMemberID,nActivationType);
	//中心是否开启激活设置
	if (CNetBarConfig::GetInstance()->GetNeedActivation() == 0)
	{
		IBA_LOG(_T("CServletHellper::ActivationMember Result - FALSE - NeedActivation was set to FALSE"));
		return FALSE;
	}

	//实名是否允许上机激活
	if (!theApp.GetRealName()->IsCheckInEnabled())
	{
		IBA_LOG(_T("CServletHellper::ActivationMember Result - FALSE - CheckIn is Disabled for this RealName"));
		return FALSE;
	}

	CActivationMethod Activation;

	Activation.SetMemberId(nMemberID);
	Activation.SetMode( CActivationMethod::ACTIVATION_YES );
	Activation.SetDotType(nActivationType);

	IBA_LOG(_T("Temp.CServletHellper::ActivationMember DoActivation - for MemberId = %d"),nMemberID);
	theApp.GetCurCashier()->DoActivation(Activation);

	if (Activation.GetStatusCode() == 0)
	{	
		IBA_LOG(_T("CServletHellper::ActivationMember Result - TRUE - MemberId = %d"),nMemberID);
		//如果是金桥实名，上传用户信息
		if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameJQ)
		{
			try
			{
			SendRegDataToProxy(nMemberID);
			}
			catch (...)
			{
				IBA_LOG(_T("向实名接口发送激活信息出错! %d"), GetLastError());
			}

		}
		return TRUE;
	}
	else
	{
		theApp.IBAMsgBox(Activation.GetStatusMessage());
	}

	IBA_LOG(_T("CServletHellper::ActivationMember Result - FALSE - Execution error with Center!"));
	return FALSE;
}

/*************************************************
Function:       // Synchronize
Description:    // 与本地Server同步
Calls:          // 
Table Accessed: // 
Table Updated: // 
Input:          // 
                  // 
Output:         // 
Return:         // 
Others:         // 
*************************************************/
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

/*************************************************
Function:       // Execute
Description:    // CheckCard从中心取得MemberId，激活用户，中心充值
Calls:          // CServletHellper::GetMemberId,ActivationMember,theApp.GetCurCashier()->DoCredit
Return:         // 未注册的账号，不允许充值，金额错误，激活失败，充值失败均返回FALSE.其他返回TRUE
*************************************************/
BOOL CCreditHelpper::Execute()
{
	m_nMemberId = CServletHellper::GetMemberId(m_strNetId); //从中心取得MemberId

	//未注册的账号，返回FALSE
	if (m_nMemberId == 0)
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_HASNOTUSED); //此账号未注册！
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
		theApp.IBAMsgBox(IDS_MONEYERROR); //请输入开户金额！
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
		strTmp.Format(IDS_CREDITRESULT2, m_strNetId, m_nCreditMoney / 100.0, Credit.GetCreditPresent() / 100.0); //%s，本次充值：%.2f 元， 赠送：%.2f 元。
		theApp.IBAMsgBox(strTmp);
		return TRUE;
	}
	else
	{
		theApp.IBAMsgBox(Credit.GetStatusMessage());
	}

	return FALSE;
}

/*************************************************
Function:       // Execute
Description:    // 从便捷通道栏取输入(space前为账号,后为金额)，
				// 网络正常 - 正常充值(CheckCard从中心取得MemberId，激活用户，中心充值)
				// 网络恢复模式且不为本地用户，正常充值
				// 网络断开且为本地用户的，进行本地充值
Calls:          // CCreditHelpper::Execute, CCashierHelper::CreditLocalUser
Input:          // strCmdLine - 便捷通道输入：格式为NetId CreditAmount
Return:         // 充值成功返回TRUE,否则返回FALSE
*************************************************/

BOOL CCreditHelpper::Execute(CString strCmdLine)
{
	IBA_LOG(_T("便捷通道快速充值"));
	CStringArray sa;
	
	CIBAHelpper::SplitLine(strCmdLine, sa, ' ');

	if (sa.GetCount() < 2)
	{
		theApp.IBAMsgBox(IDS_PARAMERROR);  //对不起，命令参数有误，请重新输入！
		return FALSE;
	}

	//空格前为NetId，后为待充值金额
	CString strNetId = sa.GetAt(0);
	INT nCreditMoney = _tstof(sa.GetAt(1)) * 100;

	//2012/03/05-8238-gxx: 
	if (CIBAConfig::GetInstance()->GetMaxCreditMoney() != 0) //每笔充值的最大金额，为0表示没有限制，在IBA.ini中设置
	{
		INT nMax = CIBAConfig::GetInstance()->GetMaxCreditMoney()*100;
		if (nCreditMoney > nMax)
		{
			theApp.IBAMsgBox(IDS_OVERFLOW_MAXMONEY); //当前的充值金额超过收银台设置的最大充值金额
			return FALSE;
		}
	}
	// --end

	//网络正常，进行充值
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
		//网络恢复模式且不为本地用户，正常充值
		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
		{
			if (!ActivemMember.GetIsLocalUser())
			{
				SetNetId(strNetId);
				SetCreditMoney(nCreditMoney);
				return Execute();
			}
		}
		//网络断开且为本地用户的，才进行本地充值.此处需修改？？
		//else
		//{
		//	if (!ActivemMember.GetIsLocalUser())
		//	{
		//		return FALSE;
		//	}
		//}
		//网络断开的情况
		else
		{
			if (nCreditMoney <= 0)
			{
				return FALSE;
			}

			CString strErrMsg = CCashierHelper::DoCredit_InLocal(ActivemMember, nCreditMoney);
			if(strErrMsg.IsEmpty())
			{
				return TRUE;
			}
			return FALSE;
		}
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

/*************************************************
Function:       //DeregisterUser
Description:    //临时用户退款.
Return:			//成功返回TRUE,失败返回FALSE
*************************************************/
BOOL CDeregisterHelpper::DeregisterUser(UINT nMemberId,CString& ErrMsg)
{
	ErrMsg.Empty();
	CActiveMember ActiveMember;
	if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember))
	{
		ErrMsg.Format(_T("激活列表获取用户失败"));
		return FALSE;
	}
	//用户在线,通知UDO下机
	if(ActiveMember.GetIsOnLine())
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
		if(nIndex >= 0)
		{
			if (nMemberId == ActiveMember.GetRoomMemberId()) //包房用户,nCheckoutType = 2
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId, 2);
			}
			else												//非包房用户,nCheckoutType = 0
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, nMemberId, 0);
			}
			Sleep(500);
		}
	}

	//获取CardId
	UINT nCardId;
	CIBADAL::GetInstance()->GetCardIdFromMember(ActiveMember.GetNetId(),nCardId);
	//临时用户退款
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(nMemberId);
	Deregister.SetPersonalId(ActiveMember.GetPersonalID());
	Deregister.SetMemberName(ActiveMember.GetUserName(FALSE));
	Deregister.SetCardId(nCardId);
	Deregister.SetPassword(ActiveMember.GetPassword());
	//Deregister.SetEndUsingTime(m_strLastLogoutTime);

	BOOL bRes = theApp.GetCurCashier()->DoDeregister(Deregister); 

	//退款失败,返回错误信息
	if(FALSE == bRes)
	{
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			ErrMsg = LOAD_STRING(IDS_ERROR_DBOPERATION);//本地下就是数据库操作失败
		}
		else
		{
			ErrMsg = Deregister.GetStatusMessage();
		}		
		return FALSE;
	}
	else//成功-输出操作日志
	{
		UINT nAll = 0;
		//本地模式,本地退款
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			UINT nDeposit = 0;
			CIBADAL::GetInstance()->GetDepositFromMember(ActiveMember.GetNetId(),nDeposit);
			nAll = ActiveMember.GetAvailavleBalance() + nDeposit;

			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturn);//本地退款
			OperationInfo.SetUserName(ActiveMember.GetUserName(TRUE));
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetOperationAmount(ActiveMember.GetAvailavleBalance());
			OperationInfo.SetDeposit(nDeposit);
			OperationInfo.SetDescription(ActiveMember.GetTerminalID());
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}
		else//正常模式,退款
		{
			nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeReturn);//退款
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(ActiveMember.GetUserName(TRUE));
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetMemberId(nMemberId);
			OperationInfo.SetOperationAmount(Deregister.GetBalance());
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
			OperationInfo.SetOperationTime(ActiveMember.GetActivationTime());
			OperationInfo.SetDescription(ActiveMember.GetTerminalID());
			OperationInfo.SetClassId(ActiveMember.GetClassId());
			OperationInfo.SetTermId(ActiveMember.GetTerminalID());
			OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
			OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
			OperationInfo.SetConsumeMoney(ActiveMember.GetAdvancePayment()-nAll);

			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}

		//报声
		//CIBATTS::GetInstance()->SpeakReturnResult(nAll);                                  

		//更新终端信息(清除用户信息,刷新视图) 
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
		if (nIndex >= 0)                                                                      
		{
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			computer.ClearUserInfo();
			CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);
			CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
		}
	}
	return TRUE;
}

//自动退款功能
/*************************************************
Function:       //AllowAutoRefund
Description:    //是否允许自动退款。以下情况为不允许自动退款：
				//会员/后台设置不允许退款/后台设置只允许退押金但账户还有钱/在线的/包时的/新开账号未消费过的/已下机时间 < 收银台设置的自动退款时间
Input:          //Balance - 用户Balance信息
Return:         //允许自动退款返回true,否则返回false
*************************************************/
BOOL CDeregisterHelpper::AllowAutoRefund(CBalanceMethod & Balance)
{		
	//if (!CIBAConfig::GetInstance()->GetAutoRefundment()) //是否启用了刷卡自动退款
	//{
	//	return FALSE;
	//}

	if (Balance.IsNetbarMember()) //会员用户以及后台设置不允许退款，不允许退款
	{
		return FALSE;
	}

	if (Balance.GetAllowReturn() == 1) //允许退款 - 后台设置0:不允许 1:允许 2:只退押金
	{
		if (Balance.GetReturnAmount() > 0) //应退余额。中心返回都是0.
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

	//if (Balance.GetGuarantyMoney() > 0) //有押金的
	//{
	//	return FALSE;
	//}

	if (Balance.GetOnline() == 1) //在线 
	{
		return FALSE;
	}
	
	if (Balance.GetPayType() > 0) //包时
	{
		return FALSE;
	}

	CString logoutTime = Balance.GetLogoutTime();
	if (logoutTime.GetLength() > 8 &&  logoutTime.Left(8).CompareNoCase(_T("19700101")) == 0)//新开的账号不退
	{
		return FALSE;
	}
	//if (Balance.GetLogoutTime().CollateNoCase(_T("19700101080000")) == 0 || 
	//	Balance.GetLogoutTime().CollateNoCase(_T("19700101070000")) == 0) //新开的账号不退
	//{
	//	return FALSE;
	//}

	COleDateTime dtLogoutTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetLogoutTime());
	
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	
	COleDateTimeSpan dtSpan = dtNow - dtLogoutTime; //下机离现在多长时间了

	if (dtSpan.GetTotalMinutes() < CActiveMember::GetRefundmentTime())//已下机时间 < 收银台设置的自动退款时间
	{
		return FALSE;
	}
	
	return TRUE;
}
/*************************************************
Function:       //AllowAutoRefundInLocalMode
Description:    //是否允许自动退款。以下情况为不允许自动退款：
				//会员/后台设置不允许退款/后台设置只允许退押金但账户还有钱/在线的/包时的/新开账号未消费过的/已下机时间 < 收银台设置的自动退款时间
Input:          //Balance - 用户Balance信息
Return:         //允许自动退款返回true,否则返回false
*************************************************/
BOOL CDeregisterHelpper::AllowAutoRefundInLocalMode(CActiveMember& ActiveMember, int nParaAllowReturn, COleDateTime checkouttime)
{		


	if (nParaAllowReturn == 2) //只退押金
	{
		if (ActiveMember.GetAvailavleBalance() > 0) //合计账户还有钱，不能退
		{
			return FALSE;
		}
	}

	if (ActiveMember.GetIsOnLine() == 1) //在线 
	{
		return FALSE;
	}
	
	if (ActiveMember.GetPayType() > 0) //包时
	{
		return FALSE;
	}

	if(CIBAHelpper::IsValidTime(checkouttime) && checkouttime > COleDateTime(1970,1,1,0,0,0))
	{
		CString logoutTime = checkouttime.Format(_T("%y%m%d%H%M%S"));
		if (logoutTime.GetLength() > 8 &&  logoutTime.Left(8).CompareNoCase(_T("19700101")) == 0)//新开的账号不退
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	COleDateTime dtNow = COleDateTime::GetCurrentTime();
	
	COleDateTimeSpan dtSpan = dtNow - checkouttime; //下机离现在多长时间了

	if (dtSpan.GetTotalMinutes() < CActiveMember::GetRefundmentTime())//已下机时间 < 收银台设置的自动退款时间
	{
		return FALSE;
	}
	
	return TRUE;
}
/*************************************************
Function:       //SendRegDataToProxy
Description:    //向实名上报用户信息，如RegisterDlg::SendRegDataToProxy
Calls:          //auditInterface.regUser_
Table Accessed: //None
Table Updated:  //None
Input:          //nMemberId - 用户号
Output:         //None
Return:         //None
Others:         //None
*************************************************/
void CServletHellper::SendRegDataToProxy(UINT nMemberId)
{
	if(!PathFileExists(theApp.GetWorkPath() + _T("\\IBABin\\AuditInterface.dll")))
	{
		return ;
	}
	//从本地服务器中查找用户信息
	CString sql;
	sql.Format(_T("select * from member where memberId= %d"), nMemberId);

	CString strSex;
	CString strNation; 
	int nIdType;
	int nUserClassID;
	CString strPersonalID; 
	CString strName;
	CString strAddr;
	CString strTelNum;
	CString strOfficer;
	CString strNetId;

	GxxDictionaryPtr memberPtr = CIBADAL:: GetInstance ()->ReadRecordFromDB (sql);
	if(memberPtr != NULL)
	{
		strSex = CString((memberPtr->valueForKey("sex")->stringValue()));
		strNation = IBAGlobal::StringValueForKey("job",memberPtr);
		nIdType = memberPtr->valueForKey("idType")->intValue();
		nUserClassID = memberPtr->valueForKey("classId")->intValue();
		strPersonalID = IBAGlobal::StringValueForKey("idNumber",memberPtr); 
		strName = IBAGlobal::StringValueForKey("name",memberPtr);
		strAddr = IBAGlobal::StringValueForKey("address",memberPtr);
		strTelNum = IBAGlobal::StringValueForKey("tel",memberPtr);
		strOfficer = IBAGlobal::StringValueForKey("department",memberPtr);
		strNetId = IBAGlobal::StringValueForKey("serialNum",memberPtr);
	}
	else
	{
		IBA_LOG(_T("CServletHellper::SendRegDataToProxy - No Record found in member in LocalServer for MemberId = %d, Can not send info to RealName after activation!"),nMemberId);
		return;
	}


	CAuditInterface auditInterface;
	if(!auditInterface.Load())
	{
		IBA_LOG0(_T("加载实名接口动态库AuditInterface.dll失败！请检查AuditInterface.dll的版本！"));
		return ;
	}
	auditInterface.uninit_();
	CStringA strIpA = CT2A(CNetBarConfig::GetInstance()->GetProxyIP());
	auditInterface.init_(strIpA, 7834);
	// 性别
	//m_cboSex.GetLBText(m_cboSex.GetCurSel(), strSex);

	int isex;
	if (strSex == _T("0")) isex = 1;else isex  = 0;
	int nNation = NS_REALNAME::CRZXRealName::GetNationCode(strNation);
	UINT nNetbarId = CNetBarConfig::GetInstance()->GetNetBarId();
	COleDateTime dt;
	dt = COleDateTime::GetCurrentTime();
	CString strDatetime;
	strDatetime = dt.Format(_T("%Y-%m-%d"));

	CString validDate("2050-01-01");
	CString strBrithday = NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ(strPersonalID);

	CStringA strPersonalIDA = CT2A(strPersonalID);
	CStringA strBrithdayA = CT2A(strBrithday);
	CStringA strDatetimeA = CT2A(strDatetime);
	CStringA strValidDateA = CT2A(validDate);

	CStringA strNameA = CT2A(strName);
	CStringA strAddrA = CT2A(strAddr);
	CStringA strTelNumA = CT2A(strTelNum);
	CStringA strOfficerA = CT2A(strOfficer);
	CStringA strNetIdA = CT2A(strNetId);

	int retcode = -1;
	try
	{
	int ret = auditInterface.regUser_(nNetbarId, nIdType, strPersonalIDA, strNameA, 
		isex, "CN", strAddrA, strTelNumA, nMemberId, strBrithdayA,
		strDatetimeA, strValidDateA, nNation, strOfficerA, nUserClassID, strNetIdA, &retcode);

	if (ret == 0)
	{
		if (retcode == 1 || retcode == -6 )
		{
			IBA_LOG0(_T("向实名接口传激活信息成功（任子行实名）!"));
		}
	}
	else if (ret == ERROR_NETWORK_FAILED)
	{
		IBA_LOG0(_T("网络故障,上传用户信息失败!"));
	}
	else
	{
		IBA_LOG0(_T("其它异常!"));
	}
	}
	catch(...)
	{
		IBA_LOG(_T("向实名激活抛出异常:%d"), GetLastError());
	}

}