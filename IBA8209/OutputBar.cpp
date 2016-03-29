
#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "CreditDlg.h"
#include "ServletHellper.h"
#include "Dal\IBADAL.h"
#include ".\outputbar.h"
#include "MainFrm.h"
#include "Process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const INT LIST_COUNT = 5;

static const INT LIST_OPERATION = 0;  // 操作日志
static const INT LIST_CARD = 1;       // 刷卡记录
static const INT LIST_CHECKOUT = 2;   // 结账记录
static const INT LIST_CONSUME = 3;    // 消费记录
static const INT LIST_MSG = 4;        // 消息记录

static const TCHAR *lpFileNameSuffix[LIST_COUNT] = 
{ 
	_T("_operation.dat"),
	_T("_card.dat"), 
	_T("_checkOut.dat"),
	_T("_consume.dat"), 
	_T("_msg.dat")
};
static COutputListCtrl *lpListCtrl[LIST_COUNT] = {NULL};
/////////////////////////////////////////////////////////////////////////////
// CIBAOutputBar

BEGIN_MESSAGE_MAP(CIBAOutputBar, CBCGPDockingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_OPERATIONLOG, OnOperationLog)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_EXPORTEXCEL, OnExportExcel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIBAOutputBar construction/destruction

CIBAOutputBar::CIBAOutputBar()
{
	// TODO: add one-time construction code here
}

CIBAOutputBar::~CIBAOutputBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CIBAOutputBar message handlers
//创建
int CIBAOutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableGripper(FALSE);//不显示标题CAPTION

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CBCGPTabWnd::STYLE_3D_ONENOTE, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	m_wndTabs.EnableTabSwap(FALSE);//禁止TAB窗体的拖动功能

	lpListCtrl[0] = &m_wndListInfo;
	lpListCtrl[1] = &m_wndListActive;
	lpListCtrl[2] = &m_listCheckOut;
	lpListCtrl[3] = &m_wndConsumeInfo;
	lpListCtrl[4] = &m_wndMsgInfo;

	if (!CreateOperationList())
	{
		return -1;
	}

 	if (!CreateActiveList())
 	{
 		return -1;
 	}

	// 2011/10/17-8206-gxx: 
	if (!CreateCheckOutList())
	{
		return -1;
	}

	if (!CreateMsgList())
	{
		return -1;
	}

	if (!CreateConsumeList())
	{
		return -1;
	}
	
	m_wndTabs.AddTab(&m_wndListInfo, LOAD_STRING(IDS_OPERATELOG));//添加操作信息页
	m_wndTabs.AddTab(&m_wndListActive, LOAD_STRING(IDS_ACTIVELOG));//添加激活记录页
	m_wndTabs.AddTab(&m_listCheckOut, LOAD_STRING(IDS_CHECKOUT_RECORD)); // 结账记录
	m_wndTabs.AddTab(&m_wndConsumeInfo, LOAD_STRING(IDS_RETURNLOG));//退款
	m_wndTabs.AddTab(&m_wndMsgInfo, LOAD_STRING(IDS_CLIENTINFO));//添加用户信息
	
	m_wndTabs.RecalcLayout();
	m_wndTabs.SetActiveTab(0);
	
	


	return 0;
}

