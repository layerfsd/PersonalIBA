

#pragma once

#include "PPMessageBox.h"


/////////////////////////////////////////////////////////////////////////////
// CPPMessageBox dialog

class L2_EXT_CLASS CPPMessageBox 
{
// Construction
public:
	CPPMessageBox();   // standard constructor
	~CPPMessageBox();

	int MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nStyle = MB_OK | MB_ICONEXCLAMATION, const PPMSGBOXPARAMS * pMsgBox = NULL);
	int MessageBoxIndirect(const PPMSGBOXPARAMS * pMsgBox = NULL);

	PPMSGBOXPARAMS * GetMessageBoxParams();
	void SetMessageBoxParams(const PPMSGBOXPARAMS * pMsgBoxParams);

	void SetTimeouts(int nAutoclick, int nDisable = 0, BOOL bGlobalDisable = FALSE);
	void SetCustomButtons(LPCTSTR lpszButtonNames = NULL);

	//Backgrounds
	void SetBackground(DWORD dwArea, PPMSGBOXAREA_BK * pAreaBk);
	void SetBackground(DWORD dwArea, int nEffectBk = -1, COLORREF crStartBk = -1, COLORREF crEndBk = -1, COLORREF crMidBk = -1);
	void SetSeparator(DWORD dwArea, int nSepType = PPMSGBOX_SEP_NONE, int nSepAlign = PPMSGBOX_ALIGN_LEFT, LPCTSTR lpszSepText = NULL);

	void SetNotify(DWORD dwReportMsgID) {m_MsgParam.dwReportMsgID = dwReportMsgID;};

	//Localization
	void ClearAllButtonsText();
	void SetButtonText(DWORD dwBtnID, LPCTSTR lpszText = NULL);
	LPCTSTR GetButtonText(DWORD dwBtnID);

protected:
	typedef std::map<DWORD, CString> mapBtnText;
	mapBtnText m_mapButtonText;
	mapLocalBtnText m_pLocalBtnText;

	PPMSGBOXPARAMS m_MsgParam;

	CString m_sHeaderSepText;
	CString m_sControlSepText;
	CString m_sMoreInfoSepText;
	CString m_sCustomButtonText;

	CString GetString(LPCTSTR lpszText);
	void PackText();
};

