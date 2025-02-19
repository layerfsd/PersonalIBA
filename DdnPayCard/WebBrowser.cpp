// WebBrowser.cpp : 实现文件
//

#include "stdafx.h"
#include ".\WebBrowser.h"
#include "DlgCheckPwd.h"

// CWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CHtmlView)

CWebBrowser::CWebBrowser()
: m_lpEventHandler(NULL)
, m_bContextMenuEnabled(TRUE)
, m_bIsFlatStyle(FALSE)
{
	EnableContextMenu(FALSE);
}

CWebBrowser::~CWebBrowser()
{
}

BEGIN_MESSAGE_MAP(CWebBrowser, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


void CWebBrowser::SetEventHandler(CWebBrowserEventHandler* lpEventHandler /*= NULL*/)
{
	m_lpEventHandler = lpEventHandler;
}

// CWebBrowser 消息处理程序

void CWebBrowser::OnTitleChange(LPCTSTR lpszText)
{
	if (m_lpEventHandler != NULL)
	{
		m_lpEventHandler->WebBrowser_TitleChange(*this, lpszText);
	}

	CHtmlView::OnTitleChange(lpszText);
}

void CWebBrowser::OnDocumentComplete(LPCTSTR lpszURL)
{
	if (m_lpEventHandler != NULL)
	{
		m_lpEventHandler->WebBrowser_DocumentComplete(*this, lpszURL);
	}

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


void CWebBrowser::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel)
{
	CHtmlView::OnNewWindow2(ppDisp, Cancel);

	if (m_lpEventHandler != NULL)
	{
		m_lpEventHandler->WebBrowser_NewWindow2(*this, ppDisp, Cancel);
	}
}

void CWebBrowser::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, 
									LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, 
									LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData,
		lpszHeaders, pbCancel);

	if (m_lpEventHandler != NULL)
	{
		m_lpEventHandler->WebBrowser_BeforeNavigate2(*this, lpszURL, nFlags, lpszTargetFrameName, 
			baPostedData, lpszHeaders, pbCancel);
	}
}

void CWebBrowser::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	CHtmlView::OnNavigateError(lpszURL, lpszFrame, dwError, pbCancel);

	if (m_lpEventHandler != NULL)
	{
		m_lpEventHandler->WebBrowser_NavigateError(*this, lpszURL, lpszFrame, dwError, pbCancel);
	}
}

HRESULT CWebBrowser::OnGetExternal(LPDISPATCH *lppDispatch)
{
	if (m_lpEventHandler != NULL)
	{
		return m_lpEventHandler->WebBrowser_GetExternal(*this, lppDispatch);
	}
	else
	{
		return CHtmlView::OnGetExternal(lppDispatch);
	}
}

HRESULT CWebBrowser::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	if (IsFlatStyle())
	{
		pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
	}

	return S_OK;
}

HRESULT CWebBrowser::OnShowContextMenu(DWORD dwID, LPPOINT ppt, LPUNKNOWN pcmdtReserved, 
									   LPDISPATCH pdispReserved)
{
	if (!IsContextMenuEnabled()) //屏蔽右键菜单和键盘菜单键
	{
		return S_OK;
	}

	return S_FALSE;
}

BOOL CWebBrowser::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->wParam == 'Q')
	{
		// 按下 CTRL+ALT+Q就释放右键菜单功能
		if ( GetKeyState(VK_MENU) & 0x8000 && GetKeyState(VK_CONTROL) & 0x8000)
		{
			// 2014-4-11 - qsc 
			if(!IsContextMenuEnabled())
			{
				CDlgCheckPwd dlgCheckPwd;
				if(IDOK == dlgCheckPwd.DoModal())
				{
					EnableContextMenu(TRUE);
				}	
			}
		}
	}
	return CHtmlView::PreTranslateMessage(pMsg);
}
