
#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "MainFrm.h"
#include "Socket\LocalServer.h"
#include "ShortMsgDlg.h"
#include "MsgPopupWindow.h"
#include "CreditDlg.h"
#include "MsgBoxDlg.h"
#include "IBAHelpper.h"
#include "BundleTimeDlg.h"
#include "selcomputerdlg.h"
#include "UnlockScreenDlg.h"
#include "NDCreditDlg.h"
#include "NDReturnDlg.h"
#include "NDCheckInDlg.h"
#include ".\JXC\JXCMainDlg.h"
#include "ActiveMemberView.h"
#include ".\computerlistview.h"
#include "Tts\IBATTS.h"
#include "DdnPcpInc.h"
#include "CashierHelper.h"
#include "BossPwdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CComputerListView
//注意，由于本类的控件被重新创建，造成消息传递有误，
//才必须在顶层窗体发送消息给CComputerListView
IMPLEMENT_DYNCREATE(CComputerListView, CBCGPGridView)

BEGIN_MESSAGE_MAP(CComputerListView, CBCGPGridView)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()

	ON_UPDATE_COMMAND_UI(IDM_SNAPSCREEN, OnCheckClient)
	ON_UPDATE_COMMAND_UI(IDM_GETLOG, OnCheckClient)
	ON_UPDATE_COMMAND_UI(IDM_POWEROFF, OnCheckClient)
	ON_UPDATE_COMMAND_UI(IDM_UNUSUAL, OnCheckUnusal)
	ON_UPDATE_COMMAND_UI(IDM_PAYOUT, OnCheckPayOut)
	ON_UPDATE_COMMAND_UI(IDM_CREDITONLINE, OnCheckCredit)
	ON_UPDATE_COMMAND_UI(IDM_REBOOT, OnCheckClient)
	ON_UPDATE_COMMAND_UI(IDM_POWERON, OnCheckNotClient)
	ON_UPDATE_COMMAND_UI(IDM_COLUMN_SET, OnUpdateColumnSet)
	ON_UPDATE_COMMAND_UI(IDM_CONSUMELIST2, OnUpdateConsumelist)
	ON_UPDATE_COMMAND_UI(IDM_SENDSHORTMESSAGE, OnUpdateSendShortMessage)
	ON_UPDATE_COMMAND_UI(IDM_BUNDTIMEONLINE, OnUpdateBundtimeonline)
	ON_UPDATE_COMMAND_UI(IDM_TERMCONSUME2, OnUpdateTermConsume)

	ON_COMMAND_RANGE(IDM_MINCOLUMNCMD, IDM_MAXCOLUMNCMD, OnShowColumn)//自定义添加的列菜单消息

	ON_COMMAND(IDM_CREDITONLINE, OnCreditOnline)
	ON_COMMAND(IDM_COLUMN_SET, OnColumnSet)
	ON_COMMAND(IDM_PAYOUT, OnPayout)
	ON_COMMAND(IDM_UNUSUAL, OnChangeComputer)
	ON_COMMAND(IDM_REBOOT, OnReboot)
	ON_COMMAND(IDM_SNAPSCREEN, OnSnapScreen)
	ON_COMMAND(IDM_GETLOG, OnGetLog)
	ON_COMMAND(IDM_POWEROFF, OnPowerOff)
	ON_COMMAND(IDM_POWERON, OnPowerOn)
	ON_COMMAND(IDM_SENDSHORTMESSAGE, OnSendShortMessage)
	ON_COMMAND(IDM_MYREFRESH, OnRefresh)
	ON_COMMAND(IDM_CONSUMELIST2, OnConsumelist)
	ON_COMMAND(IDM_TERMCONSUME2, OnTermConsume)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_BUNDTIMEONLINE, OnBundtimeonline)
	
	ON_COMMAND(IDM_RETAILONLINE, OnRetailonline)
	ON_UPDATE_COMMAND_UI(IDM_RETAILONLINE, OnUpdateRetailonline)
	ON_COMMAND(IDM_PING, OnPing)
	ON_COMMAND(IDM_UNLOCKSCREEN, OnUnlockScreen)
	ON_UPDATE_COMMAND_UI(IDM_UNLOCKSCREEN, OnUpdateUnlockScreen)
	ON_MESSAGE(WM_COMPVIEW_REFRESH, OnCompRefresh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComputerListView construction/destruction

CComputerListView::CComputerListView()
: m_lpComputerGird(NULL)
{
	CLocalServer::GetInstance()->SetComputerListView(this);//传递指针，作外部调用
}

CComputerListView::~CComputerListView()
{
	//L2_SAFE_DELETE(m_lpComputerGird)
	if (m_lpDataXMReceiver != NULL)
	{
		delete m_lpDataXMReceiver;
		m_lpDataXMReceiver = NULL;
	}
}

INT_PTR CComputerListView::MsgBox(LPCTSTR lpszText, UINT nType /*= MB_OK*/, LPCTSTR lpszCaption /*= NULL*/)
{
	CMsgBoxDlg dlg(this);
	dlg.SetCaption(lpszCaption);
	dlg.SetText(lpszText);
	dlg.SetType(nType);

	return dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CComputerListView message handlers

int CComputerListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;
	//关联计算机队列,并且传递指针
	m_lpComputerGird = new CComputerGrid(&CLocalServer::GetInstance()->ComputerList);

	m_pWndGridCtrl = m_lpComputerGird;
	//创建控件,实际上，这是GRIDView内部进行的操作
	if (!m_lpComputerGird->Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1))//创建
	{
		return -1;
	}

	SetTimer(TimeIdOfPing, IntervalOfPing, NULL);//连接超时处理

	m_lpDataXMReceiver = new CDataXMReceiverT<CComputerListView>
		(this, OnDataArrival);

	BOOL bRet = m_lpDataXMReceiver->Create(_T("DdnPcp-0ED70A77-233A-43c0-9490-962283436DF5"));
	if (!bRet)
	{
		IBA_LOG0(_T("CComputerListView-创建进程间通信对象失败"));
	}

	return 0;
}

