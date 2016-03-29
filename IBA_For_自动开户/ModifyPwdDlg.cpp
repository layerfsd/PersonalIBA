// ModifyPwdDlg.cpp : 实现文件
//修改密码对话框

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include ".\modifypwddlg.h"
#include "Socket\LocalServer.h"
#include "ActiveMemberList.h"
#include "IBAHelpper.h"
#include "Encrypt.h"

// CModifyPwdDlg 对话框

IMPLEMENT_DYNAMIC(CModifyPwdDlg, CNDDialog)
CModifyPwdDlg::CModifyPwdDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CModifyPwdDlg::IDD, pParent)
{
	m_strIdCardSN = theApp.GetLastNetID();
}

CModifyPwdDlg::~CModifyPwdDlg()
{
}

void CModifyPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberID);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strIdCardSN);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
}


BEGIN_MESSAGE_MAP(CModifyPwdDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CModifyPwdDlg 消息处理程序

void CModifyPwdDlg::OnBnClickedOk()
{
	UpdateData();

	CWaitCursor Wait;

	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		if (m_LocalActiveMember.GetIsLocalUser() == FALSE)
		{
			theApp.IBAMsgBox(_T("断网情况下，不允许中心用户修改密码"));
			AutoClose();
			return;
		}

		DoLocalModeOperate();

		AutoClose();
	}
	else if (m_LocalActiveMember.GetIsLocalUser())
	{
		DoLocalModeOperate();

		AutoClose();
	}
	else
	{
		CResetPwdMethod ResetPwd;

		ResetPwd.SetMemberId(_ttoi(m_strMemberID));
		ResetPwd.SetPassword(m_strPwd);
		ResetPwd.SetPersonalId(m_strPersonalID);

		theApp.GetCurCashier()->DoResetPwd(ResetPwd);

		GetDlgItem(IDOK)->GetWindowRect(m_ToolTipRect);

		SetToolTipPos(IDOK);

		if (ResetPwd.GetStatusCode() == 0)
		{
			theApp.IBAMsgBox(IDS_MODIFYPWDSUCCESS);
		}
		else
		{	
			theApp.IBAMsgBox(IDS_MODIFYPWDFAILE);
		}

		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

		AutoClose();
	}
}


BOOL CModifyPwdDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	m_strIdCardSN = m_strNetId;

	UpdateData(FALSE);

	//CIBAHelpper::CreateFont(m_BigFont, 22, 900);
	//自动查询在刷卡及扫描证件的情况下采用
	if (m_bAutoQuery)
	{
		SetTimer(TimeIdOfAutoQuery, 100, NULL);//只查询一次
	}
	else
	{
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	}

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_BigFont);
	((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->SetLimitText(8);

	AddFocusEnter(IDC_EDIT_PASSWORD, IDOK);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CModifyPwdDlg::ClearData()
{
	m_strPwd.Empty();
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strNetBar.Empty();
	m_strPersonalID.Empty();
	m_strUserClass.Empty();
	m_strAllBalance.Empty();
	GetDlgItem(IDOK)->EnableWindow(FALSE);

}

void CModifyPwdDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);
	
	QueryBalance(nMemberId, nCardId);

	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	UpdateData(FALSE);

}

void CModifyPwdDlg::OnNewCard(UINT nCardId)
{
	((CEdit *)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDOK))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_IDCARDSNO))->SetFocus();

	CNDDialog::OnNewCard( nCardId );
}

void CModifyPwdDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT m_nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
	m_strAllBalance.Format(_T("%.2f"), m_nAllMoney / 100.0);

	((CEdit*)GetDlgItem(IDC_EDIT_PASSWORD))->EnableWindow(TRUE);
}

void CModifyPwdDlg::OnTimer(UINT nIDEvent)
{
	if (TimeIdOfAutoQuery == nIDEvent)
	{
		KillTimer(nIDEvent);

		if (m_bAutoQuery)
		{
			m_bAutoQuery = FALSE;
			OnBnClickedButtonQuery();
		}
	}
	CNDDialog::OnTimer(nIDEvent);
}

BOOL CModifyPwdDlg::DoLocalModeQuery()
{
	BOOL bRet = CNDDialog::DoLocalModeQuery();
	if (bRet)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(CNetBarConfig::GetInstance()->GetNetBarId());
		m_strMemberID.Format(_T("%d,本地用户"), (int)m_nMemberId);
		UpdateData(FALSE);
	}
	return bRet;
}

BOOL CModifyPwdDlg::DoLocalModeOperate()
{
	m_strPwd = CIBAHelpper::EncodePassword(m_strPwd);
	m_LocalActiveMember.SetPassword(m_strPwd);

	BOOL bRet = CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMemberDB(m_LocalActiveMember);
	
	if (bRet)
	{
		theApp.IBAMsgBox(IDS_MODIFYPWDSUCCESS);
	}
	else
	{	
		theApp.IBAMsgBox(IDS_MODIFYPWDFAILE);
	}

	return bRet;
}

