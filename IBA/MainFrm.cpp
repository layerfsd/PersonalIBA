
#include "stdafx.h"
#include <Psapi.h>
#include "IBA.h"
#include "Cashier.h"
#include "ServletHellper.h"
#include "IBALoginDlg.h"
#include "ShortMsgDlg.h"
#include "OffDutyDlg.h"
#include "CreditReverseDlg.h"
#include "IBAAboutDlg.h"
#include "CreditDlg.h"
#include "ModifyPwdDlg.h"
#include "RegisterDlg.h"
#include "IBAPropertySheet.h"
#include "ModifyMemberDlg.h"
#include "BundleTimeDlg.h"
#include "ChangeUserSNDlg.h"
#include "ComputerListView.h"
#include "NDUpgradeDlg.h"
#include "NDCardCreditDlg.h"
#include "Socket\LocalServer.h"
#include "..\GCardID_RZX\RealName.h"
#include "RealName\IBARealName.h"
#include "NetBarConfigPage.h"
#include "NetBarInfoPage.h"
#include "ClientConfigPropertySheet.h"
#include "RealNameConfigPage.h"
#include "MsgBoxDlg.h"
#include ".\MainFrm.h"
#include "..\Lib\icDllChain.h"
#include "PointExchangeDlg.h"
#include "IBAAgent.h"
#include "BossPwdDlg.h"
#include "cardderegisterdlg.h"
#include "cardopenconsumedlg.h"
#include "memberinfodlg.h"
#include "mergememberdlg.h"
#include "NDBundleTime2Dlg.h"
#include "allcheckout.h"
#include "ActiveMemberView.h"
#include "NDCreditDlg.h"
#include "NDReturnDlg.h"
#include ".\jxc\JXCMainDlg.h"
#include "IdentityToolbarButton.h"
#include "NDCreditDirectDlg.h"
#include "DlgDutyLocal.h"
#include "IBAHelpper.h"
#include "DdnPcpInc.h"
#pragma comment(lib,"..\\lib\\DdnPcp.lib")
#include "Encrypt.h"
#include "NetBarConfig.h"
#include "NetbarRoom.h"
#include "Dal\RightsManagement.h"
#include "CashierHelper.h"
#include "ScanReco.h"
#include "Servlet\CashRegisterLoginMethod.h"
#include "Servlet\ServletMethod.h"
#include "RealName\JuBao.h"
#include "Servlet\GetSystemConfigMethod.h"
#include "Servlet\CommonParam.h"
#include "RealName\IZd.h"
#include "ThreadHelpper.h"
#include "ActiveMemberList.h"
#include "CurrentNetBarUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND CMainFrame::m_hOperationLog = NULL; //日志窗口句柄，其他线程用来显示日志

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMECHANGE()
	ON_WM_DESTROY()
	ON_WM_COPYDATA()
	ON_WM_TIMER()
	//ON_WM_NCPAINT()

	ON_MESSAGE(WM_CLIENT_DIANKA, OnClientDianka)
	ON_MESSAGE(WM_REPLYMESSAGE, OnReplyMessage)
	ON_MESSAGE(WM_REALNAME, OnRealName)
	ON_MESSAGE(WM_CHILDCLOSE, OnChildClose)
	ON_MESSAGE(WM_IREADER_CARDIN, OnCardIn)
	ON_MESSAGE(WM_EXECUTEREALNAME, OnExecuteRealName)

	ON_MESSAGE(WM_TOOLBAR_LABEL_TEXT, OnToolbarSetLabelText)
	ON_MESSAGE(WM_UPDATE_IBA_STATUS, OnUpdateIBAStatus)

	ON_MESSAGE(C_CANCLOSE, OnCanClose)

	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(BCGM_TOOLBARMENU, OnToolbarContextMenu)

	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_IBASET, OnIbaSet)
	ON_COMMAND(IDM_NETBARSET, OnNetbarSet)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_COMMAND(ID_VIEW_OUTLOOKBAR, OnViewOutlookBar)
	ON_COMMAND(ID_VIEW_CAPTIONBAR, OnViewCaptionBar)
	ON_COMMAND(IDM_FASTLANE, OnViewFastLane)

	ON_COMMAND(IDM_CALC, OnCalc)
	ON_COMMAND(IDM_NOTEPAD, OnNotepad)
	ON_COMMAND(IDM_LOGPATH, OnLogPath)

	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnUpdateAppLook)

	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTIONBAR, OnUpdateViewCaptionBar)
	ON_UPDATE_COMMAND_UI(IDM_FASTLANE, OnUpdateViewFastLane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTLOOKBAR, OnUpdateViewOutlookBar)

	ON_UPDATE_COMMAND_UI(IDM_NEWUSER, OnUpdateNewUser)
	ON_UPDATE_COMMAND_UI(IDM_MODIFYPWD, OnUpdateModifyPwd)
	ON_UPDATE_COMMAND_UI(IDM_MODIFYUSERINFO, OnUpdateModifyUserinfo)
	ON_UPDATE_COMMAND_UI(IDM_CREDIT, OnUpdateCredit)
	ON_UPDATE_COMMAND_UI(IDM_CREDITREVERSE, OnUpdateCreditReverse)
	ON_UPDATE_COMMAND_UI(IDM_UPGRADE, OnUpdateUpgrade)
	ON_UPDATE_COMMAND_UI(IDM_CARDCREDIT, OnUpdateCardCredit)
	ON_UPDATE_COMMAND_UI(IDM_CHANGESN, OnUpdateChangeSN)
	ON_UPDATE_COMMAND_UI(IDM_BOUNDTIME, OnUpdateBoundTime)
	ON_UPDATE_COMMAND_UI(IDM_CONSUMELIST, OnUpdateConsumeList)
	ON_UPDATE_COMMAND_UI(IDM_TERMCONSUME, OnUpdateTermConsume)
	ON_UPDATE_COMMAND_UI(IDM_OPERATION, OnUpdateOperation)
	ON_UPDATE_COMMAND_UI(IDM_SHIFTCASH, OnUpdateShiftCasht)
	ON_UPDATE_COMMAND_UI(IDM_IBASET, OnUpdateIbaSet)
	ON_UPDATE_COMMAND_UI(IDM_NETBARSET, OnUpdateNetbarSet)
	ON_UPDATE_COMMAND_UI(IDM_REALNAMESET, OnUpdateRealNameSet)
	ON_UPDATE_COMMAND_UI(IDM_CLIENTSET, OnUpdateNetbarSet)
	ON_UPDATE_COMMAND_UI(IDM_OTHERSET, OnUpdateNetbarSet)
	ON_UPDATE_COMMAND_UI(IDM_SELL, OnUpdateSell)
	ON_UPDATE_COMMAND_UI(IDM_POINTEXCHANGE, OnUpdatePointexchange)
	ON_UPDATE_COMMAND_UI(IDM_SEATOPERATION, OnUpdateSeatoperation)
	ON_UPDATE_COMMAND_UI(IDM_ALLLOG, OnUpdateAllLog)
	ON_UPDATE_COMMAND_UI(IDM_CREDITDIRECT,OnUpdateCreditDirect)
	ON_UPDATE_COMMAND_UI(ID_DUTY_OFF,OnUpdateOffDuty)

	ON_COMMAND(IDM_CREDIT, OnCredit)
	ON_COMMAND(IDM_NEWUSER, OnNewUser)
	ON_COMMAND(ID_DUTY_OFF, OnDutyOff)
	ON_COMMAND(IDM_CREDITREVERSE, OnCreditReverse)
	ON_COMMAND(IDM_MODIFYPWD, OnModifyPwd)
	ON_COMMAND(IDM_MODIFYUSERINFO, OnModifyUserinfo)
	ON_COMMAND(IDM_BOUNDTIME, OnBoundTime)
	ON_COMMAND(IDM_CHANGESN, OnChangeSN)
	ON_COMMAND(IDM_CONSUMELIST, OnConsumeList)
	ON_COMMAND(IDM_TERMCONSUME, OnTermConsume)
	ON_COMMAND(IDM_UPGRADE, OnUpgrade)
	ON_COMMAND(IDM_CARDCREDIT, OnCardCredit)
	ON_COMMAND(IDM_SHIFTCASH, OnShiftCash)
	ON_COMMAND(IDM_OPERATION, OnOperation)
	ON_COMMAND(IDM_CLIENTSET, OnClientSet)
	ON_COMMAND(IDM_SELL, OnSell)
	ON_COMMAND(IDM_POINTEXCHANGE, OnPointExchange)
	ON_COMMAND(IDM_AGENT, OnAgent)
	ON_COMMAND(IDM_SEATOPERATION, OnSeatoperation)
	ON_COMMAND(IDM_SERVERWEB, OnServerweb)
	ON_COMMAND(IDM_SHORTMSGQUERY, OnShortmsgquery)
	ON_COMMAND(IDM_CREDITDIRECT,OnCreditDirect)
	
	ON_COMMAND(IDM_CARDOPENCONSUME, OnCardopenconsume)
	ON_UPDATE_COMMAND_UI(IDM_CARDOPENCONSUME, OnUpdateCardopenconsume)
	ON_COMMAND(IDM_ALLLOG, OnAllLog)
	ON_COMMAND(IDM_REALNAMESET, OnRealnameSet)

	ON_COMMAND(IDM_RN_OPENCARD, OnRnOpencard)
	ON_COMMAND(IDM_RN_LOSE, OnRnLose)
	ON_COMMAND(IDM_RN_MODIFYPWD, OnRnModifypwd)
	ON_COMMAND(IDM_RN_ADDMEMBER, OnRnAddmember)
	ON_COMMAND(IDM_RN_DEREG, OnCardderegister)

	ON_COMMAND_RANGE(IDM_SCAN1, IDM_READ2, OnScan)
	ON_COMMAND_RANGE(IDM_SCAN_PASSPORT, IDM_SCAN_PASSPORT, OnScan)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SCAN1, IDM_READ2, OnUpdateScan)
	ON_UPDATE_COMMAND_UI(IDM_SCAN_PASSPORT, OnUpdateScan)
	ON_UPDATE_COMMAND_UI(IDM_RN_DEREG, OnUpdateCardDeregister)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_RN_OPENCARD, IDM_RN_ADDMEMBER, OnUpdateRealName)

	ON_COMMAND(IDM_MERGEMEMBER, OnMergemember)
	ON_UPDATE_COMMAND_UI(IDM_MERGEMEMBER, OnUpdateMergemember)
	ON_COMMAND(IDM_MUTICHECKOUT, OnMuticheckout)
	ON_UPDATE_COMMAND_UI(IDM_MUTICHECKOUT, OnUpdateMuticheckout)
	ON_COMMAND(IDM_PRINTTEMPLATE, OnPrintTemplate)
	ON_COMMAND(IDM_SPEECHTEMPLATE, OnSpeechTemplate)
	ON_COMMAND(IDM_BOUNDTIME2, OnBoundtime2)
	ON_UPDATE_COMMAND_UI(IDM_BOUNDTIME2, OnUpdateBoundtime2)
	ON_UPDATE_COMMAND_UI(IDM_DIANKA, OnUpdateDianka)

	ON_COMMAND(ID_ZENGSHOUBAO, OnZengshoubao)
	ON_COMMAND(IDM_DIANKA, OnDianKa)
	ON_COMMAND(ID_LABEL, OnLabelClick)

	ON_COMMAND(IDM_ROOM, OnRoom)
	ON_UPDATE_COMMAND_UI(IDM_ROOM, OnUpdateRoom)
	ON_COMMAND(ID_LOCK_SCREEN, OnLockScreen)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_COMMAND(IDM_READ_CARD, OnReadCard)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_2007_1);

	m_bDiankaSellCash = FALSE;
	m_pJuBao = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnAppLook(m_nAppLook);

	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	if (!CreateCommonBar())
	{
		return -1;
	}

	EnableDocking(CBRS_ALIGN_LEFT);

	if (!CreateTaskPane())
	{
		return -1;
	}

	if (!CreateOutPut())
	{
		return -1;
	}

	EnableAutoHideBars(CBRS_ALIGN_LEFT);

	if (!RegisterHotKey(m_hWnd, 1, MOD_CONTROL, 'Q'))
	{
		MessageBox(_T("注册热键Ctrl+Atl+Q失败"), _T("错误"),MB_OK|MB_ICONWARNING);
	}
	