void CComputerListView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	AdjustLayout();
}

//取得当前所选终端信息
BOOL CComputerListView::GetCurComputerInfo(CComputerInfo & ComputerInfo)
{
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();

	if (pRow != NULL) 
	{
		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(pRow->GetData()))
		{
			ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(pRow->GetData());

			return TRUE;
		}
	}

	return FALSE;
}

//通过行序号得到终端信息
BOOL CComputerListView::GetComputerInfoFromRow(CComputerInfo & ComputerInfo, INT RowIndex)
{
	CBCGPGridRow* pRow = GetGridCtrl()->GetRow(RowIndex);

	if (pRow != NULL) 
	{
		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(pRow->GetData()))
		{
			ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(pRow->GetData());

			return TRUE;
		}
	}

	return FALSE;
}

//是否有客户端
void CComputerListView::OnCheckClient(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		bEnable = ComputerInfo.HasClient();
	}

	pCmdUI->Enable(bEnable);
}
//是否有用户
void CComputerListView::OnCheckUser(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		bEnable = ComputerInfo.HasUser();
	}

	pCmdUI->Enable(bEnable);
}
//是否没有客户端
void CComputerListView::OnCheckNotClient(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		if (ComputerInfo.GetComputerStatus() != CComputerInfo::ECS_THIEF)
		{
			bEnable = !ComputerInfo.HasClient();
		}
	}

	pCmdUI->Enable(bEnable);
}
//检查是否具有充值权限同时是在线、挂起、黑户、VIP、余额不足等状态
void CComputerListView::OnCheckCredit(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		CComputerInfo ComputerInfo;
		if (GetCurComputerInfo(ComputerInfo) && ComputerInfo.HasUser())
		{
			switch(CLocalServer::GetInstance()->ActiveMemberList.GetUserStateByNetId(ComputerInfo.GetNetId()))
			{
			case CIBAGlobal::emUserStateCenter:
			case CIBAGlobal::emUserStateNULL:
				pCmdUI->Enable(FALSE);
				return;
			}
		}
	}

	BOOL bEnable = FALSE;

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		if (theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CREDIT")))
		{
			CComputerInfo ComputerInfo;

			if (GetCurComputerInfo(ComputerInfo))
			{
				bEnable = ComputerInfo.HasUser();
			}
		}
	}

	pCmdUI->Enable(bEnable);
}
//结账权限（充值权限+用户存在）
void CComputerListView::OnCheckPayOut(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		if (theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CREDIT")))
		{
			CComputerInfo ComputerInfo;

			if (GetCurComputerInfo(ComputerInfo))
			{
				bEnable = (ComputerInfo.GetMemberID() > 0);
			}
		}
	}

	pCmdUI->Enable(bEnable);
}
//异常换机权限
void CComputerListView::OnCheckUnusal(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		if (theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_UNUSUAL")))
		{
			CComputerInfo ComputerInfo;

			if (GetCurComputerInfo(ComputerInfo))
			{
				bEnable = ComputerInfo.HasUser();
				//添加2种状态，分别是离线和贼
				if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_OFFLINE)
				{
					bEnable = TRUE;
				}
				//挂机状态不能换机
				if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_SUSPEND)
				{
					bEnable = FALSE;
				}
			}
		}
	}

	pCmdUI->Enable(bEnable);
}
//显示列
void CComputerListView::OnShowColumn(UINT id)
{
	INT nIndex = id - IDM_MINCOLUMNCMD;//列号

	if (nIndex >= 0 && nIndex < GetGridCtrl()->GetColumnCount())
	{//设置列的显示
		GetGridCtrl()->SetColumnVisible(nIndex, !GetGridCtrl()->GetColumnVisible(nIndex));
		GetGridCtrl()->AdjustLayout();//更新
	}
}
//列的设置菜单
void CComputerListView::OnColumnSet()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_IBASET);
}
//右键弹出处理
void CComputerListView::OnContextMenu(CWnd*, CPoint point)
{
	m_SelectRows.RemoveAll();

	CMenu menu;

	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();

	if (pRow != NULL)//存在选中的子行
	{
		CList<CBCGPGridItem*, CBCGPGridItem*> lstSelected;
		GetGridCtrl()->GetSelectedItems(lstSelected);//多重选择队列

		POSITION pos = lstSelected.GetHeadPosition();//第一个

		while (pos != NULL)
		{
			CBCGPGridItem* pItem = lstSelected.GetNext(pos);//下一个			
			m_SelectRows.Add((UINT)pItem->GetParentRow()->GetData());//保存选中的序号			
		}

		SendMessage(WM_LBUTTONUP);//弹起

		VERIFY(menu.LoadMenu(IDR_COMPUTER_MENU));//载入
		CMenu* pPopup = menu.GetSubMenu(0);//第一个子菜单
		ASSERT(pPopup != NULL);

#ifndef __USING_INTER_RESOURCE
		HMENU hTmp = pPopup->GetSubMenu(6)->GetSafeHmenu();
		pPopup->ModifyMenu((UINT_PTR)hTmp,MF_POPUP,(UINT_PTR)hTmp,LOAD_STRING(IDS_REMOTE_OPERATION));
#endif

		//弹出菜单
		theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->Detach(), point.x, point.y, this, TRUE);
	}
	else
	{
		//扫描鼠标位置，点击在列栏上，才弹出此菜单
		CRect rect;
		m_pWndGridCtrl->GetClientRect(rect);
		rect.bottom = 21; //栏高度大概为21个像素
		ClientToScreen(rect);

		if (rect.PtInRect(point))//点击在栏上
		{
			VERIFY(menu.LoadMenu(IDR_MENU_COLUMN));//载入列菜单

			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			for (INT i = 0; i < GetGridCtrl()->GetColumnCount(); i++)//列枚举
			{//动态插入与列项有关的菜单
				pPopup->InsertMenu(i, MF_BYPOSITION, IDM_MINCOLUMNCMD + i, GetGridCtrl()->GetColumnName(i));
				pPopup->CheckMenuItem(i, MF_BYPOSITION | (GetGridCtrl()->GetColumnVisible(i) ? MF_CHECKED : MF_UNCHECKED));
			}

			theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->Detach(), point.x, point.y, this, TRUE);
		}
	}
}

