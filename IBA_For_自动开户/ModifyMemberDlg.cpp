// ModifyMemberDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "Cashier.h"
#include "IdentityNumber.h"
#include ".\modifymemberdlg.h"
#include "Dal\IBADAL.h"

// CModifyMemberDlg 对话框

IMPLEMENT_DYNAMIC(CModifyMemberDlg, CNDDialog)
CModifyMemberDlg::CModifyMemberDlg(CWnd* pParent /*=NULL*/, BOOL bMSuccessClose/* = FALSE*/
								   , BOOL bCanEditCardSN/* = TRUE*/, LPCTSTR szCardSN/* = NULL*/)
: CNDDialog(CModifyMemberDlg::IDD, pParent)
, m_nNetbarID(0), m_bMSuccessClose(bMSuccessClose)
,m_bCanEditCardSN(bCanEditCardSN)
, m_strAllBalance(_T(""))
{
	if(szCardSN == NULL)
		m_strIdCardSN = CCurrentNetBarUser::GetInstance()->GetNetId();
	else
		m_strIdCardSN = szCardSN;
}

CModifyMemberDlg::~CModifyMemberDlg()
{
}

void CModifyMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CNDDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SEX, m_cboSex);//性别
	DDX_Control(pDX, IDC_COMBO_IDTYPE, m_cboIDType);//证件类型
	DDX_Text(pDX, IDC_STATIC_MEMBERID, m_strMemberID);//用户号
	DDX_Text(pDX, IDC_STATIC_NETBAR, m_strNetBar);//开户网吧
	DDX_Text(pDX, IDC_STATIC_CLASS, m_strUserClass);//用户级别
	DDX_Text(pDX, IDC_STATIC_ALL, m_strAllBalance);//账户合计
	DDX_Text(pDX, IDC_EDIT_PERSONALID, m_strPersonalID);//证件号
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strName);//用户姓名
	DDX_Text(pDX, IDC_EDIT_IDCARDSNO, m_strIdCardSN);//上网账号
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);//上网密码
	DDX_Text(pDX, IDC_EDIT_TELNUM, m_strTelNum);//手机号码
	DDV_MaxChars(pDX, m_strTelNum, 20);
}


BEGIN_MESSAGE_MAP(CModifyMemberDlg, CNDDialog)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
END_MESSAGE_MAP()


// CModifyMemberDlg 消息处理程序

BOOL CModifyMemberDlg::OnInitDialog()
{
	CNDDialog::OnInitDialog();

	CIBAString strTmp;

	//性别
	strTmp.LoadString(IDS_MAN);
	m_cboSex.AddString(strTmp);	
	strTmp.LoadString(IDS_WOMAN);
	m_cboSex.AddString(strTmp);

	m_cboSex.SetCurSel(0);

	BOOL bAllowTempUser = FALSE;

	for (INT i = 0; i < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); i++)
	{
		int nIdx = m_cboIDType.AddString(CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeName());

		UINT nTmp = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(i).GetIDTypeID();
		m_cboIDType.SetItemData(nIdx, nTmp);
	}

	SetEditFont();
	m_cboIDType.SetFont(&m_Font);
	m_cboSex.SetFont(&m_Font);

	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);
	if(m_bCanEditCardSN)
	{
		GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();
	}
	else
	{
		GetDlgItem(IDC_EDIT_IDCARDSNO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
		OnBnClickedButtonQuery();
	}
	
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFont(&m_BigFont);
	if(0 != CNetBarConfig::GetInstance()->GetTelVerify())	// 电话验证
	{
		GetDlgItem(IDC_EDIT_TELNUM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_13)->ShowWindow(SW_SHOW);
	}
	AddFocusEnter(IDC_EDIT_PASSWORD, IDC_BUTTON_MODIFY);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//点击修改资料
void CModifyMemberDlg::OnBnClickedButtonModify()
{
	CWaitCursor Wait;

	UpdateData();

	DWORD_PTR nIdType = m_cboIDType.GetItemData(m_cboIDType.GetCurSel()); 
	//如果是身份证，则进行身份验证
	if (11 == nIdType || 12 == nIdType)
	{	
		CIdentityNumber IdentityNumber(m_strPersonalID);
		//无效身份证
		if (!IdentityNumber.IsValid())
		{
			SetToolTipPos(IDC_EDIT_PERSONALID);
			ShowToolTip(IDS_IDNUMERROR);
			GetDlgItem(IDC_EDIT_PERSONALID)->SetFocus();
			return ;
		}
	}

	if(!m_strTelNum.IsEmpty())
	{
		for(int i = 0; i < m_strTelNum.GetLength(); i++)
		{
			if(
				!isdigit(m_strTelNum.GetAt(i)) &&
				!(m_strTelNum.GetAt(i) != '_') &&
				!(m_strTelNum.GetAt(i) != '-')
			  )
			{
				SetToolTipPos(IDC_EDIT_TELNUM);
				ShowToolTip(IDS_ERROR_TELNUM_TIPS);
				GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
				return ;
			}
			// 不能大于13位
			if(m_strTelNum.GetLength() > 13)
			{
				SetToolTipPos(IDC_EDIT_TELNUM);
				ShowToolTip(IDS_ERROR_TELNUM_COUNT);
				GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
				return ;
			}
		}
	}
	else
	{
		if(0 != CNetBarConfig::GetInstance()->GetTelVerify())
		{
			SetToolTipPos(IDC_EDIT_TELNUM);
			CString strTips;
			strTips.LoadString(IDS_ERROR_TELNUM_EMPTY);
			GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
			ShowToolTip(strTips, TRUE);
			return ;
		}
	}

	CModifyMemberMethod ModifyMember;
	
	ModifyMember.SetSex(m_cboSex.GetCurSel());
	ModifyMember.SetUsername(m_strName);

	int nTmp = m_cboIDType.GetCurSel();
	ModifyMember.SetIdType(m_cboIDType.GetItemData(nTmp));
	ModifyMember.SetPersonalId(m_strPersonalID);
	ModifyMember.SetOldPersonalId(m_strOldPersonalId);
	ModifyMember.SetOpenNetBarId(m_nNetbarID);
	ModifyMember.SetPassword(m_strPwd);
	ModifyMember.SetMemberName(m_strMemberName);
	ModifyMember.SetCountryId(_T("CN"));
	ModifyMember.SetTel(m_strTelNum);
	ModifyMember.SetJob(m_strNation);
	ModifyMember.SetIdDepart(m_strIdDepart);
	ModifyMember.SetAddress(m_strAddress);

	theApp.GetCurCashier()->DoModifyMember(ModifyMember);

	if (ModifyMember.GetStatusCode() == 0)
	{
		GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_QUERY)->SetFocus();
		SetToolTipPos(IDC_BUTTON_MODIFY);
		ShowToolTip(IDS_MODIFYMEMBERINFOSUCCESS);
		if(m_bMSuccessClose)
		{
			EndModalLoop(IDOK);
		}
		else
		{
			CIBADAL::GetInstance()->UpdateMemberInfo(m_nMemberId,
				m_strNation, m_strIdDepart, m_strAddress, m_strTelNum);
		}
	}
	//else if(ModifyMember.GetStatusCode() == 70)
	//{
	//	SetToolTipPos(IDC_EDIT_TELNUM);
	//	CString strTips;
	//	if(m_strTelNum.IsEmpty())
	//	{
	//		strTips.LoadString(IDS_ERROR_TELNUM_EMPTY);
	//	}
	//	else
	//	{
	//		strTips.LoadString(IDS_ERROR_TELNUM_ALERADY_BIND);
	//	}
	//	GetDlgItem(IDC_EDIT_TELNUM)->SetFocus();
	//	ShowToolTip(strTips, TRUE);
	//}
	else
	{
		SetToolTipPos(IDC_BUTTON_MODIFY);
		ShowToolTip(ModifyMember.GetStatusMessage(), TRUE);
	}
}

