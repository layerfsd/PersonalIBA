// NDTermConsumeView.cpp : 实现文件
//
#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "Cashier.h"
#include "Socket\LocalServer.h"
#include "dal\IBADAL.h"
#include ".\ndtermconsumeview.h"


// CNDTermConsumeView

IMPLEMENT_DYNCREATE(CNDTermConsumeView, CIBAFormView)

CNDTermConsumeView::CNDTermConsumeView()
: CIBAFormView(CNDTermConsumeView::IDD)
{
}

CNDTermConsumeView::~CNDTermConsumeView()
{
}

void CNDTermConsumeView::DoDataExchange(CDataExchange* pDX)
{
	CIBAFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_strResult);
	DDX_Control(pDX, IDC_LIST_CONSUMELIST, m_ConsumeList);
	DDX_Control(pDX, IDC_COMBO_TERMID, m_cboTermID);
	DDX_Control(pDX, IDC_BUTTON_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_BUTTON_ENDTIME, m_EndTime);
}

BEGIN_MESSAGE_MAP(CNDTermConsumeView, CIBAFormView)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_WM_SIZE()
	//ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CNDTermConsumeView 消息处理程序

void CNDTermConsumeView::OnInitialUpdate()
{
	CIBAFormView::OnInitialUpdate();

	InitListView();
	InitTimeControl();

	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_Font);
	m_cboTermID.SetFont(&m_Font);

	SetTimer(1000, 1000 * 60, NULL);
}

void CNDTermConsumeView::InitListView()
{
	CIBAString strHeader;
	strHeader.LoadString(IDS_TERMCONSUMELISTHEADER);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {150, 80, 150, 130, 130, 70, 80};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_ConsumeList.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_ConsumeList.SetFont(&m_Font);
	m_ConsumeList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CNDTermConsumeView::OnBnClickedButtonQuery()
{
	CString strTermId;
	m_cboTermID.GetWindowText(strTermId);
	strTermId.Trim();

	if (strTermId.IsEmpty())
	{
		SetToolTipPos(IDC_COMBO_TERMID);
		ShowToolTip(IDS_INPUTERROR);
		return;
	}

	CTermConsumeMethod TermConsume;
	TermConsume.SetTermed(strTermId);

	CString strTmp = m_StartTime.ToString();
	strTmp.Remove('-');
	strTmp.Remove(':');
	strTmp.Remove(' ');
	strTmp.Append(_T("00"));
	TermConsume.SetBeginTime(strTmp);

	strTmp = m_EndTime.ToString();
	strTmp.Remove('-');
	strTmp.Remove(':');
	strTmp.Remove(' ');
	strTmp.Append(_T("00"));
	TermConsume.SetEndTime(strTmp);

	theApp.GetCurCashier()->DoTermConsume(TermConsume);

	m_ConsumeList.DeleteAllItems();

	UINT nAllMoney = 0; 

	if (TermConsume.GetStatusCode() == 0)
	{
		for (INT i = 0; i < (INT)TermConsume.GetRecordCount(); i++)
		{
			CNetBarUser NetBarUser;
			CIBADAL::GetInstance()->GetNetBarUserInfo(TermConsume.GetMemberId(i), NetBarUser);
			
			m_ConsumeList.InsertItem(i, NetBarUser.GetNetId());
			m_ConsumeList.SetItemText(i, 1, NetBarUser.GetUserName());
			m_ConsumeList.SetItemText(i, 2, NetBarUser.GetCertificateID());

			CString strTmp;

			strTmp = TermConsume.GetBeginTime(i);
			strTmp.Insert(2, _T("-"));
			strTmp.Insert(5, _T(" "));
			strTmp.Insert(8, _T(":"));
			strTmp.Insert(11, _T(":"));

			m_ConsumeList.SetItemText(i, 3, strTmp);

			strTmp = TermConsume.GetEndTime(i);
			strTmp.Insert(2, _T("-"));
			strTmp.Insert(5, _T(" "));
			strTmp.Insert(8, _T(":"));
			strTmp.Insert(11, _T(":"));

			m_ConsumeList.SetItemText(i, 4, strTmp);
			strTmp.Format(_T("%.2f"), TermConsume.GetAmount(i) / 100.0);
			m_ConsumeList.SetItemText(i, 5, strTmp);

	
			strTmp.Format(_T("%d"), TermConsume.GetMemberId(i));
			m_ConsumeList.SetItemText(i, 6, strTmp);

			m_ConsumeList.SetItemData(i, i);

			nAllMoney += TermConsume.GetAmount(i);
		}

		//if (TermConsume.GetRecordCount() == 0)
		//{
		//	SetToolTipPos(IDC_BUTTON_QUERY);
		//	ShowToolTip(IDS_NORECORD);
		//}
		//else
		//{
			m_strResult.Format(IDS_TERMCONSUMERESULT, TermConsume.GetRecordCount(), nAllMoney / 100.0);
			UpdateData(FALSE);
		//}
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_QUERY);
		ShowToolTip(TermConsume.GetStatusMessage(), TRUE);
	}
}

