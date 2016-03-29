// BundleTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Cashier.h"
#include "IBAPrint.h"
#include "IBAHelpper.h"
#include "tts\IBATTS.h"
#include "Socket\LocalServer.h"
#include ".\bundletimedlg.h"
#include "ComputerInfo.h"
#include "selcomputerdlg.h"
#include "MainFrm.h"
#include "NDCreditDlg.h"
#include "ComputerListView.h"
#include "selfdefinebundtimedlg.h"

// CDlgBundleTime 对话框

IMPLEMENT_DYNAMIC(CDlgBundleTime, CNDDialog)
CDlgBundleTime::CDlgBundleTime(CWnd* pParent /*=NULL*/)
: CNDDialog(CDlgBundleTime::IDD, pParent)
, m_nSelBundleTimeId(0), m_nSelBundleTimePrice(0)
, m_bAfterBundleTime(FALSE)
, m_nLastMemberID(0), m_nCurrBundleTimeIndex(0)
{
	m_nBudleTimeMoney = 0;
}

CDlgBundleTime::~CDlgBundleTime()
{
}

void CDlgBundleTime::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_BUNDLETIME, m_BundleTimeList);
	DDX_Control(pDX, IDC_CHECK_PRINT, m_btnPrint);
	DDX_Control(pDX, IDC_STATIC_ALL, m_btnAllBalance);
	DDX_Control(pDX, IDC_BUTTON_CREDIT, m_btnCredit);
	DDX_Control(pDX, IDC_BUTTON_BALANCE, m_btnBalance);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strOpenNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_BTAREA, m_strBundleTimeArea);
	DDX_Text(pDX, IDC_STATIC_BTENDTIME, m_strBundleTimeEndTime);
	DDX_Text(pDX, IDC_STATIC_BTID, m_strBundleTimeID);
	DDX_Text(pDX, IDC_STATIC_BTMONEY, m_strBundleTimeMoney);
	DDX_Text(pDX, IDC_STATIC_BTMONEY2, m_strBundleTimeMoney2);
	DDX_Text(pDX, IDC_STATIC_BTSTARTTIME, m_strBundleTimeStartTime);
	DDX_Text(pDX, IDC_STATIC_BTYPE, m_strBundleTimeType);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
}


BEGIN_MESSAGE_MAP(CDlgBundleTime, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BUNDLETIME, OnNMClickListBundletime)
	ON_BN_CLICKED(IDC_BUTTON_CANCELBT, OnBnClickedButtonCancelbt)
	ON_BN_CLICKED(IDC_BUTTON_FORWARDBT, OnBnClickedButtonForwardbt)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEBT, OnBnClickedButtonChangebt)
	ON_BN_CLICKED(IDC_BUTTON_REPRINT, OnBnClickedButtonReprint)
	ON_BN_CLICKED(IDC_CHECK_PRINT, OnBnClickedCheckPrint)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELFBUNDTIME, OnBnClickedSelfbundtime)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_ALL, OnStnClickedStaticAll)
	ON_BN_CLICKED(IDC_BUTTON_CREDIT, OnBnClickedButtonCredit)
	ON_BN_CLICKED(IDC_BUTTON_BALANCE, OnBnClickedButtonBalance)
END_MESSAGE_MAP()


// CDlgBundleTime 消息处理程序

//   (包时-提前包时-转包时-取消包时-自定义包时)

