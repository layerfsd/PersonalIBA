#include "stdafx.h"
#include "IBAHelpper.h"
#include "Cashier.h"
#include "IBA.h"
#include "MainFrm.h"
#include "DialogManager.h"
#include "RealName\IBARealName.h"
#include "ActiveMemberView.h"
#include "Socket\LocalServer.h"
#include ".\NDDialog.h"
#include "Dal\IBADAL.h"

HWND CNDDialog::hWndPre = NULL;

CNDDialog::CNDDialog(UINT nIDTemplate, CWnd *pParent/*= NULL*/)
: CIBADialog(nIDTemplate, pParent)
, m_bFromScan(FALSE), m_nMemberId(0), m_nCardId(0)
, m_bIsMember(FALSE), m_bOnLine(FALSE), m_nUserClassID(0)
, m_bAutoQuery(FALSE)
{
	m_nMoneyOnlineAmount = 0;
	m_nMoneyBalanceW = 0;
	m_nMoneyBalanceY = 0;
	m_nMoneyBalanceZ = 0;

	EnableVisualManagerStyle();

	if (m_strNetId.IsEmpty())
	{
		m_strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
	}

	m_nDlgTemplateID = nIDTemplate;
	m_bCenterDisconnect = FALSE;
	m_bNeedActivation = FALSE;

}

CNDDialog::~CNDDialog()
{
}

void CNDDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strNetId);
}

BEGIN_MESSAGE_MAP(CNDDialog, CIBADialog)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_BN_CLICKED(IDC_BUTTON_SCANRECO, OnBnClickedButtonScanreco)
	ON_COMMAND(IDM_SCAN1, OnScan1)
	ON_COMMAND(IDM_SCAN2, OnScan2)
	ON_COMMAND(IDM_READ2, OnRead2)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CNDDialog::OnOK()
{
	//CIBADialog::OnOK();
}

void CNDDialog::OnCancel()
{
	CIBADialog::OnCancel();
}

void CNDDialog::SetNetId(CString newVal, BOOL bAutoQuery)
{
	if (newVal.IsEmpty())
	{
		//m_strNetId = theApp.GetLastNetID();
		m_strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
	}
	else
	{
		m_strNetId = newVal;
	}

	m_bAutoQuery = bAutoQuery;
}

BOOL CNDDialog::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	CBitmap bmp;
	bmp.LoadBitmap(IDB_NETBARUSER);

	m_Images.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);
	m_Images.Add(&bmp, RGB(250, 50, 250));

	CWnd* pScanBtn = GetDlgItem(IDC_BUTTON_SCANRECO);

	if (pScanBtn != NULL)
	{ 
		pScanBtn->EnableWindow(theApp.GetRealName()->IsScanEnabled());

		if (CNetBarConfig::GetInstance()->GetMustScan() == 2)
		{
			pScanBtn->ShowWindow(SW_HIDE);
		}
	}

	return TRUE;  
}

BOOL CNDDialog::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam) //按回车
	{
		CWnd* pWnd = GetFocus(); //焦点何处

		if (pWnd != NULL)
		{
			for (INT i = 0; i < m_FocusEnterMap.GetCount(); i++)
			{
				if (pWnd->GetDlgCtrlID() == m_FocusEnterMap.GetAt(i).nFocusID)
				{
					if (GetDlgItem(m_FocusEnterMap.GetAt(i).nEnterID)->IsWindowEnabled())
					{
						return PostMessage(WM_COMMAND, MAKEWPARAM(m_FocusEnterMap.GetAt(i).nEnterID, BN_CLICKED)); //模拟按钮单击事件
					}
					else
					{
						return TRUE;
					}
				}			
			}
		}
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}


void CNDDialog::CheckCard(LPCTSTR strNetId)
{
	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strNetId);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	const INT nStatus = checkCard.GetStatusCode();

	if (nStatus == 0 || nStatus == 75) //用户已经作废
	{
		if (checkCard.GetMemberId() != 0) //已经开过的卡,充值退卡
		{
			m_nMemberId = checkCard.GetMemberId();
			m_nCardId = checkCard.GetCardId();

			OnOldCard(m_nCardId, m_nMemberId);				
		}
		else //新卡注册
		{
			OnNewCard(m_nCardId);
		}
	}
	else if (nStatus == -1)
	{
		// 2011/07/12-8201-gxx: 断网执行本地查询
		DoLocalModeQuery();  
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_QUERY);
		ShowToolTip(checkCard.GetStatusMessage(), TRUE);
	}

	m_bFromScan = FALSE;
}	

