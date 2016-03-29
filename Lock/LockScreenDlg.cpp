// LockScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Lock.h"
#include ".\LockScreenDlg.h"
#include "LockDlg.h"

// CLockScreenDlg 对话框

IMPLEMENT_DYNAMIC(CLockScreenDlg, CDialog)
CLockScreenDlg::CLockScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLockScreenDlg::IDD, pParent)
{
	m_initWidth_ = 0;

	m_initHeight_ = 0;
}

CLockScreenDlg::~CLockScreenDlg()
{
}

void CLockScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORDEDIT, m_PasswordEdit);
	DDX_Control(pDX, IDB_ULOCK, m_ULockBtn);
}


BEGIN_MESSAGE_MAP(CLockScreenDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDB_ULOCK, OnBnClickedUlock)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLockScreenDlg 消息处理程序

BOOL CLockScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_initWidth_ = GetSystemMetrics(SM_CXSCREEN);
	m_initHeight_ = GetSystemMetrics(SM_CYSCREEN);

#ifdef _DEBUG
	SetWindowPos((CWnd *) &wndTop, 0, 0, m_initWidth_ / 4, m_initHeight_ / 4, SWP_NOACTIVATE);
#else
	SetWindowPos((CWnd *) &wndTop, 0, 0, m_initWidth_, m_initHeight_, SWP_NOACTIVATE);
#endif

	m_imgScreen.LoadFromOtherResource(AfxGetResourceHandle(), IDR_LOCKSCREENBKPNG, _T("PNG"));

	m_ULockBtn.ModifyStyle(0, BS_OWNERDRAW);
	m_ULockBtn.LoadPng(IDR_ULOCKPNG, 2);

	CRect rcPassWordEdit;

	GetDlgItem(IDC_PASSWORDEDIT)->GetClientRect(rcPassWordEdit);

	rcPassWordEdit.left = 10;

	rcPassWordEdit.top = m_initHeight_ - rcPassWordEdit.Height() - 20;

	GetDlgItem(IDC_PASSWORDEDIT)->SetWindowPos(NULL, rcPassWordEdit.left, rcPassWordEdit.top, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

	GetDlgItem(IDC_PASSWORDEDIT)->GetWindowRect(rcPassWordEdit);

	GetDlgItem(IDC_PASSWORDEDIT)->SetFocus();

	int nUx = rcPassWordEdit.right + 10;

	int nUy = rcPassWordEdit.top + 2;

	m_ULockBtn.SetWindowPos(NULL, nUx, nUy,  0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);

	SetTimer(TimeIDOfLockScreen, 100, NULL);

	return FALSE;  // return TRUE unless you set the focus to a control
	
}

void CLockScreenDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TimeIDOfLockScreen)
	{
		ShowWindow(SW_SHOWNOACTIVATE);
		UpdateWindow();
		SetWindowPos(&CWnd::wndTopMost , 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CLockScreenDlg::OnBnClickedUlock()
{
	CString strPassWord;

	GetDlgItem(IDC_PASSWORDEDIT)->GetWindowText(strPassWord);

	if (strPassWord.IsEmpty())
	{
		return;
	}

	CLockDlg *MainWnd = (CLockDlg*) AfxGetMainWnd();

	if (-1 == MainWnd->SendMessage(WM_CHECKPASSWORD, 0, (LPARAM)&strPassWord))
	{
		return;
	}
	else
	{
		MainWnd->DestroyWindow();
	}

	//DestroyWindow();
}

BOOL CLockScreenDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));

	if (m_imgScreen.IsNull()) //没图片
	{
		return CDialog::OnEraseBkgnd(pDC);
	}

	//int nRateScreen = initHeight_ / (initWidth_ / 16);
	//int nRateImage =  m_imgScreen.GetHeight() / (m_imgScreen.GetWidth() / 16); 

	//if (nRateScreen <= 10) //宽屏幕 
	//{
	//	if (nRateImage > 10)
	//	{
	//		int nTmpWidth = rect.Height() * 16 / nRateImage;
	//		rect.SetRect((rect.Width() - nTmpWidth) / 2, 0, (rect.Width() - nTmpWidth) / 2 + nTmpWidth, rect.Height());
	//	}
	//}
	//else
	//{
	//	if (nRateImage <= 10) //宽图片
	//	{	
	//		int nTmpHeight = rect.Width() * 16 / nRateImage;
	//		rect.SetRect(0, (rect.Height() - nTmpHeight) / 2, rect.Width(), (rect.Height() - nTmpHeight) / 2 + nTmpHeight);
	//	}
	//}

	m_imgScreen.Draw(pDC->GetSafeHdc(), rect);

	return TRUE;
}

void CLockScreenDlg::OnClose()
{
	return;
}

BOOL CLockScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg-> message == WM_KEYDOWN) 
	{ 
		if(pMsg-> wParam == VK_ESCAPE) 
			return   TRUE; 
	}

	return CDialog::PreTranslateMessage(pMsg);
}