BOOL CDlgBundleTime::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	m_btnAllBalance.SetFont(&m_Font);
	m_btnAllBalance.SetLinkColor(RGB(0, 0, 255));  // 钱用蓝色表示
	m_btnAllBalance.SetVisitedColor(RGB(0, 0, 255));

	m_btnBalance.SizeToContent();
	m_btnCredit.SizeToContent();

	InitListView();
	SetEditFont();

	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	
	GetDlgItem(IDC_EDIT_CREDITMONEY)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_CREDITMONEY)->SetFont(&m_BigFont);

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		CIBAString strTmp;
		strTmp.LoadString(IDS_BTNEEDMONEY);
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(strTmp);

		m_btnCredit.EnableWindow(FALSE);
		m_btnCredit.ShowWindow(SW_HIDE);

		m_btnAllBalance.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC0)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_CREDITMONEY)->ShowWindow(SW_HIDE);
	}
	else
	{
		m_btnBalance.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT_CREDITMONEY)->ShowWindow(SW_HIDE);
	}

	GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(FALSE);//重复打印按钮隐藏
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE);//自定义包时
	GetDlgItem(IDC_SELFBUNDTIME)->ShowWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断

	//{ 2011/05/24-gxx: 自动查询在刷卡及扫描证件的情况下采用
	if (m_bAutoQuery)
	{
		// 2011/07/08-8201-gxx: 判断如果为本地用户不允许包时
		if (GetUserState() == CIBAGlobal::emUserStateLocal)
		{
			// 账号对应的用户为本地用户
			theApp.IBAMsgBox(IDS_DISABLEBT);
			OnCancel();
		}
		//
		SetTimer(TimeIdOfAutoQuery, 100, NULL);//只查询一次
	}
	else
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_IDCARDSNO);
		ASSERT(pWnd);
		if (pWnd)
		{
			pWnd->SetFocus();
		}
	}
	//}

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgBundleTime::CheckPopedom(LPCTSTR lpszModuleName)
{
	BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 

	if (bRet)
	{
		bRet = theApp.GetCurCashier()->HasPopedomOf(lpszModuleName);
	}

	return bRet;
}

void CDlgBundleTime::OnGetBalanceOK(CBalanceMethod & Balance)
{
	CNDDialog::OnGetBalanceOK(Balance);

	// 2012/02/20-8237-gxx: 将EnableWindow(TRUE)改动
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断
	
	if (m_bAfterBundleTime || Balance.GetPayType() > 0
		|| 0 != CNetBarConfig::GetInstance()->GetCannotSelfBundleTime())
	{
		// 已经包时 or 付款类型为:累计封顶、足额预扣

		GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE); // 禁用自定义包时
	}

	if (m_nMemberId != m_nLastMemberID)
	{
		// 重新读取包时列表

		GetBundleTimeList();
		m_nLastMemberID = m_nMemberId;
	}	

	GetBundleTimeAction();

	//{ 2011/05/20-gxx: 
	if (!m_strBundleTimeEndTime.IsEmpty())
	{
		COleDateTime dtEndTime;

		dtEndTime.ParseDateTime(m_strBundleTimeEndTime);
		COleDateTimeSpan dtSpan = dtEndTime - COleDateTime::GetCurrentTime();

		//{ 2011/06/09-gxx: 
		int nBTLimit = CNetBarConfig::GetInstance()->GetBTLimitTime();
		if ( nBTLimit==0 )
		{
			nBTLimit = 50;
		}
		//}

		if (dtSpan.GetTotalMinutes() < nBTLimit) //可以自定义续包时
		{
			// 2012/02/20-8237-gxx: 将EnableWindow(TRUE)改动
			GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(CheckPopedom(_T("IBAP_CUSTOMBUNDLE")));//权限判断
		}
	}
	
	//}
	
	m_btnPrint.SetCheck( CIBAConfig::GetInstance()->GetPrintTicketBT(m_bIsMember));

	


	UpdateData(FALSE);
}