void CNDDialog::OnNewCard(UINT nCardId)
{
	SetToolTipPos(IDC_EDIT_IDCARDSNO);
	ShowToolTip(IDS_HASNOTUSED);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
}

void CNDDialog::OnOldCard(UINT nCardId, UINT nMemberId)
{
	ReQuery();
}

BOOL CNDDialog::QueryBalance(UINT nMemberId, UINT nCardId)
{
	CBalanceMethod& Balance = CCurrentNetBarUser::GetInstance()->Balance;

	Balance.SetMemberId(nMemberId);
	Balance.SetCardId(nCardId);
	Balance.SetRound(0);

	theApp.GetCurCashier()->DoBalance(Balance);

	int nStatus = Balance.GetStatusCode();
	if (nStatus == 0)
	{
		OnGetBalanceOK(Balance);

		theApp.UpdateToolbarLastID(Balance.GetSerailNum());

		return TRUE;
	}
	else
	{
		OnGetBalanceFailed(Balance);
		return FALSE;
	}
}

BOOL CNDDialog::ReQuery()
{
	return QueryBalance(m_nMemberId, m_nCardId);
}

void CNDDialog::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_bOnLine = (Balance.GetOnline() == 1); //1 表示用户在线
	m_bIsMember = Balance.IsNetbarMember();
	
	m_strNetId = Balance.GetSerailNum();
	
	m_strName.Format(_T("%s(%s)"), Balance.GetUserName(), CIBAHelpper::GetSexAsString(Balance.GetSex()));
	m_strPersonalID = Balance.GetPersonalId();
	
	m_nUserClassID = Balance.GetUserClass();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());

	m_strMemberName = Balance.GetMemberName();
	m_strOpenNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());
	m_strTermId = Balance.GetTermId();

	m_strAllBalance.Format(_T("%.2f"), Balance.GetTotalBalacne() / 100.0);

	m_nMoneyBalanceW = Balance.GetCashBalance();
	m_nMoneyBalanceY = Balance.GetUsePresent();
	m_nMoneyBalanceZ = Balance.GetCreditPresent();
	m_nMoneyOnlineAmount = Balance.GetOnlineAmount();

	m_nCurrentPoint = Balance.GetCurrentPoint();
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, m_ActiveMember);

	m_bNeedActivation = (Balance.GetNeedActivation() == 1);
}

void CNDDialog::OnGetBalanceFailed(CBalanceMethod & Balance)
{

}

void CNDDialog::OnBnClickedButtonQuery()
{
	CWaitCursor Wait;

	UpdateData();

	if (m_strNetId.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO);
		ShowToolTip(IDS_INPUTNETID);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
		return;
	}

	ClearData();

	// 2011/07/08-8201-gxx: 判断是否进入本地模式查询
	// 1. 断网状态下，要进入本地模式查询。
	// 2. 恢复状态下，只有本地用户需要进入本地模式查询。
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken ||
		(  CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore &&
		   GetUserState() == CIBAGlobal::emUserStateLocal) )
	{
		DoLocalModeQuery();   // 进入本地模式查询
	}
	else
	{
		// 2011/07/12-8201-gxx: 正常状态和恢复状态下，中心用户调用CheckCard
		CheckCard(m_strNetId);
	}
	
	UpdateData(FALSE);
}

void CNDDialog::ClearData()
{

}

void CNDDialog::AddFocusEnter(UINT nFocusID, UINT nEnterID)
{
	TFocusEnter FocusEnter = {nFocusID, nEnterID};

	m_FocusEnterMap.Add(FocusEnter);
}

void CNDDialog::OnBnClickedButtonScanreco()
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_SCAN);
	CMenu* pPopup = menu.GetSubMenu(0);
	pPopup->RemoveMenu(4, MF_BYPOSITION);

	CRect rect;
	GetDlgItem(IDC_BUTTON_SCANRECO)->GetWindowRect(rect);
	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->Detach(), rect.CenterPoint().x, rect.CenterPoint().y, this, TRUE);
}

void CNDDialog::OnScan1()
{
	ScanReco(1);
}

void CNDDialog::OnScan2()
{
	ScanReco(2);
}

