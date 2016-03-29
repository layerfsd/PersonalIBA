#include "StdAfx.h"
#include "TipsShow.h"

CTipsShow::CTipsShow()
{
	m_lpExternal = NULL;

	m_hWndLastTips = NULL;
}

CTipsShow::~CTipsShow( void )
{

}

int CTipsShow::ShowTips( LPCTSTR szURL)
{
	//全屏是否弹出

	if (!m_TipsParam.bFullScreenShow && IsFullScreenMode())
	{
		return -1; //-1表全屏不能弹
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTipsWnd *pTipsWnd = CreateTipsByWndMode();

	if (pTipsWnd)
	{
		CString strURL = szURL;

		pTipsWnd->ShowTips(strURL);
	}

	return 0; //表示成功
}

int CTipsShow::ShowTips( LPCTSTR szTitle, LPCTSTR szContent )
{
	//全屏是否弹出

	if (!m_TipsParam.bFullScreenShow && IsFullScreenMode())
	{
		return -1; //-1表全屏不能弹
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTipsWnd *pTipsWnd = CreateTipsByWndMode();

	if (pTipsWnd)
	{
		CString strTitle = szTitle;

		CString strContent = szContent;

		pTipsWnd->ShowTips(strTitle, strContent);
	}

	return 0; //表示成功
}

void CTipsShow::SetShowParam( Tips_ShowParam tips_ShowParam )
{
	size_t nlengthmin = tips_ShowParam.length < sizeof Tips_ShowParam ? tips_ShowParam.length:sizeof Tips_ShowParam;

	memcpy(&m_TipsParam, &tips_ShowParam, nlengthmin);
}


BOOL CTipsShow::IsFullScreenMode()
{
	BOOL bRet = FALSE;

	HWND hDesktopWindow = GetDesktopWindow();
	HWND hShellWindow = GetShellWindow();

	DWORD dwDesktopWindow = 0, dwShellWindow = 0;

	GetWindowThreadProcessId(hDesktopWindow, &dwDesktopWindow);
	GetWindowThreadProcessId(hShellWindow, &dwShellWindow);

	const INT nWidth = GetSystemMetrics(SM_CXSCREEN);
	const INT nHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwTmp = 0;

	HWND hTmp = FindWindowEx(NULL, NULL, NULL, NULL);

	while (hTmp != NULL)
	{
		CRect rect;

		GetWindowRect(hTmp, &rect);

		if (nWidth == rect.Width() && nHeight == rect.Height())
		{
			GetWindowThreadProcessId(hTmp, &dwTmp);

			if (dwTmp != dwShellWindow && dwTmp != dwDesktopWindow)
			{
				bRet = TRUE;

				TRACE("FullScreen:%d\n", dwTmp);

				break;
			}
		}

		hTmp = FindWindowEx(NULL, hTmp, NULL, NULL);
	}

	return bRet;
}

CTipsWnd* CTipsShow::CreateTipsByWndMode()
{
	CTipsWnd *pTipsWnd = NULL;

	pTipsWnd = new CTipsWnd();

	LPCSTR szClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	if (!pTipsWnd->Create(szClassName, _T("DdnTipsWnd"), WS_VISIBLE,
		CRect(0, 0, 0, 0), NULL, NULL))
	{
		return NULL;
	}

	pTipsWnd->SetTipsParam(m_TipsParam);
	pTipsWnd->m_lpExternalDisp = m_lpExternal;

	pTipsWnd->ShowWindow(SW_HIDE);

	m_hWndLastTips = pTipsWnd->GetSafeHwnd();

	return pTipsWnd;
}

void CTipsShow::SetExternal(LPDISPATCH lpExternal)
{
	m_lpExternal = lpExternal;
}

void CTipsShow::HideTips()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IsWindow(m_hWndLastTips))
	{
		CWnd* pTipsWnd = CWnd::FromHandlePermanent(m_hWndLastTips);

		if (pTipsWnd != NULL)
		{
			pTipsWnd->DestroyWindow();
		}
	}
}