void CDlgBundleTime::GetBundleTimeList()
{
	m_BundleTimeList.DeleteAllItems();

	BundleTimeList.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBundleTimeList(BundleTimeList);

	if (BundleTimeList.GetStatusCode() == 0)
	{
		for (INT i = 0; i < (INT)BundleTimeList.GetRecordCount(); i++)
		{
			CString strTmp;

			strTmp.Format(_T("%d"), BundleTimeList.GetBundleId(i));
			m_BundleTimeList.InsertItem(i, strTmp);

			strTmp = BundleTimeList.GetStartTime(i);
			m_BundleTimeList.SetItemText(i, 1, strTmp);

			strTmp = BundleTimeList.GetEndTime(i);
			m_BundleTimeList.SetItemText(i, 2, strTmp);

			strTmp.Format(_T("%d"), BundleTimeList.GetTimePass(i));
			m_BundleTimeList.SetItemText(i, 3, strTmp);

			strTmp.Format(_T("%.2f"), BundleTimeList.GetPrice(i) / 100.0);
			m_BundleTimeList.SetItemText(i, 4, strTmp);

			strTmp = BundleTimePayType(BundleTimeList.GetBundleType(i));
			m_BundleTimeList.SetItemText(i, 5, strTmp);

			strTmp = BundleTimeList.GetDescription(i);
			m_BundleTimeList.SetItemText(i, 6, strTmp);

			m_BundleTimeList.SetItemData(i, i);
		}
	}
}

void CDlgBundleTime::GetBundleTimeAction()
{
	CBundleTimeActionMethod BundleTimeAction;
	BundleTimeAction.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoBundleTimeAction(BundleTimeAction);
	
	if (BundleTimeAction.GetStatusCode() == 0 && 
		BundleTimeAction.GetNetBarId() > 0  // 0表示无包时
		)
	{
		m_strBundleTimeID.Format(_T("%d"), BundleTimeAction.GetTimeId());
		
		for (INT i = 0; i < (INT)BundleTimeList.GetRecordCount(); i++)
		{
			if (BundleTimeAction.GetTimeId() == BundleTimeList.GetBundleId(i))
			{
				m_nCurrBundleTimeIndex = i;
				break;
			}
		}
		
		m_strBundleTimeStartTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetStartTime());
		m_strBundleTimeEndTime = CIBAHelpper::FormatCenterTime(BundleTimeAction.GetEndTime());
		m_strBundleTimeType = BundleTimePayType(BundleTimeAction.GetPayType());
		m_strBundleTimeMoney.Format(_T("%.2f"), BundleTimeAction.GetAmount1() / 100.0);
		m_strBundleTimeMoney2.Format(_T("%.2f"), BundleTimeAction.GetAmount2() / 100.0);
		m_nBudleTimeMoney = BundleTimeAction.GetAmount1();
		
		m_strBundleTimeArea = CNetBarConfig::GetInstance()->GetPCClassName(BundleTimeAction.GetPcClass());
		
		if (m_strBundleTimeArea.IsEmpty())
		{
			m_strBundleTimeArea = LOAD_STRING(IDS_NOLIMITAREA);
		}

		GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(TRUE); // 允许取消包时
	}
}	

CString CDlgBundleTime::BundleTimePayType(UINT nBundleType)
{	
	CIBAString strTmp;

	strTmp.LoadString(IDS_BUNDLETIMETYPE);

	if (1 == nBundleType)
	{
		strTmp = strTmp.Left(strTmp.Find('|'));
	}
	else
	{
		strTmp = strTmp.Mid(strTmp.Find('|') + 1);
	}

	return strTmp;
}