void CIBAOutputBar::OnSize(UINT nType, int cx, int cy) 
{	
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	
	m_wndTabs.SetWindowPos(NULL, 0, 0, cx, cy , /*SWP_NOMOVE |*/ SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTabs.RecalcLayout();
}

void CIBAOutputBar::SetCreditInfo(COperationInfo &OperationInfo)
{
	m_wndListInfo.SetItemText(0, 2, OperationInfo.GetUserName());

	m_wndListInfo.SetItemText(0, 3, FormatNetId(OperationInfo.GetNetId()));

	CString strTmp;
	strTmp.Format(_T("%.2f"), OperationInfo.GetOperationAmount() / 100.0);
	m_wndListInfo.SetItemText(0, 4, strTmp);
	
	CString strDescription;

	if (OperationInfo.GetPresentAmount() > 0)
	{
		strDescription.AppendFormat(LOAD_STRING(IDS_PRESENT_FORMAT), OperationInfo.GetPresentAmount() / 100.0); // 赠送:%.2f
	}
	
	if (OperationInfo.GetDeposit() > 0)
	{
		if (!strDescription.IsEmpty())
		{
			strDescription.Append(_T(","));
		}

		strDescription.AppendFormat(LOAD_STRING(IDS_DEPOSIT_FORMAT), OperationInfo.GetDeposit() / 100.0); // 押金:%.2f
	}

	if (!OperationInfo.GetPassWord().IsEmpty())
	{
		if (CNetBarConfig::GetInstance()->GetRealNamePassword() < 2)
		{
			if (!strDescription.IsEmpty())
			{
				strDescription.Append(_T(","));
			}

			strDescription.AppendFormat(LOAD_STRING(IDS_PASSWORD_FORMAT), OperationInfo.GetPassWord()); // 密码:%s
		}
	}

	if (OperationInfo.GetIsBundTime())
	{
		if (!strDescription.IsEmpty())
		{
			strDescription.Append(_T(","));
		}

		strDescription.AppendFormat(_T(""), OperationInfo.GetBundTimeAmount() / 100.0); // 包时金额:%.2f
	}

	if (!OperationInfo.GetDescription().IsEmpty())
	{
		if (!strDescription.IsEmpty())
		{
			strDescription.Append(_T(","));
		}

		strDescription.Append(OperationInfo.GetDescription());
	}
	
	m_wndListInfo.SetItemText(0, 5, strDescription);
}

BOOL CIBAOutputBar::AddCheckOutLog(COperationInfo& OperationInfo)
{
	CIBAString strTmp;

	BOOL bOK = FALSE;
	switch(OperationInfo.GetOperationType())
	{//结账时间|用户级别|终端号|上机时间|上网账号|姓名|证件号码|金额|备注
	case CIBAGlobal::emOpTypeAutoReturn:
		{
			m_listCheckOut.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
			CString strTmp;
			strTmp = CNetBarConfig::GetInstance()->GetUserClassName(OperationInfo.GetClassId());
			m_listCheckOut.SetItemText(0, 1, strTmp); // 用户级别
			strTmp = OperationInfo.GetNetId();
			m_listCheckOut.SetItemText(0,4, strTmp);

			strTmp = OperationInfo.GetUserName();
			m_listCheckOut.SetItemText(0,5, strTmp);

			strTmp = OperationInfo.GetIdNum();
			m_listCheckOut.SetItemText(0,6, strTmp);

			// 备注显示为自动退款
			m_listCheckOut.SetItemText(0, 8,  CIBAGlobal::GetOperationName(CIBAGlobal::emOpTypeAutoReturn));
		}
		bOK = TRUE;
		break;
	case CIBAGlobal::emOpTypeVipReturn:
		{
			m_listCheckOut.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
			CString strTmp;
			strTmp = CNetBarConfig::GetInstance()->GetUserClassName(OperationInfo.GetClassId());
			m_listCheckOut.SetItemText(0, 1, strTmp); // 用户级别

			strTmp = OperationInfo.GetTermId();
			m_listCheckOut.SetItemText(0, 2, strTmp); // 终端号

			if (CIBAHelpper::IsValidTime(OperationInfo.GetCheckInTime()))
			{
				strTmp = OperationInfo.GetCheckInTime().Format(_T("%H:%M:%S"));
				m_listCheckOut.SetItemText(0, 3, strTmp);
			}

			strTmp = OperationInfo.GetNetId();
			m_listCheckOut.SetItemText(0,4, strTmp);

			strTmp = OperationInfo.GetUserName();
			m_listCheckOut.SetItemText(0,5, strTmp);

			strTmp = OperationInfo.GetIdNum();
			m_listCheckOut.SetItemText(0,6, strTmp);

			m_listCheckOut.SetItemText(0,8, OperationInfo.GetDescription());
			
			bOK = TRUE;
			break;
		}
		
	case CIBAGlobal::emOpTypeRoomReturn:
		{
			m_listCheckOut.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
			CString strTmp;
			strTmp = CNetBarConfig::GetInstance()->GetUserClassName(OperationInfo.GetClassId());
			m_listCheckOut.SetItemText(0, 1, strTmp); // 用户级别

			strTmp = OperationInfo.GetTermId();
			m_listCheckOut.SetItemText(0, 2, strTmp); // 终端号

			if (CIBAHelpper::IsValidTime(OperationInfo.GetCheckInTime()))
			{
				strTmp = LOAD_STRING(IDS_ROOM_OPEN_TIME);
				strTmp += OperationInfo.GetCheckInTime().Format(_T("%H:%M:%S"));
				m_listCheckOut.SetItemText(0, 3, strTmp);
			}

			strTmp = OperationInfo.GetNetId();
			m_listCheckOut.SetItemText(0,4, strTmp);

			strTmp = OperationInfo.GetUserName();
			m_listCheckOut.SetItemText(0,5, strTmp);

			strTmp = OperationInfo.GetIdNum();
			m_listCheckOut.SetItemText(0,6, strTmp);

			m_listCheckOut.SetItemText(0,8, OperationInfo.GetDescription());
			
			bOK = TRUE;
			break;
		}
		
	case CIBAGlobal::emOpTypeReturn:
		{
			m_listCheckOut.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
			CString strTmp;
			strTmp = CNetBarConfig::GetInstance()->GetUserClassName(OperationInfo.GetClassId());
			m_listCheckOut.SetItemText(0, 1, strTmp); // 用户级别

			strTmp = OperationInfo.GetTermId();
			m_listCheckOut.SetItemText(0, 2, strTmp); // 终端号

			if (CIBAHelpper::IsValidTime(OperationInfo.GetCheckInTime()))
			{
				strTmp = OperationInfo.GetCheckInTime().Format(_T("%H:%M:%S"));
				m_listCheckOut.SetItemText(0, 3, strTmp);
			}

			strTmp = OperationInfo.GetNetId();
			m_listCheckOut.SetItemText(0,4, strTmp);

			strTmp = OperationInfo.GetUserName();
			m_listCheckOut.SetItemText(0,5, strTmp);

			strTmp = OperationInfo.GetIdNum();
			m_listCheckOut.SetItemText(0,6, strTmp);


			strTmp.Format(_T("%.2f"), OperationInfo.GetConsumeMoney() / 100.0);
			m_listCheckOut.SetItemText(0,7, strTmp);

			CString strDescription;

			// IDS_OPTYPE_BALANCE: "余额:%.2f"
			// IDS_OPTYPE_BALANCE_EX: "余额:%.2f + 押金:%.2f"
			if (OperationInfo.GetDeposit() > 0)
			{
				strDescription.Format(LOAD_STRING(IDS_OPTYPE_BALANCE_EX),
					OperationInfo.GetOperationAmount() / 100.0,
					OperationInfo.GetDeposit() / 100.0);
			}
			else
			{
				strDescription.Format(LOAD_STRING(IDS_OPTYPE_BALANCE),
					OperationInfo.GetOperationAmount() / 100.0);
			}

			if (!OperationInfo.GetDescription().IsEmpty())
			{
				if (!strDescription.IsEmpty())
				{
					strDescription.Append(_T(","));
				}

				strDescription.Append(OperationInfo.GetDescription());
			}

			m_listCheckOut.SetItemText(0, 8, strDescription);
		}
		bOK = TRUE;
		break;
	}

	if (bOK)
		SaveToFile( lpFileNameSuffix[LIST_CHECKOUT], *lpListCtrl[LIST_CHECKOUT]);

	// 2014-6-13 - qsc
	// 发送消息给恒信收银台，做结账操作
	{
		static int nExist = -1;
		if(-1 == nExist)
		{
			CProcess process;
			process.Attach("NetBar.exe");		// 恒信收银台进程
			if(process.IsRunning())
			{
				nExist = 1;
			}
			else
			{
				nExist = 0;
			}
		}

		if(1 == nExist)
		{
			CString str = OperationInfo.GetTermId();
			for(int i = 0; i < str.GetLength(); ++i)
			{
				char ch = str.GetAt(i);
				if(ch < '0' || ch > '9')
				{
					str.Remove(ch);	//	恒信的终端号是纯数字
				}
			}	
			CIBAHelpper::AutoHXPayout(str);
		}
	}
	return FALSE;
}

void CIBAOutputBar::AddOperationLog(COperationInfo& OperationInfo)
{
	CIBAString strTmp;

	m_wndListInfo.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));

	
	switch (OperationInfo.GetOperationType())
	{
	case 0://无
		{
			if (OperationInfo.GetIsBundTime())
			{
				m_wndListInfo.SetItemText(0, 1, CIBAGlobal::GetOperationName(CIBAGlobal::emOpTypeBundleTime));
				m_wndListInfo.SetItemText(0, 2, OperationInfo.GetUserName());
				m_wndListInfo.SetItemText(0, 3, FormatNetId(OperationInfo.GetNetId()));
				
				strTmp.Format(_T("%.2f"), OperationInfo.GetOperationAmount() / 100.0);
				m_wndListInfo.SetItemText(0, 4, strTmp);

			}
		}

	case CIBAGlobal::emOpTypeOpen://开户
	case CIBAGlobal::emOpTypeCredit://充值
	case CIBAGlobal::emOpTypeCreditDelete://冲正
	case CIBAGlobal::emOpTypeLocalCredit: // 2011/08/01-8201-gxx: 本地充值
	case CIBAGlobal::emOpTypeCreditModify: // 修改充值
	case CIBAGlobal::emOpTypeChangeMachine://换机
		{
			m_wndListInfo.SetItemText(0,1,CIBAGlobal::GetOperationName(OperationInfo.GetOperationType()));
			SetCreditInfo(OperationInfo);
			break;
		}
	case CIBAGlobal::emOpTypeVipReturn: //会员结账
	case CIBAGlobal::emOpTypeBrokenReturnVip: // 会员断网结账
	case CIBAGlobal::emOpTypeRoomReturn:
		{
			m_wndListInfo.SetItemText(0, 1,CIBAGlobal::GetOperationName(OperationInfo.GetOperationType()));
			
			m_wndListInfo.SetItemText(0, 2, OperationInfo.GetUserName());
			m_wndListInfo.SetItemText(0, 3, OperationInfo.GetNetId());

			CString strDescription;

			if (!OperationInfo.GetDescription().IsEmpty())
			{
				if (!strDescription.IsEmpty())
				{
					strDescription.Append(_T(","));
				}

				strDescription.Append(OperationInfo.GetDescription());
			}

			m_wndListInfo.SetItemText(0, 5, strDescription);

			break;
		}
	
	case CIBAGlobal::emOpTypeReturn: //退款
	case CIBAGlobal::emOpTypeAutoReturn: //自动退款
	case CIBAGlobal::emOpTypeReturnBalance: //退余额
	case CIBAGlobal::emOpTypeLocalReturn: // 本地用户退款
	case CIBAGlobal::emOpTypeBrokenReturn: // 断网退款
		{
			m_wndListInfo.SetItemText(0, 1, CIBAGlobal::GetOperationName(OperationInfo.GetOperationType()));

			m_wndListInfo.SetItemText(0, 2, OperationInfo.GetUserName());
			m_wndListInfo.SetItemText(0, 3, FormatNetId(OperationInfo.GetNetId()));

			strTmp.Format(_T("%.2f"), OperationInfo.GetAllAmount() / 100.0);
			m_wndListInfo.SetItemText(0, 4, strTmp);

			CString strDescription;

			// IDS_OPTYPE_BALANCE: "余额:%.2f"
			// IDS_OPTYPE_BALANCE_EX: "余额:%.2f + 押金:%.2f"
			if (OperationInfo.GetDeposit() > 0)
			{
				strDescription.Format(LOAD_STRING(IDS_OPTYPE_BALANCE_EX),
					OperationInfo.GetOperationAmount() / 100.0,
					OperationInfo.GetDeposit() / 100.0);
			}
			else
			{
				strDescription.Format(LOAD_STRING(IDS_OPTYPE_BALANCE),
					OperationInfo.GetOperationAmount() / 100.0);
			}

			if (!OperationInfo.GetDescription().IsEmpty())
			{
				if (!strDescription.IsEmpty())
				{
					strDescription.Append(_T(","));
				}

				strDescription.Append(OperationInfo.GetDescription());
			}

			m_wndListInfo.SetItemText(0, 5, strDescription);

			break;
		}
	case CIBAGlobal::emOpTypeOpenLocal:// 本地开户
		{
			m_wndListInfo.SetItemText(0, 1, CIBAGlobal::GetOperationName(CIBAGlobal::emOpTypeOpenLocal)); 

			strTmp.LoadString(IDS_LOCALUSER);
			OperationInfo.SetDescription(strTmp);  // 显示本地用户备注
			SetCreditInfo(OperationInfo);
			break;
		}

	case CIBAGlobal::emOpTypeBundleTime://包时
		{
			m_wndListInfo.SetItemText(0, 1, CIBAGlobal::GetOperationName(CIBAGlobal::emOpTypeBundleTime));
			m_wndListInfo.SetItemText(0, 2, OperationInfo.GetUserName());
			m_wndListInfo.SetItemText(0, 3, OperationInfo.GetNetId());

			strTmp.Format(_T("%.2f"), OperationInfo.GetOperationAmount() / 100.0);
			m_wndListInfo.SetItemText(0, 4, strTmp);

			m_wndListInfo.SetItemText(0, 5, OperationInfo.GetDescription());

			break;
		}
	default:
		IBA_ASSERT2(FALSE,"未知的操作类型");
		break;
	}

	INT nType = OperationInfo.GetOperationType();

	if (OperationInfo.GetOperationType() == 2 && 
		CIBAGlobal::NetworkStatus != CIBAGlobal::emNetworkBroken &&
		OperationInfo.GetIsResumeData() == FALSE)
	{
		OperationInfo.SetOperationType(0);
		OperationInfo.SetIsMember(FALSE); //退款的肯定是非会员
		QueryConsume(OperationInfo, TRUE);
	}
	else
	{
		m_wndTabs.SetActiveTab(0);
	}

	// 保存到文件
	SaveToFile( lpFileNameSuffix[LIST_OPERATION], *lpListCtrl[LIST_OPERATION]);
}

