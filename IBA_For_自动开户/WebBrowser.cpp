// WebBrowser.cpp : 实现文件
//

#include "stdafx.h"
#include ".\WebBrowser.h"


// CWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CHtmlView)

CWebBrowser::CWebBrowser()
: m_bContextMenuEnabled(TRUE)
{
}

CWebBrowser::~CWebBrowser()
{
}

BEGIN_MESSAGE_MAP(CWebBrowser, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CWebBrowser 消息处理程序

void CWebBrowser::OnTitleChange(LPCTSTR lpszText)
{
	// TODO: 在此添加专用代码和/或调用基类

	CHtmlView::OnTitleChange(lpszText);
}

void CWebBrowser::OnDocumentComplete(LPCTSTR lpszURL)
{
	// TODO: 在此添加专用代码和/或调用基类

	CHtmlView::OnDocumentComplete(lpszURL);
}


void CWebBrowser::OnDestroy()
{
	m_pBrowserApp.Release();

	m_wndBrowser.DestroyWindow();

	CWnd::OnDestroy();
}

int CWebBrowser::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CWebBrowser::PostNcDestroy()
{
	//防止自删除
	//CHtmlView::PostNcDestroy();
}

BOOL CWebBrowser::PreTranslateMessage(MSG* pMsg)
{
	if (WM_RBUTTONUP == pMsg->message || WM_CONTEXTMENU == pMsg->message)
	{
		if (!IsContextMenuEnabled()) //屏蔽右键菜单和键盘菜单键
		{
			return TRUE;
		}
	}

	return CHtmlView::PreTranslateMessage(pMsg);
}
