// NDCreditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "MainFrm.h"
#include "Cashier.h"
#include "IBAPrint.h"
#include "IBAHelpper.h"
#include "RealName\IBARealName.h"
#include "ServletHellper.h"
#include "dal\IBADAL.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include "ComputerListView.h"
#include "ActiveMemberView.h"
#include "DialogManager.h"
#include "BundleTimeDlg.h"
#include "NDReturnDlg.h"
#include ".\ndcreditdlg.h"
#include "CashierHelper.h"
#include "RealName\AuditInterface.h"
#include "RealName\RZXRealName.h"


// CNDCreditDlg 对话框

IMPLEMENT_DYNAMIC(CNDCreditDlg, CNDDialog)
CNDCreditDlg::CNDCreditDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDCreditDlg::IDD, pParent)
, m_bAutoQuery(FALSE), m_nCreditMoney(0)
, m_bNoPresent(FALSE), m_bLinkEnabled(TRUE)
, m_strBirthday(_T("")), m_strPerID("")
, m_bPrintTicket(FALSE), m_nIDType(0)
{
	m_nCurrentPoint = 0;
}

CNDCreditDlg::~CNDCreditDlg()
{

}

void CNDCreditDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CREDIT, m_strCreditMoney);
	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strUserInfo);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
	DDX_Text(pDX, IDC_STATIC_TERMID, m_strTermId);

	DDX_Check(pDX, IDC_CHECK_PRESENT, m_bNoPresent);

	DDX_Control(pDX, IDC_EDIT_CREDIT, m_edtCreditMoney);
	DDX_Control(pDX, IDC_BUTTON_CANCELACTIVE, m_btnCancelActivation);
	DDX_Control(pDX, IDC_BUTTON_BUNDLETIME, m_btnBundleTime);
	DDX_Control(pDX, IDC_BUTTON_RETURN, m_btnReturn);
	DDX_Control(pDX, IDC_BUTTON_CONSUMELIST, m_btnConsumeList);
	DDX_Control(pDX, IDC_STATIC_ALL,m_btnBalanceAll);
	DDX_Control(pDX, IDC_BUTTON_CONSUMELIST2, m_btnZengshoubao);
	DDX_Control(pDX, IDC_BUTTON_DETAIL, m_btnDetail);
	DDX_Text(pDX, IDC_STATIC_BIRTHDAY, m_strBirthday);

	DDX_Check(pDX, IDC_CHECK_TICKET, m_bPrintTicket);

	DDX_Control(pDX, IDC_STATIC_ZCIMG, m_staticPhoto_zc);
	DDX_Control(pDX, IDC_STATIC_ZJIMG, m_staticPhoto_zj);
}


BEGIN_MESSAGE_MAP(CNDCreditDlg, CNDDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCELACTIVE, OnBnClickedButtonCancelactive)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_RETURN, OnBnClickedButtonReturn)
	ON_BN_CLICKED(IDC_BUTTON_BUNDLETIME, OnBnClickedButtonBundletime)
	ON_BN_CLICKED(IDC_BUTTON_CONSUMELIST, OnBnClickedButtonConsumelist)
	ON_STN_CLICKED(IDC_STATIC_ALL, OnStnClickedStaticAll)
	ON_BN_CLICKED(IDC_BUTTON_CONSUMELIST2, OnBnClickedButtonConsumelist2)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL, OnBnClickedButtonDetail)
	ON_BN_CLICKED(IDC_BTN_RN_REG, OnBnClickedBtnRnReg)
	ON_MESSAGE(C_CANCLOSE, OnCanClose)
	ON_BN_CLICKED(IDC_BTN_PHOTO, OnBnClickedBtnPhoto)
END_MESSAGE_MAP()


// CNDCreditDlg 消息处理程序