void CDlgBundleTime::InitListView()
{
	//L"编号|开始时间|结束时间|时长|金额|包时类型|其他信息"
	
	CIBAString strHeader;
	strHeader.LoadString(IDS_BUNDLETIMELISTHEADER);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT nWidth[] = {40, 70, 70, 40, 50, 70, 220};

	const UINT nFormat[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_LEFT};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_BundleTimeList.InsertColumn(i, strArray.GetAt(i), nFormat[i], nWidth[i]);
	}

	m_BundleTimeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDlgBundleTime::CheckBundleTimeCondition(INT nBtIndex, BOOL& bNowCanBT, BOOL& bNowCanForwardBT, BOOL& bNowChangeBT)
{
	CString strNow(COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
	CString strStart(BundleTimeList.GetStartTime(nBtIndex));
	CString strEnd(BundleTimeList.GetEndTime(nBtIndex));

	if (strStart.IsEmpty())
	{
		bNowCanBT = TRUE;
	}
	else
	{
		if (strNow.Compare(strStart) >= 0)
		{
			if (strNow.Compare(strEnd) < 0)
			{
				bNowCanBT = TRUE;
			}
			else if (strNow.Compare(_T("23:59:59")) <= 0 && strStart.Compare(strEnd) > 0) //跨天
			{	
				bNowCanBT = TRUE;
			}
		}
		else
		{
			if (strNow.Compare(strEnd) < 0 && strStart.Compare(strEnd) > 0)//跨天
			{
				bNowCanBT = TRUE;
			}
		}
	}

	//现在不能包时且没有包时，则可以提前包时

	bNowCanForwardBT = (!bNowCanBT && m_strBundleTimeID.IsEmpty());

	if (bNowCanBT) //所选择的包时符合当前时间
	{
		if (!m_strBundleTimeID.IsEmpty()) //已经包时了
		{
			COleDateTime dtEndTime;

			dtEndTime.ParseDateTime(m_strBundleTimeEndTime);

			COleDateTimeSpan dtSpan = dtEndTime - COleDateTime::GetCurrentTime();

			//{ 2011/06/09-gxx: 
			int nBTLimit = CNetBarConfig::GetInstance()->GetBTLimitTime();
			if ( nBTLimit==0 )
			{
				nBTLimit = 50;
			}
			//}
				
			if (dtSpan.GetTotalMinutes() >= nBTLimit) //不可以续包时
			{
				bNowCanBT = FALSE; 
		
				INT nCurrBtId = _ttoi(m_strBundleTimeID);

				//判断可不可以转包时
				if (m_nSelBundleTimeId != nCurrBtId
					&& !strStart.IsEmpty()  
					&& strStart == BundleTimeList.GetStartTime(m_nCurrBundleTimeIndex)
					&& strEnd == BundleTimeList.GetEndTime(m_nCurrBundleTimeIndex)
					&& m_nSelBundleTimePrice >= BundleTimeList.GetPrice(m_nCurrBundleTimeIndex) 
					) 
				{
					bNowChangeBT = TRUE;

					m_nSelBundleTimePrice -= BundleTimeList.GetPrice(m_nCurrBundleTimeIndex); //转包时补差价
				}
			}
		}
	}
}

void CDlgBundleTime::OnNMClickListBundletime(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT nIndex = m_BundleTimeList.GetSelectionMark();

	if (nIndex >= 0)
	{
		INT nBtIndex = m_BundleTimeList.GetItemData(nIndex);

		m_nSelBundleTimeId = BundleTimeList.GetBundleId(nBtIndex);
		m_nSelBundleTimePrice = BundleTimeList.GetPrice(nBtIndex);

		BOOL bNowCanBT = FALSE; 
		BOOL bNowCanForwardBT = FALSE;
		BOOL bNowChangeBT = FALSE;
		
		CheckBundleTimeCondition(nBtIndex, bNowCanBT, bNowCanForwardBT, bNowChangeBT);

		GetDlgItem(IDOK)->EnableWindow(bNowCanBT);
		GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(bNowCanForwardBT);
		GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(bNowChangeBT);

		CString strTmp;
		strTmp.Format(_T("%.2f"), m_nSelBundleTimePrice / 100.0);
		GetDlgItem(IDC_STATIC_CREDITMONEY)->SetWindowText(strTmp);
		GetDlgItem(IDC_EDIT_CREDITMONEY)->SetWindowText(strTmp);
	}

	*pResult = 0;
}

void CDlgBundleTime::OnBnClickedButtonCancelbt()
{
	CString strTmp;
	strTmp.Format(IDS_CANCELBUNDLETIME, m_strName);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	CWaitCursor Wait;

	CCancelBundleTimeMethod CancelBundleTime;

	CancelBundleTime.SetMemberId(m_nMemberId);

	theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);

	CString strRet;

	if (CancelBundleTime.GetStatusCode() == 0)
	{
		OnBnClickedButtonQuery();

		strRet = LOAD_STRING(IDS_BTCANCELOK);

		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			strRet = LOAD_STRING(IDS_BTCANCELOK2);
		}

		m_bAfterBundleTime = FALSE;

		m_nBudleTimeMoney = 0;

		/*CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember))
		{
			
			CLocalServer::GetInstance()->ActiveMemberList.CheckActiveMember(m_nCardId, m_nMemberId, ActiveMember);
		}*/
	}
	else
	{
		strRet = CancelBundleTime.GetStatusMessage();
	}

	SetToolTipPos(IDC_BUTTON_CANCELBT);
	ShowToolTip(strRet, CancelBundleTime.GetStatusCode() != 0);
}