#ifdef DEBUG
	SetTimer(1006, 60000, NULL);
#else
	SetTimer(1006, 3600*1000, NULL);
#endif
	
	// 2014-6-13 - qsc
	if(CIBAGlobal::emRealnameZD == CNetBarConfig::GetInstance()->GetAuditType())
	{
		// 2014-10-11 - qsc
		// m_pThreadProxy->Start();
		CIZd::GetInstance()->Load();

		CRegKey RegKey;
		ULONG uLen = 256;

		if(ERROR_SUCCESS == RegKey.Create(HKEY_CURRENT_USER, 
			_T("Software\\iKServer\\Console")))
		{
			RegKey.SetDWORDValue(_T("ManageWnd"), (DWORD)GetSafeHwnd());
		}
		else
		{
			IBA_LOG0(_T("智多实名-写入句柄失败！"));
		}
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_OVERLAPPEDWINDOW;

	BOOL bRet = CBCGPFrameWnd::PreCreateWindow(cs) ;

	WNDCLASS wc = {0};   

	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wc);   
	wc.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);  

	//一个特殊的类名，不要修改，其他程序依赖此类名来查找本窗口进行通讯
	wc.lpszClassName = _T("IBA_MAINWINDOW_CLASS");  

	AfxRegisterClass(&wc);   

	cs.lpszClass = wc.lpszClassName;   
	theApp.m_nCmdShow = SW_HIDE; //用于隐藏主窗口
	
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */);

	pDlgCust->Create ();
}

LRESULT CMainFrame::OnToolbarContextMenu(WPARAM, LPARAM lp)
{
	//CPoint point (BCG_GET_X_LPARAM(lp), BCG_GET_Y_LPARAM(lp));

	//CMenu menu;
	//VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

	//CMenu* pPopup = menu.GetSubMenu(0);
	//ASSERT(pPopup != NULL);

	//CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	//pPopupMenu->Create (this, point.x, point.y, pPopup->Detach ());

	return 0;
}

LRESULT CMainFrame::OnToolbarReset(WPARAM wp, LPARAM)
{
	UINT uiToolBarId = (UINT)wp;

	if (IDR_FASTDLG == uiToolBarId)
	{
		m_wndFastLane.ReplaceButtons();	
	}
	else if (IDR_MAINFRAME == uiToolBarId )
	{
		CIdentityToolbarButton btnLabel(ID_LABEL,&m_wndToolBar);
		btnLabel.SetCtrlWindowText(m_strLastIdInToolbar);
		int i = m_wndToolBar.ReplaceButton(ID_LABEL,btnLabel);

#ifndef __USING_INTER_RESOURCE

		const UINT nSize = 11;
		UINT nCmds[nSize] = {
			ID_DUTY_OFF,
			IDM_MYREFRESH,            // 刷新(F1)
			IDM_NEWUSER,              // 开户(F2)
			IDM_CREDIT,               // 充值(F3)
			IDM_CREDITREVERSE,        // 充值修正(F4)
			IDM_CARDCREDIT,           // 充值卡充值(F5)
			IDM_MODIFYPWD,            // 修改密码(F7)
			IDM_BOUNDTIME,            // 包时(F10)
			IDM_SELL,                 // 商品销售(F12)
			ID_ZENGSHOUBAO,
			IDM_DIANKA                // 点卡销售(F11)
		};
		TCHAR *pAccclerators[nSize] = {
			_T(""),
			_T("(F1)"),
			_T("(F2)"),
			_T("(F3)"),
			_T("(F4)"),
			_T("(F5)"),
			_T("(F7)"),
			_T("(F10)"),
			_T("(F12)"),	
			_T(""),
			_T("(F11)")
		};
		for(UINT n = 0; n < nSize; n++)
		{
			int nIndex = m_wndToolBar.CommandToIndex(nCmds[n]);
			if (nIndex < 0)
			{
				continue;
			}
			CString strTxt = LOAD_STRING(nCmds[n]);
			if (strTxt == _T("Unknown string ID"))
			{
				continue;
			}
			strTxt.TrimLeft(_T('\n'));
			int nRes = strTxt.ReverseFind(_T('('));
			if (nRes > 0)
			{
				strTxt = strTxt.Left(nRes);
			}

			strTxt += pAccclerators[n];
			m_wndToolBar.SetButtonText(nIndex, strTxt);	
		}
		
		CString strMainCaption = LOAD_STRING(IDS_MAIN_MENU_CAPTION);
		CStringArray strArray;
		CIBAHelpper::SplitLine(strMainCaption,strArray);
		ASSERT(strArray.GetCount() == 7);
	
		for(int n = 0; n < strArray.GetCount(); n++)
		{
			m_wndMenuBar.SetButtonText(n, strArray.GetAt(n));	
		}

#endif
	
		return 0;
	}

	return 0;
}

BOOL CMainFrame::OnShowPopupMenu(CBCGPPopupMenu* pMenuPopup)
{
	//---------------------------------------------------------
	// Replace ID_VIEW_TOOLBARS menu item to the toolbars list:
	//---------------------------------------------------------
	// return CBCGPFrameWnd::OnShowPopupMenu (pMenuPopup);

	if (pMenuPopup != NULL &&
		pMenuPopup->GetMenuBar()->CommandToIndex(ID_VIEW_TOOLBARS) >= 0)
	{
		if (CBCGPToolBar::IsCustomizeMode ())
		{
			//----------------------------------------------------
			// Don't show toolbars list in the cuztomization mode!
			//----------------------------------------------------
			return FALSE;
		}

		pMenuPopup->RemoveAllItems();

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_TOOLBAR));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		pMenuPopup->GetMenuBar()->ImportFromMenu(*pPopup, TRUE);
	}

	return TRUE;
}

