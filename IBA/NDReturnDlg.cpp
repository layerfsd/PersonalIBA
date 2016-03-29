// NDReturnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "MainFrm.h"
#include "Cashier.h"
#include "IBAPrint.h"
#include "IBAHelpper.h"
#include "ServletHellper.h"
#include "dal\IBADAL.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include "RealName\IBARealName.h"
#include "ComputerListView.h"
#include "ActiveMemberView.h"
#include "DialogManager.h"
#include "NDCreditDlg.h"
#include "BundleTimeDlg.h"
#include ".\ndreturndlg.h"
#include "ActiveMemberList.h"
#include ".\Socket\LocalServer.h"
#include "CashierHelper.h"
#include "GlobalFunctions.h"
using namespace IBAGlobal;
// CNDReturnDlg 对话框

IMPLEMENT_DYNAMIC(CNDReturnDlg, CNDDialog)
CNDReturnDlg::CNDReturnDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDReturnDlg::IDD, pParent)
, m_bAutoQuery(FALSE), m_bUserHasCard(FALSE)
, m_bPrintTicket(FALSE)
{
	m_nConsumeMoney = 0;
}

CNDReturnDlg::~CNDReturnDlg()
{
}

void CNDReturnDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strUserInfo);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strOpenNetBar);
	DDX_Text(pDX, IDC_STATIC_TERMID, m_strTermId);
	DDX_Text(pDX, IDC_STATIC_BTENDTIME, m_strBundleTimeInfo);
	DDX_Control(pDX, IDC_BUTTON_CONSUMELIST, m_btnConsumeList);
	DDX_Control(pDX, IDC_BUTTON_CREDIT, m_btnCredit);
	DDX_Control(pDX, IDC_BUTTON_RETURN2, m_btnReturn2);
	DDX_Control(pDX, IDC_BUTTON_BUNDLETIME, m_btnBundleTime);
	DDX_Control(pDX, IDC_BUTTON_CONSUMELIST2, m_btnZSBConsumeList);
	DDX_Control(pDX, IDC_BUTTON_CANCELACTIVE, m_btnCancelActivation);

	DDX_Check(pDX, IDC_CHECK_TICKET, m_bPrintTicket);
}


BEGIN_MESSAGE_MAP(CNDReturnDlg, CNDDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREDIT, OnBnClickedButtonCredit)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CONSUMELIST, OnBnClickedButtonConsumelist)
	ON_BN_CLICKED(IDC_BUTTON_RETURN2, OnBnClickedButtonReturn2)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_BUNDLETIME, OnBnClickedButtonBundletime)
	ON_BN_CLICKED(IDC_BUTTON_CONSUMELIST2, OnBnClickedButtonConsumelist2)
	ON_BN_CLICKED(IDC_BUTTON_CANCELACTIVE, OnBnClickedButtonCancelactive)
END_MESSAGE_MAP()


// CNDReturnDlg 消息处理程序
/*************************************************
Function:       // CNDReturnDlg::OnInitDialog
Description:    // 进行初始设置。根据当前实名设置，显示/隐藏ScanReco(扫描)按钮
				// 如存在localActiveMember，创建计时器，0.1s后，
					//本地模式:执行OnBnClickedButtonQuery。
					//本地模式查询，设置按钮显示，根据会员类型调整窗口显示，金额向下取整。标记m_bNDHasLocalQuery。
					//1.用m_localActiveMember填充Dialog,显示NetId在工具栏。标记m_bNDHasLocalQuery。
					//2.禁用包时,消费记录,增收宝记录按钮
					//3.根据用户类型设置窗口标题,结账按钮文字
					//4.设置用户信息,总金额(进行向下50取整)(同时设置m_localActiveMember的AvailableBalance)-都算上加上押金
					//5.刷新显示
				// 如不存在localActiveMember，调用OnGetBalanceOK,设置窗口的m_nMemberId和m_nCardId from CCurrentNetBarUser::Balance，用CCurrentNetBarUser来为退款对话框设置各成员值，并刷新显示
					//正常模式
					//为Dialog设置各成员值,将总金额(账户所有金额)格式化为固定格式方便显示,(该处四舍五入是在中心进行的)
					//填充押金,登出时间,用户信息,根据收银台设置是否打印,
					//为包时的临时用户设置包时信息显示和退款按钮显示
					//刷新窗口显示(更新所有值) 
Calls:          // OnGetBalanceOK
Table Accessed: // None
Table Updated:  // None
Input:          // None
Output:         // None
Return:         // FALSE. （只要当需要设置焦点在某个控件上时才返回TRUE）
Others:         // None
*************************************************/
BOOL CNDReturnDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog(); //进行初始设置。根据当前实名设置，显示/隐藏ScanReco(扫描)按钮

	InitControls(); //初始化控件字体

	//2015-0828 liyajun
	if (m_ActiveMember.GetMemberID() > 0) //如存在localActiveMember，创建计时器用于AutoQuery,m_bAutoQuery=TRUE
	{
		m_bAutoQuery = TRUE;
	}	
	if (m_bAutoQuery)
	{
		SetTimer(TimeIdOfAutoQuery, 100, NULL); //0.1s后，本地模式:执行OnBnClickedButtonQuery将m_localActiveMember的值填充到Dialog各成员中.
	}
	else                                       //如不存在localActiveMember，设置窗口的m_nMemberId和m_nCardId from CCurrentNetBarUser::Balance
	{
		m_nMemberId = CCurrentNetBarUser::GetInstance()->Balance.GetMemberId();
		m_nCardId = CCurrentNetBarUser::GetInstance()->Balance.GetCardId(); 
		OnGetBalanceOK(CCurrentNetBarUser::GetInstance()->Balance); //用CCurrentNetBarUser来为退款对话框设置各成员值，并刷新显示

		SetTimer(TimeIdOfAutoQuery, 800, NULL); //正常模式，定时器只有一个依设置而定的设置焦点动作
	}	

	GetDlgItem(IDOK)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNDReturnDlg::OnBnClickedButtonCredit()
{
	CBCGPDialog::OnCancel();

	CNDCreditDlg dlg;
	//if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	//{
	//	dlg.SetLocalActiveMember(m_LocalActiveMember);
	//}
	//else
	//{
	dlg.SetNetId(m_strNetId);
	//}
	dlg.DoModal();
}

void CNDReturnDlg::OnBnClickedCancel()
{
	AutoClose();
}