BOOL CNDCreditDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();
	
	CIBAGlobal::m_hCurCreditDlg = GetSafeHwnd();
	//{ 2011/07/08-8201-gxx: 本地模式判断
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken ||
		GetUserState() == CIBAGlobal::emUserStateLocal)
	{
		GetDlgItem(IDC_BUTTON_BUNDLETIME)->ShowWindow(FALSE);
	}
	else
	{
		
	}

	if(0 == CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		CRect rcBtn, rcWin;
		GetDlgItem(IDC_BUTTON_CONSUMELIST2)->GetWindowRect(&rcBtn);
		ScreenToClient(&rcBtn);
		GetWindowRect(rcWin);
		MoveWindow( rcWin.left, rcWin.top, rcBtn.right, rcWin.Height(), FALSE);
	}

	if(1 != CNetBarConfig::GetInstance()->GetSoftSfreg())
	{
		GetDlgItem(IDC_BTN_PHOTO)->ShowWindow(SW_HIDE);
	}

	//{ 2011/06/20-gxx: 不允许挂机充值(先屏蔽)
	///*CActiveMember ActiveMember;
	//CComputerInfo ComputerInfo;
	//BOOL bGetOK = CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(m_strNetId,ActiveMember);
	//if ( bGetOK &&
	//	CLocalServer::GetInstance()->ComputerList.GetComputerInfo(ActiveMember.GetMemberID(), ComputerInfo))
	//{
	//	if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_SUSPEND)
	//	{
	//		theApp.IBAMsgBox(IDS_GUAJIERROR); 
	//		AutoClose();
	//		return TRUE;
	//	}	 
	//}*/
	//}

	CIBAHelpper::CreateFont(m_BigFont, 22, 900);
	
	m_edtCreditMoney.SetFont(&m_BigFont);
	m_edtCreditMoney.DisableMask();
	m_edtCreditMoney.SetValidChars(_T("0123456789."));
	m_edtCreditMoney.SetLimitText(6);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	m_btnBalanceAll.SetFont(&m_BigFontUnderline);
	m_btnBalanceAll.SetLinkColor(RGB(0,0,255));
	m_btnBalanceAll.SetVisitedColor(RGB(0,0,255));

	GetDlgItem(IDC_STATIC_TERMID)->SetFont(&m_Font);

	m_btnBundleTime.EnableWindow(m_bLinkEnabled);
	m_btnReturn.EnableWindow(m_bLinkEnabled);

	if (m_bAutoQuery)
	{
		SetTimer(TimeIdOfAutoQuery, 100, NULL);
	}
	else
	{
		m_nMemberId = CCurrentNetBarUser::GetInstance()->Balance.GetMemberId();
		m_nCardId = CCurrentNetBarUser::GetInstance()->Balance.GetCardId();
		OnGetBalanceOK(CCurrentNetBarUser::GetInstance()->Balance);

		SetTimer(TimeIdOfAutoQuery, 800, NULL);
	}

	m_staticPhoto_zj.SetPicBk(RGB(180, 180, 180));
	m_staticPhoto_zc.SetPicBk(RGB(180, 180, 180));

	//  2014-4-28
	// 2014-4-25 - qsc 后面确认不需要实名注册按钮后可以删除按钮
	// QSCT1
	GetDlgItem(IDC_BTN_RN_REG)->ShowWindow(SW_HIDE);

	// 2014-11-5 - qsc 智多下不能“取消激活”防止，取消激活和结账导致发了多次结账信息给实名
	if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType() )
	{
		GetDlgItem(IDC_BUTTON_CANCELACTIVE)->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNDCreditDlg::OnBnClickedOk()
{
	UpdateData();

	if (m_LocalActiveMember.GetIsLocalUser())
	{
		DoLocalModeOperate();
		return;
	}
	else
	{
		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
		{
			SetToolTipPos(IDOK);
			ShowToolTip(_T("断网情况下，只允许本地用户充值"), TRUE);
			return;
		}
	}

	// 2011/06/29-gxx: 是否可以充值, 挂机中不允许充的
	if (!CLocalServer::GetInstance()->ActiveMemberList.IsAllowCredit(m_nMemberId))
	{
		return;
	}

	if (m_strCreditMoney.IsEmpty())
	{
		AutoClose();
		return;
	}

	//m_nCreditMoney = _tstof(m_strCreditMoney) * 100;
	m_nCreditMoney = CIBAHelpper::DecT2Int(m_strCreditMoney);

	if (m_nCreditMoney > 999999) //金额不能超过1万
	{
		ShowToolTip(IDS_MAXMONEY, IDC_EDIT_CREDIT, TRUE);
		return;
	}	

	// 2012/03/05-8238-gxx: 
	if (CIBAConfig::GetInstance()->GetMaxCreditMoney() != 0)
	{
		INT nMax = CIBAConfig::GetInstance()->GetMaxCreditMoney()*100;
		if (m_nCreditMoney > nMax)
		{
			ShowToolTip(IDS_OVERFLOW_MAXMONEY, IDC_EDIT_CREDIT, TRUE);
			CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CREDIT);
			if (pEdit)
			{
				pEdit->SetFocus();
				pEdit->SetSel(0, m_strCreditMoney.GetLength());
			}
			
			return ;
		}
	}
	// --end

	if (m_bNoPresent)
	{
		if (IDOK != MsgBox(IDS_NOPRESENTCREDITASK, MB_OKCANCEL | MB_ICONQUESTION))
		{
			return;
		}
	}

	CWaitCursor WaitCursor;

	CCreditMethod Credit;

	Credit.SetMemberId(m_nMemberId);
	Credit.SetTermTime(CIBAHelpper::GetCurrentTimeAsString());
	Credit.SetActivityId(0);
	Credit.SetNoPresent(m_bNoPresent);
	Credit.SetAmount(m_nCreditMoney);

	theApp.GetCurCashier()->DoCredit(Credit);
	//充值成功后设置各种状态
	
	if (Credit.GetStatusCode() == 0)
	{	
		CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
			m_nMemberId, ActiveMember))
		{
			UINT nMoney = Credit.GetAvailableBalance();
			ActiveMember.SetAvailavleBalance(Credit.GetAvailableBalance());

			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember, TRUE);
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
		}

		if (m_bPrintTicket)
		{
			CString strTicket;
			CIBAPrint::GetCreditTicket(strTicket);

			strTicket.Replace(_T("[name]"), m_strName);
			strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
			strTicket.Replace(_T("[memberclass]"), m_strUserClass);
			CString strCreditMoney;
			strCreditMoney.Format(_T("%.2f"), m_nCreditMoney/100.0);
			strTicket.Replace(_T("[inmoney]"), strCreditMoney);
			strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
			//实名要求强制密码，则不打印密码
			if (CNetBarConfig::GetInstance()->GetRealNamePassword() >= 2)
			{
				strTicket.Replace(_T("[password]"), _T("*****"));
			}
			else
			{
				strTicket.Replace(_T("[password]"), m_strTmpUserPwd);
			}

			strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
			CString strMemberID;
			strMemberID.Format(_T("%d"), m_nMemberId);
			strTicket.Replace(_T("[memberid]"), strMemberID);
			CString strTotalMoeny;
			strTotalMoeny.Format(_T("%.2f"), Credit.GetAvailableBalance()/100.0);
			strTicket.Replace(_T("[totalmoney]"), strTotalMoeny);

			CIBAPrint::Print(strTicket);
		}

		//清空充值的编辑框
		m_strCreditMoney.Empty();
		m_edtCreditMoney.SetWindowText(_T(""));
		m_edtCreditMoney.SetFocus();

		{//写到充值信息日志记录里
		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(1);
		OperationInfo.SetUserName(m_strName);
		OperationInfo.SetNetId(m_strNetId);
		OperationInfo.SetOperationAmount(m_nCreditMoney);
		OperationInfo.SetDeposit(_ttoi(m_strDeposit) * 100);
		OperationInfo.SetPassWord(m_strTmpUserPwd);
		OperationInfo.SetPresentAmount(Credit.GetCreditPresent());
		OperationInfo.SetDescription(m_strTermId);
		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));
		}

		{//写入数据库
		CCreditInfoDB CreditInfo;
		CreditInfo.SetCardSerial(m_strNetId);
		CreditInfo.SetCreditDate(CIBAHelpper::FormatCenterTime(Credit.GetTrantime()));
		CreditInfo.SetRefNo(Credit.GetRefNo());
		CreditInfo.SetCreditPresent(Credit.GetCreditPresent());
		CreditInfo.SetCreditAmount(m_nCreditMoney);
		CreditInfo.SetOperator(theApp.GetCurCashier()->GetName());
		CreditInfo.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
		CreditInfo.SetCashRegisterID(CIBAConfig::GetInstance()->GetCashRegisterID());
		CreditInfo.SetMemberId(m_nMemberId);
		CreditInfo.SetSerialNo(Credit.GetLocalSerialNo());
		CreditInfo.SetCassId(m_nUserClassID);
		CIBADAL::GetInstance()->AddCreditRecord(CreditInfo);
		}

		if (m_bOnLine) //发送充值消息
		{			
			CString strTmp;
			strTmp.Format(IDS_ONLINECREDIT, m_nCreditMoney / 100.0, Credit.GetCreditPresent() / 100.0);
			CIBALog::GetInstance()->Write(_T("充值发消息-----begin"));
			CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(m_nMemberId, strTmp);
			CIBALog::GetInstance()->Write(_T("充值发消息-----end"));
		}

		CIBATTS::GetInstance()->SpeakCreditResult(m_nCreditMoney, Credit.GetCreditPresent(), Credit.GetAvailableBalance());

		m_nAllMoney = Credit.GetAvailableBalance();
		m_nCurrentPoint += Credit.GetIncreasedPoint();

		DrawUserInfo();

		AutoClose(IDOK);
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(Credit.GetStatusMessage(), TRUE);

		CNDDialog::OnBnClickedButtonQuery();
	}
}

void CNDCreditDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance);

	m_strDeposit.Format(_T("%.2f"), Balance.GetGuarantyMoney() / 100.0);//押金
	
	m_nAllMoney = Balance.GetTotalBalacne();

	m_strStartTime = Balance.GetStartTime();

	if (Balance.GetIsMember() == 0) //临时用户密码
	{
		m_strTmpUserPwd = Balance.GetPassword();
	}
	else
	{
		m_strTmpUserPwd.Empty();
	}

	m_btnCancelActivation.ShowWindow(!m_bOnLine);

	// 2011/09/07-8201-gxx: 
	m_bPrintTicket = CIBAConfig::GetInstance()->GetPrintCreditTicket(m_bIsMember);

	m_strPerID = Balance.GetPersonalId();
	m_nIDType = Balance.GetIdType();
	DrawUserInfo();
}

void CNDCreditDlg::OnBnClickedButtonCancelactive()
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


HBRUSH CNDCreditDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ALL)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_BIRTHDAY)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}
	else if (CTLCOLOR_EDIT == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_CREDIT)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}		
	}

	return hbr;
}

void CNDCreditDlg::OnBnClickedCancel()
{
	AutoClose();
}

void CNDCreditDlg::SetNetId(CString newVal)
{
	m_strNetId = newVal;

	m_bAutoQuery = TRUE;
}

