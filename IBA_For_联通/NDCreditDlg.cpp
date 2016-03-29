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
#include "ChinaUnicomConfig.h"
#include "DlgCapture.h"


// CNDCreditDlg 对话框

IMPLEMENT_DYNAMIC(CNDCreditDlg, CNDDialog)
CNDCreditDlg::CNDCreditDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CNDCreditDlg::IDD, pParent)
, m_bAutoQuery(FALSE), m_nCreditMoney(0)
, m_bNoPresent(FALSE), m_bLinkEnabled(TRUE)
, m_strBirthday(_T(""))
, m_bPrintTicket(FALSE)
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
	DDX_Control(pDX, IDC_BTN_ACTIVE, m_btnActive);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_staticImage);
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
	ON_BN_CLICKED(IDC_BTN_ACTIVE, OnBnClickedBtnActive)
	ON_BN_CLICKED(IDC_BTN_RECAPTURE, OnBnClickedBtnRecapture)
END_MESSAGE_MAP()


// CNDCreditDlg 消息处理程序

BOOL CNDCreditDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	//{ 2011/07/08-8201-gxx: 本地模式判断
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken ||
		GetUserState() == CIBAGlobal::emUserStateLocal)
	{
		GetDlgItem(IDC_BUTTON_BUNDLETIME)->ShowWindow(FALSE);
	}
	else
	{

	}
	//}



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

	m_btnActive.SetTextColor(RGB(0, 0, 255));

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

	m_nCreditMoney = _tstof(m_strCreditMoney) * 100;

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
			CIBAPrintA4::GetInstance()->SetOwner(theApp.m_pMainWnd);
			CIBAPrintA4::GetInstance()->m_strName = m_strName;
			CIBAPrintA4::GetInstance()->m_strNumber = m_strNetId;
			CIBAPrintA4::GetInstance()->m_strID = m_strPersonalID;
			CIBAPrintA4::GetInstance()->m_strAddress = _T(""); // 没有住址

			CChinaUnicomConfig::ClassIDItem item;
			if (CChinaUnicomConfig::GetInstance()->GetClassIDItem(m_nUserClassID, item))
			{
				CIBAPrintA4::GetInstance()->m_strContent[0] = _T("产品名称：") + item.strName;
				CIBAPrintA4::GetInstance()->m_strContent[1] = _T("产品特征：") + item.strDetail;
				CIBAPrintA4::GetInstance()->m_strContent[2] = _T("免费时间：") + item.strTime;
			}

#ifdef DEBUG
			CIBAPrintA4::GetInstance()->DoPrintPriview();
#else
			CIBAPrintA4::GetInstance()->DoPrint();
#endif
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

	DrawUserInfo();

	GetDlgItem(IDC_BTN_ACTIVE)->EnableWindow(TRUE);
	m_btnActive.SetTextColor(RGB(0, 0, 255));

	m_staticImage.ShowPic(CIBAConfig::GetInstance()->GetImagesPath() + m_strNetId + _T(".jpg"));
	m_staticImage.Invalidate();

	if (GetDlgItem(IDC_BTN_RECAPTURE))
	{
		GetDlgItem(IDC_BTN_RECAPTURE)->EnableWindow(TRUE);
	}
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
		m_bNeedActivation = TRUE;

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
	m_btnActive.ShowWindow(FALSE);

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

	m_nCreditMoney = _tstof(m_strCreditMoney) * 100;

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

void CNDCreditDlg::OnBnClickedBtnActive()
{
	SetToolTipPos(IDC_BTN_ACTIVE);

	//中心是否开启激活设置
	if (CNetBarConfig::GetInstance()->GetNeedActivation() == 0)
	{
		ShowToolTip(_T("不需要激活"), FALSE);
		IBA_LOG0(_T("void CCreditDlg::OnBnClickedBtnActive()-> 不需要激活"));
		return ;
	}
	if (m_bNeedActivation)
	{
		CActivationMethod Activation;

		Activation.SetMemberId(m_nMemberId);
		Activation.SetMode( CActivationMethod::ACTIVATION_YES );

		theApp.GetCurCashier()->DoActivation(Activation);

		if (Activation.GetStatusCode() == 0)
		{	
			ShowToolTip(_T("激活成功"), FALSE);
			IBA_LOG0(_T("void CCreditDlg::OnBnClickedBtnActive()-> 激活成功"));
			m_bNeedActivation = FALSE;
		}
		else
		{
			ShowToolTip(Activation.GetStatusMessage(), TRUE);
			IBA_LOG(_T("void CCreditDlg::OnBnClickedBtnActive()-> 激活失败,errorInfo=%s"), Activation.GetStatusMessage());
		}
	}
	else
	{
		ShowToolTip(_T("已激活，您不需要再激活"), FALSE);
		IBA_LOG0(_T("void CCreditDlg::OnBnClickedBtnActive()-> 已激活，不需要再激活"));
	}
}

void CNDCreditDlg::OnBnClickedBtnRecapture()
{
	CDlgCapture dlgCap(m_strNetId);

	if (dlgCap.DoModal() == IDOK)
	{
		m_staticImage.ShowPic(CIBAConfig::GetInstance()->GetImagesPath() + m_strNetId + _T(".jpg"));
		m_staticImage.Invalidate();
	}
	else
	{
		m_staticImage.ShowPic(_T(""));
	}
}