void CIBAOutputBar::AddActiveLog()
{
	NS_SERVLET::CBalanceMethod & Balance = CCurrentNetBarUser::GetInstance()->Balance;

	m_wndListActive.InsertItem(0, COleDateTime::GetCurrentTime().Format(_T("%H:%M:%S")));
	
	m_wndListActive.SetItemText(0, 1, Balance.GetUserName() + _T("(") 
		+ CIBAHelpper::GetSexAsString(Balance.GetSex())  + _T(")"));

	m_wndListActive.SetItemText(0, 2, Balance.GetSerailNum());

	CString strTmp;//用户级别
	strTmp = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_wndListActive.SetItemText(0, 3, strTmp);

	//结算的总账户= 现金额 + 用户赠送金额 + 当前充值的金额 + 上网预留金
	INT nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();

	strTmp.Format(_T("%.2f"), nAllMoney / 100.0);
	m_wndListActive.SetItemText(0, 4, strTmp);

	strTmp.Format(_T("%d"), Balance.GetCurrentPoint());

	m_wndListActive.SetItemText(0, 5, strTmp);

	m_wndListActive.SetItemText(0, 6, Balance.GetPersonalId());

	if (m_wndListActive.GetItemCount() > 50)
	{
		m_wndListActive.DeleteItem(50);
	}
	
	m_wndTabs.SetActiveTab(1);

	SaveToFile( lpFileNameSuffix[LIST_CARD], *(lpListCtrl[LIST_CARD]));
}