void CNDCreditDlg::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfAutoQuery == nIDEvent)
	{
		KillTimer(nIDEvent);

		if (m_bAutoQuery)
		{
			m_bAutoQuery = FALSE;
			OnBnClickedButtonQuery();
		}
		else
		{
			if (theApp.GetRealName()->RobFocusAfterSwipingCard())
			{
				SimulateClickMe();	
			}
		}
	}
	else if (TimeIdOfKeepFocus == nIDEvent)
	{
		if (GetFocus() != NULL)
		{
			if (GetFocus()->GetDlgCtrlID() != m_edtCreditMoney.GetDlgCtrlID())
			{
				m_edtCreditMoney.SetFocus();
			}
		}
	}
	else if(TimerIdOfDrawUserInfo == nIDEvent)
	{
		KillTimer(nIDEvent);
		DrawUserInfo();
	}

	CNDDialog::OnTimer(nIDEvent);
}

void CNDCreditDlg::OnBnClickedButtonReturn()
{
	CBCGPDialog::OnCancel();

	CNDReturnDlg dlg;
	dlg.SetNetId(m_strNetId);
	dlg.DoModal();
}

void CNDCreditDlg::OnBnClickedButtonBundletime()
{
	CBCGPDialog::OnCancel();

	CDlgBundleTime dlg;
	dlg.SetNetId(m_strNetId, TRUE);
	dlg.DoModal();

	return;
}

void CNDCreditDlg::OnBnClickedButtonConsumelist()
{
	m_edtCreditMoney.SetFocus();

	COperationInfo OperationInfo;

	OperationInfo.SetMemberId(m_nMemberId);
	OperationInfo.SetUserName(m_strName);
	OperationInfo.SetNetId(m_strNetId);

	CActiveMember ActiveMember;
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember);

	OperationInfo.SetOperationTime(ActiveMember.GetActivationTime());
	OperationInfo.SetIsMember(ActiveMember.IsMember());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, MAKELONG(2,0), (LPARAM)&OperationInfo);
}

