// ClearDataSetPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "ClearDataSetPage.h"
#include "Dal\IBADAL.h"
#include "IBAHelpper.h"


// CClearDataSetPage 对话框

IMPLEMENT_DYNAMIC(CClearDataSetPage, CIBAPage)
CClearDataSetPage::CClearDataSetPage(CWnd* pParent /*=NULL*/)
	: CIBAPage(CClearDataSetPage::IDD)
{
}

CClearDataSetPage::~CClearDataSetPage()
{
}

void CClearDataSetPage::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_BTN_ENDTIME, m_EndTime);
}


BEGIN_MESSAGE_MAP(CClearDataSetPage, CIBAPage)
	ON_BN_CLICKED(IDC_BTN_CLAEAN_SHORTMSG, OnBnClickedClearShortMsg)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CClearDataSetPage 消息处理程序

BOOL CClearDataSetPage::OnInitDialog()
{
	CIBAPage::OnInitDialog();

	InitTimeCtrl();

	m_ToolTip.Create(this);
	m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	m_ToolTip.SetEffectBk(12);
	m_ToolTip.SetBorder(RGB(200, 200, 200));

	return TRUE;
}

// 初始化时间控件
void CClearDataSetPage::InitTimeCtrl(void)
{
	CIBAHelpper::CreateFont(m_Font, 14);

	//时间按钮控件的设置
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

void CClearDataSetPage::OnBnClickedClearShortMsg()
{
	COleDateTime oleBegin = m_StartTime.GetDate();
	COleDateTime oleEnd = m_EndTime.GetDate();

	CString strSql, strBeginTime, strEndTime;
	strBeginTime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"), 
		oleBegin.GetYear(), oleBegin.GetMonth(), oleBegin.GetDay(), 
		oleBegin.GetHour(), oleBegin.GetMinute(), oleBegin.GetSecond());
	strEndTime.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"), 
		oleEnd.GetYear(), oleEnd.GetMonth(), oleEnd.GetDay(), 
		oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());

	strSql.Format(_T("Delete FROM shortMessage Where SendTime Between '%s' And '%s'"),
		strBeginTime, strEndTime);

	SetToolTipPos(IDC_BTN_CLAEAN_SHORTMSG);
	CIBAString strToolTip;
	if(CIBADAL::GetInstance()->ExecuteSQL(strSql))
	{
		strToolTip.LoadString(IDS_CLEAR_SHORTMSG_SUCCESSED);
		m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	}
	else
	{
		strToolTip.LoadString(IDS_CLEAR_SHORTMSG_FAILED);
		m_ToolTip.SetColorBk(RGB(255, 100, 100), RGB(255, 120, 120), RGB(255, 100, 100));
	}
	m_ToolTip.ShowHelpTooltip(&m_ToolTipRect.TopLeft(), strToolTip);
	SetFocus();
}

BOOL CClearDataSetPage::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CIBAPage::PreTranslateMessage(pMsg);
}

void CClearDataSetPage::SetToolTipPos(UINT uID)
{
	if (GetDlgItem(uID) != NULL)
	{
		GetDlgItem(uID)->GetWindowRect(m_ToolTipRect);
	}
	else
	{
		GetDlgItem(IDOK)->GetWindowRect(m_ToolTipRect);
	}
}

void CClearDataSetPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		m_ToolTip.HideTooltip();
	}

	CIBAPage::OnShowWindow(bShow, nStatus);
}
