// MsgBoxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include ".\MsgBoxDlg.h"

// CMsgBoxDlg 对话框

IMPLEMENT_DYNAMIC(CMsgBoxDlg, CBCGPDialog)
CMsgBoxDlg::CMsgBoxDlg(CWnd* pParent /*=NULL*/)
: CStringLoaderDlg<CBCGPDialog>(CMsgBoxDlg::IDD, pParent)
, m_nType(MB_OK)
{
	EnableVisualManagerStyle();
	m_bAutoClose = 0;
}

CMsgBoxDlg::~CMsgBoxDlg()
{
}

void CMsgBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_INFO, m_strText);
}


BEGIN_MESSAGE_MAP(CMsgBoxDlg, CBCGPDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDNO, OnBnClickedNo)
	ON_BN_CLICKED(IDYES, OnBnClickedYes)
	ON_BN_CLICKED(IDRECONNECT, OnBnClickedRetryConnection)
	ON_BN_CLICKED(IDENTERLOCAL, OnBnClickedEnterLocal)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMsgBoxDlg 消息处理程序

BOOL CMsgBoxDlg::OnInitDialog()
{
	CStringLoaderDlg<CBCGPDialog>::OnInitDialog();

	GetDlgItem(IDC_STATIC_INFO)->SetWindowText(m_strText);

	if(m_nType == (MB_OK|MB_ICONQUESTION|MB_SYSTEMMODAL))
	{
		SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE);//保持窗口在最前
	}	

	InitWindow();
	InitIcon();
	InitButton();

	CenterWindow();

	if (m_bAutoClose)
	{
		SetTimer(10, 1000,NULL);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMsgBoxDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_ImageList.Draw(&dc, 0, CPoint(15, 15), ILD_NORMAL);
}

void CMsgBoxDlg::OnBnClickedOk()
{
	OnOK();
}

void CMsgBoxDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CMsgBoxDlg::OnBnClickedNo()
{
	EndDialog(IDNO);
}

void CMsgBoxDlg::OnBnClickedYes()
{
	EndDialog(IDYES);
}

void CMsgBoxDlg::OnBnClickedRetryConnection()
{
	//OnOK();
	EndDialog(IDRECONNECT);
}

void CMsgBoxDlg::OnBnClickedEnterLocal()
{
	//OnCancel();
	EndDialog(IDENTERLOCAL);
}

void CMsgBoxDlg::InitIcon()
{
	m_ImageList.Create(32, 32, ILC_COLOR32, 0, 0); 

	// 2011/08/15-8201-gxx:                         //弹出询问是否进入本地模式时使用
	if (m_nType == (MB_OK|MB_ICONEXCLAMATION) || m_nType == (MB_RETRYCANCEL|MB_ICONEXCLAMATION))
	{
		m_ImageList.Add(::LoadIcon(NULL, IDI_EXCLAMATION));
		return;
	}

	if (m_nType & MB_ICONINFORMATION)
	{
		m_ImageList.Add(::LoadIcon(NULL, IDI_INFORMATION));
	}
	else if (m_nType & MB_ICONQUESTION)
	{
		m_ImageList.Add(::LoadIcon(NULL, IDI_QUESTION));
	}
	else if (m_nType & MB_ICONERROR)
	{
		m_ImageList.Add(::LoadIcon(NULL, IDI_ERROR));
	}
	else
	{
		m_ImageList.Add(::LoadIcon(NULL, IDI_INFORMATION));
	}
}