void CMainFrame::OnViewOutlookBar() 
{
	ShowControlBar(&m_wndTaskPane, !(m_wndTaskPane.IsVisible ()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewOutlookBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndTaskPane.IsVisible());
}

void CMainFrame::OnViewCaptionBar() 
{
	ShowControlBar(&m_wndOutput, !(m_wndOutput.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
}

void CMainFrame::OnViewFastLane() 
{
	ShowControlBar(&m_wndFastLane, !(m_wndFastLane.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewFastLane(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndFastLane.IsVisible());
}

void CMainFrame::ChangeSkin(UINT nSkinId)
{
	CBCGPDockManager::SetDockMode(BCGP_DT_SMART);

	switch (nSkinId)
	{
	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 Look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPVisualManager::GetInstance ();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2008 Look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPVisualManager::GetInstance ();
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_1:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_2:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007_4:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	default:
		ASSERT (FALSE);
	}
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode(BCGP_DT_SMART);

	m_nAppLook = id;
	
	theApp.WriteInt(_T("ApplicationLook"), m_nAppLook);

	ChangeSkin(id);

	CBCGPDockManager* pDockManager = GetDockManager();
	if (pDockManager != NULL)
	{
		ASSERT_VALID(pDockManager);
		pDockManager->AdjustBarFrames();
	}

	RecalcLayout();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

	CJXCMainDlg::AutoChangeSkin();
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::CreateTaskPane()
{
	CIBAString strTmp;
	strTmp.LoadString(IDS_STANDARD);

	CRect Rect;
	GetClientRect(Rect);

	CRect rectDummy(0, 0, 200, Rect.bottom);
	if (!m_wndTaskPane.Create(strTmp, this, rectDummy, FALSE, 104,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT,CBRS_BCGP_REGULAR_TABS, CBRS_BCGP_RESIZE))
	{
		TRACE0("Failed to create task pane\n");
		return FALSE;      // fail to create
	}

	m_wndTaskPane.EnableDocking(CBRS_ALIGN_LEFT);
	DockControlBar(&m_wndTaskPane);

	return TRUE;
}

BOOL CMainFrame::CreateOutPut()
{
	if (!m_wndOutput.Create(_T(""), this, CSize(150, 120),
		FALSE /* Has gripper */, 123,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,CBRS_BCGP_REGULAR_TABS, CBRS_BCGP_RESIZE))
	{
		TRACE0("Failed to create output bar\n");
		return FALSE;      // fail to create
	}

	m_hOperationLog = m_wndOutput.GetSafeHwnd();

	m_wndOutput.EnableDocking(CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndOutput);

	{
		if (!m_wndSeatStatusBar.Create(_T(""), this, CSize(150, 120),
			FALSE /* Has gripper */, 456,
			WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,CBRS_BCGP_REGULAR_TABS, CBRS_BCGP_RESIZE))
		{
			TRACE0("Failed to create output bar\n");
			return FALSE;      // fail to create
		}

		/*m_hOperationLog = m_wndOutput.GetSafeHwnd();*/

		m_wndSeatStatusBar.EnableDocking(CBRS_ALIGN_BOTTOM);
		DockControlBar(&m_wndSeatStatusBar);
	}
	return TRUE;
}

BOOL CMainFrame::CreateCommonBar()
{
	CBCGPToolBar::EnableQuickCustomization();

	if (!m_wndMenuBar.CreateMenuBar(this))
	{
		TRACE0("Failed to create menubar\n");
		return FALSE;      // fail to create
	}

	DockControlBar(&m_wndMenuBar);

	//*******************************************************************************

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_HIDE_INPLACE, 
		CRect(1, 1, 1, 1), AFX_IDW_TOOLBAR+1) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, IDB_TOOLBAR256))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;      // fail to create
	}

	SIZE sizeImage = {32, 32};

	CBCGPToolBar::GetLargeImages()->Load(IDB_TOOLBARLARGE);
	CBCGPToolBar::GetLargeImages()->SetImageSize(sizeImage, TRUE);
	CBCGPToolBar::SetLargeIcons(TRUE);

	CIBAString strTmp;
	strTmp.LoadString(IDS_STANDARD);
	m_wndToolBar.SetWindowText(strTmp);

	strTmp.LoadString(IDS_COSTOM);
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strTmp);
	m_wndToolBar.EnableTextLabels();

	DockControlBar(&m_wndToolBar);

	//*******************************************************************************

	if (!m_wndFastLane.CreateFastLane(this))
	{
		return FALSE;
	}

	DockControlBar(&m_wndFastLane);


	//*******************************************************************************

	UINT indicators[] = {0, 0, 0};

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;      // fail to create
	}

	//m_wndStatusBar.SetPaneStyle(1, SBPS_STRETCH | SBPS_NOBORDERS);
	//m_wndStatusBar.EnablePaneDoubleClick();

	return TRUE;
}

void CMainFrame::OnCalc()
{
	ShellExecute(GetSafeHwnd(), _T("open"), _T("calc.exe"), NULL, NULL, SW_NORMAL);
}

void CMainFrame::OnNotepad()
{
	ShellExecute(GetSafeHwnd(), _T("open"), _T("notepad.exe"), NULL, NULL, SW_NORMAL);
}

void CMainFrame::OnLogPath()
{
	ShellExecute(GetSafeHwnd(), _T("open"), theApp.GetWorkPath() + _T("\\IBALog"), NULL, NULL, SW_NORMAL);
}

void CMainFrame::OnPrintTemplate()
{
	ShellExecute(GetSafeHwnd(), _T("open"), theApp.GetWorkPath() + _T("\\IBAConfig\\PrintTemplate"), NULL, NULL, SW_NORMAL);
}

void CMainFrame::OnSpeechTemplate()
{
	ShellExecute(GetSafeHwnd(), _T("open"), theApp.GetWorkPath() + _T("\\IBAConfig\\IBATTS.ini"), NULL, NULL, SW_NORMAL);
}

BOOL CMainFrame::OnMenuButtonToolHitTest(CBCGPToolbarButton* pButton, TOOLINFO* pTI)
{
	ASSERT_VALID (pButton);
	ASSERT (pTI != NULL);

	CString strText = pButton->m_strText;
	strText.Remove(_T('&'));

	if (strText.IsEmpty())
	{
		return FALSE;
	}

	pTI->lpszText = (LPTSTR)::calloc((strText.GetLength() + 1), sizeof(TCHAR));
	lstrcpy(pTI->lpszText, strText);

	return TRUE;
}

LRESULT CMainFrame::OnReplyMessage(WPARAM nID, LPARAM)
{
	CShortMsgDlg dlg;

	if (nID >= 10000)
	{
		dlg.SetSendType(2);
	}
	else
	{
		dlg.AddReceiver((UINT)nID);
		dlg.SetSendType(1);
	}

	dlg.DoModal();

	return 0;
}

void CMainFrame::OnUpdateCredit(CCmdUI *pCmdUI)
{
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	CRightsManagement::GetInstance()->SetUI_IBAP_CREDIT(pCmdUI);
}

