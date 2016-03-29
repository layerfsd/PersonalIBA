#pragma once


// CCrashShowDlg 对话框

class CCrashShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrashShowDlg)

public:
	CCrashShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCrashShowDlg();

// 对话框数据
	enum { IDD = IDD_CRASH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};