/*************************************************
Function:       // OnBnClickedOk
Description:    // IBA进行结账，断网情况下，进行本地结账，结账不成功(主要是localMember表操作不成功)则弹出提示。 
				// 联网情况下，会员调用PayoutMember，临时用户通知客户端下机，调用DeregisterUser注销
Calls:          // DoLocalModeOperate，PayoutMember，DeregisterUser
Table Accessed: // None
Table Updated: //  None
Input:          // None
Output:         // None
Return:         // None
Others:         // 退款界面点击退款
*************************************************/
void CNDReturnDlg::OnBnClickedOk()
{
	CWaitCursor WaitCursor;
	UpdateData();

	CString strErrMsg;
	BOOL bSuccess(FALSE);
	if (m_bIsMember)//会员结账
	{
		bSuccess = PayoutMember(strErrMsg);
	}
	else//临时用户退款
	{
		bSuccess = DeregisterUser(strErrMsg);
	}

	if(bSuccess)//成功,退出
	{
		CBCGPDialog::OnOK();
	}
	else//失败,显示失败信息
	{
		SetToolTipPos(IDOK);
		ShowToolTip(strErrMsg,TRUE); 
		OnBnClickedButtonQuery();
	}
}

void CNDReturnDlg::OnBnClickedButtonConsumelist()
{
	GetDlgItem(IDOK)->SetFocus();

	COperationInfo OperationInfo;

	OperationInfo.SetMemberId(m_nMemberId);
	OperationInfo.SetUserName(m_strName);
	OperationInfo.SetNetId(m_strNetId);

	OperationInfo.SetOperationTime(m_ActiveMember.GetActivationTime());
	OperationInfo.SetIsMember(m_ActiveMember.IsMember());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, MAKELONG(2,0), (LPARAM)&OperationInfo);
}

void CNDReturnDlg::OnBnClickedButtonConsumelist2()
{
	GetDlgItem(IDOK)->SetFocus();

	COperationInfo OperationInfo;

	OperationInfo.SetMemberId(m_nMemberId);
	OperationInfo.SetUserName(m_strName);
	OperationInfo.SetNetId(m_strNetId);

	OperationInfo.SetOperationTime(m_ActiveMember.GetActivationTime());
	OperationInfo.SetIsMember(m_ActiveMember.IsMember());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, MAKELONG(2,4), (LPARAM)&OperationInfo);
}

/*************************************************
Function:       //PayoutMember
Description:    //会员结账.在线DocheckOut2,不在线DoActivation(ACTIVATION_NO).
Calls:          //DoCheckout2 or DoActivation
Return:         //请求失败，返回FALSE.
*************************************************/
BOOL CNDReturnDlg::PayoutMember(CString &ErrMsg)
{
	ErrMsg.Empty();

	CWaitCursor WaitCursor;
	CActiveMember ActiveMember;
	if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId,ActiveMember))
	{
		ErrMsg = _T("用户已结账");
		IBA_LOG(_T("用户已结账"));
		return FALSE;
	}

	//会员在线
	if (ActiveMember.GetIsOnLine())
	{
		//人工登出会员
		CCheckout2Method Checkout2;
		Checkout2.SetSafeLogout(0); 
		Checkout2.SetMemberId(m_nMemberId);
		BOOL bRes = theApp.GetCurCashier()->DoCheckout2(Checkout2);	

		//退款失败,返回错误信息(正常模式下,只有返回-1才算失败)
		if(FALSE == bRes)
		{
			if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
			{
				ErrMsg = LOAD_STRING(IDS_ERROR_DBOPERATION);//本地下就是数据库操作失败
				return FALSE;
			}
			else
			{
				if(-1 == Checkout2.GetStatusCode())
				{
					ErrMsg = Checkout2.GetStatusMessage();
					return FALSE;
				}				
			}			
		}

		//只要连接成功
		if(TRUE == bRes || -1!= Checkout2.GetStatusCode())
		{
			//更改终端信息并更新终端视图(清除用户信息,状态改为锁屏)
			INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);
			if (nIndex >= 0) 
			{
				if(0 == Checkout2.GetStatusCode() || TRUE == bRes )
				{
					CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, m_nMemberId,2);//设置为2,会员下机
				}
				else
				{
					//如用户已登出或其他错误情况,向UDO说明是换机.
					CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, m_nMemberId,0);//设置为0,换机/余额不足被踢出
				}				
				Sleep(1000);
				CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
				computer.SetComputerStatus(CComputerInfo::ECS_LOCKED);
				computer.ClearUserInfo();
				CLocalServer::GetInstance()->ComputerList.UpdateComputer(nIndex, computer);
				CLocalServer::GetInstance()->GetComputerListView()->Refresh(FALSE);
			}
			//打印小票
			if (m_bPrintTicket)
			{
				CString strTicket;
				CIBAPrint::GetRefundTicket(strTicket,m_bIsMember);

				strTicket.Replace(_T("[name]"), m_strName);
				strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
				strTicket.Replace(_T("[memberclass]"), m_strUserClass);
				//CString strTmp;
				//strTmp.Format(_T("%.2f"), m_nConsumeMoney/100.0);
				//strCreditMoney.Format(_T("%.2f"), m_nCreditMoney/100.0);
				//strTicket.Replace(_T("[Consume]"), strTmp); // 消费
				strTicket.Replace(_T("[return]"), _T("0.00")); // 会员用户退款金额为0
				strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
				strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
				CString strMemberID;
				strMemberID.Format(_T("%d"), m_nMemberId);
				strTicket.Replace(_T("[memberid]"), strMemberID);
				strTicket.Replace(_T("[totalmoney]"), m_strAllBalance);

				CIBAPrint::Print(strTicket);
			}
		}
		else//请求失败,返回-1.返回错误信息
		{
			ErrMsg = Checkout2.GetStatusMessage();
			return FALSE;
		}
	}
	else//会员不在线.取消激活
	{
		BOOL bSuccess(TRUE);
		//取消激活,从激活列表中删除,更新视图
		CActivationMethod Activation;
		Activation.SetMemberId(m_nMemberId);
		Activation.SetMode(CActivationMethod::ACTIVATION_NO);

		//本地模式
		if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			//查询LocalMember是否有记录
			CLocalMember lMember;
			BOOL bHasRecord = CIBADAL::GetInstance()->GetLocalMemberByMemberID(m_nMemberId,lMember);
			if(bHasRecord)//有记录,更新退款信息
			{
				COleDateTime CheckoutTime = COleDateTime::GetCurrentTime();
				if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(m_nMemberId,CheckoutTime,TRUE))
				{
					IBA_LOG(_T("Error.更新本地结账记录失败."));
					bSuccess = FALSE;
				}
			}
			else//无记录,创建记录,插入记录,更新记录
			{
				GxxDictionaryPtr lmPrt = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(m_nMemberId);  //创建记录
				if(lmPrt)
				{
					if(CIBADAL::GetInstance()->InsertNewLocalMember(lmPrt))//插入记录
					{
						COleDateTime CheckoutTime(0,0,0,0,0,0);//取消激活时,登出时间设为无效
						if(0 > CIBADAL::GetInstance()->UpdateLocalMember_CheckOutInfo(m_nMemberId,CheckoutTime,TRUE))//更新记录
						{
							IBA_LOG(_T("Error.更新本地结账记录失败."));
							//插入成功但更新失败的，仍当作成功
						}	
					}
					else
					{
						IBA_LOG(_T("Error.为结账插入本地记录失败."));
						bSuccess =  FALSE;
					}
				}
				else
				{
					IBA_LOG(_T("Error.为结账创建本地记录失败."));
					bSuccess = FALSE;
				}
			}
		}
		else//正常模式
		{
			//取消激活
			theApp.GetCurCashier()->DoActivation(Activation);
		}

		//if(FALSE == theApp.GetCurCashier()->DoActivation(Activation))
		//{
		//	ErrMsg = Activation.GetStatusMessage();
		//	return FALSE;
		//}

		//金桥实名不在线也进行实名下机 - 孙珂要求201160114
		//if(bSuccess)
		//{
		//	if(CIBAGlobal::emRealnameJQ == CNetBarConfig::GetInstance()->GetAuditType())
		//	{
		//		IBA_LOG(_T("金桥实名下机 - 用户不在线,TermId及IPAddress为空"));
		//		CString strTerminalID(_T(""));
		//		CString strComputerIP(_T(""));
		//		theApp.GetRealName()->AuditUserCheckOut(strTerminalID,strComputerIP, m_nMemberId);
		//	}
		//}

		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(m_nMemberId);
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
	}

	//输出操作日志
	COperationInfo OperationInfo;
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)//本地模式 - 本地结账
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturnVip);
	}
	else
	{
		OperationInfo.SetOperationType(CIBAGlobal::emOpTypeVipReturn);
	}		
	OperationInfo.SetNetId(m_strNetId);
	OperationInfo.SetUserName(m_strName);
	OperationInfo.SetDescription(m_strTermId);
	OperationInfo.SetClassId(ActiveMember.GetClassId());
	OperationInfo.SetCheckInTime(ActiveMember.GetCheckInTime());
	OperationInfo.SetIdNum(ActiveMember.GetPersonalID());
	OperationInfo.SetTermId(ActiveMember.GetTerminalID());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

	return TRUE;
}