BOOL CIBAOutputBar::CreateOperationList()
{
	const DWORD dwViewStyle = LVS_REPORT | LVS_NOSORTHEADER | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	CIBAHelpper::CreateFont(m_Font, 14);

	if (!m_wndListInfo.Create(dwViewStyle, CRect(1, 1, 1, 1), &m_wndTabs, 2))
	{
		TRACE0("Failed to create output view\n");
		return FALSE;      // fail to create
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_LOGINFOLISTGHEADER);
	//strHeader = L"时间|操作类型|姓名|上网账号|金额|备注";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {70, 120, 100, 150, 60, 200};

	int nColCount = sizeof(Width)/sizeof(UINT);
	if( nColCount != strArray.GetCount() )
	{
		IBA_ASSERT2(false,"字符串分割的数组长度不相等,请检查字符串表中的字符串是否有改动");

		nColCount = nColCount < strArray.GetCount() ?
			nColCount : strArray.GetCount();  // 防止发生意外
	}

	for (INT i = 0; i < nColCount; i++)
	{
		m_wndListInfo.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	

	return TRUE;
}

BOOL CIBAOutputBar::CreateActiveList()
{
	const DWORD dwViewStyle = LVS_REPORT | LVS_NOSORTHEADER | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndListActive.Create(dwViewStyle, CRect(1, 1, 1, 1), &m_wndTabs, 3))
	{
		TRACE0("Failed to create Activeoutput view\n");
		return FALSE;      // fail to create
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_LOGACTIVELISTGHEADER);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {70, 90, 150, 80, 100, 80, 150};

	int nColCount = sizeof(Width)/sizeof(UINT);
	if( nColCount != strArray.GetCount() )
	{
		IBA_ASSERT2(false,"字符串分割的数组长度不相等,请检查字符串表中的字符串是否有改动");

		nColCount = nColCount < strArray.GetCount() ?
			nColCount : strArray.GetCount();  // 防止发生意外
	}

	for (INT i = 0; i < nColCount; i++)
	{
		m_wndListActive.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	return TRUE;
}

BOOL CIBAOutputBar::CreateMsgList()
{
	const DWORD dwViewStyle = LVS_REPORT | LVS_NOSORTHEADER | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndMsgInfo.Create(dwViewStyle, CRect(1, 1, 1, 1), &m_wndTabs, 4))
	{
		TRACE0("Failed to create Activeoutput view\n");
		return FALSE;      // fail to create
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_LOGMSGLISTGHEADER);
	//strHeader = L"时间|终端号|消息";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {130, 70, 450};

	int nColCount = sizeof(Width)/sizeof(UINT);
	if( nColCount != strArray.GetCount() )
	{
		IBA_ASSERT2(false,"字符串分割的数组长度不相等,请检查字符串表中的字符串是否有改动");

		nColCount = nColCount < strArray.GetCount() ?
			nColCount : strArray.GetCount();  // 防止发生意外
	}

	for (INT i = 0; i < nColCount; i++)
	{
		m_wndMsgInfo.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	//查询本班次的信息
	//UpdateMsgLog();

	return TRUE;
}

BOOL CIBAOutputBar::CreateConsumeList()
{
	const DWORD dwViewStyle = LVS_REPORT | LVS_NOSORTHEADER | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndConsumeInfo.Create(dwViewStyle, CRect(1, 1, 1, 1), &m_wndTabs, 5))
	{
		TRACE0("Failed to create Consumeoutput view\n");
		return FALSE;      // fail to create
	}

	CIBAString strHeader;
	//strHeader.LoadString(IDS_RETURNLOG);
	strHeader.LoadString(IDS_CONSUME_LIST_HEAD);//L"消费类型|金额|详细信息|其他|上网账号|姓名";

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {80, 80, 300, 80, 130, 100};

	int nColCount = sizeof(Width)/sizeof(UINT);
	if( nColCount != strArray.GetCount() )
	{
		IBA_ASSERT2(false,"字符串分割的数组长度不相等,请检查字符串表中的字符串是否有改动");

		nColCount = nColCount < strArray.GetCount() ?
			nColCount : strArray.GetCount();  // 防止发生意外
	}

	for (INT i = 0; i < nColCount; i++)
	{
		m_wndConsumeInfo.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	return TRUE;
}

BOOL CIBAOutputBar::CreateCheckOutList()
{
	const DWORD dwViewStyle = LVS_REPORT | LVS_NOSORTHEADER | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_listCheckOut.Create(dwViewStyle, CRect(1, 1, 1, 1), &m_wndTabs, 6))
	{
		TRACE0("Failed to create Consumeoutput view\n");
		return FALSE;      // fail to create
	}

	CIBAString strHeader;
	strHeader.LoadString(IDS_REFUND_RECORD_HEAD); // _T("结账时间|用户级别|终端号|上机时间|上网账号|姓名|证件号码|金额|备注");

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {90, 80, 80, 140, 140, 80,140,100,170};

	int nColCount = sizeof(Width)/sizeof(UINT);
	if( nColCount != strArray.GetCount() )
	{
		strHeader = _T("结账时间|用户级别|终端号|上机时间|上网账号|姓名|证件号码|金额|备注");
		strArray.RemoveAll();
		CIBAHelpper::SplitLine(strHeader, strArray);
		nColCount = strArray.GetCount();
	}

	for (INT i = 0; i < nColCount; i++)
	{
		m_listCheckOut.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	return TRUE;
}

void CIBAOutputBar::AddMsgLog(CClientShortMessage & strMsgInfo)
{
	m_wndMsgInfo.InsertItem(0, strMsgInfo.GetRevTime());
	m_wndMsgInfo.SetItemText(0, 1, strMsgInfo.GetTermID());
	m_wndMsgInfo.SetItemText(0, 2, strMsgInfo.GetMsgBody());

	m_wndTabs.SetActiveTab(3);

	SaveToFile( lpFileNameSuffix[LIST_MSG], *lpListCtrl[LIST_MSG] );
}

void CIBAOutputBar::UpdateMsgLog()
{
	////////////////////////查询本班次的信息////////////////////
	CClientShortMsgArray  MsgArray;
	COleDateTime Start, End;
	if (theApp.GetCurCashier()->IsOnDuty())
	{
		Start.ParseDateTime(theApp.GetCurCashier()->GetOnDutyTime());//上班时间
	}
	else
	{	//一天前
		Start = COleDateTime::GetCurrentTime();
		Start -= COleDateTimeSpan(1, 0, 0, 0);
	}
	CString strStart = Start.Format(_T("%Y-%m-%d %H:%M"));

	End = COleDateTime::GetCurrentTime();
	End += COleDateTimeSpan(1, 0, 0, 0);
	CString strEnd = End.Format(_T("%Y-%m-%d %H:%M"));

	CIBADAL::GetInstance()->GetClientShortMsg(MsgArray, strStart, strEnd);

	for (INT i = 0; i < MsgArray.GetCount(); i++)
	{
		AddMsgLog(MsgArray.GetAt(i));
	}
	////////////////////////////////////////////////
}

void CIBAOutputBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//重载，禁止双击时CContrl的响应
	CWnd::OnLButtonDblClk(nFlags, point);
}

