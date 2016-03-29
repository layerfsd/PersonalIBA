#pragma once
#include "IBACaptureImage.h"


// CDlgCapture 对话框

class CDlgCapture : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgCapture)

public:
	CDlgCapture(LPCTSTR lpszImageName, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCapture();

// 对话框数据
	enum { IDD = IDD_DIALOG_CAPTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CIBACaptureImage m_capImage;
	CString m_strImageName;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