void CDlgBundleTime::ClearData()
{
	m_strName.Empty();
	m_strUserClass.Empty();
	m_strPersonalID.Empty();
	m_strBundleTimeID.Empty();
	m_strBundleTimeStartTime.Empty();
	m_strBundleTimeEndTime.Empty();
	m_strBundleTimeType.Empty();
	m_strBundleTimeMoney.Empty();
	m_strBundleTimeMoney2.Empty();
	m_strBundleTimeArea.Empty();
	m_nSelBundleTimeId = 0;

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CANCELBT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELFBUNDTIME)->EnableWindow(FALSE);
	
	//查询，重新清标志
	m_bAfterBundleTime = FALSE;
	GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC_CREDITMONEY)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_CREDITMONEY)->SetWindowText(_T(""));
}

void CDlgBundleTime::OnBnClickedButtonForwardbt()
{
	CString strTmp;

	strTmp.Format(IDS_ASKBUNDLETIME);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	if (!m_strBundleTimeID.IsEmpty())
	{
		CCancelBundleTimeMethod CancelBundleTime;
		CancelBundleTime.SetMemberId(m_nMemberId);
		theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);
	}

	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(FALSE);

	if (!DoBundleTime(1)) 
	{
		
	}

	GetDlgItem(IDC_BUTTON_FORWARDBT)->EnableWindow(TRUE);
}

void CDlgBundleTime::OnBnClickedButtonChangebt()
{
	CString strTmp;

	strTmp.Format(IDS_ASKBUNDLETIME);

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(FALSE);

	if (!DoBundleTime(2)) //包时失败
	{
		
	}

	GetDlgItem(IDC_BUTTON_CHANGEBT)->EnableWindow(TRUE);
}

