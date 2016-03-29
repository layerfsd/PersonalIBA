// TipsWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include ".\TipsWnd.h"
#include "PcpThread.h"
#include "DdnPayCard.h"


// CTipsWnd

IMPLEMENT_DYNAMIC(CTipsWnd, CWnd)
CTipsWnd::CTipsWnd()
: m_ClientRect(CRect(0, 0, 0, 0)), m_pWebBrowser(0), m_Factor(255)
, m_blisComplete(FALSE), m_nTimeCount(0), m_lpExternalDisp(NULL)
{
	memset(&m_tips_showParam, 0, sizeof(Tips_ShowParam));

	EnableAutomation();
}

CTipsWnd::~CTipsWnd()
{
	m_TextFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTipsWnd, CWnd)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_WNDCLOSEBTN, &CTipsWnd::OnBnClickedClosebtn)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTipsWnd, CWnd) 
	DISP_FUNCTION(CTipsWnd, "WndClose", HideTips, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTipsWnd, "WndLooK", OnLookDetail, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CTipsWnd, "SetAppearTime", SetAppearTime, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CTipsWnd, "GetHostApp", GetHostApp, VT_DISPATCH, VTS_NONE)
END_DISPATCH_MAP()

// CTipsWnd 消息处理程序

void CTipsWnd::WebBrowser_TitleChange( CWebBrowser& Sender, LPCTSTR lpszText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CComPtr<IDispatch> pDispatch = m_pWebBrowser->GetHtmlDocument();

	if (pDispatch)
	{
		CComQIPtr<IHTMLDocument2> pDocument = pDispatch;

		CComBSTR bstr;

		HRESULT hr = pDocument->get_title(&bstr);

		CString strTitle(bstr);

		if (strTitle.Find(_T("http")) < 0 && strTitle.IsEmpty())
		{

			return;
		}
		SetDlgItemText(IDC_WNDTIPSTYPE, strTitle); //设置标题类型名称

		//InvalidateRect(m_ClientRect, TRUE);

		m_blisComplete = TRUE;

		StartDisplayWindow();

	}
}

void CTipsWnd::WebBrowser_DocumentComplete( CWebBrowser& Sender, LPCTSTR lpszURL )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_pWebBrowser->GetLocationURL() == "about:blank")
	{
		return ;
	}

	HideBrowserBorder();

	CComPtr<IDispatch> pDispatch = m_pWebBrowser->GetHtmlDocument();

	if (pDispatch)
	{
		CComQIPtr<IHTMLDocument2> pDocument = pDispatch;

		CComBSTR bstr;

		HRESULT hr = pDocument->get_title(&bstr);

		CString strTitle(bstr);

		SetDlgItemText(IDC_WNDTIPSTYPE, strTitle); //设置标题类型名称

		//InvalidateRect(m_ClientRect, TRUE);

		m_blisComplete = TRUE;

		StartDisplayWindow();

		RedrawWindow();
	}
}

void CTipsWnd::WebBrowser_NewWindow2(CWebBrowser& Sender,LPDISPATCH* ppDisp, BOOL* Cancel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShowWindow(SW_HIDE);
}

HRESULT CTipsWnd::WebBrowser_GetExternal(CWebBrowser& Sender,LPDISPATCH *lppDispatch )
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_lpExternalDisp != NULL)
	{
		*lppDispatch = GetIDispatch(TRUE); //m_lpExternalDisp;//

		return S_OK;
	}

	return S_FALSE;
}

LPDISPATCH CTipsWnd::GetHostApp()
{
	//AFX_MANAGE_STATE(AfxGetAppModuleState());

	m_lpExternalDisp->AddRef(); //必引用一次

	return m_lpExternalDisp;
}

void CTipsWnd::HideTips()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());

	OnBnClickedClosebtn();

	//ShowWindow(SW_HIDE);
}

BOOL CTipsWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	if (!CreateEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, lpszClassName, NULL, WS_POPUP,
		100, 100, 150, 50, NULL, NULL, NULL))
	{
		return FALSE;
	}

	InitResource();
	
	CreateWebBrower();

	CalculateControlPostion();

	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | 0x80000);

	return TRUE;
}

BOOL CTipsWnd::OnEraseBkgnd(CDC* pDC)
{
	//if (DrawWindow(pDC))
	{
		return TRUE;
	}

	return CWnd::OnEraseBkgnd(pDC);
}