void CMainFrame::OnCredit()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_CREDIT())
	{
		CCreditDlg dlg;
		dlg.SetNetId();
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateNewUser(CCmdUI *pCmdUI)
{
	CRightsManagement::GetInstance()->SetUI_IBAP_REGISTER(pCmdUI);

	if (CNetBarConfig::GetInstance()->GetMonitorAuditProxy())
	{
		if (!theApp.GetCurCashier()->m_bAuditProxyRunning)
		{
			pCmdUI->Enable(FALSE);
		}
	}

	//2015-0828 liyajun
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnNewUser()
{
	//2015-0911 liyajun 禁止网络断开后，而状态还没变时进行开户
	//if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)
	//{
	//	MessageBox(_T("网络已断开，无法开户！"));
	//	return;
	//}
	//if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
	//{
		CGetTimeMethod time;
		time.Execute();

		if (time.GetStatusCode() < 0)
		{
			MessageBox(_T("网络已断开，无法开户！"));
			IBA_TRACE(_T("开户检测网络-网络已断开，禁止开户！"));
			return;
		}
	//}
	if (CRightsManagement::GetInstance()->Take_IBAP_REGISTER())
	{
		CRegisterDlg dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnAppAbout()
{
	CIBAAboutDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnDutyOff()
{
#ifndef _DEBUG
	if (!CRightsManagement::GetInstance()->Take_IBAP_ONOFFDUTY(FALSE))
	{
		return;
	}
#endif
	
	//CString sql;
	//sql.Format(_T("select count(*) from localMember"));
	//if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0)
	//{
	//	MessageBox(_T("禁止交班操作！本地模式下数据正在上报中心..."));
	//	return;
	//}
	
	BOOL bNormal = TRUE;
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkRestore)
	{
		bNormal = FALSE;
	}
	else
	{
		CArray<CLocalConsumeInfo,CLocalConsumeInfo&> InfoArray; 
		CIBADAL::GetInstance()->GetLocalConsumes(InfoArray);
		if (InfoArray.GetCount() > 0)
		{
			bNormal = FALSE;
		}
	}

	if (theApp.GetCurCashier()->IsOnDuty() == FALSE)
	{
		bNormal = TRUE; // 能弹出上班界面
	}
	
	//}

	if (bNormal)
	{
		CDlgIBADuty dlg;

		if (theApp.GetCurCashier()->IsOnDuty())//是否在上班
		{
			if (dlg.DoModal() == IDOK)
			{
				//{ 2011/04/19-gxx: 判断在交班的时候,IBA是否有更新
				theApp.UpdateIBA();
				//}

				CJXCMainDlg::DestroyJxcMainDlg();//销毁非模态的进销存对话框

				UpdateStatusText();//更新状态栏

				CIBALoginDlg dlg2;//登入对话框
				dlg2.SetLoginType(1);//调整位置，不对原顶层窗体做要求

				if (dlg2.DoModal() == IDOK)
				{
					UpdateStatusText();//更新状态栏
				}
				else
				{
					CBCGPFrameWnd::OnClose();//退出
				}
			}
		}
		else
		{
			dlg.DoModal();
			UpdateStatusText();
		}
	}
	else
	{
		CDlgDutyLocal dlg;
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateCreditReverse(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		//断网下，禁用充值修正功能
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_CREDITREVERSE(pCmdUI);
	}
	
}

void CMainFrame::OnCreditReverse()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_CREDITREVERSE())
	{
		CCreditReverseDlg dlg;
		dlg.SetNetId();
		dlg.DoModal();
	} 
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CBCGPFrameWnd::OnSize(nType, cx, cy);

	//设置状态栏的位置和大小
	if (m_wndStatusBar.GetSafeHwnd() != NULL)
	{
		m_wndStatusBar.SetPaneWidth(0, 0); //空的， 不显示
		m_wndStatusBar.SetPaneWidth(2, 250);
		//m_wndStatusBar.SetPaneWidth(3, 170);

		int nTmp = cx - 251;

		if (nTmp > 0)
		{
			m_wndStatusBar.SetPaneWidth(1, nTmp);
		}
	}
}

void CMainFrame::UpdateStatusText(CString strStatInfo)
{//统计数据

	UNIT_AUTO_TRACE("CMainFrame::UpdateStatusText(CString strStatInfo)");

	if (!strStatInfo.IsEmpty())
	{
		m_wndStatusBar.SetPaneText(1, strStatInfo, TRUE);
		//2015-0824 liyajun 弹出提示框以显示整个内容
		//while(strStatInfo.ReverseFind(_T('\n')) + STATUSTOOLTIPWIDTH - 1 < strStatInfo.GetLength())
		//{
		//	strStatInfo.Insert(strStatInfo.ReverseFind(_T('\n')) + STATUSTOOLTIPWIDTH + 1 ,_T("\r\n"));
		//}
		m_wndStatusBar.SetTipText(1,strStatInfo);
	}

	CString strTmp;

	//当前时间
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		strTmp.Format(LOAD_STRING(IDS_ONDUTY), theApp.GetCurCashier()->GetName(),
			theApp.GetCurCashier()->GetOnDutyTime());
	}
	else
	{
		strTmp.Format(LOAD_STRING(IDS_NOTONDUTY), theApp.GetCurCashier()->GetName());
	}

	m_wndStatusBar.SetPaneText(2, strTmp, TRUE);
}

void CMainFrame::OnClose()
{
	if (IsIconic())
	{
		OpenIcon();
	}

	CString strInfo;
	strInfo.Format(LOAD_STRING(IDS_EXITQUESTION), theApp.GetCurCashier()->GetName());

	CMsgBoxDlg dlg;
	dlg.SetText(strInfo);
	dlg.SetType(MB_OKCANCEL | MB_ICONQUESTION);

	if (dlg.DoModal() == IDOK)
	{
		CIBALog::GetInstance()->Write(_T("Goodbye, IBA!"));

		theApp.GetRealName()->UnInitRealName();

		//theApp.GetIBAView()->SendMessage(WM_CHILDCLOSE, GetDlgCtrlID(), (LPARAM)theApp.GetIBAView()->m_hDodonewView);
		// 2014-3-26 - qsc 文通的退出的时候关闭设备
		if(9 == CIBAConfig::GetInstance()->GetScanId())
		{
			CloseDevice();
		}
		CBCGPFrameWnd::OnClose();
	}

	CCashierHelper::UninitDiankaPlatform();

	// 2015-0828 liyajun
	CRegKey regKey;
	LONG lRet = regKey.Open(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA"));

	if (ERROR_SUCCESS == lRet)
	{
		DWORD dwValue = 0;
		lRet = regKey.QueryDWORDValue(_T("IsFN"), *(DWORD*)&dwValue);
		if (ERROR_SUCCESS == lRet)
		{
			regKey.DeleteValue(_T("IsFN"));
			regKey.DeleteSubKey(_T("IsFN"));
		}

		regKey.Close();
	}

	//2015-0906 liyajun 因CSingleton类不执行析构函数，在此处进行设置保存
	if(CIBAConfig::GetInstance()->GetHasLoaded())
	{
		CIBAConfig::GetInstance()->SaveConfig();
	}

}

void CMainFrame::OnUpdateIbaSet(CCmdUI *pCmdUI)
{
	CRightsManagement::GetInstance()->SetUI_IBAP_CONFIG(pCmdUI, FALSE);
}

void CMainFrame::OnIbaSet()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_CONFIG(FALSE))
	{
		CSetOfIBAPropertySheet dlg;
		
		if (dlg.DoModal() == IDOK)
		{
			CIBAConfig::GetInstance()->SaveConfig();

			CLocalServer::GetInstance()->ComputerList.UpdateAllComputerStatus();
			CLocalServer::GetInstance()->GetComputerListView()->Refresh(TRUE);
			//CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
		}
	} 
}

void CMainFrame::OnUpdateNetbarSet(CCmdUI *pCmdUI)
{
	CRightsManagement::GetInstance()->SetUI_IBAP_NETBARSET(pCmdUI, FALSE);
}

void CMainFrame::OnNetbarSet()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_NETBARSET(FALSE))
	{
		CDlgNetbarConfig dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateModifyPwd(CCmdUI *pCmdUI)
{
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	CRightsManagement::GetInstance()->SetUI_IBAP_MODIFYPWD(pCmdUI);

	if (CNetBarConfig::GetInstance()->GetForbidModifyPwd() == 1) // 禁用修改密码
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnModifyPwd()
{
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		theApp.IBAMsgBox(_T("断网情况下,不允许修改密码"));
		return;
	}
	
	if (CRightsManagement::GetInstance()->Take_IBAP_MODIFYPWD())
	{
		CModifyPwdDlg dlg;
		dlg.SetNetId();
		dlg.DoModal();
	} 
}

void CMainFrame::OnCreditDirect()
{	
	if (CRightsManagement::GetInstance()->Take_IBAP_CREDITDIRECT())
	{
		CNDCreditDirectDlg dlg;
		dlg.SetNetId();
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateModifyUserinfo(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CRightsManagement::GetInstance()->SetUI_IBAP_MODIFYUSER(pCmdUI);

	if (CNetBarConfig::GetInstance()->GetNoModifyMember() == 1)
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnModifyUserinfo()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_MODIFYUSER())
	{
		CModifyMemberDlg dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateBoundTime(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_BUNDLETIME(pCmdUI);
	}
	
}

void CMainFrame::OnBoundTime()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_BUNDLETIME())
	{
		CDlgBundleTime dlg;
		dlg.SetNetId();
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateChangeSN(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CRightsManagement::GetInstance()->SetUI_IBAP_CHANGESN(pCmdUI);
}

void CMainFrame::OnChangeSN()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_CHANGESN())
	{
		CDlgChangeNetId dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateConsumeList(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable( CRightsManagement::GetInstance()->Has_IBAP_CONSUMELIST() );
}

void CMainFrame::OnConsumeList()
{
	theApp.GetIBAView()->ShowComsumeListView();
}

void CMainFrame::OnUpdateTermConsume(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(CRightsManagement::GetInstance()->Has_IBAP_TERMCONSUME());
}

void CMainFrame::OnTermConsume()
{
	theApp.GetIBAView()->ShowTermConsumeView();
}

void CMainFrame::OnUpdateUpgrade(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	
	// 2014-2-28 - qsc
	if(0 != CNetBarConfig::GetInstance()->GetCanNotUserUpgrade())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CRightsManagement::GetInstance()->SetUI_IBAP_UPGRADE(pCmdUI);
}

void CMainFrame::OnUpgrade()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_UPGRADE())
	{
		CNDUpgradeDlg dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnUpdateCardCredit(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		// 断网下，禁用充值卡充值功能
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_CARDCREDIT(pCmdUI);
	}
}

void CMainFrame::OnCardCredit()
{
	//2015-0828 liyajun
	if (CRightsManagement::GetInstance()->Take_IBAP_CARDCREDIT())
	{
		if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
		{
			CNDCardCreditDlg dlg;
			dlg.SetNetId();
			dlg.DoModal();
		}
		else 
		{
			CString strNetId = CCurrentNetBarUser::GetInstance()->GetNetId();
			CActiveMember ActiveMember;
			if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMemberNetID(strNetId, ActiveMember, TRUE))
			{

				CNDCardCreditDlg dlg;
				//dlg.SetLocalActiveMember(ActiveMember);
				dlg.SetNetId(strNetId);
				dlg.DoModal();
			}
		}
	} 
}

void CMainFrame::OnUpdateShiftCasht(CCmdUI *pCmdUI)
{
	//BOOL bFlag = TRUE;
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		//bFlag = FALSE;
		return;
	}

	//bFlag = CRightsManagement::GetInstance()->Has_IBAP_CASHSHIFTQUERY();
	//pCmdUI->Enable(bFlag);
	pCmdUI->Enable(CRightsManagement::GetInstance()->Has_IBAP_CASHSHIFTQUERY());

	if (!CIBAConfig::GetInstance()->GetShowShiftTotalInfo())
	{
		pCmdUI->Enable(FALSE);
		//bFlag = FALSE;
	}

	//if(!bFlag)
	//{
	//	// 不能使用就删除"收银员交款查询"这个子菜单
	//	CMenu *pMenu = CMenu::FromHandle ( m_wndMenuBar.GetHMenu() );
	//	if(pMenu)
	//	{
	//		CMenu *pSubMenu = pMenu->GetSubMenu (2);
	//		if(pSubMenu)
	//		{
	//			pSubMenu->RemoveMenu(IDM_SHIFTCASH, MF_BYCOMMAND);
	//		}
	//	}
	//}
}

