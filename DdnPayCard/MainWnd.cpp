// MainWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DdnPayCard.h"
#include "PayCardDlg.h"
#include ".\mainwnd.h"
#include "ITipsShow.h"
#include "MDataXSender.h"
#include <WtsApi32.h>
#include "Hooks.h"

// CMainWnd

IMPLEMENT_DYNAMIC(CMainWnd, CWnd)
CMainWnd::CMainWnd()
{
	m_lpPcDlg = NULL;

	m_lpDataXMReceiver = NULL;
	m_bOldNetbar = FALSE;


	EnableAutomation();
}

CMainWnd::~CMainWnd()
{
	if (m_lpDataXMReceiver != NULL)
	{
		delete m_lpDataXMReceiver;
		m_lpDataXMReceiver = NULL;
	}
}


BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	ON_MESSAGE(WM_HOSTMSG, OnHostMsg)
	ON_MESSAGE(WM_WTSSESSION_CHANGE, OnSessionChange)
	ON_WM_DESTROY()
	ON_WM_CREATE()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMainWnd, CWnd) 
	DISP_FUNCTION(CMainWnd, "TipsNotify", OnTipsNotify, VT_EMPTY, VTS_I4)
END_DISPATCH_MAP()


void CMainWnd::OnTipsNotify( INT nCmd )
{
	if (1 == nCmd) //显示点卡
	{
		CreatePcDlg(TRUE);
	}
}

LRESULT CMainWnd::OnHostMsg(WPARAM wParam, LPARAM lParam)
{
	const INT_PTR nCmd = wParam;

	if (0 == nCmd) //显示
	{
		CreatePcDlg(FALSE);
	}
	else if (1 == nCmd)//销毁
	{
		DestroyPcDlg();
	}
	else if (2 == nCmd)
	{
		CreatePcDlg(TRUE);
	}

	CHooks::CheckIsInRemoteDesktop();

	return 0;
}

BOOL CMainWnd::CreatePcDlg(BOOL bCashPay)
{
	// QSCTT
	GXX_LOG_HIT(_T("1"));
	if (m_lpPcDlg != NULL)
	{
		// QSCTT
		GXX_LOG_HIT(_T("2"));
		if (bCashPay)
		{
			// QSCTT
			GXX_LOG_HIT(_T("3"));
			m_lpPcDlg->OpenHomePage(bCashPay, TRUE);
			// QSCTT
			GXX_LOG_HIT(_T("4"));
		}

		m_lpPcDlg->BringWindowToTop();
		m_lpPcDlg->ShowWindow(SW_NORMAL);

		return TRUE;
	}
	// QSCTT
	GXX_LOG_HIT(_T("5"));
	m_lpPcDlg = new CPayCardDlg;

	if (!m_lpPcDlg->AnalyzeParamString(m_strParams))
	{
		// QSCTT
		GXX_LOG_HIT(_T("6"));
		delete m_lpPcDlg;

		m_lpPcDlg = NULL;

		return FALSE;
	}
	else
	{
		// QSCTT
		GXX_LOG_HIT(_T("7"));
		m_bOldNetbar = m_lpPcDlg->m_bOldNetbar;
		m_lpPcDlg->Create(CPayCardDlg::IDD, CWnd::GetDesktopWindow());
		// QSCTT
		GXX_LOG_HIT(_T("8"));
		m_lpPcDlg->OpenHomePage(bCashPay,FALSE);
		// QSCTT
		GXX_LOG_HIT(_T("9"));

		m_lpPcDlg->BringWindowToTop();
		m_lpPcDlg->ShowWindow(SW_NORMAL);
	}

	return TRUE;
}

void CMainWnd::DestroyPcDlg()
{
	if (m_lpPcDlg != NULL)
	{
		m_lpPcDlg->ShowWindow(SW_HIDE);
		m_lpPcDlg->DestroyWindow();

		delete m_lpPcDlg;
		m_lpPcDlg = NULL;
	}
}


void CMainWnd::OnDestroy()
{
	WTSUnRegisterSessionNotification(GetSafeHwnd());

	DestroyPcDlg();

	if (m_lpDataXMReceiver != NULL)
	{
		delete m_lpDataXMReceiver;
		m_lpDataXMReceiver = NULL;
	}

	CWnd::OnDestroy();
}

int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	WTSRegisterSessionNotification(GetSafeHwnd(), NOTIFY_FOR_ALL_SESSIONS);

	m_lpDataXMReceiver = new NS_DataX::CDataXMReceiverT<CMainWnd>
		(this, OnDataArrival);

	BOOL bRet = m_lpDataXMReceiver->Create(_T("DdnPcp-CA591950-AA6A-495a-9ABE-C02374F479FE"));

	ASSERT(bRet);

	return 0;
}

INT CMainWnd::OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, LPVOID lpBuf, size_t cbLen)
{
	if (1 == nMsg)
	{
		Tips_ShowParam tips_ShowParam = {0};
		tips_ShowParam.length = sizeof Tips_ShowParam;
		tips_ShowParam.nDelayDisappearTime = 10;

		ITipsShow *pTipsShow = CreateTipsShowObject();
		pTipsShow->SetShowParam(tips_ShowParam);
		pTipsShow->SetExternal(GetIDispatch(TRUE));


		TCHAR szURL[1024] = {0};
		memcpy(szURL, lpBuf, cbLen);
		pTipsShow->ShowTips(szURL);

		//pTipsShow->ShowTips(str, "<a href=\"javascript:window.external.GetHostApp().TipsNotify(1);\">显示!</a><a href=\"javascript:window.external.Close();\">关闭!</a>");

		delete pTipsShow;
	}

	return 0;
}

LRESULT CMainWnd::OnSessionChange(WPARAM wParam, LPARAM lParam)
{
	CHooks::CheckIsInRemoteDesktop();

	return 0;
}