void CNDReturnDlg::InitControls()
{
	CIBAHelpper::CreateFont(m_BigFont, 22, 900);

	GetDlgItem(IDC_STATIC_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_TERMID)->SetFont(&m_Font);

	// 2014-11-5 - qsc 智多下不能“取消激活”防止取消激活和结账导致发了多次结账信息给实名
	if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType() )
	{
		GetDlgItem(IDC_BUTTON_CANCELACTIVE)->ShowWindow(SW_HIDE);
	}
}

/*************************************************
Function:       // CNDReturnDlg::OnGetBalanceOK
Description:    // 为Dialog设置各成员值,将总金额(账户所有金额)格式化为固定格式方便显示(根据需要显示不同金额),
				// 填充押金,登出时间,用户信息,根据收银台设置是否打印,
				// 为包时的临时用户设置包时信息显示和退款按钮显示
				// 刷新窗口显示(更新所有值)
Others:         // Virtual function. 
*************************************************/
void CNDReturnDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance); //为Dialog各成员赋值. 设置m_ActiveMember

	if (m_nUserClassID == 1) //临时用户
	{
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(IDS_NONVIP_RETURN)); //显示为：普通卡结账(&R)		
		SetWindowText(LOAD_STRING(IDS_PAYOUTNONMEMBER)); // 显示为: 普通卡结账
	}
	else //VIP会员
	{		
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(IDS_VIP_RETURN)); //显示为：会员卡结账(&R)
		SetWindowText(LOAD_STRING(IDS_PAYOUTMEMBER)); // 显示为: 会员卡结账
	}	
	//如果是包房用户，显示为包房结账 
	if (m_ActiveMember.GetRoomMemberId() == m_nMemberId)
	{
		UINT nID = m_bIsMember?IDS_ROOM_CHECKOUT:IDS_ROOM_RETURN;
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(nID)); //显示为：包房结账(&R)
		GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(nID));
		SetWindowText(LOAD_STRING(nID)); 
	}

	ShowBundleTimeInfo(Balance); //为包时的临时用户设置包时信息显示和退款按钮显示

	m_strUserInfo.Format(_T("%s, %s"), m_strName, m_strUserClass); //用户信息="用户姓名, 用户级别"

	m_strDeposit.Format(_T("%.2f"), (double)m_nMoneyGuaranty / 100.0); //押金字符串

	m_strLastLogoutTime = Balance.GetLogoutTime(); //登出时间,yyyymmddhhmmss

	m_nConsumeMoney = Balance.GetLastConsumeAmount(); //上次消费金额(分)，在线值为0

	//会员或设置了不允许退款，显示金额不包括押金
	if(Balance.GetIsMember() || !Balance.GetAllowReturn())
	{
		GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_BALANCE)); //显示为：余额
		m_strAllBalance.Format(_T("￥%.2f"), (double)m_nMoneyAllInAccount / 100.0); //不包括押金
	}
	else //允许退款
	{	
		if(Balance.GetAllowReturn()==2)//只退押金
		{
			GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_REAL_RETURN)); //显示为：实退
			m_strAllBalance.Format(_T("￥%.2f"), (double)m_nMoneyGuaranty / 100.0); //只退押金
		}
		else
		{
			GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_REAL_RETURN)); //显示为：实退
			//m_strAllBalance.Format(_T("￥%.2f"), (double)(m_nMoneyAllInAccount + m_nMoneyGuaranty) / 100.0); //包括押金
			//改为显示Balance的returnAmount
			m_strAllBalance.Format(_T("￥%.2f"), (double)(Balance.GetReturnAmount() + m_nMoneyGuaranty) / 100.0); //包括押金
		}
	}	