void CMainFrame::OnShiftCash()
{
	theApp.GetIBAView()->ShowShiftCashView();
}

void CMainFrame::OnUpdateOperation(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(CRightsManagement::GetInstance()->Has_IBAP_OPERATIONQUERY());
}

void CMainFrame::OnOperation()
{
	theApp.GetIBAView()->ShowOperationView();
}

LRESULT CMainFrame::OnRealName(WPARAM wp, LPARAM lp)
{
	IBA_LOG0(_T("刷卡..."));

	// 2011/12/08-8233-gxx: 8209版本的特殊控制,需要注释下面的代码

#ifndef OPEN_GLW_CARD_TESTING
	if (lp != 100 && CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameGLW) 
	{
		// 如果是过滤王实名 并且不是proxy 发过来的
		IBA_LOG0(_T("无效刷卡信息!"));
		return -1;
	}
#endif
	//

	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		CString strTmp;

		strTmp.Format(IDS_PLEASEONDUTYFISRT, theApp.GetCurCashier()->GetName());

		theApp.IBAMsgBox(strTmp);

		return -1;
	}

	if (999 == wp)//通用模拟刷卡
	{
		return theApp.GetRealName()->OnRealNameMsg(999);
	}

	return theApp.GetRealName()->OnRealNameMsg(0);
}

void CMainFrame::OnRnOpencard()
{
	theApp.GetRealName()->InvokeRealName(1);
}

void CMainFrame::OnRnLose()
{
	theApp.GetRealName()->InvokeRealName(2);
}

void CMainFrame::OnRnModifypwd()
{
	theApp.GetRealName()->InvokeRealName(3);
}

void CMainFrame::OnRnAddmember()
{
	theApp.GetRealName()->InvokeRealName(4);
}

void CMainFrame::OnRealnameSet()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_NETBARSET(FALSE))
	{
		CConfigOfRealName dlg;
		dlg.DoModal();
	} 
}

void CMainFrame::OnClientSet()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_NETBARSET(FALSE))
	{
		CClientConfigPropertySheet dlg;
		dlg.DoModal();
	}
}

void CMainFrame::OnTimeChange()
{
	CBCGPFrameWnd::OnTimeChange();

	CIBALog::GetInstance()->Write(_T("CMainFrame::OnTimeChange"));
}

void CMainFrame::OnDestroy()
{
	CJXCMainDlg::DestroyJxcMainDlg();

	CBCGPFrameWnd::OnDestroy();
}

void CMainFrame::OnUpdateSell(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_RETAIL(pCmdUI);
	}
}

void CMainFrame::OnSell()
{
	if (!CRightsManagement::GetInstance()->Take_IBAP_RETAIL())
	{
		return;
	}

	CJXCMainDlg::ShowJxcMainDlg();
}

LRESULT CMainFrame::OnChildClose(WPARAM wp, LPARAM)
{

	EmptyWorkingSet(GetCurrentProcess);

	return 0;
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CIBALog::GetInstance()->Write(_T("CMainFrame::OnCopyData"));

	if(CIBAGlobal::emRealnameWD ==  CNetBarConfig::GetInstance()->GetAuditType())
	{
		if (pCopyDataStruct->dwData == 0) //网盾刷卡
		{
			IBA_LOG0(_T("Enter 网盾刷卡..."));

			if (theApp.GetCurCashier()->IsOnDuty())
			{
				theApp.GetRealName()->OnRealNameMsg(0, pCopyDataStruct->lpData, pCopyDataStruct->cbData);
			}
			else
			{
				CString strTmp;

				strTmp.Format(IDS_PLEASEONDUTYFISRT, theApp.GetCurCashier()->GetName());

				theApp.IBAMsgBox(strTmp);
			}

			IBA_LOG0(_T("Leave 网盾刷卡"));
		}
	}
	else if(CIBAGlobal::emRealnameZD ==  CNetBarConfig::GetInstance()->GetAuditType())
	{
		IBA_LOG(_T("Enter 智多CopyData... %d"), pCopyDataStruct->dwData - (IK_BASE)); // 2014-8-28 - qsc
		
		if (!theApp.GetCurCashier()->IsOnDuty())
		{
			CString strTmp;

			strTmp.Format(IDS_PLEASEONDUTYFISRT, theApp.GetCurCashier()->GetName());

			theApp.IBAMsgBox(strTmp);
		}
		
		theApp.GetRealName()->OnRealNameMsg(CIBAGlobal::emRealnameZD, pCopyDataStruct, sizeof(COPYDATASTRUCT));

		IBA_LOG0(_T("Leave 智多CopyData"));
	}
	

	return CBCGPFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CMainFrame::OnCardIn(WPARAM wp , LPARAM)
{
	if (!theApp.GetCurCashier()->IsOnDuty())
	{
		return -1;
	}

	theApp.GetRealName()->OnCardIn();

	return 0;
}

void CMainFrame::StartIBA()
{
	UNIT_AUTO_TRACE("CMainFrame::StartIBA()--");

	CIBALog::GetInstance()->Write(_T("StartIBA"));
	
	if (0 == CNetBarConfig::GetInstance()->GetIsShowRzxPhoto())
	{
		m_wndTaskPane.RemoveGroup(2);
		m_wndTaskPane.m_dlgUserPhoto.ShowWindow(SW_HIDE);
	}
	

	// 2011/05/17-gxx: 例如账户直充的控制配置是在Menubar创建后读取的，添加代码让菜单重新设置
	m_wndMenuBar.ResetAll();

	// 2015-3-5-tmelody, 登录成功设置需上报标识
	// 等待LocalGetComputerList回调成功后进行上报
	CLocalServer::GetInstance()->SetNeedReportPCInfo(TRUE);

	CLocalServer::GetInstance()->ConnectServer();
	CLocalServer::GetInstance()->ActiveMemberList.LoadFromDB();
	CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(TRUE);
	

	CString strTmp;
	strTmp = LOAD_STRING(IDS_IBATITLE);
	strTmp.Insert(0, _T(" - "));
	strTmp.Insert(0, CNetBarConfig::GetInstance()->GetNetBarName());

	CString strTel(_T("  (24小时售后客服：4006683755)"));

	SetWindowText(strTmp+strTel);

	ShowWindow(SW_SHOWMAXIMIZED);
	UpdateWindow();
	UpdateStatusText();

	EmptyWorkingSet(GetCurrentProcess);

	//初始化实名系统
	if (!theApp.GetRealName()->InitRealName())
	{
		theApp.IBAMsgBox(_T("初始化实名认证失败"));
		CBCGPFrameWnd::OnClose();

	}
	else
	{
		SetTimer(1000, 1000 * 10, NULL); //10秒一次

		// 2011/05/05-gxx: 开启获取滚动文字
		m_wndFastLane.StartThread();
	}

	CJuBao* pTmp = NULL;

	// 2013-9-11-qscadd:聚宝接口初始化
	if(1 == CNetBarConfig::GetInstance()->GetEnableJuBao())
	{
		if(!m_pJuBao)
		{
			m_pJuBao = new CJuBao;
		}
		if(!m_pJuBao->RegCallBackFunc())
		{
			AfxMessageBox(_T("聚宝注册初始化失败！"));
			IBA_LOG0(_T("注册聚宝回调函数失败！"));
		}
		if(!m_pJuBao->InitInterface("testxzg001","610","sntestxzg001"))
		{
			AfxMessageBox(_T("聚宝初始化失败！"));
			IBA_LOG0(_T("聚宝初始化失败！"));
		}
		pTmp = GetJubaoInf();
	}

	if (CNetBarConfig::GetInstance()->GetEnableID2Reader())
	{
		HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("_ID2READER_ALLOW_MUTEX_"));
		if(hMutex == NULL)
			return;

		if(GetLastError() != ERROR_ALREADY_EXISTS)
		{
			WinExec(".\\IBABin\\ID2Reader.exe", SW_HIDE);
		}
		else
		{
			CloseHandle(hMutex);
		}
	}
	else
	{
		// 空白照片删除 QSC 2013-5-7 10:46:16
		//m_wndTaskPane.RemoveGroup(2);
		//m_wndTaskPane.m_dlgUserPhoto.ShowWindow(SW_HIDE);
	}
	OnStartIBAEnd();
}