// 点击包时
void CDlgBundleTime::OnBnClickedOk()
{
	CString strTmp;

	if (m_strBundleTimeID.IsEmpty())
	{
		strTmp.Format(IDS_ASKBUNDLETIME);
	}
	else
	{
		strTmp.Format(IDS_ASKBUNDLETIMEAGEIN);
	}

	if (IDOK != MsgBox(strTmp, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	if (!m_strBundleTimeID.IsEmpty())
	{
		CCancelBundleTimeMethod CancelBundleTime;
		CancelBundleTime.SetMemberId(m_nMemberId);
		theApp.GetCurCashier()->DoCancelBundleTime(CancelBundleTime);
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	if (!DoBundleTime(0)) //包时失败
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	
}

// 0:普通包时   
// 1:预包时
// 2:转包时
// 3:自定义包时
BOOL CDlgBundleTime::DoBundleTime(UINT nType)
{
	CWaitCursor Wait;

	CBundleTimeMethod BundleTime;

	BundleTime.SetMemberId(m_nMemberId);
	BundleTime.SetReserve(nType);
	BundleTime.SetTimeId(m_nSelBundleTimeId);

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		BundleTime.SetCreditAmount(m_nSelBundleTimePrice);
	}

	theApp.GetCurCashier()->DoBundleTime(BundleTime);

	if (BundleTime.GetStatusCode() >= 0 && BundleTime.GetNeedRelogin())
	{
		//在线包时,需要通知客户端重新登录

		CActiveMember ActiveMember;

		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember)) 
		{
			CActiveMemberList::TReLoginRequest MyReLoginRequest = {0};
			MyReLoginRequest.lastRefNo = ActiveMember.GetRefNo();
			MyReLoginRequest.memberId = m_nMemberId;
			MyReLoginRequest.firstTryTime = GetTickCount();

			CLocalServer::GetInstance()->ActiveMemberList.AddReLoginRequest(MyReLoginRequest);
			CLocalServer::GetInstance()->LocalBundleTime(m_nMemberId);
		}
	}

	CString strRet;

	if (BundleTime.GetStatusCode() == 0)
	{
		// 2011/07/29-8201-gxx: 更新现金包时时，预付款和明细的刷新
		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(m_nMemberId, m_nSelBundleTimePrice, 0, 0);
		}

		if (nType == 2)
		{
			CIBATTS::GetInstance()->SpeakBundleTimePatch(BundleTime.GetAmount());
		}
		else
		{
			CIBATTS::GetInstance()->SpeakBundleTimeResult(BundleTime.GetAmount());
		}
		

		ReQuery();

		strRet = LOAD_STRING(IDS_BTSUCCESSED);

		if (m_btnPrint.GetCheck() == BST_CHECKED)
		{
			PrintTicket();
		}

		m_bAfterBundleTime = TRUE;//包时成功标志置位
		OnBnClickedCheckPrint();//重复打印按钮显示

		COperationInfo OperationInfo;
		OperationInfo.SetOperationType(7);
		OperationInfo.SetUserName(m_strName);
		OperationInfo.SetNetId(m_strNetId);
		OperationInfo.SetOperationAmount(BundleTime.GetAmount());
		OperationInfo.SetDescription(
			CIBAHelpper::CenterTimeToOleDateTime(BundleTime.GetStartTime()).Format(_T("%H:%M"))
			+ _T(" - ") +  
			CIBAHelpper::CenterTimeToOleDateTime(BundleTime.GetEndTime()).Format(_T("%H:%M")));

		::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM)(&OperationInfo));

		/*UINT nAmount = BundleTime.GetAmount();
		CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember)) 
		{
			nAmount = ActiveMember.GetAmount() + nAmount;
			ActiveMember.SetAmount( nAmount );
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember,TRUE);
		}*/
	}
	else
	{
		m_bAfterBundleTime = FALSE;
		strRet = BundleTime.GetStatusMessage();
	}

	if (nType == 0)
	{
		SetToolTipPos(IDOK);
	}
	else if (nType == 1)
	{
		SetToolTipPos(IDC_BUTTON_FORWARDBT);
	}
	else if (nType == 2)
	{
		SetToolTipPos(IDC_BUTTON_CHANGEBT);
	}

	ShowToolTip(strRet, !m_bAfterBundleTime);
	
	if (BundleTime.GetStatusCode() > 0 && BundleTime.GetNeedRelogin())
	{
		CIBAHelpper::WaitFor(3000); //等待3秒
	}

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	return m_bAfterBundleTime;
}

