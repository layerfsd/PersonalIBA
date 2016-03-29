// IBAAboutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include ".\IBAAboutDlg.h"
#include "IBAHelpper.h"


// CIBAAboutDlg 对话框

IMPLEMENT_DYNAMIC(CIBAAboutDlg, CBCGPDialog)
CIBAAboutDlg::CIBAAboutDlg(CWnd* pParent /*=NULL*/)
: CStringLoaderDlg<CBCGPDialog>(CIBAAboutDlg::IDD, pParent)
{
	EnableVisualManagerStyle();
}

CIBAAboutDlg::~CIBAAboutDlg()
{
}

void CIBAAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_VER, m_strVer);
	DDX_Control(pDX, IDC_BUTTON_DODONEW, m_btnDodonew);
	DDX_Control(pDX, IDC_BUTTON_SUNLIKE, m_btnSunlike);
	DDX_Control(pDX, IDC_BUTTON_DUDUNIU, m_btnDodoniu);
}


BEGIN_MESSAGE_MAP(CIBAAboutDlg, CBCGPDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DODONEW, OnBnClickedButtonDodonew)
	ON_BN_CLICKED(IDC_BUTTON_SUNLIKE, OnBnClickedButtonSunlike)
	ON_BN_CLICKED(IDC_BUTTON_DUDUNIU, OnBnClickedButtonDuduniu)
END_MESSAGE_MAP()


// CIBAAboutDlg 消息处理程序

void CIBAAboutDlg::OnBnClickedOk()
{
	CBCGPDialog::OnOK();
}

BOOL CIBAAboutDlg::OnInitDialog()
{
	CStringLoaderDlg<CBCGPDialog>::OnInitDialog();

	//{ 2011/12/12-8235-gxx: 
	CString strVer = CIBALog::GetInstance()->GetAppVer();
	CStringArray stA;
	CIBAHelpper::SplitLine(strVer,stA,_T('.'));
	m_strVer = stA.GetAt(0)+_T(".")+stA.GetAt(1);

	CString strTime = CIBALog::GetInstance()->GetAppBuildTime();

	CString strBuild;
	strBuild.Format(_T("Build: %s%s Time:%s"),stA.GetAt(2),stA.GetAt(3),strTime);

	GetDlgItem(IDC_STATIC_BUILD)->SetWindowText(strBuild);
	//m_strVer = CIBALog::GetInstance()->GetAppVer();
	//}
	
	

	CString str;
	str.Format(_T("Build "));

	UpdateData(FALSE);

	m_btnSunlike.SizeToContent();
	m_btnDodonew.SizeToContent();
	m_btnDodoniu.SizeToContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIBAAboutDlg::OnBnClickedButtonDodonew()
{
	m_btnDodonew.OpenURL();
}

void CIBAAboutDlg::OnBnClickedButtonSunlike()
{
	m_btnSunlike.OpenURL();
}

void CIBAAboutDlg::OnBnClickedButtonDuduniu()
{
	m_btnDodoniu.OpenURL();
}