void CNDDialog::OnRead2()
{
	ScanReco(3);
}

void CNDDialog::ScanReco(UINT nType)
{
	CWaitCursor Wait;

	CNetBarUser NetBarUser;

	if (theApp.GetRealName()->ScanUserInfo(nType, NetBarUser))
	{
		m_bFromScan = TRUE;
		m_strNetId = NetBarUser.GetCertificateID();
		UpdateData(FALSE);
		OnBnClickedButtonQuery();
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_SCANRECO);
		ShowToolTip(IDS_SCANFAILED);
	}
}


void CNDDialog::InitWindowPos()
{
	if (IsWindow(hWndPre))
	{
		CRect rect;

		::GetWindowRect(hWndPre, rect);

		SetWindowPos(NULL, rect.left - 10, rect.top - 25, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
	}

	hWndPre = GetSafeHwnd();
}


void CNDDialog::OnTimer(UINT nIDEvent)
{
	CIBADialog::OnTimer(nIDEvent);

	if (TimerIdOfRePosDialogs == nIDEvent)
	{
		KillTimer(nIDEvent);
		//theApp.GetDialogManager()->ReposDialogs();
	}
}

void CNDDialog::RePosDialogs()
{
	SetTimer(TimerIdOfRePosDialogs, 100, NULL);
}

BOOL CNDDialog::IsMeGotFocus()
{
	CWnd* pWnd = GetFocus();

	if (pWnd != NULL)
	{
		if (pWnd->GetDlgCtrlID() == GetDlgCtrlID())
		{
			return TRUE;
		}
		else
		{
			CWnd* pOwner = pWnd->GetParentOwner();

			if (pOwner != NULL)
			{
				if (pOwner->GetDlgCtrlID() == GetDlgCtrlID())
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CNDDialog::SimulateClickMe()
{
	CPoint point;
	GetCursorPos(&point);

	CRect rect;
	GetWindowRect(rect);

	DWORD dx = (rect.left + 10) * 65536 / GetSystemMetrics(SM_CXSCREEN);
	DWORD dy = (rect.top + 10) * 65536 / GetSystemMetrics(SM_CYSCREEN);
	
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP,
		dx, dy, 0, GetMessageExtraInfo());

	SetCursorPos(point.x, point.y);
}

void CNDDialog::OnCenterDisconnect()
{
	// do nothing
}

int CNDDialog::GetUserState()
{
	return CLocalServer::GetInstance()->ActiveMemberList.GetUserStateByNetId(m_strNetId);
}

BOOL CNDDialog::DoLocalModeQuery()
{
	m_LocalActiveMember = CActiveMember();
	CActiveMember& ActiveMember = m_LocalActiveMember;

	BOOL bRet = FALSE;


	if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(m_strNetId, m_LocalActiveMember))
	{
		return FALSE;
	}
	

	m_nMemberId = m_LocalActiveMember.GetMemberID();


	/*CActiveMember memActive;
	if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(m_strNetId,memActive))
	{
		m_bOnLine = memActive.GetIsOnLine();
		m_LocalActiveMember.SetIsOnLine(m_bOnLine);
	}
	else
	{
		m_bOnLine = FALSE;
	}*/
	m_bOnLine = m_LocalActiveMember.GetIsOnLine();


	m_bIsMember = m_LocalActiveMember.IsMember();
	m_strNetId= m_LocalActiveMember.GetNetId();
	m_strName.Format(_T("%s(%s)"), m_LocalActiveMember.GetUserName(), m_LocalActiveMember.GetSexAsString()); 

	m_strPersonalID = m_LocalActiveMember.GetPersonalID();

	m_nUserClassID = m_LocalActiveMember.GetClassId();
	m_strUserClass = m_LocalActiveMember.GetUserClassName();

	//m_strMemberName = m_LocalActiveMember.GetUserName();
	m_strOpenNetBar = CNetBarConfig::GetInstance()->GetNetBarName(CNetBarConfig::GetInstance()->GetNetBarId());
	m_strTermId = m_LocalActiveMember.GetTerminalID();

	m_strAllBalance.Format(_T("%.2f"), m_LocalActiveMember.GetAvailavleBalance() / 100.0);

	theApp.UpdateToolbarLastID(m_strNetId);

	return TRUE;
}

BOOL CNDDialog::DoLocalModeOperate()
{
	return TRUE;
}