void CMainFrame::OnStartIBAEnd()
{
	if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken)
	{
		CThreadHelpper::StartThead((LPTHREAD_START_ROUTINE)CThreadHelpper::GetSysConfig);	
	}
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	CBCGPFrameWnd::OnTimer(nIDEvent); //这个放前面

	if (nIDEvent == 1001)
	{
		//初始化实名系统
		if (!theApp.GetRealName()->InitRealName())
		{
			theApp.IBAMsgBox(_T("初始化实名认证失败"));
			CBCGPFrameWnd::OnClose();

		}
		else
		{
			SetTimer(1000, 1000 * 10, NULL); //10秒一次

			// 2011/05/05-gxx: 开启获取滚动文字
			m_wndFastLane.StartThread();
		}
		KillTimer(1001);
	}
	else if(nIDEvent == 1000)
	{
		if (!theApp.GetRealName()->CheckRealName()) //检查实名是否正常
		{
			theApp.GetRealName()->UnInitRealName();

			CBCGPFrameWnd::OnClose();
		}
	}
	else if(nIDEvent == 1005)
	{
		// 主收银台才做这样的判断
		BOOL bAuditProxyRunning = TRUE;
		HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("_SUNLIKEAUDITPROXY_MUTEX_"));
		if (hMutex != NULL)
		{
			CloseHandle(hMutex);
			CString strProxy(theApp.GetWorkPath());
			strProxy.Append(_T("\\IBABin\\SunlikeAuditProxy.exe"));

			CL2Process SunlikeAuditProxy;
			SunlikeAuditProxy.Attach(strProxy);
			if (!SunlikeAuditProxy.IsRunning())
			{
				bAuditProxyRunning = FALSE;
			}
		}
		else
		{
			bAuditProxyRunning = FALSE;
		}
		if (bAuditProxyRunning == FALSE)
		{
			CNetBarConfig::GetInstance()->SetAllowOpenUser(FALSE);
		}
		else
		{
			CNetBarConfig::GetInstance()->SetAllowOpenUser(TRUE);
		}
	}
	else if(nIDEvent == 1006)
	{
#ifdef DEBUG
		if (GetTickCount() - CIBAGlobal::s_dwStartUpTickcount >= 60*60*1000)
#else
		if (GetTickCount() - CIBAGlobal::s_dwStartUpTickcount >= 24*3600*1000)
#endif
		
		{ //2015-0828 liyajun
			if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken) {
				return ;
			}

			CCashRegisterLoginMethod CashRegisterLogin;
			BOOL bRet = FALSE;

			CashRegisterLogin.SetCashRegisterId(CIBAConfig::GetInstance()->GetCashRegisterID());
			CashRegisterLogin.SetCashRegisterVersion(CIBALog::GetInstance()->GetAppVer());
			CashRegisterLogin.SetCashRegisterType(1);//普通模式，需要验证，无本地数据库
			CashRegisterLogin.SetUserId(theApp.GetCurCashier()->GetName());
			CashRegisterLogin.SetPassword(theApp.GetCurCashier()->GetPassword());

			time_t tm = time(NULL);
			CString strOrg;
			strOrg.Format(_T("%ld"), tm);

			CIBALog::GetInstance()->WriteFormat(_T("时间戳:%d"), tm);
			CIBALog::GetInstance()->WriteFormat(_T("%s"), theApp.GetCurCashier()->GetPassword());

			CIBALog::GetInstance()->WriteFormat(_T("fileHash:%s"), CIBALog::GetInstance()->GetFileHash());

			CString strTmp;
			CEncrypt::CalcMD5(theApp.GetCurCashier()->GetPassword(), strTmp);

			CIBALog::GetInstance()->WriteFormat(_T("%s"), strTmp);

			strOrg += CIBALog::GetInstance()->GetFileHash();
			strOrg += strTmp.Left(16);
			strOrg += strTmp.Left(16);
			CString strRes;
			CEncrypt::CalcMD5(strOrg, strRes);

			CashRegisterLogin.SetTimeChuo(tm);
			CashRegisterLogin.SetCheckCode(strRes);

			bRet = CashRegisterLogin.Execute();

			if (!bRet) //连接中心失败
			{
				return ;
			}

			if (CashRegisterLogin.GetStatusCode() == 0) //执行成功
			{
				//载入中心配置的控制参数字串
				CNetBarConfig::GetInstance()->PraseControlOption(CashRegisterLogin.GetControlOption());
			}
			else
			{
				return;
			}


			if (CNetBarConfig::GetInstance()->GetForceQuitParam() == 1)
			{
				CString thisVer = CIBALog::GetInstance()->GetAppVer();
				thisVer.Remove(_T('.'));
				CString strVer = CNetBarConfig::GetInstance()->GetForceQuitVersion();
				if (thisVer.Compare(strVer) < 0)
				{
					// 低于中心的版本
					KillTimer(nIDEvent);

					theApp.IBAMsgBox(IDS_IBA_RESTART_TIP);
					theApp.AutoRestart();
				}
			}
		}
	}
	//刷新好旺宝的定时器
	if(nIDEvent == 1100)
	{
		KillTimer(1100);
		QrData qrData;
		qrData.nCode = 0;
		CNetBarConfig::GetInstance()->SetQrData(&qrData);
		CNetBarConfig::GetInstance()->m_threadManipulateHaowangbao->Start();
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	//快捷通道焦点拦截
	if (m_wndFastLane.RobFocus(pMsg))
	{
		return TRUE;
	}

	return CBCGPFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnPointExchange()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_USEPOINT())
	{
		CNDPointExchangeDlg IBAAgent;
		IBAAgent.DoModal();
	} 
}

void CMainFrame::OnUpdatePointexchange(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CRightsManagement::GetInstance()->SetUI_IBAP_USEPOINT(pCmdUI);
}

void CMainFrame::OnAgent()
{
	CDlgIBAAgent IBAAgent;
	IBAAgent.DoModal();
}

void CMainFrame::OnSeatoperation()
{
	theApp.GetIBAView()->ShowSeatLayoutView();
}

void CMainFrame::OnUpdateSeatoperation(CCmdUI *pCmdUI)
{
	//2015-0820 liyajun
	pCmdUI->Enable(theApp.GetCurCashier()->IsOnDuty());
}


LRESULT CMainFrame::OnExecuteRealName(WPARAM wp, LPARAM lp)
{
	return theApp.GetRealName()->OnExecuteMsg((UINT)wp);
}

void CMainFrame::OnServerweb()
{
	ShellExecute(GetSafeHwnd(), _T("open"), _T("http://news.duduniu.cn:8080/ido/temp/ddnwordhelp/Contents.htm"), NULL, NULL, SW_NORMAL);
}

void CMainFrame::OnShortmsgquery()
{
	theApp.GetIBAView()->ShowShortMsgView();
}

void CMainFrame::OnCardderegister()
{
	CRealNameDeRegDlg CardDeRegisterdlg;
	CardDeRegisterdlg.DoModal();
}

void CMainFrame::OnCardopenconsume()
{
	theApp.GetIBAView()->ShowCardOpenConsumeView();
}

