// MergeMember.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "MergeMemberDlg.h"
#include "IBAHelpper.h"
#include "Cashier.h"

#include ".\Socket\LocalServer.h"

// CMergeMemberDlg 对话框

IMPLEMENT_DYNAMIC(CMergeMemberDlg, CNDDialog)
CMergeMemberDlg::CMergeMemberDlg(CWnd* pParent /*=NULL*/)
: CNDDialog(CMergeMemberDlg::IDD, pParent)
, m_nUserClassID(0), m_nAllMoney(0)
, m_nUserClassID2(0), m_nAllMoney2(0)
, m_bPrimary(TRUE)
{
}

CMergeMemberDlg::~CMergeMemberDlg()
{
}

void CMergeMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strNetId);
	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberId);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalId);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetbarName);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strClassName);
	DDX_Text(pDX, IDC_STATIC_WSCZ, m_strWSAmount);
	DDX_Text(pDX, IDC_STATIC_YSCZ, m_strYSAmount);
	DDX_Text(pDX, IDC_STATIC_ZSZH, m_strZSAmount);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllAmount);

	DDX_Text(pDX, IDC_EDIT_IDCARDSNO2, m_strNetId2);
	DDX_Text(pDX, IDC_STATIC_USERNAME2, m_strUserName2);
	DDX_Text(pDX, IDC_STATIC_MEMBERID2, m_strMemberId2);
	DDX_Text(pDX, IDC_STATIC_PERSONALID2, m_strPersonalId2);
	DDX_Text(pDX, IDC_STATIC_NETBAR2, m_strNetbarName2);
	DDX_Text(pDX, IDC_STATIC_CLASS2, m_strClassName2);
	DDX_Text(pDX, IDC_STATIC_WSCZ2, m_strWSAmount2);
	DDX_Text(pDX, IDC_STATIC_YSCZ2, m_strYSAmount2);
	DDX_Text(pDX, IDC_STATIC_ZSZH2, m_strZSAmount2);
	DDX_Text(pDX, IDC_STATIC_ALL2, m_strAllAmount2);
	DDX_Control(pDX, IDC_EDIT_IDCARDSNO, m_edtNetId);
	DDX_Control(pDX, IDC_EDIT_IDCARDSNO2, m_edtNetId2);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassWord);
}


BEGIN_MESSAGE_MAP(CMergeMemberDlg, CNDDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
	ON_EN_SETFOCUS(IDC_EDIT_IDCARDSNO, OnEnSetfocusEditIdcardsno)
	ON_EN_SETFOCUS(IDC_EDIT_IDCARDSNO2, OnEnSetfocusEditIdcardsno2)
END_MESSAGE_MAP()


// CMergeMemberDlg 消息处理程序

void CMergeMemberDlg::OnBnClickedOk()
{
	UpdateData();

	CMergeMemberMethod MergeMemberMethod ;
	//当前用户号
	MergeMemberMethod.SetMemberId(_tstoi(m_strMemberId2));
	//当前网吧ID
	MergeMemberMethod.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
	//目标用户号
	MergeMemberMethod.SetDestMemberId(_tstoi(m_strMemberId));
	//当前用户密码,换户接口密码传"PASSWORDPASSWORD"时，中心不验密码。
	MergeMemberMethod.SetPassword(m_strPassWord);
	//MergeMemberMethod.SetPassword(_T("PASSWORDPASSWORD"));
	theApp.GetCurCashier()->DoMergeMember(MergeMemberMethod);

	if (MergeMemberMethod.GetStatusCode() == 0)
	{
		//包时的日志
		CString strInfo;
		strInfo.Format(_T("MemberID :%s MergeMemberMethod is well done to %s!"), 
			m_strMemberId2, m_strMemberId);
		CIBALog::GetInstance()->Write(strInfo);//写日志

		//重新刷新信息
		m_strWSAmount2 = _T("0.00");
		m_strYSAmount2 = _T("0.00");
		m_strZSAmount2 = _T("0.00");
		m_strAllAmount2 = _T("0.00");
		m_bPrimary = TRUE;
		DoQuery(m_strNetId);

		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

		SetToolTipPos(IDOK);
		ShowToolTip(MergeMemberMethod.GetStatusMessage());

		CLocalServer::GetInstance()->ActiveMemberList.RemoveActiveMember(_ttoi(m_strMemberId2));
		CLocalServer::GetInstance()->GetActiveMemberView()->OnRefresh();
	} 
	else
	{
		CString strInfo;
		strInfo.Format(_T("MemberID :%s MergeMemberMethod is error to %s!"), 
			m_strMemberId2, m_strMemberId);
		CIBALog::GetInstance()->Write(strInfo);//写日志

		SetToolTipPos(IDOK);
		ShowToolTip(MergeMemberMethod.GetStatusMessage(), TRUE);
	}

}

void CMergeMemberDlg::InitControls()
{
	CIBAHelpper::CreateFont(m_FontAllMoney, 18, 700);
	GetDlgItem(IDC_STATIC_ALL)->SetFont(&m_FontAllMoney);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	GetDlgItem(IDC_STATIC_ALL2)->SetFont(&m_FontAllMoney);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC1)->SetFont(&m_BigFont);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
}

