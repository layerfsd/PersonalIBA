#pragma once


// CDlgFindWnd 对话框

class CDlgFindWnd : public CDialog
{
	DECLARE_DYNAMIC(CDlgFindWnd)

public:
	CDlgFindWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgFindWnd();

// 对话框数据
	enum { IDD = IDD_DLG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	void SearchNext();

	BOOL m_bCase;   // 匹配大小写
	BOOL m_bAll;    // 完全匹配
	CString m_strKeyword;
	CString m_strLastKeyword;


	int m_nLastResult;
};