//归化成标准10位数字，大于9位则原样返回
CString CIBAOutputBar::FormatNetId(CString strNetId)
{
	INT nLength = strNetId.GetLength();
	
	if (nLength > 9)
	{
		return strNetId;
	}
	else
	{
		CString strRet = _T("0000000000");
		strRet = strRet.Mid(0, 10 - nLength);
		strRet.Append(strNetId);

		return strRet;
	}
}

void CIBAOutputBar::QueryConsume(COperationInfo &OperationInfo, BOOL bNeedReturnRecord,UINT nConsumeType)
{	
	//strHeader = L"时间|上网账号|姓名|消费类型|消费金额|收银员|备注|用户号";
	
	CWaitCursor wait;

	m_wndConsumeInfo.DeleteAllItems();
	m_wndTabs.SetActiveTab(3);

	if (OperationInfo.GetMemberId() == 0)
	{
		UINT nCardId = 0;

		UINT nMemberId = CServletHellper::GetMemberId(OperationInfo.GetNetId(), nCardId);

		CCurrentNetBarUser::GetInstance()->Balance.SetMemberId(nMemberId);
		CCurrentNetBarUser::GetInstance()->Balance.SetCardId(nCardId);
		CCurrentNetBarUser::GetInstance()->Balance.SetRound(0);//取整

		theApp.GetCurCashier()->DoBalance(CCurrentNetBarUser::GetInstance()->Balance);

		if (CCurrentNetBarUser::GetInstance()->Balance.GetStatusCode() == 0)
		{
			OperationInfo.SetMemberId(nMemberId);
			OperationInfo.SetUserName(CCurrentNetBarUser::GetInstance()->Balance.GetUserName());
		}
		else
		{
			return;
		}
	}

	CConsumeListMethod ConsumeListMethod;
	ConsumeListMethod.SetConsumeType(nConsumeType);//全部消费查询
	ConsumeListMethod.SetNeedBusinessName(2);

	if (OperationInfo.GetIsMember()) //临时用户不传开始时间
	{
		COleDateTime NowTime = COleDateTime::GetCurrentTime(), StartTime;

		StartTime = NowTime - COleDateTimeSpan(1, 0, 0, 0);

		if (OperationInfo.GetOperationType() > 0)
		{
			StartTime = NowTime - COleDateTimeSpan(1, 0, 0, 0);
		}
		else
		{
			if (StartTime < OperationInfo.GetOperationTime())
			{
				StartTime = OperationInfo.GetOperationTime();
			}
		}

		ConsumeListMethod.SetBeginTime(StartTime.Format(_T("%Y%m%d%H%M%S")));
	}

 	ConsumeListMethod.SetMemberId(OperationInfo.GetMemberId());
 
 	theApp.GetCurCashier()->DoConsumeList(ConsumeListMethod);
 
 	if (ConsumeListMethod.GetStatusCode() == 0)
	{
		//消费方式
		CIBAString strConsumeName; //= L"上网|充值|账户购物|增收宝消费|卡购物(旧)|退款|充值卡|增收宝充值";
		strConsumeName.LoadString(IDS_CONSUMETYPE);

		CStringArray saConsumeName;
		CIBAHelpper::SplitLine(strConsumeName, saConsumeName);

		for (INT i = 0; i < (INT)ConsumeListMethod.GetRecordCount(); i++)
		{
			//消费类型|金额|详细信息|收银员|上网账号|姓名

			m_wndConsumeInfo.InsertItem(i, saConsumeName.GetAt(ConsumeListMethod.GetConsumeType(i) - 1));

			CString strTmp;
			//时间
			strTmp = ConsumeListMethod.GetBeginTime(i);
			strTmp.Insert(2, _T("-"));
			strTmp.Insert(5, _T(" "));
			strTmp.Insert(8, _T(":"));
			//strTmp.Insert(11, _T(":"));
			strTmp = strTmp.Left(strTmp.GetLength() - 2);
			m_wndConsumeInfo.SetItemText(i, 2, strTmp);

			/**
			*消费类型0: 全部 1:上网 2:充值 3:卡购物 4：嘟嘟牛消费 5:卡购物(旧) 6:退款 7：充值卡 8：嘟嘟牛充值
			*/
			switch (ConsumeListMethod.GetConsumeType(i))
			{
			case 1://上网
				{
					if (ConsumeListMethod.GetBeginTime(i) != ConsumeListMethod.GetEndTime(i))
					{					
						CString strTmp2(ConsumeListMethod.GetEndTime(i));
						strTmp2.Insert(2, _T("-"));
						strTmp2.Insert(5, _T(" "));
						strTmp2.Insert(8, _T(":"));
						//strTmp2.Insert(11, _T(":"));

						strTmp2 = strTmp2.Left(strTmp2.GetLength() - 2);

						strTmp.Append(_T(" - "));
						strTmp.Append(strTmp2);

						m_wndConsumeInfo.SetItemText(i, 2, strTmp);
					}
				}
				//这里不要break
			case 3:
			case 4:
			case 5://3卡购物 4：嘟嘟牛消费 5:卡购物(旧)4种消费是支出方式
				strTmp.Format(_T("%.2f"), -ConsumeListMethod.GetAmount(i) / 100.0);
				m_wndConsumeInfo.SetItemText(i, 1, strTmp);
				break;

			case 2:
			case 7:
			case 8://充值，充值卡和嘟嘟牛充值，为收入方式
				strTmp.Format(_T("%.2f"), ConsumeListMethod.GetAmount(i) / 100.0);
				m_wndConsumeInfo.SetItemText(i, 1, strTmp);
				break;

			case 6://退款
				strTmp.Format(_T("%.2f"), ConsumeListMethod.GetAmount(i) / 100.0);
				m_wndConsumeInfo.SetItemText(i, 1, strTmp);
				bNeedReturnRecord = FALSE;
				break;
			}

			strTmp = m_wndConsumeInfo.GetItemText(i, 2);
			strTmp.Append(_T(" "));
			strTmp.Append(ConsumeListMethod.GetDescription(i));
			m_wndConsumeInfo.SetItemText(i, 2, strTmp);

			strTmp = ConsumeListMethod.GetBusinessName(i);
			m_wndConsumeInfo.SetItemText(i, 3, strTmp);

			//上网账号
			m_wndConsumeInfo.SetItemText(i, 4, OperationInfo.GetNetId());
			//姓名
			m_wndConsumeInfo.SetItemText(i, 5, OperationInfo.GetUserName());
		}

		if (bNeedReturnRecord) //如果中心没同步过来，则自己补上一条退款记录
		{
			m_wndConsumeInfo.InsertItem(0, saConsumeName.GetAt(5));

			CString strTmp;

			strTmp.Format(_T("%.2f"), OperationInfo.GetOperationAmount() / 100.0);
			
			m_wndConsumeInfo.SetItemText(0, 1, strTmp);

			m_wndConsumeInfo.SetItemText(0, 2, COleDateTime::GetCurrentTime().Format(_T("%m-%d %H:%M")));

			m_wndConsumeInfo.SetItemText(0, 3, theApp.GetCurCashier()->GetName());
			//上网账号
			m_wndConsumeInfo.SetItemText(0, 4, OperationInfo.GetNetId());
			//姓名
			m_wndConsumeInfo.SetItemText(0, 5, OperationInfo.GetUserName());
		}
	}
}