//#ifdef __USING_INTER_RESOURCE
//	m_strAllBalance.Format(_T("￥%.2f"), m_nMoneyAllInAccount / 100.0);    //格式化余额显示
//#else
//	m_strAllBalance.Format(_T("%.2f"), m_nMoneyAllInAccount / 100.0); 
//#endif	

	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, m_ActiveMember); //获取激活用户信息

	//m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintReturnTicket(m_bIsMember); m_nUserClassID == 2
	m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintReturnTicket(m_nUserClassID == 2); 
	
	UpdateData(FALSE); //根据变量值，更新窗口显示

	RedrawWindow();

	//COleDateTimeSpan span = COleDateTime::GetCurrentTime() - m_ActiveMember.GetActivationTime();
	//if (span.GetTotalSeconds() <= 5.0)
	//{
	//	// 不允许在激活后5秒内弹出退款界面, 在此弹出充值界面
	//	CNDReturnDlg::OnBnClickedButtonCredit();
	//	return;
	//}
}


HBRUSH CNDReturnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ALL)
		{
			if (m_bIsMember)
			{
				pDC->SetTextColor(RGB(0, 0, 240)); // 会员用蓝色表示
			}
			else
			{
				pDC->SetTextColor(RGB(255, 0, 0)); // 临时用户用红色表示
			}
		}
		
	}

	return hbr;
}

/*************************************************
Function:       //DeregisterUser
Description:    //临时用户退款.
Return:			//成功返回TRUE,失败返回FALSE
*************************************************/
BOOL CNDReturnDlg::DeregisterUser(CString &ErrMsg,UINT nReturnMode /*= 0*/)
{
	ErrMsg.Empty();
	//用户在线,通知UDO下机
	if(m_ActiveMember.GetIsOnLine())
	{
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);
		if(nIndex >= 0)
		{
			if (m_nMemberId == m_ActiveMember.GetRoomMemberId()) //包房用户,nCheckoutType = 2
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, m_nMemberId, 2);
			}
			else												//非包房用户,nCheckoutType = 0
			{
				CLocalServer::GetInstance()->LocalCheckOutUser(nIndex, m_nMemberId, 0);
			}
			Sleep(500);
		}
	}
	//临时用户退款
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(m_nMemberId);
	Deregister.SetPersonalId(m_strPersonalID);
	Deregister.SetMemberName(m_strMemberName);
	Deregister.SetCardId(m_nCardId);
	Deregister.SetPassword(m_strPwd);
	Deregister.SetEndUsingTime(m_strLastLogoutTime);

	BOOL bRes = theApp.GetCurCashier()->DoDeregister(Deregister,nReturnMode); 

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
			nAll = m_nMoneyAllInAccount + m_nMoneyGuaranty;
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeBrokenReturn);//本地退款
			OperationInfo.SetUserName(m_strName);
			OperationInfo.SetNetId(m_strNetId);
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetOperationAmount(m_nMoneyAllInAccount);
			OperationInfo.SetDeposit(m_nMoneyGuaranty);
			OperationInfo.SetDescription(m_strTermId);
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}
		else//正常模式,退款
		{
			nAll = Deregister.GetBalance() + Deregister.GetGuarantyMoney();//合计
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeReturn);//退款
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(m_strName);
			OperationInfo.SetNetId(m_strNetId);
			OperationInfo.SetMemberId(m_nMemberId);
			OperationInfo.SetOperationAmount(Deregister.GetBalance());
			OperationInfo.SetAllAmount(nAll);
			OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
			OperationInfo.SetOperationTime(m_ActiveMember.GetActivationTime());
			OperationInfo.SetDescription(m_strTermId);
			OperationInfo.SetClassId(m_ActiveMember.GetClassId());
			OperationInfo.SetTermId(m_ActiveMember.GetTerminalID());
			OperationInfo.SetIdNum(m_ActiveMember.GetPersonalID());
			OperationInfo.SetCheckInTime(m_ActiveMember.GetCheckInTime());
			OperationInfo.SetConsumeMoney(m_ActiveMember.GetAdvancePayment()-nAll);

			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}

		//报声
		CIBATTS::GetInstance()->SpeakReturnResult(nAll);

		//隐藏消费窗口
		m_btnConsumeList.ShowWindow(SW_HIDE);                                        

		//打印小票
		if (m_bPrintTicket) 
		{
			CString strTicket;
			CIBAPrint::GetRefundTicket(strTicket,m_bIsMember);

			strTicket.Replace(_T("[name]"), m_strName);
			strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
			strTicket.Replace(_T("[memberclass]"), m_strUserClass);
			//CString strTmp;
			//strTmp.Format(_T("%.2f"), nAll/100.0);			
			//strTicket.Replace(_T("[Consume]"), strTmp); // 消费
			strTicket.Replace(_T("[return]"), m_strAllBalance);
			strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
			strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
			CString strMemberID;
			strMemberID.Format(_T("%d"), m_nMemberId);
			strTicket.Replace(_T("[memberid]"), strMemberID);
			strTicket.Replace(_T("[totalmoney]"), _T("0.00")); // 临时用户退款金额为0

			CIBAPrint::Print(strTicket);
		}
		//更新终端信息(清除用户信息,刷新视图) 
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);
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

void CNDReturnDlg::SetNetId(CString newVal)
{
	m_strNetId = newVal;

	m_bAutoQuery = TRUE;
}

/*************************************************
Function:       //OnTimer
Description:    //执行指定任务.执行完，销毁定时器.
				//本地模式:执行OnBnClickedButtonQuery。
				-			//本地模式查询，设置按钮显示，根据会员类型调整窗口显示，金额向下取整。标记m_bNDHasLocalQuery。
							//1.用m_localActiveMember填充Dialog,显示NetId在工具栏。标记m_bNDHasLocalQuery。
							//2.禁用包时,消费记录,增收宝记录按钮
							//3.根据用户类型设置窗口标题,结账按钮文字
							//4.设置用户信息,总金额(进行向下50取整)(同时设置m_localActiveMember的AvailableBalance)-都算上加上押金
							//5.刷新显示
				//正常模式:视具体实名设置设置焦点
Calls:          //OnBnClickedButtonQuery
Table Accessed: //None
Table Updated:  //None
Input:          //nIDEvent - 定时器ID，只处理TimeIdOfAutoQuery，且处理完就销毁
Output:         //None
Return:         //None
Others:         //None
*************************************************/
void CNDReturnDlg::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfAutoQuery == nIDEvent) //自动查询定时器
	{
		KillTimer(nIDEvent);

		if (m_bAutoQuery)    //本地模式-本地模式的OnInitDialog中设置
		{
			m_bAutoQuery = FALSE;
			OnBnClickedButtonQuery();  
			//本地模式查询，设置按钮显示，根据会员类型调整窗口显示，金额向下取整。标记m_bNDHasLocalQuery。
			//1.用m_localActiveMember填充Dialog,显示NetId在工具栏。标记m_bNDHasLocalQuery。
			//2.禁用包时,消费记录,增收宝记录按钮
			//3.根据用户类型设置窗口标题,结账按钮文字
			//4.设置用户信息,总金额(进行向下50取整)(同时设置m_localActiveMember的AvailableBalance)-都算上加上押金
			//5.刷新显示
		}
		else
		{
			if (theApp.GetRealName()->RobFocusAfterSwipingCard()) //若使用的实名设置了在刷卡后自动设置焦点，自动移动鼠标以及设置焦点
			{
				SimulateClickMe();	
			}
		}
	}

	CNDDialog::OnTimer(nIDEvent);
}