void CComputerListView::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TimeIdOfPing : 
		{
		if (!CLocalServer::GetInstance()->LocalPing())//本地连接
		{
			Refresh(FALSE);
		}

		break;

		}
	case TimeIdOfSetFocus ://延时接收焦点，主要是用来防止出现回车现象

		KillTimer(TimeIdOfSetFocus);
		SetFocus();

		break;
	}

	CBCGPGridView::OnTimer(nIDEvent);
}

void CComputerListView::AdjustLayout()
{//重新更新位置
	if (GetGridCtrl()->GetSafeHwnd() != NULL)
	{
		CRect rectClient;
		GetClientRect(rectClient);
		GetGridCtrl()->SetWindowPos (NULL, 0, 0,  rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
//更新状态栏
void CComputerListView::Refresh(BOOL bRedrawAll)
{
	m_lpComputerGird->Refresh(bRedrawAll);

	CIBAString strTmp;

	if (CLocalServer::GetInstance()->IsConnected())
	{
		strTmp = MakeStatText();
	}
	else
	{
		strTmp.LoadString(IDS_DISCONNECT);
	}

	((CMainFrame*)AfxGetMainWnd())->UpdateStatusText(strTmp);
}
//拷屏
void CComputerListView::OnSnapScreen()
{
	if (CNetBarConfig::GetInstance()->GetForbidCaptureScreen())
	{
		CDlgBossPwd dlg;
		if (dlg.DoModal() != IDOK)
			return;
	}
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();
	if (pRow != NULL) CLocalServer::GetInstance()->LocalCaptureScreen(pRow->GetData());
}

void CComputerListView::OnGetLog()
{
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();
	if (pRow != NULL) CLocalServer::GetInstance()->LocalGetLog(pRow->GetData());
}

void CComputerListView::OnPowerOff()
{
	//多选关闭
	for (INT ii = 0; ii <  GetGridCtrl()->GetRowCount(); ii++)
	{
		if (GetGridCtrl()->IsRowSelected(ii))
		{
			CBCGPGridRow* pRow = GetGridCtrl()->GetRow(ii);
			if (pRow != NULL)
			{
				CComputerInfo ComputerInfo;
				GetComputerInfoFromRow(ComputerInfo, ii);
				if (ComputerInfo.HasClient())
				{
					INT_PTR nIndex = pRow->GetData();

					CLocalServer::GetInstance()->LocalShutdownComputer(pRow->GetData(), FALSE);
				}
			}
		}
	}
}

void CComputerListView::OnPowerOn()
{
	//多选启动
	for (INT ii = 0; ii <  GetGridCtrl()->GetRowCount(); ii++)
	{
		if (GetGridCtrl()->IsRowSelected(ii))
		{
			CComputerInfo ComputerInfo;
			GetComputerInfoFromRow(ComputerInfo, ii);
			if (!ComputerInfo.HasClient())
			{
				CBCGPGridRow* pRow = GetGridCtrl()->GetRow(ii);
				CLocalServer::GetInstance()->LocalStartupComputer(pRow->GetData());
			}
		}
	}
}
//群体接收短信
void CComputerListView::OnSendShortMessage()
{
	CShortMsgDlg dlg;
	dlg.SetReceivers(m_SelectRows);
	dlg.DoModal();
}

void CComputerListView::OnClientShortMessage(SHORT_MESSAGE_INFO& shortMsgInfo)
{	
	//{ 2011/11/15-8210-gxx: 

	CString str = CA2T(shortMsgInfo.shortMsg.message);

	int nSpl = str.Find(_T('|'));
	if (nSpl > 0 && str.Left(nSpl).CompareNoCase(_T("p-ddnparam")) == 0)
	{
		// 购买点卡的消息
		CString strTmp(CA2T(shortMsgInfo.shortMsg.message));
		strTmp = strTmp.Mid(11);

		CString strTerm(CA2T(shortMsgInfo.shortMsg.termID));
		if (strTmp.Find(_T('?')) >= 0)
		{
			strTmp.AppendFormat(_T("&TermId=%s"), strTerm);
		}
		else
		{
			strTmp.AppendFormat(_T("?TermId=%s"), strTerm);
		}

		CActiveMember ActiveMember;
		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(shortMsgInfo.shortMsg.memberID,ActiveMember))
		{
			strTmp.AppendFormat(_T("&UserName=%s"), ActiveMember.GetUserName());
		}
		else
		{
			strTmp += _T("&UserName=未知姓名");
		}

		IBA_LOG(_T("客户端请求购买点卡,URL=%s"), strTmp);

		CCashierHelper::CashSellDianka(strTmp);
	}

	//}
	else if (shortMsgInfo.shortMsg.messageType !=2 ) // 非余额不足消息
	{
		//弹出式
		CBCGPPopupWindow* pPopup = new CMsgPopupWindow; //这里不需要删除，框架会自动删除的

		// Create indirect:
		CBCGPPopupWndParams params;//设置窗口参数

		CString strMsg, strTermId, strTermIP, strTmp;

		strTermId = CA2W(shortMsgInfo.shortMsg.termID);//终端ID

		// 2011/06/17-gxx: 打补丁，IP长度等于15时，拷贝字符串出问题
		char IP[LEN_IPADDR+1];
		memcpy(IP, shortMsgInfo.shortMsg.computerIP, LEN_IPADDR);
		IP[LEN_IPADDR] = '\0';
		strTermIP = CA2W(IP);//终端IP

		strTermId.Trim();
		strTermIP.Trim();

		if (!strTermIP.IsEmpty())//IP 不是空 表示是客户端信息
		{
			strTmp = LOAD_STRING(IDS_CLIENTINFO);
			strMsg.Format(LOAD_STRING(IDS_FORMTERMIP), strTermId, strTermIP);//设置终端的URL
			params.m_nURLCmdID = (UINT)CLocalServer::GetInstance()->ComputerList.Find(strTermId, strTermIP);
			params.m_hIcon = AfxGetApp()->LoadIcon(IDR_UDO);
		}
		else
		{
			params.m_nURLCmdID = 10000;
			strTmp = LOAD_STRING(IDS_CASHIERINFO);
			params.m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		}

		//接受客户端消息
		strMsg.Append(CA2W(shortMsgInfo.shortMsg.message));

		params.m_strText = strMsg;

		RECT rt = {-1, -1, -1, -1};

		if (CIBAConfig::GetInstance()->GetMsgLeftBottom())// 从左下角弹出
		{
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		}

		if (!CIBAConfig::GetInstance()->GetMsgAutoHide())
		{
			pPopup->SetAutoCloseTime(0);
		}

		pPopup->Create(NULL, params, 0, CPoint(rt.left, rt.bottom));//创建窗体

		pPopup->SetWindowText(strTmp);//显示消息

	}

	CClientShortMessage csm;
	csm.SetMemberId( shortMsgInfo.shortMsg.memberID);
	csm.SetMsgBody( (LPCTSTR)CA2T(shortMsgInfo.shortMsg.message));
	csm.SetMsgID( shortMsgInfo.shortMsg.ID);
	csm.SetMsgType( shortMsgInfo.shortMsg.messageType);
	csm.SetRevTime( (LPCTSTR)CA2T(shortMsgInfo.shortMsg.sendTime));
	csm.SetTermID( (LPCTSTR)CA2T(shortMsgInfo.shortMsg.termID));
	csm.SetTermIP( (LPCTSTR)CA2T(shortMsgInfo.shortMsg.computerIP));

	if (csm.GetMsgType() == 2)
	{
		if (CNetBarConfig::GetInstance()->GetReminderBalance())
		{
			static DWORD nTimeCount = GetTickCount() - 10000;
			if (GetTickCount()-nTimeCount > 10000)// 防止在10秒连续播放
			{
				NS_TTS::CIBATTS::GetInstance()->SpeakReminderBalance(csm.GetTermID());
				nTimeCount = GetTickCount();
			}
			
		}
		csm.SetMsgBody(LOAD_STRING(IDS_BALANCE_REMINDER)); // 余额不足
	}

	//写到信息对话框中
	//((CMainFrame*)AfxGetMainWnd())->m_wndOutput.UpdateMsgLog();
	((CMainFrame*)AfxGetMainWnd())->m_wndOutput.AddMsgLog(csm);
}

void CComputerListView::OnCreditOnline()
{
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		CString strTmp = ComputerInfo.GetNetId();

		if (!strTmp.IsEmpty() && (ComputerInfo.HasUser() 
			|| ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_OFFLINE))
		{	
			CNDCreditDlg dlg;
			dlg.SetNetId(strTmp);
			dlg.DoModal();
		}
	}
}

CString CComputerListView::MakeStatText()
{//取监控的统计数据
	CComputerList::CStatInfo StatInfo;
	CLocalServer::GetInstance()->ComputerList.GetStatInfo(StatInfo);

	CString strRet;

	UINT nTmp = StatInfo.GetAllCount();//用户的总数目，包括异常用户

	if (StatInfo.GetAllCount() == 0)
	{
		nTmp = 1;//防止出现除数为0
	}

	strRet.Format(LOAD_STRING(IDS_STATTEXT), StatInfo.GetAllCount(), 
		StatInfo.GetOnline(), StatInfo.GetOnline() * 100.0 / nTmp,//百分率
		StatInfo.GetLock(), StatInfo.GetLock() * 100.0 / nTmp,
		StatInfo.GetUnLock(), StatInfo.GetUnLock() * 100.0 / nTmp,
		StatInfo.GetThiefCount(),StatInfo.GetThiefCount() * 100.0 / nTmp,
		StatInfo.GetCommonUserCount(), StatInfo.GetCommonUserCount() * 100.0 / nTmp,
		StatInfo.GetMemberUserCount(), StatInfo.GetMemberUserCount() * 100.0 / nTmp,
		StatInfo.GetBundTimePayCount(), StatInfo.GetBundTimePayCount() * 100.0 / nTmp
		);

	return strRet;
}

void CComputerListView::OnPayout()
{
	CComputerInfo ComputerInfo;

	if (!GetCurComputerInfo(ComputerInfo))
	{
		return;
	}

	CActiveMember ActiveMember;

	if (!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(ComputerInfo.GetMemberID(), ActiveMember))
	{
		return;
	}

	CNDReturnDlg dlg;
	dlg.SetNetId(ActiveMember.GetNetId());
	dlg.DoModal();
}

//换机
void CComputerListView::OnChangeComputer()
{	////////////////查询是否有用户////////////
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();

	if (pRow == NULL) return;

	INT_PTR nIndex = pRow->GetData();

	if (CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(nIndex)) return;

	CComputerInfo &ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(pRow->GetData());

	CNDCheckInDlg dlg;
	dlg.SetCheckIn(FALSE);
	dlg.SetNetId(ComputerInfo.GetNetId());
	dlg.DoModal();

}
 
void CComputerListView::OnReboot()
{//重新启动
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		for (INT ii = 0; ii <  GetGridCtrl()->GetRowCount(); ii++)
		{
			if (GetGridCtrl()->IsRowSelected(ii))
			{
				CComputerInfo ComputerInfo;
				GetComputerInfoFromRow(ComputerInfo, ii);
				if (ComputerInfo.HasClient())
				{
					CBCGPGridRow* pRow = GetGridCtrl()->GetRow(ii);
					CLocalServer::GetInstance()->LocalShutdownComputer(pRow->GetData(), TRUE);
				}
			}
		}
	}
}

void CComputerListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CBCGPGridView::OnLButtonUp(nFlags, point);

	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndTaskPane.ShowUserInfo(ComputerInfo);
	}
}

void CComputerListView::OnUpdateColumnSet(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 

	if (bRet)
	{
		bRet = theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CONFIG"));
	}

	pCmdUI->Enable(bRet);
}

void CComputerListView::OnRefresh()
{
	if (CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		//取系统时间
		CGetTimeMethod GetTimeMethod;
		GetTimeMethod.Execute();

		//非断网时使能
		if (GetTimeMethod.GetStatusCode() == -1)
		{
			return;
		}
		else if (GetTimeMethod.GetStatusCode() == 0)
		{
			GetTimeMethod.SyncLocalTime();
		}
	}

	CLocalServer::GetInstance()->LocalGetComputerList();
}

void CComputerListView::OnUpdateConsumelist(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	BOOL bEnable = FALSE;

	if (theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CONSUMELIST")))
	{
		CComputerInfo ComputerInfo;

		if (GetCurComputerInfo(ComputerInfo))
		{
			bEnable = ComputerInfo.HasUserInfo();
		}
	}

	pCmdUI->Enable(bEnable);
}

void CComputerListView::OnConsumelist()
{
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		CActiveMember ActiveMember;

		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(ComputerInfo.GetMemberID(), ActiveMember))
		{
			COperationInfo OperationInfo;
			OperationInfo.SetMemberId(ActiveMember.GetMemberID());
			OperationInfo.SetUserName(ActiveMember.GetUserName());
			OperationInfo.SetNetId(ActiveMember.GetNetId());
			OperationInfo.SetOperationTime(ActiveMember.GetActivationTime());
			OperationInfo.SetIsMember(ActiveMember.IsMember());

			::SendMessage(CMainFrame::m_hOperationLog, WM_OPERATIONLOG, 2, (LPARAM)&OperationInfo);
		}
		
		//theApp.GetIBAView()->QueryUserComsumeList(ComputerInfo.GetNetId());
	}
}