LRESULT CIBAOutputBar::OnOperationLog(WPARAM wp, LPARAM lp)
{
	WORD wKind = LOWORD(wp);
	switch (wKind)
	{
	case 0:
		{
			COperationInfo* pOperationInfo = (COperationInfo*)lp;
			AddCheckOutLog(*pOperationInfo);
			AddOperationLog(*pOperationInfo);
			m_wndListInfo.ActiveRemind();
		}

		break;

	case 1:

		AddActiveLog();
		break;

	case 2:
		{
			COperationInfo* pOperationInfo = (COperationInfo*)lp;
			QueryConsume(*pOperationInfo,FALSE,HIWORD(wp));
			m_wndConsumeInfo.ActiveRemind();
		}

		break;
	}

	

	return 0;
}

void CIBAOutputBar::RefreshAllInfoList()
{
	for (int i =0; i < LIST_COUNT; i++)
	{
		RefreshInfoList(lpFileNameSuffix[i], *lpListCtrl[i]);
	}
}

void CIBAOutputBar::RefreshInfoList(CString strFileSuffix, COutputListCtrl& listCtrl)
{
	listCtrl.DeleteAllItems();

	CString strFileName = theApp.GetWorkPath();
	strFileName += _T("\\IBAConfig\\operateInfo\\");
	strFileName += theApp.GetCurCashier()->GetName();
	strFileName += strFileSuffix;

	if( !::PathFileExists(strFileName) )
	{
		return ;
	}

	CString strInfo;
	CIBAHelpper::ReadFile(strFileName,strInfo);
	CStringArray arrStrOperations;
	CIBAHelpper::SplitLine(strInfo,arrStrOperations,_T('\n'));

	int nLines = arrStrOperations.GetCount();

	for(int i = 0 ; i < nLines ; i ++)
	{
		CString& strLine = arrStrOperations[i];
		CStringArray strArray;
		CIBAHelpper::SplitLine(strLine,strArray,_T('|'));

		listCtrl.InsertItem(0,_T(""));

		IBA_ASSERT2(strArray.GetCount() == listCtrl.GetHeaderCtrl().GetItemCount(),"输出记录文件的字段的个数要与列表的列数相等");
		
		for(int j = 0; j < strArray.GetCount(); j++ )
		{
			listCtrl.SetItemText(0, j, strArray[j]);
		}
	}
	//
}