BOOL CNDReturnDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_F3 == pMsg->wParam)
		{
			OnBnClickedButtonCredit();
			return TRUE;
		}
	}

	return CNDDialog::PreTranslateMessage(pMsg);
}

void CNDReturnDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( int(m_nMemberId) > 0)
	{
		m_Images.Draw(&dc, m_bIsMember ? 1 : 2, CPoint(30, 25), ILD_NORMAL);
	}
	else if( int(m_nMemberId) < 0 )
	{
		m_Images.Draw(&dc, 5, CPoint(30, 25),ILD_NORMAL);
	}
}

/*************************************************
Function:       // ShowBundleTimeInfo
Description:    // 为包时用户设置"包时信息"显示，控制临时用户时"退包时后余额"按钮显示或隐藏(临时用户余额+押金>包时费用即显示)
Calls:          // ..
Table Accessed: // None
Table Updated:  // None
Input:          // Balance - 其中的信息用来设置包时信息,判断金额
Output:         // ..
Return:         // None
Others:         // None
*************************************************/
void CNDReturnDlg::ShowBundleTimeInfo(CBalanceMethod & Balance)
{
	if (Balance.GetPayType() > 0)  //如果不是普通计费,i.e.累计封顶或足额预扣时才进行以下设置
	{
		COleDateTime StartTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetStartTime());
		COleDateTime EndTime = CIBAHelpper::CenterTimeToOleDateTime(Balance.GetEndTime());

		COleDateTimeSpan dtSpan;

		if (COleDateTime::GetCurrentTime() < StartTime)   //还没到包时开始时间,剩余时间为包时时长
		{
			dtSpan = EndTime - StartTime;
		} 
		else                                              //已经开始了包时,剩余时间为包时结束时间-当前时间
		{
			dtSpan = EndTime - COleDateTime::GetCurrentTime();
		}

		m_strBundleTimeInfo.Format(_T("%s 至 %s，剩余 %d:%.2d\r\n\r\n金额(%.1f元)"),   //设置包时时间信息，将显示在IDC_STATIC_BTENDTIME(包时信息栏)
			StartTime.Format(_T("%H:%M")),    
			EndTime.Format(_T("%H:%M")),
			(LONG)dtSpan.GetTotalHours(), dtSpan.GetMinutes(), 
			Balance.GetPrice() / 100.0       //包时金额
			);

		//显示退余额按钮
		//if (!m_bIsMember && (m_nAllMoney - Balance.GetPrice() > 0)) 
		if ((m_nUserClassID==1) && (m_nMoneyAllInAccount - Balance.GetPrice() > 0)) //临时用户且总金额>包时金额，显示"退包时后余额"按钮
		{
			m_btnReturn2.ShowWindow(SW_NORMAL);
		}
		else                                                        //否则隐藏"退包时后余额"按钮
		{
			m_btnReturn2.ShowWindow(SW_HIDE);
		}
	}
}

