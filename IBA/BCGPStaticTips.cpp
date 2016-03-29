#include "stdafx.h"
#include ".\bcgpstatictips.h"

// CBCGPStaticTips

IMPLEMENT_DYNAMIC(CBCGPStaticTips, CBCGPStatic)
CBCGPStaticTips::CBCGPStaticTips()
:m_hwndTips(NULL)
{
}

CBCGPStaticTips::~CBCGPStaticTips()
{
}


BEGIN_MESSAGE_MAP(CBCGPStaticTips, CBCGPStatic)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CBCGPStaticTips::SetWindowText( LPCTSTR lpszString )
{
	CBCGPStatic::SetWindowText(lpszString);

	if(!m_hwndTips || !IsWindow(m_hwndTips))
	{
		CreateToolTip();
	}
	else
	{
		SetToolTip(lpszString);
	}
}



// CBCGPStaticTips 消息处理程序


int CBCGPStaticTips::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CreateToolTip();

	return 0;
}

HWND CBCGPStaticTips::CreateToolTip() 
{ 
	if(!m_hwndTips && !IsWindow(m_hwndTips))
	{
		m_hwndTips = CreateWindowEx(WS_EX_TOPMOST, 
			TOOLTIPS_CLASS, 
			NULL, 
			WS_POPUP | TTS_ALWAYSTIP, //| TTS_BALLOON, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			GetParent()->GetSafeHwnd(), 
			NULL, 
			AfxGetInstanceHandle(), 
			NULL); 
	}


	if (!m_hwndTips) 
	{ 
		return (HWND)NULL; 
	} 
	CString strWndText;
	GetWindowText(strWndText);

	HWND hWnd =GetSafeHwnd();
	TOOLINFO toolInfo = {0}; 
	toolInfo.cbSize = sizeof(TOOLINFO); 
	toolInfo.hwnd = GetParent()->GetSafeHwnd(); 
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS; 
	toolInfo.uId = (UINT_PTR)GetSafeHwnd(); 
	toolInfo.lpszText = (LPTSTR)strWndText.GetString(); 
	::SendMessage(m_hwndTips, TTM_ADDTOOL, 0, (LPARAM)&toolInfo); 
	return m_hwndTips; 
} 

LRESULT CBCGPStaticTips::SetToolTip(LPCTSTR pszText)  
{
	CString strText(pszText);
	TOOLINFO toolInfo = {0}; 
	toolInfo.cbSize = sizeof(TOOLINFO); 
	toolInfo.hwnd = GetParent()->GetSafeHwnd(); 
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS; 
	toolInfo.uId = (UINT_PTR)GetSafeHwnd(); 
	toolInfo.lpszText = (LPTSTR)strText.GetString(); 
	return ::SendMessage(m_hwndTips, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo); 
}

LRESULT CBCGPStaticTips::DeleteToolTip()  
{
	TOOLINFO toolInfo = {0}; 
	toolInfo.cbSize = sizeof(TOOLINFO); 
	toolInfo.hwnd = GetParent()->GetSafeHwnd(); 
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS; 
	toolInfo.uId = (UINT_PTR)GetSafeHwnd(); 
	return ::SendMessage(m_hwndTips, TTM_DELTOOL, 0, (LPARAM)&toolInfo); 
}
void CBCGPStaticTips::OnDestroy()
{
	CBCGPStatic::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	DeleteToolTip();
}

void CBCGPStaticTips::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	// 用MFC的对话框进行窗口子类化的时候并不会响应WM_CREATE消息
	// 因为子类话实在DoDataExchange进行的,而这个函数又是在OnInitDialog完成
	// 这个时候控件已经创建完成	
	CBCGPStatic::PreSubclassWindow();
	DWORD dwSytle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
	dwSytle |= SS_NOTIFY;
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwSytle);

	CreateToolTip();
}


void CBCGPStaticTips::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strWndText;
	GetWindowText(strWndText);
	SetToolTip(strWndText);

	CBCGPStatic::OnMouseMove(nFlags, point);
}
