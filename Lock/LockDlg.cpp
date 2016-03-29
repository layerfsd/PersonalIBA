// LockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Lock.h"
#include "LockDlg.h"
#include ".\lockdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLockDlg 对话框



CLockDlg::CLockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLockDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_plockScreendlg = NULL;
}

void CLockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDB_LOCKBTN, m_LockBtn);
	DDX_Control(pDX, IDCANCEL, m_CancleBtn);
	DDX_Control(pDX, IDC_PASSWORDEDIT, m_PassWordEdit);
	DDX_Control(pDX, IDC_RPASSWORDEDIT, m_RPassWordEdit);
}

BEGIN_MESSAGE_MAP(CLockDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_CHECKPASSWORD, OnCheckPassword)
	ON_BN_CLICKED(IDB_LOCKBTN, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLockDlg 消息处理程序

BOOL CLockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetWindowText(_T("嘟嘟牛电脑锁"));

	m_ImageBg.LoadFromOtherResource(AfxGetResourceHandle(), IDP_PASSWORDBKPNG, _T("PNG"));

	if (!m_ImageBg.IsNull())
	{
		m_ImageBg.DoForPNG();
		SetWindowPos(this, 0, 0, m_ImageBg.GetWidth(), m_ImageBg.GetHeight(), SWP_NOMOVE | SWP_NOZORDER );
	}

	CRgn MyRgn;
	RECT rectClient;
	GetClientRect(&rectClient);
	rectClient.right += 1;
	rectClient.bottom += 2;

	MoveWindow(&rectClient, TRUE);
	MyRgn.CreateRoundRectRgn(0, 0, rectClient.right, rectClient.bottom, 5, 5);
	SetWindowRgn(MyRgn, TRUE);

	CRect rect(5, 20, 10, 10);

	m_LockBtn.ModifyStyle(0, BS_OWNERDRAW);
	m_LockBtn.LoadPng(IDP_LOCKPNG, 2);

	m_CancleBtn.ModifyStyle(0, BS_OWNERDRAW);
	m_CancleBtn.LoadPng(IDR_CANCLEPNG, 2);

	GetDlgItem(IDC_PASSWORDEDIT)->SetWindowPos(NULL, 85, 54, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

	GetDlgItem(IDC_RPASSWORDEDIT)->SetWindowPos(NULL, 85, 98, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

	CreateFont();

	((CEdit*)GetDlgItem(IDC_PASSWORDEDIT))->SetLimitText(20);
	GetDlgItem(IDC_PASSWORDEDIT)->SetFont(&m_Font);
	//GetDlgItem(IDC_PASSWORDEDIT)->SetFocus();

	((CEdit*)GetDlgItem(IDC_RPASSWORDEDIT))->SetLimitText(20);
	GetDlgItem(IDC_RPASSWORDEDIT)->SetFont(&m_Font);

	SetWindowPos(&wndTopMost, 0, 0, 0, 0,  SWP_NOMOVE | SWP_NOSIZE);

	return FALSE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLockDlg::OnPaint() 
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
HCURSOR CLockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLockDlg::OnEraseBkgnd(CDC* pDC)
{
	
	pDC->SelectObject(m_Font2);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	if (!m_ImageBg.IsNull())
	{
		CRect rect;
		GetWindowRect(rect);
		m_ImageBg.Draw(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height());

		pDC->DrawText(_T("解锁密码："), CRect(20,56,100,79), DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		pDC->DrawText(_T("确认密码："), CRect(20,98,100,121), DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	}

	return TRUE;
}

void CLockDlg::CreateFont()
{

	m_Font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));  


	m_Font2.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));
	

}

BOOL CLockDlg::CreateLockDlg()
{
	BOOL bl = FALSE;
	if (!m_plockScreendlg)
	{
		m_plockScreendlg = new CLockScreenDlg(AfxGetMainWnd());
		bl = m_plockScreendlg->Create(IDD_LOCKSCREENDLG, AfxGetMainWnd());

	}

	return bl;
}

void CLockDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); 

	CDialog::OnLButtonDown(nFlags, point);
}

void CLockDlg::OnBnClickedOk()
{
	CString strPassWord;

	GetDlgItem(IDC_PASSWORDEDIT)->GetWindowText(strPassWord);

	CString strRPassWord;

	GetDlgItem(IDC_RPASSWORDEDIT)->GetWindowText(strRPassWord);
	
	if (strPassWord.IsEmpty())
	{
		MessageBox("请输入解锁密码！", "电脑锁", MB_ICONINFORMATION);

		return;
	}

	if (strRPassWord.IsEmpty())
	{
		//MessageBox("请输入验证密码!", "电脑锁", MB_ICONINFORMATION);

		GetDlgItem(IDC_RPASSWORDEDIT)->SetFocus();

		return;
	}

	if (strRPassWord != strPassWord)
	{
		MessageBox("两次输入的密码不一样！", "电脑锁", MB_ICONINFORMATION);

		return;
	}

	m_strPassWord = strPassWord;

	if (CreateLockDlg())
	{
		ShowWindow(SW_HIDE);
	}

}

void CLockDlg::OnBnClickedCancel()
{
	DestroyWindow();
}

LRESULT CLockDlg::OnCheckPassword( WPARAM wParam, LPARAM lParam )
{
	CString* pwd = (CString*)lParam;

	if (*pwd != m_strPassWord)
	{
		return -1;
	}

	return 0;
}
void CLockDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if (m_plockScreendlg)
	{
		delete m_plockScreendlg;

		m_plockScreendlg = NULL;
	}
}

void CLockDlg::PostNcDestroy()
{
	delete this;

	CDialog::PostNcDestroy();
}

void CLockDlg::OnClose()
{
	return;
}