void CComputerListView::OnUpdateTermConsume(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_TERMCONSUME")));
}

void CComputerListView::OnTermConsume()
{
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		theApp.GetIBAView()->QueryTermConsumeList(ComputerInfo.GetTerminalID());
	}
}

void CComputerListView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CBCGPGridView::OnLButtonDblClk(nFlags, point);

	if (theApp.GetCurCashier()->IsOnDuty() && theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_CREDIT")))
	{
		CComputerInfo ComputerInfo;

		if (GetCurComputerInfo(ComputerInfo))
		{
			CString strTmp = ComputerInfo.GetNetId();

			if (!strTmp.IsEmpty() && (ComputerInfo.HasUser() 
				|| ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_OFFLINE))
			{	
				CNDReturnDlg dlg;
				dlg.SetNetId(strTmp);
				dlg.DoModal();
			}
		}
	}
}

BOOL CComputerListView::FindComputer(UINT nKeyType, LPCTSTR lpszKeyValue)
{
	INT_PTR nIndex = m_lpComputerGird->GetComputerList()->LookFor(nKeyType, lpszKeyValue);

	if (nIndex < 0)
	{
		return FALSE;
	}

	CBCGPGridRow* pRow = m_lpComputerGird->FindRowByData(nIndex);

	if (pRow == NULL)
	{
		return FALSE;
	}

	m_lpComputerGird->EnsureVisible(pRow);
	m_lpComputerGird->SetCurSel(pRow);

	//这里不能直接设置焦点，要采用延时的机制，否则会产生回车事件，原因未知
	SetTimer(TimeIdOfSetFocus, 200, NULL);

	return TRUE;
}