//退包时后余额
void CNDReturnDlg::OnBnClickedButtonReturn2()
{
	if (IDOK != MsgBox(IDS_RETURN2QUESTION, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}
	UINT nReturnMode = 2;//0:正常退款 1:退款不销户 2:退包时后余额

	//CDeregisterMethod Deregister;

	//Deregister.SetMemberId(m_nMemberId);
	//Deregister.SetPersonalId(m_strPersonalID);
	//Deregister.SetMemberName(m_strMemberName);
	//Deregister.SetCardId(m_nCardId);
	//Deregister.SetPassword(m_strPwd);
	//Deregister.SetEndUsingTime(m_strLastLogoutTime);
	//Deregister.SetReturnMode(2);

	//theApp.GetCurCashier()->DoDeregister(Deregister,nReturnMode);

	//if (Deregister.GetStatusCode() == 0)
	//{	
	//	UINT nAll = Deregister.GetBalance();

	//	//{ 2011/06/30-gxx: 
	//	CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(m_nMemberId,0,0,nAll);
	//	
	//	//}
	//	
	//	CIBATTS::GetInstance()->SpeakReturn2Result(nAll);//报声

	//	//写操作日志
	//	COperationInfo OperationInfo;
	//	OperationInfo.SetOperationType(8);
	//	OperationInfo.SetIsBundTime(FALSE);
	//	OperationInfo.SetUserName(m_strName);
	//	OperationInfo.SetNetId(m_strNetId);
	//	OperationInfo.SetMemberId(m_nMemberId);
	//	OperationInfo.SetOperationAmount(0);
	//	OperationInfo.SetAllAmount(nAll);
	//	OperationInfo.SetDeposit(Deregister.GetGuarantyMoney());
	//	OperationInfo.SetOperationTime(m_ActiveMember.GetActivationTime());
	//	OperationInfo.SetDescription(m_strTermId);
	//	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);

	//	// 2014-10-10 - qsc
	///*	if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
	//	{
	//		NS_REALNAME::CheckoutInfo coi;
	//		ZeroMemory(&coi, sizeof(coi));
	//		_tcsncpy(coi.szName, m_strName, 100);
	//		_tcsncpy(coi.szNetId, m_strNetId, 100);
	//		_tcsncpy(coi.szTermId, m_ActiveMember.GetTerminalID(), 50);

	//		CString strIP;
	//		CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
	//		_tcsncpy(coi.szIP, strIP, 50);
	//		coi.nCheckoutType = CIBAGlobal::cot_TempLogout;

	//		theApp.GetRealName()->CheckOut(&coi);
	//	}*/	
	CString strErrMsg;
	if(DeregisterUser(strErrMsg,nReturnMode))
	{
		UINT nAll = 0;
		nAll = m_nMoneyAllInAccount + m_nMoneyGuaranty;
		ReQuery();		

		SetToolTipPos(IDC_BUTTON_RETURN2);
		ShowToolTipFormat(IDS_RETURN2INFO, nAll / 100.0);
	}
	else
	{	
		SetToolTipPos(IDOK);
		ShowToolTip(strErrMsg, TRUE);
	}
}

//void CNDReturnDlg::OnCenterDisconnect()
//{
//	//m_bCenterDisconnect = TRUE;
//
//	CActiveMember ActiveMember;
//
//	BOOL bRes = CLocalServer::GetInstance()
//		->ActiveMemberList.GetActiveMemberNetID(m_strNetId,ActiveMember);
//
//	m_strName.Format(_T("%s(%s)"), ActiveMember.GetUserName(), CIBAHelpper::GetSexAsString(ActiveMember.GetSex()));
//	m_strPersonalID = ActiveMember.GetPersonalID();
//
//	m_strMemberName = ActiveMember.GetUserName();
//
//#ifdef __USING_INTER_RESOURCE
//	m_strAllBalance.Format(_T("￥%.2f"), (double)ActiveMember.GetAvailavleBalance() / 100.0);
//#else
//	m_strAllBalance.Format(_T("%.2f"), (double)ActiveMember.GetAvailavleBalance() / 100.0);
//#endif
//
//	m_strOpenNetBar = CNetBarConfig::GetInstance()->GetNetBarName(ActiveMember.GetNetbarID());
//
//	m_strTermId = ActiveMember.GetTerminalID();
//
//	m_strUserInfo = m_strMemberName;
//	
//	if (ActiveMember.IsMember())
//	{
//		GetDlgItem(IDC_STATIC0)->SetWindowText(_T("余额(元)："));
//	}
//
//	UpdateData(FALSE);
//}

//2015-0828 liyajun
/*************************************************
Function:       // DoLocalModeReturn
Description:    // 本地结账.打印小票
Calls:          // CCashierHelper::ReturnLocalCenterUser
Return:         // CCashierHelper::ReturnLocalCenterUser
*************************************************/
//BOOL CNDReturnDlg::DoLocalModeReturn()
//{
//	if (m_strNetId.IsEmpty())
//	{
//		IBA_LOG(_T("Abnormal .CNDReturnDlg::DoLocalModeReturn: NetId is Empty!"));
//		return FALSE;
//	}
//
//	BOOL bRet = CCashierHelper::ReturnLocalCenterUser(m_LocalActiveMember);	
//
//	//打印小票
//	//if(bRet)
//	//{
//	//	PrintTicket(m_ActiveMember);		
//	//}
//	return bRet;
//}

void CNDReturnDlg::OnBnClickedButtonBundletime()
{
	CBCGPDialog::OnCancel();

	CDlgBundleTime dlg;
	dlg.SetNetId(m_strNetId, TRUE);
	dlg.DoModal();
}

/*************************************************
Function:       //CNDReturnDlg::DoLocalModeQuery
Description:    //本地模式查询，设置按钮显示，根据会员类型调整窗口显示，金额向下取整。标记m_bNDHasLocalQuery。
				//1.用m_localActiveMember填充Dialog,显示NetId在工具栏。标记m_bNDHasLocalQuery。
				//2.禁用包时,消费记录,增收宝记录按钮
				//3.根据用户类型设置窗口标题,结账按钮文字
				//4.设置用户信息,总金额(进行向下50取整)(根据需要进行设置)
				//5.刷新显示
Calls:          //DoLocalModeQuery,UpdateDate(FALSE),重绘窗口.
Table Accessed: //None
Table Updated:  //None
Input:          //None
Output:         //None
Return:         //
Others:         //与GetBalanceOk功能冲突？？？？否！GetBalanceOk是在正常模式下执行。
*************************************************/
BOOL CNDReturnDlg::DoLocalModeQuery()
{
	//m_bCenterDisconnect = TRUE;  //调用该函数，即认为与中心已断开连接。

	//CActiveMember ActiveMember;
	//本地模式下取m_LocalActiveMember的各值,正常模式or恢复模式下,取当前NetId的ActiveMember
	//if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	//{
	//	ActiveMember = m_LocalActiveMember;
	//}
	//else
	//{
	//	UINT nMemberId;
	//	CIBADAL::GetInstance()->GetMemberId(m_strNetId,nMemberId);
	//	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
	//}

	if (!CNDDialog::DoLocalModeQuery()) //将m_localActiveMember的值填充到Dialog各成员.将NetId显示在工具栏中。标记m_bNDHasLocalQuery=TRUE。
	{
		return FALSE;
	}
	
	if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId,m_ActiveMember)){return FALSE;}
	// 禁用包时,消费记录,增收宝记录按钮 - 断网时
	m_btnBundleTime.ShowWindow(FALSE);     // 禁用包时功能
	m_btnConsumeList.ShowWindow(FALSE);    // 禁用消费记录
	m_btnZSBConsumeList.ShowWindow(FALSE); // 禁用增收宝记录

	//根据用户类型设置窗口标题,结账按钮文字
	if (m_ActiveMember.IsMember())
	{
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(IDS_VIP_RETURN)); //显示为：会员结账(&R)
		SetWindowText(LOAD_STRING(IDS_PAYOUTMEMBER)); // 显示为: 会员结账
	}
	else 
	{
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(IDS_NONVIP_RETURN)); //显示为：普通卡结账(&R)
		SetWindowText(LOAD_STRING(IDS_PAYOUTNONMEMBER)); // 显示为: 普通卡结账
	}
	//如果是包房用户，显示为包房结账 
	if (m_ActiveMember.GetRoomMemberId() == m_nMemberId)
	{
		UINT nID = m_bIsMember?IDS_ROOM_CHECKOUT:IDS_ROOM_RETURN;
		GetDlgItem(IDOK)->SetWindowText(LOAD_STRING(nID)); //显示为：包房结账(&R)
		GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(nID));
		SetWindowText(LOAD_STRING(nID)); 
	}

	//ShowBundleTimeInfo(Balance); //为包时的临时用户设置包时信息显示和退款按钮显示

	m_strUserInfo.Format(_T("%s, %s"), m_strName, m_strUserClass); //用户信息="用户姓名, 用户级别"

	//m_strLastLogoutTime = Balance.GetLogoutTime(); //登出时间,yyyymmddhhmmss
	//m_nConsumeMoney = Balance.GetLastConsumeAmount(); //上次消费金额(分)，在线值为0

	m_strDeposit.Format(_T("%.2f"), (double)m_nMoneyGuaranty / 100.0); //押金字符串

	//取是否允许退款的设置
	UINT nAllowReturn;
	CString sql;
	sql.Format(_T("select allowReturn from member where memberId=%d"),m_ActiveMember.GetMemberID());
	GxxDictionaryPtr MemberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
	if(!MemberPtr)
	{
		IBA_LOG(_T("Error! CNDReturnDlg::DoLocalModeQuery Desp:Access DB Error! Proc: Read allowReturn from Member for MemberId = %d"),m_ActiveMember.GetMemberID());
		GXX_ASSERT(0,"CNDReturnDlg::DoLocalModeQuery从Member表中数据库读取退款设置失败！");
		nAllowReturn = 1;
	}
	else
	{
		nAllowReturn = MemberPtr->valueForKey("allowReturn")->intValue();
	}	

	UINT uAvailableBalance = m_ActiveMember.GetAvailavleBalance();//取余额，并向下按50取整，以只显示整数额
	uAvailableBalance -= uAvailableBalance % 50;
	if (uAvailableBalance < 0)
	{
		uAvailableBalance = 0;
	}
	//会员或设置了不允许退款，显示金额不包括押金
	if(m_bIsMember || nAllowReturn == 0)
	{
		GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_BALANCE));// 显示为：余额:
		m_strAllBalance.Format(_T("￥%.2f"), (double)uAvailableBalance / 100.0); //不包括押金
	}
	else //允许退款
	{	
		if(nAllowReturn==2)//只退押金
		{
			GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_REAL_RETURN)); //显示为：实退
			m_strAllBalance.Format(_T("￥%.2f"), (double)m_nMoneyGuaranty / 100.0); //只退押金
		}
		else
		{
			GetDlgItem(IDC_STATIC0)->SetWindowText(LOAD_STRING(IDS_REAL_RETURN)); //显示为：实退
			m_strAllBalance.Format(_T("￥%.2f"), (double)(uAvailableBalance + m_nMoneyGuaranty) / 100.0); //包括押金
		}
	}

	m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintReturnTicket(m_nUserClassID == 2); 

