// ChangeUserSNDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "RealName\IBARealName.h"
#include ".\changeusersndlg.h"

#include ".\Socket\LocalServer.h"

//
//class:CDlgChangeNetId 修改账号对话框
//
IMPLEMENT_DYNAMIC(CDlgChangeNetId, CNDDialog)
CDlgChangeNetId::CDlgChangeNetId(CWnd* pParent /*=NULL*/)
: CNDDialog(CDlgChangeNetId::IDD, pParent)
, m_nOpenNetbarId(0)
{
}

CDlgChangeNetId::~CDlgChangeNetId()
{
}

void CDlgChangeNetId::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_USERNAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberID);
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);
	DDX_Text(pDX, IDC_STATIC_PERSONALID, m_strPersonalID);
	DDX_Text(pDX, IDC_EDIT_IDCARDSNO2, m_strIdCardSN2);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPwd);
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);
}

BEGIN_MESSAGE_MAP(CDlgChangeNetId, CNDDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEUSERSN, OnBnClickedButtonChangeusersn)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, OnBnClickedButtonQuery)
END_MESSAGE_MAP()


BOOL CDlgChangeNetId::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	ClearData();
	SetEditFont();

	AddFocusEnter(IDC_EDIT_IDCARDSNO2, IDC_BUTTON_CHANGEUSERSN);
	AddFocusEnter(IDC_EDIT_PWD, IDC_BUTTON_CHANGEUSERSN);

	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);

	((CEdit*)GetDlgItem(IDC_EDIT_PWD))->SetLimitText(8);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgChangeNetId::OnBnClickedButtonChangeusersn()
{
	CWaitCursor Wait;

	UpdateData();

	//上网账号

	if (m_strIdCardSN2.GetLength() < 6 || m_strIdCardSN2.GetLength() > 20)
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO2);
		ShowToolTip(IDS_SNNOLONG);
		GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
		return;
	}

	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(m_strIdCardSN2);

	theApp.GetCurCashier()->DoCheckCard(checkCard);

	if (checkCard.GetStatusCode() == 0)
	{
		if (checkCard.GetMemberId() == 0)
		{
			CCardReplaceMethod CardReplace;
			CardReplace.SetPersonalId(m_strPersonalID);
			CardReplace.SetMemberName(m_strMemberName);
			CardReplace.SetCardId(checkCard.GetCardId());
			CardReplace.SetPassword(m_strPwd);
			CardReplace.SetOpenNetbarId(m_nOpenNetbarId);
			
			theApp.GetCurCashier()->DoCardReplace(CardReplace);
			
			SetToolTipPos(IDC_BUTTON_CHANGEUSERSN);
			ShowToolTip(CardReplace.GetStatusMessage());

			if (CardReplace.GetStatusCode() == 0)
			{
				GetDlgItem(IDC_BUTTON_CHANGEUSERSN)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_EDIT_PWD)->SetFocus();
			}

			//{ 2011/04/25-gxx: 更新ActiveMemberView
			
			CActiveMemberList& listMembers = CLocalServer::GetInstance()->ActiveMemberList;
			CActiveMember ActiveMember;
			if (!listMembers.GetActiveMember(_ttoi(m_strMemberID),ActiveMember))
			{
				return;
			}
			ActiveMember.SetNetId(m_strIdCardSN2);
			listMembers.UpdateActiveMember(ActiveMember,TRUE);
			CLocalServer::GetInstance()->GetActiveMemberView()->OnRefresh();

			//}
			
		}
		else
		{
			SetToolTipPos(IDC_EDIT_IDCARDSNO2);
			ShowToolTip(IDS_HASUSED);
			GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
		}

		
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_CHANGEUSERSN);
		ShowToolTip(checkCard.GetStatusMessage(), TRUE);
	}
}

void CDlgChangeNetId::ClearData()
{
	m_nOpenNetbarId = 0;
	m_strAllBalance.Empty();
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strUserClass.Empty();
	m_strPersonalID.Empty();
	m_strNetBar.Empty();
	m_strPwd.Empty();
	m_strIdCardSN2.Empty();
	m_strUserClass.Empty();
	m_strMemberName.Empty();

	GetDlgItem(IDC_BUTTON_CHANGEUSERSN)->EnableWindow(FALSE);
}

void CDlgChangeNetId::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);
	QueryBalance(nMemberId, nCardId);
	UpdateData(FALSE);
}

void CDlgChangeNetId::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_strName = Balance.GetUserName();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());
	m_strMemberName = Balance.GetMemberName();
	m_nOpenNetbarId = Balance.GetOpenNetbarId();

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT m_nAllMoney = Balance.GetTotalBalacne();
	m_strAllBalance.Format(_T("%.2f"), m_nAllMoney / 100.0);

	GetDlgItem(IDC_BUTTON_CHANGEUSERSN)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_IDCARDSNO2)->SetFocus();
}

void CDlgChangeNetId::OnBnClickedButtonQuery()
{
	CWaitCursor Wait;

	UpdateData();

	if (m_strNetId.IsEmpty())
	{
		SetToolTipPos(IDC_EDIT_IDCARDSNO);
		ShowToolTip(IDS_INPUTNETID);
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
		return;
	}

	ClearData();

	CheckCard(m_strNetId);

	UpdateData(FALSE);
}

void CDlgChangeNetId::CheckCard(LPCTSTR strNetId)
{
	CMemberInfoMethod memberInfo;
	memberInfo.SetSerialNum(strNetId);

	theApp.GetCurCashier()->DoMemberInfo(memberInfo);

	if (memberInfo.GetStatusCode() == 0)
	{
		if (memberInfo.GetMemberCount() > 0)
		{
			if (memberInfo.GetMemberId(0) != 0) //已经开过的卡,充值退卡
			{
				m_nMemberId = memberInfo.GetMemberId(0);
				OnOldCard(memberInfo.GetCardId(0), m_nMemberId);				
			}
			else //新卡注册
			{
				OnNewCard(memberInfo.GetCardId(0));
			}
		}
		else
		{//没有用户资料
			SetToolTipPos(IDC_BUTTON_QUERY);
			ShowToolTip(IDS_HASNOTUSED);
		}
	}
	else
	{
		SetToolTipPos(IDC_BUTTON_QUERY);
		ShowToolTip(memberInfo.GetStatusMessage(), TRUE);
	}

	m_bFromScan = FALSE;
}	