void CComputerListView::OnBundtimeonline()
{
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();

	if (pRow != NULL) 
	{
		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(pRow->GetData()))
		{
			CComputerInfo ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(pRow->GetData());
			
			CDlgBundleTime BundleTimeDlg;
			BundleTimeDlg.SetNetId(ComputerInfo.GetNetId(), TRUE);
			BundleTimeDlg.DoModal();
		}
	}
}

void CComputerListView::OnUpdateBundtimeonline(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	//上班限制
	BOOL bRet = theApp.GetCurCashier()->IsOnDuty(); 
	//收银台权限控制
	bRet = bRet && theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_BUNDLETIME"));
	//用户状态限制
	CComputerInfo ComputerInfo;
	GetCurComputerInfo(ComputerInfo);
	//只对生日，黑户，VIP，挂机，余额不多，在线状态开放
	bRet = bRet && ComputerInfo.HasUser();

	pCmdUI->Enable(bRet);
}

void CComputerListView::OnUpdateSendShortMessage(CCmdUI *pCmdUI)
{
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_THIEF)
		{
			pCmdUI->Enable(FALSE);
		}
	}
}

void CComputerListView::OnRetailonline()
{
	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_ONLINE)
		{
			CJXCMainDlg::RetailOnline(ComputerInfo.GetTerminalID(), ComputerInfo.GetNetId());
		}
	}
}