void CTipsWnd::OnTimer(UINT nIDEvent)
{
	if (TimerIDOfStartDisplay == nIDEvent) //开始显示
	{	
		StartDisplayWindow();
	}
	else if (TimerIDOfDisplaying == nIDEvent) //显示当中
	{
		DisplayingWindow(nIDEvent);
	}
	else if (TimerIDOfDisapper == nIDEvent) //渐隐
	{
		if (DisapperWindow(nIDEvent))
		{
			return;
		}
	}

	CWnd::OnTimer(nIDEvent);
}

HBRUSH CTipsWnd::SetStaticTitle( CDC* pDC )
{
	pDC->SetTextColor(RGB(255, 255, 255));

	pDC->SetBkMode(TRANSPARENT);   //设置背景透明

	return HBRUSH(GetStockObject(HOLLOW_BRUSH));
}

HBRUSH CTipsWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		return SetStaticTitle(pDC);
	}   

	return hbr;
}

void CTipsWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		SetLayeredWindowAttributes (RGB(255,255,255), 255, LWA_ALPHA);
	}
}

void CTipsWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y)); 
	CWnd::OnLButtonDown(nFlags, point);

}

void CTipsWnd::SetTipsParam( Tips_ShowParam tips_showParam )
{
	memcpy(&m_tips_showParam, &tips_showParam, sizeof Tips_ShowParam);
}

void CTipsWnd::ShowTips(CString strUrl)
{
	RECT ScreenRect;  //不包含任务栏的桌面大小
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID) &ScreenRect, 0); 

	INT nLeft = m_tips_showParam.DistanceRect.left;
	INT nTop =  m_tips_showParam.DistanceRect.top;

	if (0 < nLeft || 0 < nTop)
	{
		SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOSIZE | SW_SHOWNOACTIVATE | SWP_NOSENDCHANGING);
	}
	else
	{
		nLeft = ScreenRect.right - m_tips_showParam.DistanceRect.right - m_ClientRect.Width();

		nTop = ScreenRect.bottom - m_tips_showParam.DistanceRect.bottom - m_ClientRect.Height();

		SetWindowPos(NULL , nLeft, nTop, 0, 0, SWP_NOSIZE | SW_SHOWNOACTIVATE | SWP_NOSENDCHANGING);
	}

	if (0 == m_tips_showParam.nContentType) //等于0就是使用网址URL打开
	{
		OpenUrl(strUrl, "", "");
	}
	else
	{

	}

	SetTimer(TimerIDOfStartDisplay, 6000, NULL); //开始加载页面超时处理
}

void CTipsWnd::ShowTips( CString strTitle, CString strContents )
{
	m_pWebBrowser->Navigate(_T("about:blank"), NULL, NULL, NULL, NULL); 

	if (0 == m_tips_showParam.nContentType) //等于0就是使用网址URL打开
	{
		WirteWebPage(strTitle, strContents);
	}
	else
	{

	}

	RECT ScreenRect = {0};  //不包含任务栏的桌面大小
	SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID) &ScreenRect, 0); 

	INT nLeft = m_tips_showParam.DistanceRect.left;
	INT nTop =  m_tips_showParam.DistanceRect.top;

	if (0 < nLeft || 0 < nTop)
	{
		SetWindowPos(NULL, nLeft, nTop, 0, 0, SWP_NOSIZE | SW_SHOWNOACTIVATE | SWP_NOSENDCHANGING);
	}
	else
	{
		nLeft = ScreenRect.right - m_tips_showParam.DistanceRect.right - m_ClientRect.Width();

		nTop = ScreenRect.bottom - m_tips_showParam.DistanceRect.bottom - m_ClientRect.Height();

		SetWindowPos(NULL , nLeft, nTop, 0, 0, SWP_NOSIZE | SW_SHOWNOACTIVATE | SWP_NOSENDCHANGING);
	}

	SetTimer(TimerIDOfStartDisplay, 6000, NULL); //开始加载页面超时处理
}