void CNDCreditDlg::OnBnClickedButtonConsumelist2()
{
	// 查询增收宝消费记录

	m_edtCreditMoney.SetFocus();

	COperationInfo OperationInfo;

	OperationInfo.SetMemberId(m_nMemberId);
	OperationInfo.SetUserName(m_strName);
	OperationInfo.SetNetId(m_strNetId);

	CActiveMember ActiveMember;
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember);

	OperationInfo.SetOperationTime(ActiveMember.GetActivationTime());
	OperationInfo.SetIsMember(ActiveMember.IsMember());

	::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, MAKELONG(2,4), (LPARAM)&OperationInfo);
}


BOOL CNDCreditDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_F3 == pMsg->wParam && m_btnReturn.IsWindowEnabled())
		{
			OnBnClickedButtonReturn();
			return TRUE;
		}
	}

	return CNDDialog::PreTranslateMessage(pMsg);
}

void CNDCreditDlg::OnPaint()
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

void CNDCreditDlg::OnStnClickedStaticAll()
{
	if (m_nMemberId > 0)
	{
		CString strTmp;

		strTmp.Format(IDS_DETAILBALANCE, 
			m_nMoneyBalanceW / 100.0, m_nMoneyBalanceY / 100.0, 
			m_nMoneyBalanceZ / 100.0, m_nMoneyOnlineAmount / 100.0,
			(m_nMoneyBalanceW + m_nMoneyBalanceY  + m_nMoneyBalanceZ + m_nMoneyOnlineAmount) / 100.0);

		SetToolTipPos(IDC_STATIC_ALL);

		ShowToolTip(strTmp);
	}
}

void CNDCreditDlg::OnBnClickedButtonDetail()
{
	UINT nNetbarID = CNetBarConfig::GetInstance()->GetNetBarId();
	CString strCasherID = theApp.GetCurCashier()->GetName();
	CString strPwd = theApp.GetCurCashier()->GetPassword();

#ifdef _DEBUG
	CString strLinkPage = _T("http://mail.sunlike.cn:86/duduniu/index.jsp");
#else
	CString strLinkPage = _T("http://www.duduniu.cn/index.jsp");
#endif // _DEBUG

	CString strUrl = _T("http://");
	strUrl += CNetBarConfig::GetInstance()->GetQueryHost();
	strUrl.AppendFormat( _T("/netbar/common/thirdpartdodonew.jsp?netBarId=%d&userId=%s&password=%s&linkPage=%s"),
		nNetbarID, strCasherID, strPwd, strLinkPage);

	ShellExecute(NULL,_T("open"),strUrl,NULL,NULL,SW_SHOW);
}

void CNDCreditDlg::DrawUserInfo()
{
#ifdef __USING_INTER_RESOURCE
	m_strAllBalance.Format(_T("￥%.2f"), m_nAllMoney / 100.0);
#else
	m_strAllBalance.Format(_T("%.2f"), m_nAllMoney / 100.0);  // 多语言版本不要人民币符号
#endif
	

	m_strUserInfo.Format(_T("%s, %s, 积分(%d)"), m_strName, m_strUserClass, m_nCurrentPoint);

	if(0 != CNetBarConfig::GetInstance()->GetSoftSfreg())
	{	
		CString strZJImg, strOrgImg;
		// 2014-6-26 - qsc
		/*strZJImg = CIBAHelpper::GetSfregUserZJImagePath(m_nIDType, m_strPerID);
		strOrgImg = CIBAHelpper::GetSfregUserOrgImagePath(m_nIDType, m_strPerID);*/
		strZJImg = CIBAHelpper::GetUserZjImgFilePath(m_nIDType, m_strPerID);
		strOrgImg = CIBAHelpper::GetUserZcImgFilePath(m_nIDType, m_strPerID);
		IBA_LOG(_T("证件照 ：%s"), strZJImg);
		IBA_LOG(_T("注册照片 ：%s"), strOrgImg);
		m_staticPhoto_zj.ShowPic(strZJImg);
		m_staticPhoto_zc.ShowPic(strOrgImg);
		TRACE0("zjImg=");
		OutputDebugString(strZJImg);
		TRACE0("\n");
		TRACE0("OrgImg=");
		OutputDebugString(strOrgImg);
		TRACE0("\n");
	}

	UpdateData(FALSE);

	RedrawWindow();
}

