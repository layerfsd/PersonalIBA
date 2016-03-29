#include "stdafx.h"
#include "IBAHelpper.h"
#include "resource.h"
#include "MsgBoxDlg.h"
#include ".\ibadialog.h"
#include "IBA_DEBUG.h"
#include "IBALog.h"

BEGIN_MESSAGE_MAP(CIBADialog, CBCGPDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CIBADialog::CIBADialog(UINT nIDTemplate, CWnd *pParent/*= NULL*/)
: CStringLoaderDlg<CBCGPDialog>(nIDTemplate, pParent)
, m_nDialogResult(IDCANCEL)
{
	EnableVisualManagerStyle();
}

void CIBADialog::OnOK()
{
	//CBCGPDialog::OnOK();
}

void CIBADialog::OnCancel()
{
	CBCGPDialog::OnCancel();
}

/*************************************************
Function:       // CIBADialog::OnInitDialog
Description:    // CStringLoaderDlg<CBCGPDialog>::OnInitDialog()设置窗口文字,其他初始动作...
				// 设置字体,设置工具条
Calls:          //
Table Accessed: //
Table Updated:  //
Input:          //
                //
Output:         //
Return:         //
Others:         //
*************************************************/
BOOL CIBADialog::OnInitDialog()
{
	CStringLoaderDlg<CBCGPDialog>::OnInitDialog();

	CIBAHelpper::CreateFont(m_Font, 15);
	CIBAHelpper::CreateFont(m_NormalFont, 13);
	CIBAHelpper::CreateFont(m_FontUnderline,12,FW_NORMAL,TRUE);
	CIBAHelpper::CreateFont(m_BigFont, 22, 900);
	CIBAHelpper::CreateFont(m_BigFontUnderline,22,900,TRUE);
	CIBAHelpper::CreateFont(m_FontPlus1Bold,15,FW_SEMIBOLD);
	CIBAHelpper::CreateFont(m_FontSmall,12,FW_NORMAL);
	CIBAHelpper::CreateFont(m_FontSmallBold, 12, 600);

	m_ToolTip.Create(this);
	m_ToolTip.SetColorBk(RGB(200, 200, 200), RGB(220, 220, 220), RGB(200, 200, 200));
	m_ToolTip.SetEffectBk(12);
	m_ToolTip.SetBorder(RGB(200, 200, 200));

	m_ToolTip.SetColorBk(RGB(255, 100, 100), RGB(255, 120, 120), RGB(255, 100, 100));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CIBADialog::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CIBADialog::SetToolTipPos(UINT uID)
{
	if (GetDlgItem(uID) != NULL)
	{
		GetDlgItem(uID)->GetWindowRect(m_ToolTipRect);
	}
	else
	{
		GetDlgItem(IDOK)->GetWindowRect(m_ToolTipRect);
	}
}

void CIBADialog::ShowToolTip(UINT uID, BOOL bErrorInfo)
{
	CIBAString strTmp;

	strTmp.LoadString(uID);

	ShowToolTip(strTmp, bErrorInfo);
}

void CIBADialog::ShowToolTip(CString strInfo, BOOL bErrorInfo)
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

void CIBADialog::ShowToolTip(UINT uResID, UINT nPosID, BOOL bErrorInfo)
{
	SetToolTipPos(nPosID);
	ShowToolTip(uResID, bErrorInfo);
}

void CIBADialog::ShowToolTipFormat(UINT nFormatId, ...)
{
	CIBAString strTmp, strFormat;
	
	strFormat.LoadString(nFormatId);

	va_list argList;
	va_start(argList, nFormatId);
	strTmp.FormatV(strFormat, argList);
	va_end(argList);

	ShowToolTip(strTmp);
}

void CIBADialog::SetEditFont()
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

INT_PTR CIBADialog::MsgBox(LPCTSTR lpszText, UINT nType /*= MB_OK*/, LPCTSTR lpszCaption /*= NULL*/)
{
	CMsgBoxDlg dlg(this);
	dlg.SetCaption(lpszCaption);
	dlg.SetText(lpszText);
	dlg.SetType(nType);

	return dlg.DoModal();
}

INT_PTR CIBADialog::MsgBox(UINT nResId, UINT nType /*= MB_OK*/, LPCTSTR lpszCaption /*= NULL*/)
{
	CIBAString strTmp;
	strTmp.LoadString(nResId);

	return MsgBox(strTmp, nType, lpszCaption);
}

INT_PTR CIBADialog::MsgBoxFormat(UINT nType, UINT nFormatId, ...)
{
	CIBAString strTmp, strFormat;

	strFormat.LoadString(nFormatId);

	va_list argList;
	va_start(argList, nFormatId);
	strTmp.FormatV(strFormat, argList);
	va_end(argList);
	
	return MsgBox(strTmp, nType, NULL);
}

void CIBADialog::OnTimer(UINT nIDEvent)
{
	CBCGPDialog::OnTimer(nIDEvent);

	//try
	//{
	if (TimerIdOfAutoClose == nIDEvent)
	{
		KillTimer(nIDEvent);

		EndDialog(m_nDialogResult);
	}
	//}
	//catch(...)
	//{
	//	IBA_LOG0(_T("自动关闭对话框出错2"));
	//}
	
}

void CIBADialog::AutoClose(INT nDialogResult /*= IDCANCEL*/)
{
	UINT nTmp =	SetTimer(TimerIdOfAutoClose, 10, NULL); //最快的速度

	m_nDialogResult = nDialogResult;
}