//向 IHTMLDocument2  写入 HTML 文本
int CTipsWnd::WirteWebPage(CString strTitle, CString strContent)
{
	CComPtr<IDispatch> pDispatch = m_pWebBrowser->GetHtmlDocument();

	if (pDispatch)
	{
		CComQIPtr<IHTMLDocument2> pDocument = pDispatch;

		if (pDocument != NULL)
		{
			// construct text to be written to browser as SAFEARRAY

			SAFEARRAY* safe_array = SafeArrayCreateVector(VT_VARIANT, 0, 1);

			VARIANT* variant;

			SafeArrayAccessData(safe_array, (LPVOID *) &variant);

			variant->vt = VT_BSTR;

			CString string = CombinationWebPage(strTitle, strContent);

			variant->bstrVal = string.AllocSysString();

			SafeArrayUnaccessData(safe_array);

			// write SAFEARRAY to browser document

			pDocument->write(safe_array);


			SafeArrayDestroy(safe_array);
		}
	}
	return 0;

}

void CTipsWnd::OpenUrl( CString strUrl ,LPCTSTR postData, LPCTSTR headData)
{
	m_pWebBrowser->Navigate(strUrl);
}

BOOL CTipsWnd::Aminhide()
{
	m_Factor = m_Factor - 2;

	SetLayeredWindowAttributes(RGB(254, 254, 255), m_Factor, LWA_COLORKEY|LWA_ALPHA);

	if (m_Factor <= 3)
	{
		m_Factor = 255;

		return TRUE;
	}

	return FALSE;
}

void CTipsWnd::StartDisplayWindow( )
{
	SetWindowPos(&wndTopMost, 0, 0, 0, 0,  SWP_NOMOVE | SWP_NOSIZE);

	ShowWindow(SW_SHOWNOACTIVATE);

	KillTimer(TimerIDOfStartDisplay);

	SetTimer(TimerIDOfDisplaying, 1000, NULL);
}

void CTipsWnd::DisplayingWindow( UINT nTimerEvent )
{
	if (IsMouseInWindowRect()) //鼠标在窗口中停止几时
	{
		m_nTimeCount = 0; 
	}
	else 
	{
		m_nTimeCount++;

		if (m_nTimeCount >= m_tips_showParam.nDelayDisappearTime) //判断是否到达渐隐的时间
		{
			m_nTimeCount = 0;

			KillTimer(nTimerEvent);
			SetTimer(TimerIDOfDisapper, 30, NULL); // 255 / 2 * 0.025 = 大约 3秒
		}
	}
}

BOOL CTipsWnd::DisapperWindow( UINT nTimerEvent )
{
	if (IsMouseInWindowRect()) //鼠标在窗口中
	{
		m_Factor = 255;
		SetLayeredWindowAttributes(RGB(254,254,255), m_Factor, LWA_COLORKEY|LWA_ALPHA);
		KillTimer(nTimerEvent);
		SetTimer(TimerIDOfDisplaying, 1000, NULL);
	}
	else
	{
		if (Aminhide())
		{
			OnBnClickedClosebtn();

			return TRUE; 
		}//开始淡隐显示动画
	}

	return FALSE;
}

BOOL CTipsWnd::IsMouseInWindowRect()
{
	CRect	windowRect;
	GetWindowRect(&windowRect);	

	CPoint	mousePoint;
	GetCursorPos(&mousePoint);

	BOOL bResult = FALSE;

	if (windowRect.PtInRect(mousePoint))
	{
		bResult = TRUE;
	}

	return bResult;
}

void CTipsWnd::HideBrowserBorder()
{
	IHTMLDocument2* phtmldoc2 = NULL;   
	IHTMLElement* phtmlElement = NULL;   
	IDispatch* pdisp = NULL;   
	pdisp = m_pWebBrowser->GetHtmlDocument();  
	m_pWebBrowser->EnableScrollBar(FALSE);

	if (pdisp != NULL)
	{
		pdisp->QueryInterface(IID_IHTMLDocument2, (void**) &phtmldoc2);   
		pdisp->Release();
	}   

	if (phtmldoc2 != NULL)
	{
		phtmldoc2->get_body(&phtmlElement);   
		phtmldoc2->Release();
	}   

	if (phtmlElement != NULL)
	{
		
		IHTMLStyle* phtmlStyle;   
		phtmlElement->get_style(&phtmlStyle);   

		IHTMLBodyElement *pelem = NULL;

		phtmlElement->QueryInterface(IID_IHTMLBodyElement, (void**)&pelem );

		if (phtmlStyle != NULL && pelem != NULL)
		{
			phtmlStyle->put_overflow(L"hidden");   
			phtmlStyle->put_border(L"no");//   去除边框   
			pelem->put_scroll(L"no");

			pelem->Release();
			phtmlStyle->Release();   
			phtmlElement->Release();
		}
	}
}


