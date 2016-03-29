#pragma once


// CDlgHit 对话框

class CDlgHit : public CDialog
{
	DECLARE_DYNAMIC(CDlgHit)

public:
	CDlgHit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgHit();

// 对话框数据
	enum { IDD = IDD_DLG_HIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL m_bNextNotHit;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