BOOL CNDCreditDlg::DoLocalModeQuery()
{
	if (!CNDDialog::DoLocalModeQuery())
	{
		return FALSE;
	}

	// 2011/07/29-8201-gxx: 断网，或者本地用户
	m_btnBundleTime.ShowWindow(FALSE);     // 禁用包时功能
	m_btnConsumeList.ShowWindow(FALSE);    // 禁用消费记录
	m_btnZengshoubao.ShowWindow(FALSE); // 禁用增收宝记录
	m_btnCancelActivation.ShowWindow(FALSE); // 禁用取消激活

	//m_nMoneyBalanceW = Balance.GetCashBalance();
	//m_nMoneyBalanceY = Balance.GetUsePresent();
	//m_nMoneyBalanceZ = Balance.GetCreditPresent();
	//m_nMoneyOnlineAmount = Balance.GetOnlineAmount();

	//m_nCurrentPoint = Balance.GetCurrentPoint();
	//m_strDeposit.Format(_T("%.2f"), Balance.GetGuarantyMoney() / 100.0);//押金

	m_nAllMoney = m_LocalActiveMember.GetAvailavleBalance();

	//m_strStartTime = m_LocalActiveMember.GetActivationTime();

	//if (Balance.GetIsMember() == 0) //临时用户密码
	//{
	//	m_strTmpUserPwd = Balance.GetPassword();
	//}
	//else
	//{
	//	m_strTmpUserPwd.Empty();
	//}

	m_btnCancelActivation.ShowWindow(!m_bOnLine);

	//UpdateData(FALSE);
	DrawUserInfo();


	return TRUE;
}

BOOL CNDCreditDlg::DoLocalModeOperate()
{
	ASSERT(m_LocalActiveMember.GetIsLocalUser());

	if (m_LocalActiveMember.GetMemberID() == 0)
	{
		return FALSE;
	}

	// 2011/06/29-gxx: 是否可以充值, 挂机中不允许充的
	if (!CLocalServer::GetInstance()->ActiveMemberList.IsAllowCredit(m_nMemberId))
	{
		return FALSE;
	}

	if (m_strCreditMoney.IsEmpty())
	{
		AutoClose();
		return FALSE;
	}

	//m_nCreditMoney = _tstof(m_strCreditMoney) * 100;
	m_nCreditMoney = CIBAHelpper::DecT2Int(m_strCreditMoney);

	if (m_nCreditMoney > 999999) //金额不能超过1万
	{
		ShowToolTip(IDS_MAXMONEY, IDC_EDIT_CREDIT, TRUE);
		return FALSE;
	}	

	if (m_bNoPresent)
	{
		if (IDOK != MsgBox(IDS_NOPRESENTCREDITASK, MB_OKCANCEL | MB_ICONQUESTION))
		{
			return FALSE;
		}
	}

	BOOL bOK = CCashierHelper::CreditLocalUser(m_LocalActiveMember, m_nCreditMoney);
	if (bOK)
	{
		m_strCreditMoney.Empty();
		m_edtCreditMoney.SetWindowText(_T(""));
		m_edtCreditMoney.SetFocus();
		m_nAllMoney = m_LocalActiveMember.GetAvailavleBalance();
		m_nCurrentPoint += 0;
		DrawUserInfo();
		AutoClose(IDOK);
	}
	else
	{
		SetToolTipPos(IDOK);
		ShowToolTip(IDS_CREDITERRORLOCAL, TRUE);
	}
	
	return bOK;
}


void CNDCreditDlg::OnBnClickedBtnRnReg()
{
	// TODO: 在此添加控件通知处理程序代码
	// 2014-4-28 - qsc 任子行实名 QSCT1
	//if(CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameRZX)
	if(1 == CNetBarConfig::GetInstance()->GetDomainId())
	{
		try
		{
			SendRegDataToProxy();
		}
		catch (...)
		{
			IBA_LOG(_T("向实名接口发送注册信息出错! %d"), GetLastError());
		}

	}

	// 2014-4-25 - qsc 
	//// 2014-2-26 - qsc
	/*try
	{
		ExeSfreg();
	}
	catch(...)
	{
		MsgBox(_T("实名注册出错，请稍候再试！"));
	}*/

	
}


