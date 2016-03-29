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
, m_bAutoQuery(FALSE)/*, m_bNDHasLocalQuery(FALSE)*/
{
	m_nMoneyOnlineAmount = 0;
	m_nMoneyBalanceW = 0;
	m_nMoneyBalanceY = 0;
	m_nMoneyBalanceZ = 0;
	m_nMoneyAllInAccount=0;
	m_nMoneyGuaranty=0;

	EnableVisualManagerStyle();

	if (m_strNetId.IsEmpty())
	{
		m_strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
	}

	m_nDlgTemplateID = nIDTemplate;
	//m_bCenterDisconnect = FALSE;

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

/*************************************************
Function:       // CNDDialog::OnInitDialog
Description:    // CIBADialog::OnInitDialog进行初始设置。
				// 根据当前实名设置，显示/隐藏ScanReco(扫描)按钮
Calls:          // None
Table Accessed: // None
Table Updated:  // None
Input:          // None
Output:         // None
Return:         // None
Others:         // None
*************************************************/
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

/*************************************************
Function:       // CNDDialog::CheckCard
Description:    // 将NetId作为SerialNum执行DoCheckCard.
				//  已注册 - OnOldUser间接调用CNDDialog::QueryBalance 执行DoBalance查询余额，为Dialog各成员赋值. 设置m_ActiveMember. 或执行派生类改写的新函数.
				//  未注册 - OnNewUser()不进行动作.应该是执行派生类改写的新函数.
				//  网络不通 - DoLocalModeQuery().用m_ActiveMember填充Dialog各成员.将NetId显示在工具栏中。标记m_bNDHasLocalQuery=TRUE。
Calls:          // DoCheckCard, OnOldUser/OnNewUser/DoLocalModeQuery
Table Accessed: // None
Table Updated:  // None
Input:          // strNetId - 用做SerialNum来CheckCard
Output:         // None
Return:         // None
Others:         // OnNewUser及OnOldUser均为virtual类型
*************************************************/
void CNDDialog::CheckCard(LPCTSTR strNetId)
{
	CIDCheckCardMethod checkCard;      //将NetId作为SerialNum进行CheckCard
	checkCard.SetSerialNum(strNetId);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	const INT nStatus = checkCard.GetStatusCode();

	if (nStatus == 0 || nStatus == 75) //校验成功或发现用户已经作废
	{
		if (checkCard.GetMemberId() != 0) //如果用户已经注册，DoBalance并用返回结果给Dialog各成员赋值。
		{
			m_nMemberId = checkCard.GetMemberId();
			m_nCardId = checkCard.GetCardId();

			OnOldCard(m_nCardId, m_nMemberId);	//间接调用CNDDialog::QueryBalance 执行DoBalance查询余额，为Dialog各成员赋值. 设置m_ActiveMember.			
		}
		else // 用户未注册，新卡注册(调用派生类成员函数,都会给Dialog成员赋值且设置m_ActiveMember?)
		{
			OnNewCard(m_nCardId);
		}
	}
	//else if (nStatus == -1)
	//{
	//	// 断网，则填充Dialog各成员值。来源为m_localActiveMember。将NetId显示在工具栏中。标记m_bNDHasLocalQuery=TRUE。
	//	DoLocalModeQuery();  
	//}
	else
	{
		SetToolTipPos(IDC_BUTTON_QUERY);
		ShowToolTip(checkCard.GetStatusMessage(), TRUE);
	}

	m_bFromScan = FALSE;
}	

//virtual function
void CNDDialog::OnNewCard(UINT nCardId)
{
	SetToolTipPos(IDC_EDIT_IDCARDSNO);
	ShowToolTip(IDS_HASNOTUSED);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
}

//virtual function.间接调用CNDDialog::QueryBalance 执行DoBalance查询余额，为Dialog各成员赋值. 设置m_ActiveMember.
void CNDDialog::OnOldCard(UINT nCardId, UINT nMemberId)
{
	ReQuery();
}

/*************************************************
Function:       // CNDDialog::QueryBalance
Description:    // 执行DoBalance查询余额，为Dialog各成员赋值. 设置m_ActiveMember.
				// 查询失败，调用派生类函数
Calls:          //
Table Accessed: //
Table Updated:  //
Input:          // nMemberId - 用于查询余额输入
                // nCardId - 用于查询余额输入
Output:         // None
Return:         // 查询成功返回TRUE。否则返回FALSE
Others:         // None
*************************************************/
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
		OnGetBalanceOK(Balance);//为Dialog各成员赋值. 设置m_ActiveMember

		//theApp.UpdateToolbarLastID(Balance.GetSerailNum()); //Temporarily comment out

		return TRUE;
	}
	else
	{
		OnGetBalanceFailed(Balance); //查询失败暂时不做处理。由于是virtual，在实例化时再做处理。
		return FALSE;
	}
}

