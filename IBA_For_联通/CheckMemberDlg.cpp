// CheckMember.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IdentityNumber.h"
#include "Servlet\ModifyMemberMethod.h"
#include "Cashier.h"
#include ".\checkmemberdlg.h"
#include "IBAHelpper.h"

// CCheckMemberDlg 对话框

IMPLEMENT_DYNAMIC(CCheckMemberDlg, CIBADialog)
CCheckMemberDlg::CCheckMemberDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CCheckMemberDlg::IDD, pParent)
,m_nRNCertificateType(0) ,m_nNetbarID(0)
,m_nSLCertificateType(0)
{

}

CCheckMemberDlg::~CCheckMemberDlg()
{
}

void CCheckMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_RNCARDID, m_strRNCardID);
	DDX_Text(pDX, IDC_STATIC_SLCARDID, m_strSLCardID);
	DDX_Text(pDX, IDC_STATIC_RNNAME, m_strRNName);
	DDX_Text(pDX, IDC_STATIC_SLNAME, m_strSLName);
	DDX_Text(pDX, IDC_STATIC_RNCERTIFICATECLASS, m_strRNCertificateType);
	DDX_Text(pDX, IDC_STATIC_SLCERTIFICATECLASS, m_strSLCertificateType);
	DDX_Text(pDX, IDC_STATIC_RNCERTIFICATEID, m_strRNCertificateID);
	DDX_Text(pDX, IDC_STATIC_SLCERTIFICATEID, m_strSLCertificateID);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPwd);
	DDX_Control(pDX, IDC_STATIC_TIP, m_STTip);
}


BEGIN_MESSAGE_MAP(CCheckMemberDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CCheckMemberDlg 消息处理程序
BOOL CCheckMemberDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	//CenterWindow();
	//ShowWindow(SW_SHOW);

	CIBAHelpper::CreateFont(m_BigFont, 16, 220);
	m_STTip.SetFont(&m_BigFont);

	DoModify();

	return FALSE;// 如果返回真，则设置TAB为1的焦点，否则，按照设定的焦点
	// 异常: OCX 属性页应返回 FALSE
}

void CCheckMemberDlg::SetRealName(CNetBarUser& NetBarUser)
{
	//用户名
	m_strRNName = NetBarUser.GetUserName();

	//证件类型
	m_nRNCertificateType = NetBarUser.GetCertificateType();
	for (INT j = 0; j < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); j++)
	{
		UINT nCertificateType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(j).GetIDTypeID();

		if (nCertificateType == NetBarUser.GetCertificateType())
		{
			m_strRNCertificateType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(j).GetIDTypeName();
			break;
		}
	}
	//证件号
	m_strRNCertificateID = NetBarUser.GetCertificateID();
}

void CCheckMemberDlg::SetServlet(NS_SERVLET::CBalanceMethod& BalanceMethod)
{
	//用户名
 	m_strSLName = BalanceMethod.GetUserName();

	//证件类型
	m_nSLCertificateType = BalanceMethod.GetIdType();//为下面修改保存序号
	for (INT j = 0; j < CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetCount(); j++)
	{
		UINT nCertificateType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(j).GetIDTypeID();

		UINT tmp = BalanceMethod.GetIdType();
		if (nCertificateType == BalanceMethod.GetIdType())
		{
			m_strSLCertificateType = CNetBarConfig::GetInstance()->m_IDTypeInfoArray.GetAt(j).GetIDTypeName();
			break;
		}
	}
	//证件号
 	m_strSLCertificateID = BalanceMethod.GetPersonalId();

	//网吧号
	m_nNetbarID = BalanceMethod.GetOpenNetbarId();

	//用户登录名，由用户号生成
	m_strMemberName = BalanceMethod.GetMemberName();
	
}

void CCheckMemberDlg::SetCardID(CString strCardID )
{
	//上网号码
	m_strRNCardID = strCardID;

	m_strSLCardID = strCardID;
}

void CCheckMemberDlg::DoModify()
{
	CWaitCursor Wait;

	UpdateData();

	NS_SERVLET::CModifyMemberMethod ModifyMember;//只更新实名存在的资料

	ModifyMember.SetUsername(m_strRNName);
	ModifyMember.SetOpenNetBarId(m_nNetbarID);
	ModifyMember.SetIdType(m_nRNCertificateType);//实名类型
	ModifyMember.SetPersonalId(m_strRNCertificateID);//实名证件号
	ModifyMember.SetOldPersonalId(m_strSLCertificateID);//中心原证件号
	ModifyMember.SetPassword(_T("PASSWORDPASSWORD"));
	ModifyMember.SetMemberName(m_strMemberName);
	ModifyMember.SetCountryId(_T("CN"));

	theApp.GetCurCashier()->DoModifyMember(ModifyMember);

	if (ModifyMember.GetStatusCode() == 0)
	{
		GetDlgItem(IDOK)->SetFocus();

		//CString strTmp;
		//strTmp.LoadString(IDS_MODIFYMEMBERINFOSUCCESS);
		//MsgBox(strTmp);
	}
	else
	{
		//SetToolTipPos(IDOK);
		//ShowToolTip(ModifyMember.GetStatusMessage());
	}
}

void CCheckMemberDlg::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
}

void CCheckMemberDlg::OnCancel()
{
	//CBCGPDialog::OnCancel();
}