void CMsgBoxDlg::InitButton()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_INFO)->GetWindowRect(rect);
	ScreenToClient(rect);
	
	INT nBtnTop = rect.bottom + 20;

	GetClientRect(rect);

	//弹出询问是否进入本地模式对话框
	if(m_nType == (MB_RETRYCANCEL|MB_ICONEXCLAMATION))
	{
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDYES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDNO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDRECONNECT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDENTERLOCAL)->ShowWindow(SW_SHOW);

		GetDlgItem(IDRECONNECT)->SetWindowPos(NULL,rect.Width()/ 2 - 78,nBtnTop,75,23,SWP_SHOWWINDOW);
		GetDlgItem(IDENTERLOCAL)->SetWindowPos(NULL, rect.Width()/ 2 + 3, nBtnTop, 75, 23, SWP_SHOWWINDOW);

		GetDlgItem(IDRECONNECT)->SetFocus();

		return;
		//if (m_nType & MB_DEFBUTTON2)
		//{
		//	GetDlgItem(IDENTERLOCAL)->SetFocus();
		//}
		//else
		//{
		//	GetDlgItem(IDRECONNECT)->SetFocus();
		//}
	}

	if (m_nType & MB_OKCANCEL)
	{
		GetDlgItem(IDRECONNECT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDENTERLOCAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDYES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDNO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_SHOW);

		GetDlgItem(IDOK)->SetWindowPos(NULL, rect.Width()/ 2 - 78, nBtnTop, 75, 23, SWP_SHOWWINDOW);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL, rect.Width()/ 2 + 3, nBtnTop, 75, 23, SWP_SHOWWINDOW);
		
		if (m_nType & MB_DEFBUTTON2)
		{
			GetDlgItem(IDCANCEL)->SetFocus();
		}
		else
		{
			GetDlgItem(IDOK)->SetFocus();
		}
	}
	else if (m_nType & MB_YESNO)
	{
		GetSystemMenu(FALSE)->RemoveMenu(1, MF_BYPOSITION);

		GetDlgItem(IDRECONNECT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDENTERLOCAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDYES)->ShowWindow(SW_SHOW);
		GetDlgItem(IDNO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

		GetDlgItem(IDYES)->SetWindowPos(NULL, rect.Width()/ 2 - 78, nBtnTop, 75, 23, SWP_SHOWWINDOW);
		GetDlgItem(IDNO)->SetWindowPos(NULL, rect.Width()/ 2 + 3, nBtnTop, 75, 23, SWP_SHOWWINDOW);

		if (m_nType & MB_DEFBUTTON2)
		{
			GetDlgItem(IDNO)->SetFocus();
		}
		else
		{
			GetDlgItem(IDYES)->SetFocus();
		}
	}
	else
	{
		GetDlgItem(IDRECONNECT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDENTERLOCAL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDYES)->ShowWindow(SW_HIDE);
		GetDlgItem(IDNO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

		GetDlgItem(IDOK)->SetWindowPos(NULL, (rect.Width() - 75) / 2, nBtnTop, 0, 0, SWP_NOSIZE);
		GetDlgItem(IDOK)->SetFocus();
	}
}

void CMsgBoxDlg::InitWindow()
{
	if (!m_strCaption.IsEmpty())
	{
		SetWindowText(m_strCaption);
	}

	CRect rect;

	CPaintDC dc(GetDlgItem(IDC_STATIC_INFO));
	
	CFont* pOldFont = (CFont*)dc.SelectStockObject(DEFAULT_GUI_FONT);

	dc.DrawText(m_strText, rect, DT_CALCRECT);
	m_TextSize = rect.Size();

	dc.SelectObject(pOldFont);

	if (m_TextSize.cx < 160)
	{
		m_TextSize.cx = 160;
	}
	
	GetDlgItem(IDC_STATIC_INFO)->SetWindowPos(NULL, 0, 0, 
		m_TextSize.cx + 15, m_TextSize.cy + 15, SWP_NOMOVE);

	SetWindowPos(NULL, 0, 0, 100 + m_TextSize.cx, 120 + m_TextSize.cy, SWP_NOMOVE);
}

HBRUSH CMsgBoxDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//if (CTLCOLOR_STATIC == nCtlColor)
	//{
	//	if (pWnd->GetDlgCtrlID() == IDC_STATIC_INFO)
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 255));
	//	}
	//}

	return hbr;
}

BOOL CMsgBoxDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
	{
		CWnd* pWnd = GetFocus(); //焦点何处

		if (pWnd != NULL)
		{
			if (pWnd->GetDlgCtrlID() == IDOK)
			{
				GetDlgItem(IDCANCEL)->SetFocus();

				return TRUE;
			}
			else if (pWnd->GetDlgCtrlID() == IDCANCEL)
			{
				GetDlgItem(IDOK)->SetFocus();

				return TRUE;
			}
			else if (pWnd->GetDlgCtrlID() == IDYES)
			{
				GetDlgItem(IDNO)->SetFocus();

				return TRUE;
			}
			else if (pWnd->GetDlgCtrlID() == IDNO)
			{
				GetDlgItem(IDYES)->SetFocus();

				return TRUE;
			}
		}
	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CMsgBoxDlg::FormatText()
{
	CString strTmp(m_strText);

	strTmp.Remove('\r');
	strTmp.Remove('\n');

	m_strText.Empty();

	const int LineMaxLen = 60;

	while (strTmp.GetLength() > LineMaxLen)
	{
		m_strText.Append(strTmp.Left(LineMaxLen) + _T("\r\n"));
		strTmp = strTmp.Mid(LineMaxLen);
	}

	m_strText.Append(strTmp);
}

void CMsgBoxDlg::SetText(CString newVal,BOOL bFormat)
{
	m_strText = newVal;
	
	if (bFormat)
	{
		FormatText();
	}
	
}

void CMsgBoxDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_nTime / 1000 == 0)
	{
		OnOK();
	}
	m_nTime -= 1000;
}

