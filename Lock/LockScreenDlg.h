#pragma once
#include "ImageEx.h"
#include "HoverButton.h"
#include "XPEdit.h"


// CLockScreenDlg 对话框

class CLockScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CLockScreenDlg)

public:
	CLockScreenDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLockScreenDlg();

// 对话框数据
	enum { IDD = IDD_LOCKSCREENDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CXPEdit m_PasswordEdit;
	CHoverButton m_ULockBtn;

	CImageEx  m_imgScreen;

	int m_initWidth_ ;
	int m_initHeight_;

	static const UINT TimeIDOfLockScreen = 100;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedUlock();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