void CMainFrame::OnScan(UINT nId)
{
	int nScanType = 0;
	switch(nId)
	{
	case IDM_SCAN1:
		nScanType = CIBAGlobal::SRT_SCAN1;
		break;

	case IDM_SCAN2:
		nScanType = CIBAGlobal::SRT_SCAN2;
		break;

	case IDM_READ2:
		nScanType = CIBAGlobal::SRT_READ2;
		break;

	case IDM_SCAN_PASSPORT:
		nScanType = CIBAGlobal::SRT_SCAN_PASSPORT;
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	if (!theApp.GetRealName()->DoScan(nScanType))
	{
		theApp.IBAMsgBox(IDS_SCANFAILED);
	}
}

void CMainFrame::OnUpdateScan(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(theApp.GetRealName()->IsScanEnabled());
}

void CMainFrame::OnAllLog()
{
	if (CIBALog::GetInstance()->GetLogOption() == (UINT)-1)
	{
		CIBALog::GetInstance()->SetLogOption(CIBAConfig::GetInstance()->GetLogLevel());
	}
	else
	{
		CIBALog::GetInstance()->SetLogOption((UINT)-1);
	}
}

void CMainFrame::OnUpdateAllLog(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CIBALog::GetInstance()->GetLogOption() == (UINT)-1);
}

void CMainFrame::OnUpdateCreditDirect(CCmdUI* pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(CNetBarConfig::GetInstance()->GetCreditDirect() &&
		theApp.GetCurCashier()->IsOnDuty());
}

void CMainFrame::OnUpdateRealName(CCmdUI* pCmdUI)
{
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->Enable(theApp.GetRealName()->IsMenuEnabled());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateCardDeregister(CCmdUI* pCmdUI)
{
	//just for RZX only!

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		pCmdUI->Enable(theApp.GetRealName()->IsRealNameExtendMenuEnabled());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnMergemember()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_MERGEMEMBER())
	{
		CMergeMemberDlg MergeMemberDlg;
		MergeMemberDlg.DoModal();
	}
}

void CMainFrame::OnUpdateMergemember(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_MERGEMEMBER(pCmdUI);
	}
}

void CMainFrame::OnMuticheckout()
{
	CDlgBossPwd PwdDlg;
	if (PwdDlg.DoModal() == IDOK)
	{
		CDlgAllCheckOut AllCheckOutDlg;
		AllCheckOutDlg.DoModal();
	}
}

void CMainFrame::OnUpdateMuticheckout(CCmdUI *pCmdUI)
{
	/*if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
	}
	else*/
	{
		CRightsManagement::GetInstance()->SetUI_IBAP_CREDIT(pCmdUI);
	}
}

void CMainFrame::OnUpdateRealNameExtendMenu(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(theApp.GetRealName()->IsRealNameExtendMenuEnabled());
}


void CMainFrame::OnBoundtime2()
{
	if (CRightsManagement::GetInstance()->Take_IBAP_BUNDLETIME2())
	{
		CNDBundleTime2Dlg dlg;
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateBoundtime2(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CRightsManagement::GetInstance()->SetUI_IBAP_BUNDLETIME2(pCmdUI);
}

void CMainFrame::OnUpdateRealNameSet(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CNetBarConfig::GetInstance()->GetAuditType() > 0);
}

void CMainFrame::UpdateToolbarLastID(const CString& str, BOOL bError)
{
	IBA_TRACE(_T("CMainFrame::UpdateToolbarLastID(const CString& netId=%s)"),str);

	CIdentityToolbarButton *pBtn = (CIdentityToolbarButton*)
		m_wndToolBar.GetButton( m_wndToolBar.CommandToIndex(ID_LABEL));

	if ( CIBAConfig::GetInstance()->GetShowLastUserNetId() ||bError)
	{
		pBtn->SetCtrlWindowText(str);
		m_strLastIdInToolbar = str;
		if (bError)
		{
			((CVCenterStatic*)pBtn->GetCtrlWnd())->SetTextColor(RGB(255,0,0),TRUE);
		}
		else
		{
			((CVCenterStatic*)pBtn->GetCtrlWnd())->SetTextColor(RGB(0,0,0),TRUE);
		}
	}
	else
	{
		pBtn->SetCtrlWindowText(_T(""));
		m_strLastIdInToolbar = _T("");
	}
	
	IBA_TRACE("设置用户视图，用户被选择.");

	CActiveMemberView *pMemberView = (CActiveMemberView*)
		theApp.GetIBAView()->GetTabControl().GetTabWnd(1);

	if (pMemberView && pMemberView->IsKindOf( RUNTIME_CLASS(CActiveMemberView) )) {
		pMemberView->SetSelUser( pMemberView->FindUserAt(1, str) );
	}
}

void CMainFrame::SelAndShowDlgByNetId(CString strNetId)
{
	CActiveMemberView *pMemberView = theApp.GetActiveMemberView();
	pMemberView->SetSelUser(pMemberView->FindUserAt(0, strNetId));

	if (CCurrentNetBarUser::GetInstance()->QueryBalance(strNetId))
	{
		CActiveMember ActiveMember;

		BOOL bExists =  CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(
			CCurrentNetBarUser::GetInstance()->Balance.GetMemberId(), ActiveMember);

		if (CCurrentNetBarUser::GetInstance()->Balance.IsNetbarMember() && !bExists)
		{
			CNDCreditDlg dlg;
			dlg.DoModal();
		}
		else
		{
			CNDReturnDlg dlg;
			dlg.DoModal();
		}
	}
	else
	{
		theApp.IBAMsgBox(IDS_HASNOTUSED);
	}
}

void CMainFrame::OnZengshoubao()
{
	theApp.GetIBAView()->ShowDodonewView();
}

void CMainFrame::OnLabelClick()
{
	CString strRefresh = m_strLastIdInToolbar;
	UpdateToolbarLastID(strRefresh);
	
	theApp.GetIBAView()->GotoComputerByNetID(m_strLastIdInToolbar,
		theApp.GetIBAView()->IsCurrentComputerView());
}

LRESULT CMainFrame::OnToolbarSetLabelText(WPARAM wP,LPARAM lP)
{
	UpdateToolbarLastID(LPCTSTR(wP), BOOL(lP));
	return 0;
}

void CMainFrame::OnUpdateOffDuty(CCmdUI *pCmdUI)
{
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		//pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
	//{
	//	CString sql;
	//	sql.Format(_T("select count(*) from localMember"));
	//	if(CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0)
	//	{

	//	}
	//	else
	//	{
	//		pCmdUI->Enable(TRUE);
	//	}
	//}
}

void CMainFrame::OnDianKa()
{
	//CCashierHelper::CashSellDianka(_T("http://192.168.10.65:8080/dodocard/cashpay/tip/alert.jsp?RequestTime=19:15:16&GameName=gameName&businessAmount=1015&RequestTimes=3&TermID=1234"));
	CCashierHelper::SellDianka();

	return;
}
void CMainFrame::OnUpdateDianka(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	BOOL bEnable = theApp.GetCurCashier()->IsOnDuty() && theApp.GetCurCashier()->HasPopedomOf(_T("IBAP_POINTCARD"));
	pCmdUI->Enable(bEnable);

	if(bEnable)
	{
		// 2014-7-21 - qsc 
		if(0 == CNetBarConfig::GetInstance()->GetEnableVirualRun())
		{
			pCmdUI->Enable(!CIBAHelpper::IsVirMach());
		}		
	}
}

void CMainFrame::OnUpdateCardopenconsume(CCmdUI *pCmdUI)
{
	// 2011/08/04-8201-gxx: 断网下禁用功能
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	pCmdUI->Enable(TRUE);
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	
#ifndef __USING_INTER_RESOURCE
	
	// 2011/08/22-8201-gxx:
	// 指定版本的文字资源来自于grc 
	//
	if (nCode == CN_UPDATE_COMMAND_UI)
	{
		if (CBCGPFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) == TRUE)
		{
			if (nID == ID_VIEW_TOOLBAR)
			{
				ASSERT(FALSE);
			}
			CCmdUI *pCmdUI = (CCmdUI*)pExtra;
			if (pCmdUI->m_pOther != NULL && pCmdUI->m_pOther == &m_wndToolBar)
			{
				// 不对工具栏做处理
			}
			else
			{
				pCmdUI->SetText(LOAD_STRING(nID));
			}

			return TRUE;
		}
		return FALSE;
	}
#endif

	return CBCGPFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMainFrame::OnUpdateIBAStatus(WPARAM wP,LPARAM lP)
{
	CWnd* pWnd = m_wndFastLane.GetDlgFastLane();
	if (pWnd)
	{
		pWnd->SendMessage(WM_UPDATE_IBA_STATUS,wP,lP);
	}
	
	return 0;
}

LRESULT CMainFrame::OnCanClose(WPARAM wP,LPARAM lP)
{
	if(NULL != CIBAGlobal::m_hCurRegisterDlg)
	{
		::SendMessage(CIBAGlobal::m_hCurRegisterDlg, C_CANCLOSE, wP, lP);
	}
	
	if(NULL != CIBAGlobal::m_hCurCreditDlg)
	{
		::SendMessage(CIBAGlobal::m_hCurCreditDlg, C_CANCLOSE, wP, lP);
	}

	return 0;
}

void CMainFrame::ShowNotice(LPCTSTR lpTitle,LPCTSTR lpInfo, LPCTSTR lpLink, BOOL bShouldSign)
{
	if (IsWindow(m_dlgNotice.m_hWnd))
	{
		m_dlgNotice.SetParams(lpTitle,lpInfo,lpLink,bShouldSign,TRUE);
		m_dlgNotice.ShowWindow(TRUE);
	}
	else
	{
		m_dlgNotice.SetParams(lpTitle,lpInfo,lpLink,bShouldSign,FALSE);
		if (m_dlgNotice.Create(IDD_DIALOG_NOTICE,this))
		{
			CRect rcSize;
			m_dlgNotice.GetWindowRect(rcSize);
			RECT rt = {-1, -1, -1, -1};
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
			m_dlgNotice.MoveWindow(CRect(rt.right-rcSize.Width(),rt.bottom-rcSize.Height(),rt.right,rt.bottom),TRUE);

			m_dlgNotice.ShowWindow(TRUE);
		}
		
	}
}

// 包房
void CMainFrame::OnRoom()
{
	CNetbarRoomDlg dlg;
	dlg.SetNetId();
	dlg.DoModal();
}

void CMainFrame::OnLockScreen()
{
	CString strPath = theApp.GetWorkPath();
	strPath += _T("\\IBABin\\Lock.exe");
	ShellExecute(NULL, _T("open"), strPath, NULL, NULL/*strWorkPath+_T("\\IBABin\\")*/, SW_SHOW);
}

void CMainFrame::OnUpdateRoom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.GetCurCashier()->IsOnDuty());
}