BOOL CTipsWnd::CreateWebBrower()
{
	m_pWebBrowser = new CWebBrowser;	

	//CRect rect(5, 26, m_ClientRect.Width() - 5, m_ClientRect.Height() - 4);

	CRect rcClient;
	GetClientRect(rcClient);

	BOOL bret = m_pWebBrowser->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rcClient, this, 0, NULL);

	m_pWebBrowser->EnableContextMenu(TRUE);

	m_pWebBrowser->SetEventHandler(this);

	return bret;

}

void CTipsWnd::InitResource()
{
	m_CloseBtn.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, CRect(0, 0, 0, 0), this, IDC_WNDCLOSEBTN);

	m_TitleStatic.Create(NULL, WS_CHILD | WS_VISIBLE , CRect(0, 0, 0, 0), this, IDC_WNDTIPSTYPE);

	m_TextFont.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		0,                  // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));			   // lpszFacename

	m_TitleStatic.SetFont(&m_TextFont, TRUE);

	m_backImg.LoadFromOtherResource(AfxGetInstanceHandle(), IDP_TIPSBK, "PNG");

	m_CloseBtn.LoadBitmap(AfxGetInstanceHandle(), IDB_TIPSCLOSEBTN, 1);
	//m_CloseBtn.SetToolTipText(IDS_CLOSE);

}

void CTipsWnd::CalculateControlPostion()
{
	m_ClientRect.right = 270;//m_backImg.GetWidth();
	m_ClientRect.bottom = 162;//m_backImg.GetHeight();

	m_pWebBrowser->SetFlatStyle(TRUE);
	//CRgn MyRgn;
	MoveWindow(&m_ClientRect, TRUE);
	//MyRgn.CreateRoundRectRgn(0, 0, m_ClientRect.right, m_ClientRect.bottom, 16, 16);
	//SetWindowRgn(MyRgn, TRUE);

	//m_CloseBtn.MoveWindow(m_ClientRect.right - 21, 8, 13, 13, TRUE);

	//m_TitleStatic.MoveWindow(40, 8, 160, 52);

	m_CloseBtn.ShowWindow(FALSE);
	m_TitleStatic.ShowWindow(FALSE);

	//m_pWebBrowser->MoveWindow(5, 26, m_ClientRect.Width() - 10, m_ClientRect.Height() - 30);
	m_pWebBrowser->MoveWindow(0, 0, m_ClientRect.Width(), m_ClientRect.Height());
}

BOOL CTipsWnd::DrawWindow(CDC* pDC)
{
	BOOL bResult = FALSE;

	if (!m_backImg.IsNull())
	{
		m_backImg.Draw(pDC->GetSafeHdc(), 0, 0, m_ClientRect.Width(), m_ClientRect.Height());

		bResult = TRUE;
	}

	return bResult;
}

void CTipsWnd::OnBnClickedClosebtn()
{
	m_blisComplete = FALSE;
	m_Factor = 255;

	KillTimer(TimerIDOfDisapper);
	KillTimer(TimerIDOfStartDisplay);
	KillTimer(TimerIDOfDisplaying);
	ShowWindow(SW_HIDE);

	PostMessage(WM_CLOSE);

}
void CTipsWnd::PostNcDestroy()
{
	delete this;
}

CString CTipsWnd::CombinationWebPage( CString strTitle, CString strContent )
{
	LPCTSTR strWebPage = _T("<html><head><title>%s</title></head><body scroll = no>%s</body></html>");

	CString strWeb;

	strWeb.Format(strWebPage, strTitle, strContent);

	return strWeb;
}


BOOL CTipsWnd::DestroyWindow()
{
	if (m_pWebBrowser != NULL)
	{
		m_pWebBrowser->DestroyWindow();
		delete m_pWebBrowser;
		m_pWebBrowser = NULL;
	}

	return __super::DestroyWindow();
}

void CTipsWnd::SetAppearTime(INT nAppearTime)
{
	m_tips_showParam.nDelayDisappearTime = nAppearTime;
}

void CTipsWnd::OnLookDetail()
{	
	::PostMessage(CPcpThread::m_hMainWnd,WM_HOSTMSG,2,0);
	OnBnClickedClosebtn();
}