//#ifdef __USING_INTER_RESOURCE
//	m_strAllBalance.Format(_T("￥%.2f"), nTmp / 100.0);
//#else
//	m_strAllBalance.Format(_T("%.2f"), nTmp / 100.0);
//#endif

	UpdateData(FALSE); //更新窗口数据显示

	RedrawWindow();
	return TRUE;
}

/*************************************************
Function:       //DoLocalModeOperate
Description:    //检查NetId和m_localActiveMember。本地结账m_localActiveMember,更新或添加localMember记录(isReturned),通知终端下机，更新用户视图和终端视图
Calls:          //DoLocalModeReturn 
Return:         //本地结账成功返回TRUE。否则返回FALSE 
*************************************************/
//BOOL CNDReturnDlg::DoLocalModeOperate()
//{
//	if (m_strNetId.IsEmpty() || m_LocalActiveMember.GetMemberID() == 0)
//	{
//		IBA_ASSERT2(0, "无效的本地用户");
//		IBA_LOG(_T("Abnormal.CNDReturnDlg::DoLocalModeOperate:NoNetId or no MemberId=0 in m_localActiveMember!"));
//		return FALSE;
//	}
//
//	return DoLocalModeReturn(); //本地结账m_localActiveMember,更新或添加localMember记录(isReturned),通知终端下机，更新用户视图和终端视图
//}

BOOL CNDReturnDlg::ReturnRoomUser()
{
	CRoomCheckOut roomCheckOut;
	roomCheckOut.SetMemberId(m_nMemberId);
	roomCheckOut.SetPCClass(m_ActiveMember.GetPCClassID());

	theApp.GetCurCashier()->DoRoomCheckOut(roomCheckOut);

	if (roomCheckOut.GetStatusCode() == 0)
	{
		CArray<UINT> arrMembers;
		if (!CLocalServer::GetInstance()->ActiveMemberList.GetRoomSMembers(m_nMemberId, arrMembers))
		{
			IBA_LOG0(_T("获取包房内的所有用户失败"));
			return FALSE;
		}

		for(int i=0; i < arrMembers.GetCount(); i++)
		{
			UINT nMemberId = arrMembers.GetAt(i);
			CActiveMember ActiveMember;
			CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember);
			INT_PTR nIn = CLocalServer::GetInstance()->ComputerList.Find(nMemberId);
			
			IBA_LOG0(_T("包房主账号结账，副账号跟着被登出和清除"));
			if ( nIn!=-1 )
			{
				// 发送给指定客户端
				CLocalServer::GetInstance()->LocalCheckOutUser(nIn, nMemberId, 2);
				Sleep(500);
			}
			CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(nMemberId);

			if (!m_bIsMember)
			{
				CDeregisterMethod Deregister;

				Deregister.SetMemberId(nMemberId);
				Deregister.SetPersonalId(ActiveMember.GetPersonalID());
				Deregister.SetMemberName(ActiveMember.GetUserName());
				Deregister.SetCardId(0);
				Deregister.SetPassword(_T(""));
				Deregister.SetEndUsingTime(m_strLastLogoutTime);

				theApp.GetCurCashier()->DoDeregister(Deregister);

				if (Deregister.GetStatusCode() == 0)
				{	
					IBA_LOG0(_T("包房的用户成功退款"));
				}
				else
				{
					IBA_LOG(_T("包房的用户退款失败, 包房ID=%d, netId=%s"), m_ActiveMember.GetPCClassID(), m_ActiveMember.GetNetId());
					SetToolTipPos(IDOK);
					ShowToolTip(Deregister.GetStatusMessage(), TRUE);
					return FALSE;
				}
			}
		}

		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);

		SetToolTipPos(IDOK);
		ShowToolTip(roomCheckOut.GetStatusMessage(), FALSE);

		// 输出操作日志
		{//写操作日志
			COperationInfo OperationInfo;
			OperationInfo.SetOperationType(CIBAGlobal::emOpTypeRoomReturn);
			OperationInfo.SetIsBundTime(FALSE);
			OperationInfo.SetUserName(m_strName);
			OperationInfo.SetNetId(m_strNetId);
			OperationInfo.SetMemberId(m_nMemberId);
			OperationInfo.SetOperationAmount(m_ActiveMember.GetAvailavleBalance());
			OperationInfo.SetAllAmount(m_nMoneyAllInAccount);
			OperationInfo.SetOperationTime(m_ActiveMember.GetActivationTime());
			CString strTmp;
			strTmp.Format(LOAD_STRING(IDS_ROOM_BODY), arrMembers.GetCount());
			OperationInfo.SetDescription(strTmp);
			OperationInfo.SetClassId(m_ActiveMember.GetClassId());
			OperationInfo.SetTermId(m_ActiveMember.GetTerminalID());
			OperationInfo.SetIdNum(m_ActiveMember.GetPersonalID());
			OperationInfo.SetCheckInTime(m_ActiveMember.GetCheckInTime());
			OperationInfo.SetConsumeMoney(m_ActiveMember.GetAdvancePayment()-m_nMoneyAllInAccount);

			// 2014-10-10 - qsc
			if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
			{
				NS_REALNAME::CheckoutInfo coi;
				ZeroMemory(&coi, sizeof(coi));
				_tcsncpy(coi.szName, m_strName, 100);
				_tcsncpy(coi.szNetId, m_strNetId, 100);
				_tcsncpy(coi.szTermId, m_ActiveMember.GetTerminalID(), 50);

				CString strIP;
				CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
				_tcsncpy(coi.szIP, strIP, 50);
				coi.nCheckoutType = m_ActiveMember.IsMember() ? CIBAGlobal::cot_MemLogout : CIBAGlobal::cot_TempLogout;

				theApp.GetRealName()->CheckOut(&coi);
			}
			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)&OperationInfo);
		}
		return TRUE;
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(roomCheckOut.GetStatusMessage(), TRUE);
		return FALSE;
	}
}