void CDlgBundleTime::PrintTicket()
{	
	CString strTicket;

	CIBAPrint::GetBundleTimeTicket(strTicket);

	strTicket.Replace(_T("[name]"), m_strName);
	strTicket.Replace(_T("[idnumber]"), m_strPersonalID);
	strTicket.Replace(_T("[memberclass]"), m_strUserClass);
	strTicket.Replace(_T("[cardserialnumber]"), m_strNetId);
	strTicket.Replace(_T("[netid]"), m_strNetId.Right(8));
	strTicket.Replace(_T("[pcclass]"), m_strBundleTimeArea);
	strTicket.Replace(_T("[money]"), m_strBundleTimeMoney);
	strTicket.Replace(_T("[starttime]"), m_strBundleTimeStartTime.Mid(11, 5));
	strTicket.Replace(_T("[endtime]"), m_strBundleTimeEndTime.Mid(11, 5));
	strTicket.Replace(_T("[totalmoney]"), m_strAllBalance);
	
	CIBAPrint::Print(strTicket);
}


void CDlgBundleTime::OnBnClickedButtonReprint()
{
	PrintTicket();
}

void CDlgBundleTime::OnBnClickedCheckPrint()
{
	UpdateData(TRUE);

	if (m_btnPrint.GetCheck() == BST_CHECKED && m_bAfterBundleTime)
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_NORMAL);
	} 
	else
	{
		GetDlgItem(IDC_BUTTON_REPRINT)->ShowWindow(SW_HIDE);
	}
}

void CDlgBundleTime::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TimeOfOnlineBundTime)
	{	
		m_bAutoQuery = FALSE;

		KillTimer(TimeOfOnlineBundTime);
	}
	else if (nIDEvent == TimeIdOfAutoQuery)
	{
		KillTimer(TimeIdOfAutoQuery);//只查询一次
		OnBnClickedButtonQuery();
	}

	CNDDialog::OnTimer(nIDEvent);
}

void CDlgBundleTime::OnBnClickedSelfbundtime()
{
	CDlgBundTimeSelfDef SelfDefineBundTimeDlg;
	
	SelfDefineBundTimeDlg.SetMemberId(m_nMemberId);
	SelfDefineBundTimeDlg.SetIsMember(m_bIsMember);

	if (SelfDefineBundTimeDlg.DoModal() == IDOK)
	{
		ReQuery();
	}
}

HBRUSH CDlgBundleTime::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_CREDITMONEY)
		{
			pDC->SetTextColor(RGB(255, 0, 0));		
		}
	}
	else
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_CREDITMONEY)
		{
			pDC->SetTextColor(RGB(0, 0, 255));		
		}
	}

	return hbr;
}

void CDlgBundleTime::OnStnClickedStaticAll()
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

void CDlgBundleTime::OnBnClickedButtonCredit()
{
	if (0 == m_nMemberId) //没有用户
	{
		return;
	}


	CNDCreditDlg dlg;
	
	dlg.SetLinkEnabled(FALSE);
	dlg.SetNetId(m_strNetId);

	if (IDOK == dlg.DoModal())
	{
		ReQuery();

		m_BundleTimeList.SetFocus();
	}
}

BOOL CDlgBundleTime::PreTranslateMessage(MSG* pMsg)
{
	//if (WM_KEYDOWN == pMsg->message)
	//{
	//	if (VK_F3 == pMsg->wParam && m_btnCredit.IsWindowVisible())
	//	{
	//		OnBnClickedButtonCredit();
	//		return TRUE;
	//	}	
	//}

	return CNDDialog::PreTranslateMessage(pMsg);
}

void CDlgBundleTime::OnBnClickedButtonBalance()
{
	if (m_nMemberId > 0)
	{
		CString strTmp;

		strTmp.Format(IDS_DETAILBALANCE, 
			m_nMoneyBalanceW / 100.0, m_nMoneyBalanceY / 100.0, 
			m_nMoneyBalanceZ / 100.0, m_nMoneyOnlineAmount / 100.0,
			(m_nMoneyBalanceW + m_nMoneyBalanceY  + m_nMoneyBalanceZ + m_nMoneyOnlineAmount) / 100.0);


		SetToolTipPos(IDC_BUTTON_BALANCE);

		ShowToolTip(strTmp);
	}
}