//// 2014-5-23 - qsc 
//void CNDCreditDlg::SendRegDataToProxy()
//{
//	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
//	CAuditInterface::RegUserInfo regUserInfo;
//	
//	COleDateTime dt;
//	dt = COleDateTime::GetCurrentTime();
//	CString strDatetime;
//	strDatetime = dt.Format(_T("%Y-%m-%d"));
//	int nMemberId = 0;
//	
//	regUserInfo.netbarID = CNetBarConfig::GetInstance()->GetNetBarId();
//	regUserInfo.IDType = m_nIDType;
//	regUserInfo.strIDNumber = CT2A(m_strPersonalID);
//	regUserInfo.strUserName = CT2A(m_strName);
//	regUserInfo.sex = Balance.GetSex();
//	regUserInfo.strCountryCode = "CN";
//	regUserInfo.strAddress = CT2A(Balance.GetAddress());
//	regUserInfo.strTelphone = CT2A(Balance.GetTel());
//	regUserInfo.memberID = nMemberId;
//	regUserInfo.strBirthDay = CT2A(NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ(m_strPersonalID));
//	regUserInfo.strOpenDay = CT2A(strDatetime);
//	regUserInfo.strValidDate = CStringA("2050-01-01");
//	regUserInfo.national = NS_REALNAME::CRZXRealName::GetNationCode(Balance.GetJob());
//	regUserInfo.strCertificateOffice = CT2A(Balance.GetIdDepart());
//	regUserInfo.CardType = m_nUserClassID;
//	regUserInfo.strSerialNum = CT2A(m_strNetId);
//	
//	g_listRegUserInfo.push_back(regUserInfo);
//	if(WAIT_OBJECT_0 == WaitForSingleObject(g_hRegUserInfo, 0) || NULL == g_hRegUserInfo)
//	{	
//		// 线程没有运行
//		g_hRegUserInfo = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RegUserThread, NULL, 0, 0);
//		CloseHandle(g_hRegUserInfo);
//	}
//	
//}

