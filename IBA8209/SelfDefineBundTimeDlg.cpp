// SelfDefineBundTimeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IBAHelpper.h"
#include "Tts\IBATTS.h"
#include ".\selfdefinebundtimedlg.h"
#include "Socket\LocalServer.h"

// CDlgBundTimeSelfDef 对话框

IMPLEMENT_DYNAMIC(CDlgBundTimeSelfDef, CIBADialog)
CDlgBundTimeSelfDef::CDlgBundTimeSelfDef(CWnd* pParent /*=NULL*/)
: CIBADialog(CDlgBundTimeSelfDef::IDD, pParent)
, m_nMemberId(0), m_bIsMember(FALSE)
{
	
}

CDlgBundTimeSelfDef::~CDlgBundTimeSelfDef()
{
}

void CDlgBundTimeSelfDef::DoDataExchange(CDataExchange* pDX)
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


BEGIN_MESSAGE_MAP(CDlgBundTimeSelfDef, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ENDTIME, OnEndTimeChanged)
	ON_EN_CHANGE(IDC_EDIT_HOUR, OnEnChangeEditHour)
	ON_EN_CHANGE(IDC_EDIT_MIN, OnEnChangeEditMin)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgBundTimeSelfDef 消息处理程序
// 初始化时间控件
BOOL CDlgBundTimeSelfDef::InitTimeCtrl(void)
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
BOOL CDlgBundTimeSelfDef::InitTimeDate(void)
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

BOOL CDlgBundTimeSelfDef::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	InitOtherCtrl();

	InitTimeCtrl();

	InitTimeDate();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 初始化控件
void CDlgBundTimeSelfDef::InitOtherCtrl(void)
{
	CIBAHelpper::CreateFont(m_BigFont, 22, 900);

	m_edtBundTimeMoney.SetFont(&m_BigFont);
	m_edtBundTimeMoney.DisableMask();
	m_edtBundTimeMoney.SetValidChars(_T("0123456789."));
	m_edtBundTimeMoney.SetLimitText(8);

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

	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		GetDlgItem(IDC_STATIC0)->ShowWindow(SW_HIDE);
		m_cboAccountType.ShowWindow(SW_HIDE);

		CIBAString strTmp;
		strTmp.LoadString(IDS_BTNEEDMONEY);
		GetDlgItem(IDC_STATIC_CAPTION)->SetWindowText(strTmp);
	}
}

void CDlgBundTimeSelfDef::OnBnClickedOk()
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

	if (IDOK != MsgBox(IDS_ASKBUNDLETIME, MB_OKCANCEL | MB_ICONQUESTION))
	{
		return;
	}

	CWaitCursor Wait;

	CBundleTimeMethod BundleTime;

	BundleTime.SetMemberId(m_nMemberId);
	BundleTime.SetReserve(3);
	BundleTime.SetTimeId(0);
	BundleTime.SetBeginTime(startTime.Format(_T("%Y%m%d%H%M%S")));
	BundleTime.SetTimePass(nAllMinites);

	INT nIdx = m_cboArea.GetCurSel();
	
	BundleTime.SetPcClass(m_cboArea.GetItemData(nIdx));
	BundleTime.SetAccountType(m_cboAccountType.GetCurSel());
	
	double dBundTimeMoney = _tstof(strMoney);
	UINT nBundTimeMoney = (dBundTimeMoney + 0.005) * 100;	//作舍入转换
	BundleTime.SetBundleAmount(nBundTimeMoney);
	
	if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
	{
		BundleTime.SetCreditAmount(nBundTimeMoney);
	}

	theApp.GetCurCashier()->DoBundleTime(BundleTime);

	if (BundleTime.GetStatusCode() >= 0 && BundleTime.GetNeedRelogin())
	{
		CActiveMember ActiveMember;

		if (CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(m_nMemberId, ActiveMember)) //在线包时
		{
			CActiveMemberList::TReLoginRequest MyReLoginRequest = {0};
			MyReLoginRequest.lastRefNo = ActiveMember.GetRefNo();
			MyReLoginRequest.memberId = m_nMemberId;
			MyReLoginRequest.firstTryTime = GetTickCount();

			CLocalServer::GetInstance()->ActiveMemberList.AddReLoginRequest(MyReLoginRequest);
			CLocalServer::GetInstance()->LocalBundleTime(m_nMemberId);
		}
	}

	if (BundleTime.GetStatusCode() == 0)
	{
		// 2011/07/29-8201-gxx: 更新现金包时时，预付款和明细的刷新
		if (CNetBarConfig::GetInstance()->GetBundleTimeWithCash(m_bIsMember))
		{
			CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(m_nMemberId, nBundTimeMoney, 0, 0);
		}

		CIBATTS::GetInstance()->SpeakBundleTimeResult(BundleTime.GetAmount());

		strRet = LOAD_STRING(IDS_BTSUCCESSED);

		MsgBox(strRet);

		OnOK();

		return;
	}
	else
	{
		strRet = BundleTime.GetStatusMessage();
		SetToolTipPos(IDOK);
		ShowToolTip(strRet, TRUE);
	}
}

BOOL CDlgBundTimeSelfDef::PreTranslateMessage(MSG* pMsg)
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

void CDlgBundTimeSelfDef::OnOK()
{
	CBCGPDialog::OnOK();
}

void CDlgBundTimeSelfDef::OnCancel()
{
	CBCGPDialog::OnCancel();
}

void CDlgBundTimeSelfDef::OnEndTimeChanged()
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

void CDlgBundTimeSelfDef::OnEnChangeEditHour()
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

void CDlgBundTimeSelfDef::OnEnChangeEditMin()
{
	OnEnChangeEditHour();
}

HBRUSH CDlgBundTimeSelfDef::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