void CNDTermConsumeView::OnSize(UINT nType, int cx, int cy)
{
	MoveWindow(0, 0, cx, cy, FALSE);

	if (m_ConsumeList.GetSafeHwnd() != NULL)
	{
		m_ConsumeList.MoveWindow(10, 100, cx - 20, cy - 105, TRUE);

		InitData();
	}
}

void CNDTermConsumeView::OnClose()
{
	INT nIdx = theApp.GetIBAView()->FindTab(GetSafeHwnd());
	theApp.GetIBAView()->GetTabControl().ShowTab(nIdx, FALSE);

	m_ConsumeList.DeleteAllItems();
	m_strResult.Empty();
	UpdateData(FALSE);
}

void CNDTermConsumeView::InitData()
{
	if (m_cboTermID.GetCount() > 0)
	{
		return;
	}

	COleDateTime dtTmp;

	if (theApp.GetCurCashier()->IsOnDuty())
	{
		dtTmp.ParseDateTime(theApp.GetCurCashier()->GetOnDutyTime());
	}
	else
	{
		dtTmp = COleDateTime::GetCurrentTime();
		dtTmp -= COleDateTimeSpan(1, 0, 0, 0);
	}

	m_StartTime.SetDate(dtTmp);

	dtTmp = COleDateTime::GetCurrentTime();
	dtTmp += COleDateTimeSpan(1, 0, 1, 0);
	m_EndTime.SetDate(dtTmp);

	for (INT i = 0; i < CLocalServer::GetInstance()->ComputerList.GetCount(); i++)
	{
		m_cboTermID.AddString(CLocalServer::GetInstance()->ComputerList.GetComputer(i).GetTerminalID());
	}
}

HBRUSH CNDTermConsumeView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBAFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	return hbr;
}

void CNDTermConsumeView::QueryTermId(LPCTSTR lpszTermId)
{
	m_cboTermID.SetWindowText(lpszTermId);
	OnBnClickedButtonQuery();
}

void CNDTermConsumeView::OnTimer(UINT nIDEvent)
{
	COleDateTime dtTmp;

	dtTmp = COleDateTime::GetCurrentTime();
	dtTmp += COleDateTimeSpan(0, 0, 1, 0);
	m_EndTime.SetDate(dtTmp);

	CIBAFormView::OnTimer(nIDEvent);
}

void CNDTermConsumeView::InitTimeControl()
{
	const UINT stateMask = 
		CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR | 
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_CHECKBOX |
		CBCGPDateTimeCtrl::DTM_TIME | 
		CBCGPDateTimeCtrl::DTM_TIME24HBYLOCALE;

	UINT stateFlags = 
		CBCGPDateTimeCtrl::DTM_SPIN |
		CBCGPDateTimeCtrl::DTM_DROPCALENDAR | 
		CBCGPDateTimeCtrl::DTM_DATE |
		CBCGPDateTimeCtrl::DTM_TIME24H |
		CBCGPDateTimeCtrl::DTM_TIME;  

	m_StartTime.m_monthFormat = 2;
	m_StartTime.SetState(stateFlags, stateMask);
	m_StartTime.SetFont(&m_Font);
	m_StartTime.SizeToContent();

	m_EndTime.m_monthFormat = 2;
	m_EndTime.SetState(stateFlags, stateMask);
	m_EndTime.SetFont(&m_Font);
	m_EndTime.SizeToContent();
}