BOOL CNDDialog::ReQuery()
{
	return QueryBalance(m_nMemberId, m_nCardId);
}

/*************************************************
Function:       // CNDDialog::OnGetBalanceOK
Description:    // 为Dialog各成员赋值. 设置m_ActiveMember
Input:          // Balance - 用于填充Dlg的参数
Others:         // virtual，可被改写
*************************************************/
void CNDDialog::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_bOnLine = (Balance.GetOnline() == 1);     //用户是否在线
	//m_bIsMember = Balance.GetIsMember();        //是否会员,不是临时用户的都是会员	
	m_bIsMember = Balance.IsNetbarMember();
	m_strNetId = Balance.GetSerailNum();		//NetId=SerialNum	
	m_strName.Format(_T("%s(%s)"), Balance.GetUserName(), CIBAHelpper::GetSexAsString(Balance.GetSex())); //用户名(性别)
	m_strPersonalID = Balance.GetPersonalId(); //用户ID	
	m_nUserClassID = Balance.GetUserClass();  //n用户级别
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass()); //用户级别
	m_strMemberName = Balance.GetMemberName(); //用户姓名
	m_strOpenNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId()); //开户网吧名
	m_strTermId = Balance.GetTermId(); //终端号
	m_nMoneyBalanceW = Balance.GetCashBalance(); //W余额 = 无送充值余额
	m_nMoneyBalanceY = Balance.GetUsePresent(); //Y余额 = 有送充值余额
	m_nMoneyBalanceZ = Balance.GetCreditPresent(); //Z余额 = 赠送账户余额
	m_nMoneyOnlineAmount = Balance.GetOnlineAmount(); //OnlineAmount预扣金额
	m_nCurrentPoint = Balance.GetCurrentPoint(); //用户当前积分
	m_nMoneyGuaranty = Balance.GetGuarantyMoney(); //押金-只有临时用户有
	m_nMoneyAllInAccount = Balance.GetTotalBalacne(); //总金额 = 无送充值余额 + 可用“有送充值”余额 + 可用“赠送帐户”余额 + 用户预扣金额

	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, m_ActiveMember); //从Map中查找用户，保存到ActiveMember中

	m_strAllBalance.Format(_T("%.2f"), Balance.GetTotalBalacne() / 100.0);
	//theApp.UpdateToolbarLastID(m_strNetId); //Temporarily comment out
}

void CNDDialog::OnGetBalanceFailed(CBalanceMethod & Balance)
{

}