void CIBAOutputBar::DeleteDutyFile()
{
	for(int i = 0; i < LIST_COUNT; i++)
	{
		DeleteListFile(lpFileNameSuffix[i]);
	}
}

void CIBAOutputBar::SaveToFile(CString strFileSuffix, COutputListCtrl& listCtrl)
{
	int i = 0;
	int nColCount = listCtrl.GetHeaderCtrl().GetItemCount();
	CString str;
	while( i <  nColCount )
	{
		CString strTmp;
		strTmp = listCtrl.GetItemText(0,i);
		str += strTmp;
		str += _T("|");

		i++;
	}
	str += _T("\n");

	CString strFileName = theApp.GetWorkPath();
	strFileName += _T("\\IBAConfig\\operateInfo\\");

	CIBAHelpper::MakeSurePathExists(strFileName); // 创建目录

	strFileName += theApp.GetCurCashier()->GetName();
	strFileName += strFileSuffix;

	CIBAHelpper::AppendWriteFile(strFileName,str);
}

void CIBAOutputBar::DeleteListFile(CString strFileSuffix)
{
	CString strFileName = theApp.GetWorkPath();
	strFileName += _T("\\IBAConfig\\operateInfo\\");
	strFileName += theApp.GetCurCashier()->GetName();

	strFileName += strFileSuffix;

	if( !::PathFileExists(strFileName) )
	{
		return ;
	}
	::DeleteFile(strFileName);
}

void CIBAOutputBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	int TabIndex = m_wndTabs.GetActiveTab();
	switch (TabIndex)
	{
	case 2:
		{
			DWORD dwPos = GetMessagePos();
			CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

			CMenu menu;
			VERIFY( menu.LoadMenu( IDR_MENU_CHECKOUT ) );
			CMenu* popup = menu.GetSubMenu(0);
			ASSERT( popup != NULL );
			popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
			break;
		}
	default:
		CBCGPDockingControlBar::OnContextMenu(pWnd, point) ;
		break;
	}
	//return ;
}

void CIBAOutputBar::OnExportExcel()
{
	UpdateData();

	//// 打开存储文件对话框
	//CFileDialog dlgSave(FALSE, _T("xls"), _T("结账记录表"));
	//dlgSave.m_ofn.lpstrFilter = _T("*.xls");
	//if(IDOK != dlgSave.DoModal())
	//	return;

	//// 获取全路径
	//CString strFullPath;
	//strFullPath = dlgSave.GetPathName();

	// 获取行数和列数
	int nRowCount = m_listCheckOut.GetItemCount();
	CBCGPHeaderCtrl& Header = m_listCheckOut.GetHeaderCtrl();
	int nColCount = Header.GetItemCount();


	CString strAppend("");
	CString strTmp("");

	for(int nRow = 0; nRow < nRowCount; nRow++)
	{
		for(int nCol = 0; nCol < nColCount; nCol++)
		{
			strTmp.Empty();
			strTmp = m_listCheckOut.GetItemText(nRow, nCol);
			// "\t" 表示跳到下一个单元格
			strAppend += strTmp + _T("\t");
		}
		// "\n" 表示跳到下一行
		strAppend += _T("\n");	
	}

	CString strFullPath;
	GetTempPath(MAX_PATH, strFullPath.GetBuffer(MAX_PATH));
	strFullPath.ReleaseBuffer();

	strFullPath += _T("结账记录表.xls");

	if(PathFileExists(strFullPath))
	{
		while(!DeleteFile(strFullPath))
		{
			static UINT nCount = 0;
			CString strReplace("");
			strReplace.Format(_T("tmp%d.xls"), nCount++);
			strFullPath.Replace(_T(".xls"), strReplace);		// 如果文件无法删除，就修改文件名

			if(nCount > 5)		// 超过5个就不要再尝试了
				break;
		}
	}

	// nRowCount+1 数据行数加上一行标题行
	m_ExpThread.SetParam(strAppend, strFullPath, nRowCount, nColCount);
	m_ExpThread.Start();
}
