// TestRealNameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestRealName.h"
#include "..\RealName.h"
#include ".\testrealnamedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL SendMessageToIBA(UINT nType)
{
	BOOL bRet = FALSE;

	HWND hIBA = FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);

	if (hIBA != NULL)
	{
		bRet = PostMessage(hIBA, WM_REALNAME, nType, NULL);
	}
	else
	{
		AfxMessageBox(_T("请先运行嘟嘟牛收银台！"));
	}

	return bRet ? 1 : 0;
}


CTestRealNameDlg::CTestRealNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestRealNameDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strIDType = "15";
	m_strPeronId = "123456789";
	m_strPwd = "123";
	m_strName = "网吧用户";
}

void CTestRealNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ED_NETID, m_strIDNumber);
	DDX_Text(pDX, IDC_ED_IDTYPE, m_strIDType);
	DDX_Text(pDX, IDC_ED_PERSONID, m_strPeronId);
	DDX_Text(pDX, IDC_ED_PWD, m_strPwd);
	DDX_Text(pDX, IDC_ED_NAME, m_strName);

}

BEGIN_MESSAGE_MAP(CTestRealNameDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()


// CTestRealNameDlg 消息处理程序

BOOL CTestRealNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestRealNameDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CTestRealNameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestRealNameDlg::OnBnClickedButton1()
{
	UpdateData();

	REQ_REGISTER_INFO REGISTER_INFO = {0};

	lstrcpy(REGISTER_INFO.szCertificateID, m_strPeronId) ;
	lstrcpy(REGISTER_INFO.szName , m_strName) ;
	lstrcpy(REGISTER_INFO.szCompany , "sunlike") ;
	lstrcpy(REGISTER_INFO.szCertificateType , m_strIDType) ;
	lstrcpy(REGISTER_INFO.szCountry , "中国") ;
	lstrcpy(REGISTER_INFO.szDetail , "模拟刷卡") ;

	//IDUser(m_strIDNumber.GetBuffer(), m_strPeronId.GetBuffer(), m_strPwd.GetBuffer(), 1, 1, REGISTER_INFO);
	IDUser(m_strIDNumber.GetBuffer(), m_strPeronId.GetBuffer(), m_strPwd.GetBuffer(), 3, 1, REGISTER_INFO);
}

void CTestRealNameDlg::OnBnClickedButton2()
{
	UpdateData();

	CRegKey regIBA;

	if (ERROR_SUCCESS == regIBA.Create(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\GLW\\OpenUser")))
	{
		regIBA.SetStringValue(_T("IDNo"), m_strIDNumber);
		
		regIBA.SetStringValue(_T("Password"), m_strPwd);
		
		//regIBA.SetDWORDValue(_T("IDCardType"), OpenUserInfo.nType);
		//regIBA.SetDWORDValue(_T("Validate"), OpenUserInfo.bValidate);

		regIBA.SetStringValue(_T("Name"), m_strName);
		regIBA.SetDWORDValue(_T("CertificateType"), _ttoi(m_strIDType));
		regIBA.SetStringValue(_T("CertificateID"), m_strPeronId);
		
		//regIBA.SetStringValue(_T("AccreditID"), OpenUserInfo.szAccreditID);
		//regIBA.SetDWORDValue(_T("PswdFlag"), OpenUserInfo.bPswdFlag);
		//regIBA.SetDWORDValue(_T("InfoFlag"), OpenUserInfo.nInfoFlag);

		regIBA.Close();
		
		SendMessageToIBA(1);
	}
}

void CTestRealNameDlg::OnBnClickedOk()
{
	OnOK();
}

void CTestRealNameDlg::OnBnClickedButton3()
{
	UpdateData();

	struct ID_CARD_INFO
	{
		char szISN[20]; //内码  （以内码为准）
		char szPSN[20]; //印刷码（暂时不使用）
	};

	ID_CARD_INFO id_card_info = {0};

	lstrcpyn(id_card_info.szISN, m_strIDNumber, 20);

	COPYDATASTRUCT copydatastruct = {0};

	copydatastruct.cbData = sizeof ID_CARD_INFO;
	copydatastruct.dwData = 0;
	copydatastruct.lpData = &id_card_info;

	HWND hIBA = ::FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);

	if (hIBA != NULL)
	{
		::SendMessage(hIBA, WM_COPYDATA, (WPARAM)GetSafeHwnd(), (LPARAM)(LPVOID)&copydatastruct);
	}
	else
	{
		AfxMessageBox(_T("请先运行嘟嘟牛收银台！"));
	}
}