BOOL CMergeMemberDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	InitControls();

	return FALSE;  
}

BOOL CMergeMemberDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd * pWnd = GetFocus();
		if (pWnd->GetDlgCtrlID() == IDC_EDIT_IDCARDSNO)
		{
			m_bPrimary = TRUE;

			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_QUERY, BN_CLICKED));

			return TRUE;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_EDIT_IDCARDSNO2)
		{
			m_bPrimary = FALSE;

			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_QUERY, BN_CLICKED));

			return TRUE;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_EDIT_PASSWORD)
		{
			m_bPrimary = TRUE;

			PostMessage(WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED));

			return TRUE;
		}
	} 
	
	return CIBADialog::PreTranslateMessage(pMsg);
}

HBRUSH CMergeMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CNDDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_ALL ||
			pWnd->GetDlgCtrlID() == IDC_STATIC_ALL2)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	return hbr;
}

void CMergeMemberDlg::OnGetBalanceOK(NS_SERVLET::CBalanceMethod & Balance)
{
	CIBAString strTmp;

	if (Balance.GetSex() == 0)
	{
		strTmp.LoadString(IDS_MAN);
	}
	else
	{
		strTmp.LoadString(IDS_WOMAN);
	}

	///////////////////////取得数据////////////////////
	if (m_bPrimary)
	{
		m_strNetId = Balance.GetSerailNum();
		m_strUserName.Format(_T("%s(%s)"), Balance.GetUserName(), strTmp);
		m_strMemberName = Balance.GetMemberName();
		m_strPersonalId = Balance.GetPersonalId();
		m_nUserClassID = Balance.GetUserClass();
		m_strClassName = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
		m_strNetbarName = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

		//无送
		m_strWSAmount.Format(_T("%.2f"), Balance.GetCashBalance() / 100.0);

		//有送
		m_strYSAmount.Format(_T("%.2f"), Balance.GetUsePresent() / 100.0);

		//赠送
		m_strZSAmount.Format(_T("%.2f"), Balance.GetCreditPresent() / 100.0);

		//合计 = 无送账户+有送账户+赠送账户+预留上网费
		m_nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
		m_strAllAmount.Format(_T("%.2f"), m_nAllMoney / 100.0);
	} 
	else
	{
		m_strNetId2 = Balance.GetSerailNum();
		m_strUserName2.Format(_T("%s(%s)"), Balance.GetUserName(), strTmp);
		m_strMemberName2 = Balance.GetMemberName();
		m_strPersonalId2 = Balance.GetPersonalId();
		m_nUserClassID2 = Balance.GetUserClass();
		m_strClassName2 = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
		m_strNetbarName2 = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());

		//无送
		m_strWSAmount2.Format(_T("%.2f"), Balance.GetCashBalance() / 100.0);

		//有送
		m_strYSAmount2.Format(_T("%.2f"), Balance.GetUsePresent() / 100.0);

		//赠送
		m_strZSAmount2.Format(_T("%.2f"), Balance.GetCreditPresent() / 100.0);

		//合计 = 无送账户+有送账户+赠送账户+预留上网费
		m_nAllMoney2 = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
		m_strAllAmount2.Format(_T("%.2f"), m_nAllMoney2 / 100.0);
	}
}

void CMergeMemberDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	if (m_bPrimary)
	{
		//如是已经注册的卡，则更新数据，统计账户，设置状态
		m_strCardID.Format(_T("%d"), nCardId);
		m_strMemberId.Format(_T("%d"), nMemberId);

		QueryBalance(nMemberId, nCardId);

		GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
	} 
	else
	{
		//如是已经注册的卡，则更新数据，统计账户，设置状态
		m_strCardID2.Format(_T("%d"), nCardId);
		m_strMemberId2.Format(_T("%d"), nMemberId);

		QueryBalance(nMemberId, nCardId);

		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	}
	
	UpdateData(FALSE);
}

BOOL CMergeMemberDlg::CheckInput()
{
	return FALSE;
}

void CMergeMemberDlg::ClearData()
{
	if (m_bPrimary)
	{
		m_strCardID.Empty();
		m_strMemberName.Empty();
		m_nUserClassID = 0;
		m_nAllMoney = 0;

		m_strUserName.Empty();
		m_strMemberId.Empty();
		m_strNetbarName.Empty();
		m_strPersonalId.Empty();
		m_strClassName.Empty();
		m_strAllAmount.Empty();
		m_strYSAmount.Empty();
		m_strWSAmount.Empty();
		m_strZSAmount.Empty();
	} 
	else
	{
		//m_strPwd.Empty();
		m_strUserName2.Empty();
		m_strMemberId2.Empty();
		m_strNetbarName2.Empty();
		m_strPersonalId2.Empty();
		m_strClassName2.Empty();
		m_strAllAmount2.Empty();
		m_strYSAmount2.Empty();
		m_strWSAmount2.Empty();
		m_strZSAmount2.Empty();

		m_strCardID2.Empty();
		m_strMemberName2.Empty();
		m_nUserClassID2 = 0;
		m_nAllMoney2 = 0;
	}

	UpdateData(FALSE);
}

void CMergeMemberDlg::OnBnClickedButtonQuery()
{
	UpdateData();

	CWnd * pWnd = GetFocus();
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_IDCARDSNO)
	{
		m_bPrimary = TRUE;
		DoQuery(m_strNetId);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EDIT_IDCARDSNO2)
	{
		m_bPrimary = FALSE;
		DoQuery(m_strNetId2);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_BUTTON_QUERY)
	{
		if (m_bPrimary)
		{
			DoQuery(m_strNetId);
		} 
		else
		{
			DoQuery(m_strNetId2);
		}
	}
}

void CMergeMemberDlg::DoQuery(CString strNetId)
{
	CWaitCursor Wait;

	if (strNetId.IsEmpty())
	{
		if (m_bPrimary)
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO);
			ShowToolTip(IDS_INPUTNETID);
			GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
		} 
		else
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO2);
			ShowToolTip(IDS_INPUTNETID);
			GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
		}
		
		return;
	}

	ClearData();

	CheckCard(strNetId);

	UpdateData(FALSE);
}

void CMergeMemberDlg::OnEnSetfocusEditIdcardsno()
{
	m_bPrimary = TRUE;
}

void CMergeMemberDlg::OnEnSetfocusEditIdcardsno2()
{
	m_bPrimary = FALSE;
}

void CMergeMemberDlg::OnNewCard(UINT nCardId)
{
	if (m_bPrimary)
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO);
		ShowToolTip(IDS_HASNOTUSED);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	} 
	else
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO2);
		ShowToolTip(IDS_HASNOTUSED);
		GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
	}
}
