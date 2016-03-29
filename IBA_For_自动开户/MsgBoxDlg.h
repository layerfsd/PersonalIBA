#pragma once

// CMsgBoxDlg 对话框

class CMsgBoxDlg : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_DYNAMIC(CMsgBoxDlg)
	DECLARE_MESSAGE_MAP()

public:
	CMsgBoxDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgBoxDlg();

// 对话框数据
	enum { IDD = IDD_MSGBOXDLG };

	void SetCaption(CString newVal) { m_strCaption = newVal; }

	void SetText(CString newVal,BOOL bFormat = TRUE);

	void SetType(UINT newVal) { m_nType = newVal; }

	void SetAutoClose(UINT nTime = 3000)
	{
		m_bAutoClose = TRUE;
		m_nTime = nTime;
	}
protected:

	CString m_strCaption;
	CString m_strText;
	UINT	m_nType;

	CImageList m_ImageList;

	CSize m_TextSize;

	BOOL m_bAutoClose;
	UINT m_nTime;

protected:
	
	void FormatText();
	void InitIcon();
	void InitButton();
	void InitWindow();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedYes();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
