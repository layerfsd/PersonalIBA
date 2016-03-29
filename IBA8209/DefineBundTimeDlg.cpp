// SelfDefineBundTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include ".\DefineBundTimeDlg.h"

// CDefineBundleTimeDlg 对话框

IMPLEMENT_DYNAMIC(CDefineBundleTimeDlg, CIBADialog)
CDefineBundleTimeDlg::CDefineBundleTimeDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(IDD, pParent)
{
	m_BundleTimeInfo.bIsSelected = FALSE;
}

CDefineBundleTimeDlg::~CDefineBundleTimeDlg()
{
}

void CDefineBundleTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON_STARTTIME, m_btnStartTime);
	DDX_Control(pDX, IDC_BUTTON_ENDTIME, m_btnEndTime);
	DDX_Control(pDX, IDC_COMBO_ZONE, m_cboArea);
	DDX_Control(pDX, IDC_COMBO_ACCOUNTTYPE, m_cboAccountType);
	DDX_Control(pDX, IDC_EDIT_BUNDTIMEMONEY, m_edtBundTimeMoney);
	DDX_Control(pDX, IDC_EDIT_HOUR, m_edtHours);
	DDX_Control(pDX, IDC_EDIT_MIN, m_edtMinutes);
}


BEGIN_MESSAGE_MAP(CDefineBundleTimeDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ENDTIME, OnEndTimeChanged)
	ON_EN_CHANGE(IDC_EDIT_HOUR, OnEnChangeEditHour)
	ON_EN_CHANGE(IDC_EDIT_MIN, OnEnChangeEditMin)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDefineBundleTimeDlg 消息处理程序
// 初始化时间控件
BOOL CDefineBundleTimeDlg::InitTimeCtrl(void)
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

	m_btnStartTime.m_monthFormat = 2;
	m_btnStartTime.SetState(stateFlags, stateMask);
	m_btnStartTime.SetFont(&m_Font);
	m_btnStartTime.SizeToContent();

	m_btnEndTime.m_monthFormat = 2;
	m_btnEndTime.SetState(stateFlags, stateMask);
	m_btnEndTime.SetFont(&m_Font);
	m_btnEndTime.SizeToContent();

	GetDlgItem(IDC_EDIT_HOUR)->SetFont(&m_Font);
	GetDlgItem(IDC_EDIT_MIN)->SetFont(&m_Font);

	return TRUE;
}

// 初始化时间控件的数据
BOOL CDefineBundleTimeDlg::InitTimeDate(void)
{
	//开始时间
	COleDateTime tTime = COleDateTime::GetCurrentTime();
	
	m_btnStartTime.SetDate(tTime);

	CString strTmp;
	strTmp.Format(_T("%d"), 8);
	m_edtHours.SetWindowText(strTmp);

	strTmp.Format(_T("%d"), 0);
	m_edtMinutes.SetWindowText(strTmp);

	return TRUE;
}

BOOL CDefineBundleTimeDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitOtherCtrl();

	InitTimeCtrl();

	InitTimeDate();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 初始化控件
void CDefineBundleTimeDlg::InitOtherCtrl(void)
{
	CIBAHelpper::CreateFont(m_BigFont, 22, 900);

	m_edtBundTimeMoney.SetFont(&m_BigFont);
	m_edtBundTimeMoney.DisableMask();
	m_edtBundTimeMoney.SetValidChars(_T("0123456789."));
	m_edtBundTimeMoney.SetLimitText(6);

	m_cboArea.SetItemData(0, 0);

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetCount(); i++)
	{
		INT nTmp = m_cboArea.AddString(CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetName());
		m_cboArea.SetItemData(nTmp, CNetBarConfig::GetInstance()->m_PCClassInfoArray.GetAt(i).GetID());
	}

	m_cboArea.SetCurSel(0);
	m_cboAccountType.SetCurSel(0);

	m_cboArea.SetFont(&m_Font);
	m_cboAccountType.SetFont(&m_Font);
	m_cboAccountType.SetCurSel(1);
}