LRESULT CMainFrame::OnClientDianka(WPARAM wParam, LPARAM lParam)
{
	m_bDiankaSellCash = TRUE;
	OnDianKa();
	return 0;
}

LRESULT CMainFrame::OnHotKey(WPARAM, LPARAM)
{
	//2015-0910 liyajun 断网情况下禁止开户
	//if(CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkNormal)
	//{
	//	MessageBox(_T("断网情况下禁止开户！"));
	//	return 0;
	//}
	CGetTimeMethod time;
	time.Execute();

	if (time.GetStatusCode() < 0)
	{
		MessageBox(_T("网络已断开，无法开户！"));
		IBA_TRACE(_T("开户检测网络-网络已断开，禁止开户！"));
		return 0;
	}
	// 1：手机号码开户
	if (CRightsManagement::GetInstance()->Take_IBAP_REGISTER() && 
		CNetBarConfig::GetInstance()->GetAuditType() == CIBAGlobal::emRealnameWD)
	{
		if (IsIconic())
		{
			ShowWindow(SW_RESTORE);
		}
		else
		{ 
			SetForegroundWindow();
		}
		CRegisterDlg dlg;
		dlg.SetPhoneNumberOpen();
		dlg.DoModal();
	} 

	OnReadCard();
	return 0;
}

void CMainFrame::OnReadCard()
{
	if (!theApp.GetRealName()->DoScan(3))
	{
		theApp.IBAMsgBox(IDS_SCANFAILED);
	}
}

//void CMainFrame::OnNcPaint()
//{
//	CBCGPFrameWnd::OnNcPaint();
//
//	CDC* pWinDC = GetWindowDC();
//
//	if (pWinDC )
//	{
//		CString strTitle ; //标题串
//		strTitle = LOAD_STRING (IDS_IBATITLE);
//		strTitle.Insert (0, _T(" - "));
//		strTitle.Insert (0, CNetBarConfig:: GetInstance()->GetNetBarName ());
//
//		CString strTel (_T("24小时售后客服：4006683755")); //电话信息串
//
//		int nCaptionWidth ; //标题栏宽
//		int nCaptionHeight ; //标题栏高
//		int nTitleWidth ; //标题串宽
//		int nTelinfoWidth ; //电话信息串宽
//		//框架宽度
//		CRect WinRect ;
//		GetWindowRect(&WinRect );
//		nCaptionWidth = WinRect .right - WinRect.left ;
//		//框架厚度
//		nCaptionHeight = GetSystemMetrics (SM_CYCAPTION);
//		//标题宽度
//		nTitleWidth = pWinDC ->GetTextExtent( strTitle).cx ;
//		//电话信息宽度
//		nTelinfoWidth = pWinDC ->GetTextExtent( strTel).cx ;
//		//其他
//		TEXTMETRIC tm ;
//		pWinDC->GetTextMetrics (&tm);
//
//		//获取当前标题框画刷
//		//DWORD dwColor = GetSysColor(COLOR_ACTIVECAPTION);
//		//int nR =  GetRValue(dwColor);
//		//int nG =  GetGValue(dwColor);
//		//int nB =  GetBValue(dwColor);
//		//pWinDC->GetSysColorBrush();
//		HBRUSH hBrush = GetSysColorBrush( COLOR_ACTIVECAPTION);
//		
//		//获取当前标题框Pen,字体
//		CFont *pFont = GetFont();
//		//获取字体颜色
//		DWORD dwFontColor = GetSysColor(COLOR_CAPTIONTEXT);
//		//获取背景颜色
//		DWORD dwColor = GetSysColor(COLOR_ACTIVEBORDER);
//		int nR =  GetRValue( dwColor);
//		int nG =  GetGValue( dwColor);
//		int nB =  GetBValue( dwColor);
//
//		//设置画笔
//		//HFONT Oldfont = pWinDC->SelectObject()
//		//HPEN OldPen = pWinDC->SelectObject(hPen);
//		//设置背景
//		pWinDC->SetBkColor(COLORREF( dwColor));
//		//设置字体颜色
//		pWinDC->SetTextColor(dwFontColor);
//
//		//CString strLeft,strRight; //分别要显示在左边和右边的字符串
//		if(nCaptionWidth > (nTitleWidth + nTelinfoWidth))
//		{
//			//CRect rect(0,0,nTitleWidth,tm.tmHeight);
//			//pWinDC->DrawText(strTitle,&rect,DT_LEFT);
//			pWinDC->TextOut (0,0,strTitle);
//			pWinDC->TextOut (nCaptionWidth - nTelinfoWidth,0,strTel );
//		}
//		else
//		{
//			CString strTmp = strTitle + strTel;
//			pWinDC->TextOut (0,0,strTmp);
//		}
//	}
//	ReleaseDC(pWinDC );
//
//}

//LRESULT CMainFrame::DefWindowProc(UINT Message,WPARAM wParam,LPARAM lParam)
//{
//	LRESULT lrst = CBCGPFrameWnd::DefWindowProc(Message,wParam,lParam);
//
//	if (!::IsWindow(m_hWnd))
//
//		return lrst;
//
//	if (Message == WM_NCPAINT || Message == WM_NCACTIVATE)
//	{
//		CDC* pWinDC = GetWindowDC();
//
//		if (pWinDC)
//		{
//			CString strTitle; //标题串
//			strTitle = LOAD_STRING(IDS_IBATITLE);
//			strTitle.Insert(0, _T(" - "));
//			strTitle.Insert(0, CNetBarConfig::GetInstance()->GetNetBarName());
//
//			CString strTel(_T("24小时售后客服：4006683755"));//电话信息串
//
//			int nCaptionWidth; //标题栏宽
//			int nCaptionHeight; //标题栏高
//			int nTitleWidth; //标题串宽
//			int nTelinfoWidth; //电话信息串宽
//			//框架宽度
//			CRect WinRect;
//			GetWindowRect(&WinRect);
//			nCaptionWidth = WinRect.right - WinRect.left;
//			//框架厚度
//			nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
//			//标题宽度
//			nTitleWidth = pWinDC->GetTextExtent(strTitle).cx;
//			//电话信息宽度
//			nTelinfoWidth = pWinDC->GetTextExtent(strTel).cx;
//			//其他
//			TEXTMETRIC tm;
//			pWinDC->GetTextMetrics(&tm);
//
//			//CString strLeft,strRight; //分别要显示在左边和右边的字符串
//			if(nCaptionWidth > (nTitleWidth + nTelinfoWidth))
//			{
//				CRect rect(0,0,nTitleWidth,tm.tmHeight);
//				//pWinDC->DrawText(strTitle,&rect,DT_LEFT);
//				pWinDC->TextOut(nCaptionWidth - nTelinfoWidth,0,strTel);
//			}
//			else
//			{
//				CString strTmp = strTitle + strTel;
//				pWinDC->TextOut(0,0,strTmp);
//			}
//		}
//		ReleaseDC(pWinDC);
//
//	}
//
//	return lrst;
//}