void CNDReturnDlg::OnBnClickedButtonCancelactive()
{
	CActivationMethod Activation;

	Activation.SetMemberId(m_nMemberId);
	Activation.SetMode( CActivationMethod::ACTIVATION_NO );

	theApp.GetCurCashier()->DoActivation(Activation);

	if (Activation.GetStatusCode() == 0)
	{	
		SetToolTipPos(IDC_BUTTON_CANCELACTIVE);
		ShowToolTip(Activation.GetStatusMessage());

		//如果是会员才从当前用户表删除，保留临时用户
		if (CCurrentNetBarUser::GetInstance()->Balance.GetIsMember() == 1)
		{
			// 2014-11-5 - qsc 会员用户才做取消激活结账操作
			// 2014-10-14 - qsc
			if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
			{
				NS_REALNAME::CheckoutInfo coi;
				ZeroMemory(&coi, sizeof(coi));
				_tcsncpy(coi.szName, m_strName, 100);
				_tcsncpy(coi.szNetId, m_strNetId, 100);
				_tcsncpy(coi.szTermId, m_ActiveMember.GetTerminalID(), 50);

				CString strIP;
				CIBADAL::GetInstance()->GetIPFromTermId(coi.szTermId, strIP);
				_tcsncpy(coi.szIP, strIP, 50);
				coi.nCheckoutType = CIBAGlobal::cot_TempLogout;

				theApp.GetRealName()->CheckOut(&coi);
			}

			CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(m_nMemberId);
		}
		else
		{
			CActiveMember ActiveMember;

			if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember))
			{
				//2011-03-14-gxx-修改-当临时用户为漫游时也从用户表删除
				//
				if (CCurrentNetBarUser::GetInstance()->Balance.GetOpenNetbarId()
					!= CNetBarConfig::GetInstance()->GetNetBarId())
				{
					// 漫游删除
					CLocalServer::GetInstance()->
						ActiveMemberList.RemoveActiveMember(m_nMemberId);
				}
				else
				{
					ActiveMember.SetActive(FALSE);
					CLocalServer::GetInstance()->
						ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
				}
			}
		}

		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);

		OnBnClickedButtonQuery();

		// 2011/10/27-8210-gxx: 
		INT_PTR nIndex = CLocalServer::GetInstance()->ComputerList.Find(m_nMemberId);
		if (nIndex >= 0)
		{
			CComputerInfo& computer = CLocalServer::GetInstance()->ComputerList.GetComputer(nIndex);
			theApp.GetRealName()->AuditUserCheckOut(
				computer.GetTerminalID(), computer.GetComputerIP(), m_nMemberId);	
		}

	}
	else
	{
		SetToolTipPos(IDC_BUTTON_CANCELACTIVE);
		ShowToolTip(Activation.GetStatusMessage(), TRUE);
	}
}

void CNDReturnDlg::PrintTicket(CActiveMember& ActiveMember)
{
	if (m_bPrintTicket)                            
	{
		if(ActiveMember.GetClassId() == 1)  //临时用户
		{
			CString strTicket;
			CIBAPrint::GetRefundTicket(strTicket,m_bIsMember);

			strTicket.Replace(_T("[name]"), m_strName);
			strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
			strTicket.Replace(_T("[memberclass]"), m_strUserClass);
			//CString strTmp;
			//strTmp.Format(_T("%.2f"), nAll/100.0);			
			//strTicket.Replace(_T("[Consume]"), strTmp); // 消费

			strTicket.Replace(_T("[return]"), m_strAllBalance);

			strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);

			strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
			CString strMemberID;
			strMemberID.Format(_T("%d"), m_nMemberId);
			strTicket.Replace(_T("[memberid]"), strMemberID);
			strTicket.Replace(_T("[totalmoney]"), _T("0.00")); // 临时用户余额为0

			CIBAPrint::Print(strTicket);
		}
		else if(ActiveMember.GetClassId() == 2)
		{
			CString strTicket;
			CIBAPrint::GetRefundTicket(strTicket,m_bIsMember);

			strTicket.Replace(_T("[name]"), m_strName);
			strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
			strTicket.Replace(_T("[memberclass]"), m_strUserClass);
			//CString strTmp;
			//strTmp.Format(_T("%.2f"), m_nConsumeMoney/100.0);
			//strCreditMoney.Format(_T("%.2f"), m_nCreditMoney/100.0);
			//strTicket.Replace(_T("[Consume]"), strTmp); // 消费

			strTicket.Replace(_T("[return]"), _T("0.00")); // 会员用户退款金额为0

			strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);

			strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
			CString strMemberID;
			strMemberID.Format(_T("%d"), m_nMemberId);
			strTicket.Replace(_T("[memberid]"), strMemberID);
			strTicket.Replace(_T("[totalmoney]"), m_strAllBalance);

			CIBAPrint::Print(strTicket);
		}
	}
}
