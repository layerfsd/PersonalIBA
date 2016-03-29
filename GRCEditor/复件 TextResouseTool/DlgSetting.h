#pragma once


// CDlgSetting 对话框

class CDlgSetting : public CStringLoaderDlg<CDialog>
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetting();

// 对话框数据
	enum { IDD = IDD_DLG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bReplace;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	BOOL m_bMenuBefore;
	afx_msg void OnBnClickedCheckChange();
	BOOL m_bCheckChange;
	CString m_strFmtChars;
	BOOL m_bForbidEditID;
};