/*************************************************
Function:       // CNDDialog::OnBnClickedButtonQuery
Description:    // 查询用户信息，用m_localActiveMember或DoBalance返回值为Dialog各成员赋值。网络正常时还需设置m_ActiveMember,Note:本地模式时,m_localActiveMember应在外部被填充.
// 1. 本地模式下，本地模式查询 - 用m_localActiveMember填充Dialog各成员值。并将NetId显示在工具栏中。
	Note:若函数被改写，请参考派生类的实现！
// 2. 正常状态下，中心模式查询 - DoCheckCard,已注册用户调用OnOldUser(基类动作为DoBalance并填充Dialog成员值,设置m_ActiveMeber)，未注册用户为OnNewUser(基类无动作)
Calls:          // DoLocalModeQuery,CheckCard
Others:         //并不一定是响应按钮消息，在某些派生对话框产生后的OnTimer消息中会调用.
*************************************************/
void CNDDialog::OnBnClickedButtonQuery()
{
	IBA_TRACE("OnBnClickedButtonQuery");
	CWaitCursor Wait;
	UpdateData();//从对话框更新数据到所有变量

	//如果账号为空，显示提示框
	if (m_strNetId.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO);
		ShowToolTip(IDS_INPUTNETID);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
		return;
	}

	ClearData(); 

	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)//Restore/Broken - 从激活列表中查找用户并赋值给成员变量
	{
		DoLocalModeQuery();
	}
	else//Normal - 从中心进行查询,并由Balance接口的返回值赋值给成员变量
	{
		CheckCard(m_strNetId);
	}
	
	UpdateData(FALSE);//更新对话框数据显示
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

/*************************************************
Function:       // CNDDialog::DoLocalModeQuery
Description:    // 本地查询用户信息,填充到窗口
Return:         // 
*************************************************/
BOOL CNDDialog::DoLocalModeQuery()
{
	//查找用户信息
	UINT nMemberId;
	if(FALSE == CIBADAL::GetInstance()->GetMemberId(m_strNetId,nMemberId)){return FALSE;}
	CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,m_ActiveMember);
	
	//填充成员变量
	m_nMemberId = m_ActiveMember.GetMemberID();
	m_bOnLine = m_ActiveMember.GetIsOnLine(); 
	m_bIsMember = m_ActiveMember.IsMember();
	//m_strNetId= m_ActiveMember.GetNetId(); //如果未激活,m_strNetId将为空
	m_strName.Format(_T("%s(%s)"), m_ActiveMember.GetUserName(), m_ActiveMember.GetSexAsString()); 
	m_strPersonalID = m_ActiveMember.GetPersonalID();
	m_nUserClassID = m_ActiveMember.GetClassId();
	m_strUserClass = m_ActiveMember.GetUserClassName();
	m_strMemberName = m_ActiveMember.GetUserName();
	m_strOpenNetBar = CNetBarConfig::GetInstance()->GetNetBarName(CNetBarConfig::GetInstance()->GetNetBarId());
	m_strTermId = m_ActiveMember.GetTerminalID();
	
	m_nMoneyAllInAccount = m_ActiveMember.GetAvailavleBalance();

	//取押金
	UINT nMoneyGuaranty = 0;
	if(FALSE == CIBADAL::GetInstance()->GetDepositFromMember(m_strNetId,nMoneyGuaranty))
	{
		m_nMoneyGuaranty = 0;
	}
	else
	{
		m_nMoneyGuaranty = nMoneyGuaranty;
	}
	//CString sql;
	//sql.Format(_T("select deposit from member where memberId=%d"),m_ActiveMember.GetMemberID());
	//GxxDictionaryPtr MemberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);

	//if(!MemberPtr)
	//{
	//	IBA_LOG(_T("Error! CNDDialog::DoLocalModeQuery从Member表中数据库读取押金失败.MemberId:%d"),ActiveMember.GetMemberID());
	//	m_nMoneyGuaranty = 0;
	//}
	//else
	//{
	//	m_nMoneyGuaranty = MemberPtr->valueForKey("deposit")->intValue();
	//}

	//余额字符串
	m_strAllBalance.Format(_T("%.2f"), (double)m_ActiveMember.GetAvailavleBalance()/ 100.0); //总金额=可用余额

	//theApp.UpdateToolbarLastID(m_strNetId); //Temporarily comment out

	return TRUE;
}

BOOL CNDDialog::DoLocalModeOperate()
{
	return TRUE;
}

void CNDDialog::SetLocalActiveMember( CActiveMember& activeMember )
{
	m_LocalActiveMember = activeMember;
}