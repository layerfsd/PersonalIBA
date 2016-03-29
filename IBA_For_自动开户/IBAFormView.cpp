// IBAFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include "MsgBoxDlg.h"
#include ".\ibaformview.h"

// CIBAFormView


CIBAFormView::CIBAFormView(UINT nIDTemplate)
: CStringLoaderView<CBCGPFormView>(nIDTemplate)
{
	EnableVisualManagerStyle(); //开启皮肤效果
}

CIBAFormView::~CIBAFormView()
{
}

BEGIN_MESSAGE_MAP(CIBAFormView, CBCGPFormView)
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CIBAFormView::OnInitialUpdate()
{
	CStringLoaderView<CBCGPFormView>::OnInitialUpdate();

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, TRUE);  //使其平面

	CIBAHelpper::CreateFont(m_Font, 14);
	CIBAHelpper::CreateFont(m_BigFont, 18, 700);

	m_ToolTip.Create(this);
	m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	m_ToolTip.SetEffectBk(12);
	m_ToolTip.SetBorder(RGB(200, 200, 200));

	
}

void CIBAFormView::SetToolTipPos(UINT uID)
{
	GetDlgItem(uID)->GetWindowRect(m_ToolTipRect);
}

void CIBAFormView::ShowToolTip(UINT uID, BOOL bErrorInfo)
{
	CIBAString strTmp;

	strTmp.LoadString(uID);

	ShowToolTip(strTmp, bErrorInfo);
}

void CIBAFormView::ShowToolTip(CString strInfo, BOOL bErrorInfo)
{
	CIBAString strToolTip;
	strToolTip.LoadString(IDS_TOOLTIPHEADER);

	if (bErrorInfo)
	{
		m_ToolTip.SetColorBk(RGB(255, 100, 100), RGB(255, 120, 120), RGB(255, 100, 100));
	}
	else
	{
		m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	}

	m_ToolTip.ShowHelpTooltip(&m_ToolTipRect.TopLeft(), strToolTip + strInfo);

	SetFocus();
}

void CIBAFormView::ShowToolTip(UINT uResID, UINT nPosID, BOOL bErrorInfo)
{
	SetToolTipPos(nPosID);
	ShowToolTip(uResID, bErrorInfo);
}

void CIBAFormView::ShowToolTipFormat(UINT nFormatId, ...)
{
	CIBAString strTmp, strFormat;

	strFormat.LoadString(nFormatId);

	va_list argList;
	va_start(argList, nFormatId);
	strTmp.FormatV(strFormat, argList);
	va_end(argList);

	ShowToolTip(strTmp);
}

void CIBAFormView::SetEditFont()
{
	CWnd* pChildWnd = GetWindow(GW_CHILD);   

	while (pChildWnd != NULL)   
	{
		TCHAR szClassName[MAX_PATH] = {0};
		GetClassName(pChildWnd->GetSafeHwnd(), szClassName, MAX_PATH);

		if (lstrcmp(szClassName, _T("Edit")) == 0)
		{
			GetDlgItem(pChildWnd->GetDlgCtrlID())->SetFont(&m_Font);
		}

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);  
	}
}

BOOL CIBAFormView::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CBCGPFormView::PreTranslateMessage(pMsg);
}

void CIBAFormView::OnKillFocus(CWnd* pNewWnd)
{
	m_ToolTip.HideTooltip();

	CBCGPFormView::OnKillFocus(pNewWnd);
}

INT_PTR CIBAFormView::MsgBox(LPCTSTR lpszText, UINT nType /*= MB_OK*/, LPCTSTR lpszCaption /*= NULL*/)
{
	CMsgBoxDlg dlg(this);
	dlg.SetCaption(lpszCaption);
	dlg.SetText(lpszText);
	dlg.SetType(nType);

	return dlg.DoModal();
}

INT_PTR CIBAFormView::MsgBox(UINT nResId, UINT nType /*= MB_OK*/, LPCTSTR lpszCaption /*= NULL*/)
{
	CIBAString strTmp;
	strTmp.LoadString(nResId);

	return MsgBox(strTmp, nType, lpszCaption);
}

INT_PTR CIBAFormView::MsgBoxFormat(UINT nType, UINT nFormatId, ...)
{
	CIBAString strTmp, strFormat;

	strFormat.LoadString(nFormatId);

	va_list argList;
	va_start(argList, nFormatId);
	strTmp.FormatV(strFormat, argList);
	va_end(argList);

	return MsgBox(strTmp, nType, NULL);
}

void CIBAFormView::OnClose()
{
	//CBCGPFormView::OnClose();

	theApp.GetIBAView()->PostMessage(WM_CHILDCLOSE, GetDlgCtrlID(), (LPARAM)GetSafeHwnd());
}