void CNDCreditDlg::SendRegDataToProxy()
{
	CAuditInterface auditInterface;
	if(!auditInterface.Load())
	{
		IBA_LOG0(_T("加载实名接口动态库AuditInterface.dll失败！请检查AuditInterface.dll的版本！"));
		return ;
	}
	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
	auditInterface.uninit_();
	CStringA strIpA = CT2A(CNetBarConfig::GetInstance()->GetProxyIP());
	auditInterface.init_(strIpA, 7834);

	int isex = Balance.GetSex();
	int nMemberId= 0;
	int nNation = NS_REALNAME::CRZXRealName::GetNationCode(Balance.GetJob());
	UINT nNetbarId = CNetBarConfig::GetInstance()->GetNetBarId();
	COleDateTime dt;
	dt = COleDateTime::GetCurrentTime();
	CString strDatetime;
	strDatetime = dt.Format(_T("%Y-%m-%d"));

	CString validDate("2050-01-01");
	CString strBrithday = NS_REALNAME::CRZXRealName::GetBirthdayFromSFZ(m_strPersonalID);

	CStringA strPersonalIDA = CT2A(m_strPersonalID);
	CStringA strNameA = CT2A(m_strName);
	CStringA strAddrA = CT2A(Balance.GetAddress());
	CStringA strTelNumA = CT2A(Balance.GetTel());

	CStringA strBrithdayA = CT2A(strBrithday);
	CStringA strDatetimeA = CT2A(strDatetime);
	CStringA strValidDateA = CT2A(validDate);
	CStringA strOfficerA = CT2A(Balance.GetIdDepart());
	CStringA strNetIdA = CT2A(m_strNetId);

	int retcode = -1;
	try
	{
		int ret = auditInterface.regUser_(nNetbarId, m_nIDType, strPersonalIDA, strNameA, 
			isex, "CN", strAddrA, strTelNumA, nMemberId, strBrithdayA,
			strDatetimeA, strValidDateA, nNation, strOfficerA, m_nUserClassID, strNetIdA, &retcode);

		if (ret == 0)
		{
			if (retcode == 1 || retcode == -6 )
			{
				IBA_LOG0(_T("向实名接口传注册信息成功（任子行实名）!"));
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
		IBA_LOG(_T("向实名注册抛出异常:%d"), GetLastError());
	}
}

void CNDCreditDlg::ExeSfreg()
{
	UpdateData();
	// -A证件号码	-B证件类别(文字)   -C姓名		-D性别
	// -E地址		-F电话号码		-G国籍		-H会员号
	// -I证件类别编码 -J民族			-K发证单位	-L卡类型 
	// -M印刷号 -R是否只读
	/*CMemberInfoMethod MemberInfo;

	MemberInfo.SetSerialNum(m_strNetId);
	theApp.GetCurCashier()->DoMemberInfo(MemberInfo);
	if(0 == MemberInfo.GetStatusCode() &&
		0 < MemberInfo.GetMemberCount())
	{*/
	// 2014-2-27 - qsc
	CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;
	CString strParam, strIDType, strSex;

	// 性别
	if(!Balance.GetSex())
	{
		strSex.LoadString(IDS_MAN);
	}
	else
	{
		strSex.LoadString(IDS_WOMAN);
	}

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); i++)
	{
		if(m_nIDType == CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeID())
		{
			strIDType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeName();
			break;
		}
	}
	
	strParam.Format(_T("-A%s -B%s -C%s -D%s -E%s -F%s -G%s -H%d -I%d -J%s -K%s -L%d -M%s -R%d"), 
		m_strPersonalID, strIDType, m_strName, strSex,
		Balance.GetAddress(), Balance.GetTel(), _T("CN"), m_nMemberId, 
		m_nIDType, Balance.GetJob(), Balance.GetIdDepart(), m_nUserClassID,
		m_strNetId, FALSE);

	IBA_LOG(_T("传递给SFREG的信息:%s"),strParam);
	const CString strSource = theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg");
	const CString strDest	  = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\imgID.jpg");
	
	if(PathFileExists(strSource))		// 拷贝一个做备份，方便查看
	{
		if(!CopyFile(strSource, theApp.GetWorkPath() + _T("\\IBATemp\\ID1.jpg"), FALSE))
		{
			IBA_LOG(_T("拷贝%s失败1!，%d"), strSource, GetLastError());
		}
	}

	if(PathFileExists(strDest))
	{
		if(!DeleteFile(strDest))
		{
			IBA_LOG(_T("删除%s失败!,%d"), strDest,GetLastError());
		}
	}

	if(PathFileExists(strSource))
	{
		if(!CopyFile(strSource, strDest, FALSE))
		{
			IBA_LOG(_T("拷贝%s失败!,%d"), strSource,GetLastError());
		}
	}
	else
	{
		IBA_LOG(_T("%s不存在！"), strSource);
	}

	if(PathFileExists(strSource))		// 拷贝完删除
	{
		if(!DeleteFile(strSource))
		{
			IBA_LOG(_T("删除%s失败!,"), strSource, GetLastError());
		}
	}
	
	CString strSFREGPath = theApp.GetWorkPath() + _T("\\IBABin\\sfreg\\sfreg.exe");
	if(CIBAHelpper::ExePath(strSFREGPath, strParam))
	{
		IBA_LOG0(_T("调用sfreg成功"));
	}
	else
	{
		IBA_LOG0(_T("调用sfreg失败"));
	}
	//}	
}

LRESULT CNDCreditDlg::OnCanClose(WPARAM wParam, LPARAM lParam)
{
	SetTimer(TimerIdOfDrawUserInfo, 1000, NULL);
	return 0;
}
void CNDCreditDlg::OnBnClickedBtnPhoto()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CIBAHelpper::TakePhoto(CIBAHelpper::GetUserZcImgPath(m_nIDType, m_strPersonalID), m_strPersonalID);
	CString strZJImg = CIBAHelpper::GetUserZjImgFilePath(m_nIDType, m_strPerID);
	CString strOrgImg = CIBAHelpper::GetUserZcImgFilePath(m_nIDType, m_strPerID);
	IBA_LOG(_T("证件照 ：%s"), strZJImg);
	IBA_LOG(_T("注册照片 ：%s"), strOrgImg);
	m_staticPhoto_zj.ShowPic(strZJImg);
	m_staticPhoto_zc.ShowPic(strOrgImg);
}