void CModifyMemberDlg::ClearData()
{
	m_strPwd.Empty();
	m_strName.Empty();
	m_strMemberID.Empty();
	m_strNetBar.Empty();
	m_strPersonalID.Empty();
	m_strUserClass.Empty();
	m_strAllBalance.Empty();
	m_strMemberName.Empty();

	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);

}

void CModifyMemberDlg::OnOldCard(UINT nCardId, UINT nMemberId)
{
	m_strMemberID.Format(_T("%d"), nMemberId);
	
	QueryBalance(nMemberId, nCardId);

	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	UpdateData(FALSE);

}

void CModifyMemberDlg::OnNewCard(UINT nCardId)
{
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PERSONALID)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_IDTYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_IDCARDSNO)->SetFocus();

	CNDDialog::OnNewCard( nCardId );
}

void CModifyMemberDlg::OnGetBalanceOK(CBalanceMethod & Balance)
{
	m_strName = Balance.GetUserName();
	m_strOldPersonalId = Balance.GetPersonalId();
	m_nNetbarID = Balance.GetOpenNetbarId();
	m_strPersonalID = Balance.GetPersonalId();
	m_strUserClass = CNetBarConfig::GetInstance()->GetUserClassName(Balance.GetUserClass());
	m_strNetBar = CNetBarConfig::GetInstance()->GetNetBarName(Balance.GetOpenNetbarId());
	m_strMemberName = Balance.GetMemberName();
	m_nMemberId = Balance.GetMemberId();
	m_cboSex.SetCurSel(Balance.GetSex());

	for (INT i = 0; i < m_cboIDType.GetCount(); i++)
	{
		if (Balance.GetIdType() == m_cboIDType.GetItemData(i))
		{
			m_cboIDType.SetCurSel(i);
			break;
		}
	}

	//合计 = 无送账户+有送账户+赠送账户+预留上网费
	INT m_nAllMoney = Balance.GetCashBalance() + Balance.GetUsePresent() + Balance.GetCreditPresent() + Balance.GetOnlineAmount();
	m_strAllBalance.Format(_T("%.2f"), m_nAllMoney / 100.0);

	if(0 != CNetBarConfig::GetInstance()->GetShenzhen() ||
		(0 != CNetBarConfig::GetInstance()->GetTelVerify())||
		1 == CNetBarConfig::GetInstance()->GetDomainId() || // 是深圳的
		99 == CNetBarConfig::GetInstance()->GetDomainId())	// 测试环境	
	{
		CMemberInfoMethod MemberInfo;

		MemberInfo.SetSerialNum(m_strIdCardSN);
		//MemberInfo.SetPersonalId( m_strPersonalID );
		//MemberInfo.SetMemberId(m_strIdCardSN);
		//MemberInfo.SetUsername( m_strMemberName );
		//MemberInfo.SetAvailable(1);// 0全部查询, 1有效查询
		theApp.GetCurCashier()->DoMemberInfo(MemberInfo);

		if(0 == MemberInfo.GetStatusCode() &&
			0 < MemberInfo.GetMemberCount())
		{
			m_strTelNum = MemberInfo.GetTel(0);
			m_strNation = MemberInfo.GetJob(0);
			m_strAddress = MemberInfo.GetAddress(0);
			m_strIdDepart = MemberInfo.GetIdDepart(0);
		}
	}
	UpdateData(FALSE);
}