void CComputerListView::OnUpdateRetailonline(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(FALSE);

	CComputerInfo ComputerInfo;

	if (GetCurComputerInfo(ComputerInfo))
	{
		if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_ONLINE)
		{
			pCmdUI->Enable(TRUE);
		}
	}
}

void CComputerListView::OnPing()
{
	CBCGPGridRow* pRow = GetGridCtrl()->GetCurSel();

	if (pRow != NULL) 
	{
		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(pRow->GetData()))
		{
			CComputerInfo ComputerInfo;

			if (GetCurComputerInfo(ComputerInfo))
			{
				if (ComputerInfo.HasUser()) //有用户则上报金额
				{
					CLocalServer::GetInstance()->LocalForceUpdateData(pRow->GetData());

					return ;
				}
			}

			CLocalServer::GetInstance()->LocalPingClient(pRow->GetData());
		}
	}
}

void CComputerListView::OnUnlockScreen()
{
	CUnlockScreenDlg dlg;
	
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	for (INT i = 0; i < GetGridCtrl()->GetRowCount(); i++)
	{
		if (!GetGridCtrl()->IsRowSelected(i))
		{
			continue;
		}

		CBCGPGridRow* pRow = GetGridCtrl()->GetRow(i);

		if (!CLocalServer::GetInstance()->ComputerList.IsInvaildIndex(pRow->GetData()))
		{
			CComputerInfo& ComputerInfo = CLocalServer::GetInstance()->ComputerList.GetComputer(pRow->GetData());

			if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
			{
				CLocalServer::GetInstance()->LocalUnlockScreen(pRow->GetData(), dlg.GetPwd());
			}
		}
	}
}