void CDefineBundleTimeDlg::OnBnClickedOk()
{
	CString strRet;

	COleDateTime startTime;
	startTime.ParseDateTime(m_btnStartTime.ToString());
	
	COleDateTime endTime;
	endTime.ParseDateTime(m_btnEndTime.ToString());

	COleDateTimeSpan sp = endTime - startTime;

	UINT nAllMinites = (LONG)sp.GetTotalMinutes();

	if (nAllMinites <= 0)
	{
		strRet = _T("包时时间有误，请重新选择！");
		SetToolTipPos(IDOK);
		ShowToolTip(strRet);
		return;
	}

	CString strMoney;
	m_edtBundTimeMoney.GetWindowText(strMoney);

	if (strMoney.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_BUNDTIMEMONEY)->SetFocus();
		return;
	}
	
	CWaitCursor Wait;

	double dBundTimeMoney = _tstof(strMoney);
	UINT nBundTimeMoney = (dBundTimeMoney + 0.005) * 100;	//作舍入转换
	INT nIdx = m_cboArea.GetCurSel();

	//2011-4-18-gxx: 保存开户时自定义包时的参数设置
	m_BundleTimeInfo.TimeId = 9999;
	m_BundleTimeInfo.BeginTime = startTime.Format(_T("%Y%m%d%H%M%S"));
	m_BundleTimeInfo.TimePass = nAllMinites;
	m_BundleTimeInfo.Amount = nBundTimeMoney;
	m_BundleTimeInfo.PcClass = m_cboArea.GetItemData(nIdx);
	m_BundleTimeInfo.AccountType = m_cboAccountType.GetCurSel();
	
	m_BundleTimeInfo.bIsSelected = TRUE;

	OnOK();

}

BOOL CDefineBundleTimeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd * pWnd = GetFocus();
		if (pWnd != NULL)
		{
			if (pWnd->GetDlgCtrlID() == IDC_EDIT_BUNDTIMEMONEY)
			{
				OnBnClickedOk();
				return TRUE;
			}
		}
	}

	return CIBADialog::PreTranslateMessage(pMsg);
}

void CDefineBundleTimeDlg::OnOK()
{
	CBCGPDialog::OnOK();
}

void CDefineBundleTimeDlg::OnCancel()
{
	CBCGPDialog::OnCancel();
}

void CDefineBundleTimeDlg::OnEndTimeChanged()
{
	COleDateTime startTime;
	startTime.ParseDateTime(m_btnStartTime.ToString());

	COleDateTime endTime;
	endTime.ParseDateTime(m_btnEndTime.ToString());

	if (endTime <= startTime)
	{
		m_btnEndTime.SetDate(startTime);
		endTime.ParseDateTime(m_btnEndTime.ToString());
	}

	COleDateTimeSpan sp = endTime - startTime;

	INT nHours = (long)sp.GetTotalHours();
	INT nMinutes = sp.GetMinutes();

	CString strTmp;
	strTmp.Format(_T("%d"), nHours);
	m_edtHours.SetWindowText(strTmp);

	strTmp.Format(_T("%d"), nMinutes);
	m_edtMinutes.SetWindowText(strTmp);

	TRACE("OnEndTimeChanged\n");
}

void CDefineBundleTimeDlg::OnEnChangeEditHour()
{
	CString strTmp;
	m_edtHours.GetWindowText(strTmp);
	INT nHours = _ttoi(strTmp);

	m_edtMinutes.GetWindowText(strTmp);
	INT nMinutes = _ttoi(strTmp);

	COleDateTime startTime;
	startTime.ParseDateTime(m_btnStartTime.ToString());

	startTime += COleDateTimeSpan(0, nHours, nMinutes, 0);

	m_btnEndTime.SetDate(startTime);
}

void CDefineBundleTimeDlg::OnEnChangeEditMin()
{
	OnEnChangeEditHour();
}

HBRUSH CDefineBundleTimeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CIBADialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_EDIT)
	{
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_BUNDTIMEMONEY)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}

	return hbr;
}
