// YB2IBADlg.cpp : 实现文件
//

#include "stdafx.h"
#include <atlbase.h>
#include "IniConfig.h"
#include "YB2IBA.h"
#include ".\yb2ibadlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYB2IBADlg 对话框



CYB2IBADlg::CYB2IBADlg(CWnd* pParent /*=NULL*/)
: CDialog(CYB2IBADlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CYB2IBADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CYB2IBADlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKETNOTIFY, OnSocketNotify)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CYB2IBADlg 消息处理程序

BOOL CYB2IBADlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if (!m_ListenSocket.Create(10001))
	{
		AfxMessageBox(_T("端口10001可能被占用了！"));
	}
	else
	{
		if (!m_ListenSocket.Listen())
		{
			AfxMessageBox(_T("端口10001可能被占用了！"));
		}
	}

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	m_strPath = strPath.Left(strPath.ReverseFind(_T('\\')) + 1);

	return FALSE;  // 除非设置了控件的焦点，否则返回 TRUE
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CYB2IBADlg::OnPaint() 
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
HCURSOR CYB2IBADlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HRESULT CYB2IBADlg::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	if (1 == wParam)
	{
		m_ClientSocket.Close();
		m_ListenSocket.Accept(m_ClientSocket);
	}
	else if (2 == wParam)
	{
		char buf[30] = {0};

		m_ClientSocket.Receive(buf, 30);

		m_strIdNumber = buf;

#ifdef _DEBUG

		AfxMessageBox(m_strIdNumber);

#endif
		
		WriteReg();
	}

	return 0;
}

BOOL CYB2IBADlg::SendMessageToIBA(UINT nType)
{
	#define WM_REALNAME WM_USER + 100

	BOOL bRet = FALSE;

	HWND hIBA = ::FindWindow(_T("IBA_MAINWINDOW_CLASS"), NULL);

	if (hIBA != NULL)
	{
		bRet = ::SendMessage(hIBA, WM_REALNAME, nType, NULL);
	}
	else
	{
		AfxMessageBox(_T("请先运行嘟嘟牛收银台！"), MB_ICONINFORMATION);
	}

	return bRet ? 1 : 0;
}

void CYB2IBADlg::WriteReg()
{
	CRegKey regIBA;

	if (ERROR_SUCCESS == regIBA.Create(HKEY_CURRENT_USER, _T("Software\\Sunlike\\IBA\\RealName\\YBWW")))
	{
		regIBA.SetStringValue(_T("HeadPic"), m_strPath + m_strIdNumber + _T(".jpg"));

		regIBA.SetStringValue(_T("Hc1"), m_strIdNumber);

		CIniConfig UserIni;
		
		UserIni.m_strIniFile = m_strPath + m_strIdNumber + _T(".ini");

		CString strTmp;

		UserIni.ReadValue(_T("HCP"), _T("Hc2"), strTmp);
		regIBA.SetStringValue(_T("Hc2"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc3"), strTmp);
		regIBA.SetStringValue(_T("Hc3"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc4"), strTmp);
		regIBA.SetStringValue(_T("Hc4"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc5"), strTmp);
		regIBA.SetStringValue(_T("Hc5"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc6"), strTmp);
		regIBA.SetStringValue(_T("Hc6"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc7"), strTmp);
		regIBA.SetStringValue(_T("Hc7"), strTmp);

		UserIni.ReadValue(_T("HCP"), _T("Hc8"), strTmp);
		regIBA.SetStringValue(_T("Hc8"), strTmp);

		regIBA.Close();

		SendMessageToIBA(0);
	}
}
void CYB2IBADlg::OnBnClickedOk()
{
	//OnOK();
}

void CYB2IBADlg::OnBnClickedCancel()
{
	//OnCancel();
}

void CYB2IBADlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	delete this;
}