void CComputerListView::OnUpdateUnlockScreen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
	
	if(2 != CNetBarConfig::GetInstance()->GetDisallowUnlock())
	{
		CComputerInfo ComputerInfo;

		if (GetCurComputerInfo(ComputerInfo))
		{
			if (ComputerInfo.GetComputerStatus() == CComputerInfo::ECS_LOCKED)
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}	
}

INT CComputerListView::OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, LPVOID lpBuf, size_t cbLen)
{
	if (nMsg == 1)
	{
		// 购买点卡成功后
		UINT nMemberID = (UINT)nParam1;
		char chInfo[1024] = {0};
		strncpy(chInfo,(const char*)lpBuf,cbLen);
		CString strInfo = CA2T(chInfo);

		CLocalServer::GetInstance()->LocalSendShortMessageByMemberID(nMemberID, strInfo);

		IBA_LOG(_T("发送点卡销售成功的消息给客户端,MemberID=%d,msg=%s"),nMemberID,strInfo);

	}
	return 0;
}

void CComputerListView::SetCurSel(int nIndex)
{
	if(m_lpComputerGird)
	{
		m_lpComputerGird->SetCurSel(-1);
	}
}

void CComputerListView::SetNewUserDefaultTerm(LPCTSTR szTermID)
{
	m_strDefaultTermID = szTermID;
}

LRESULT CComputerListView::OnCompRefresh( WPARAM wParam, LPARAM lParam )
{
	Refresh(TRUE);
	return 0;
}
