#pragma once

#include "IBADialog.h"
// CDlgRNZdCrc 对话框

class CDlgRNZdCrc : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgRNZdCrc)

public:
	CDlgRNZdCrc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRNZdCrc();

	DWORD GetCRC() { return m_dwCRC; }
// 对话框数据
	enum { IDD = IDD_DLG_REALNAME_ZDcrc };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();

public:
	DWORD m_dwCRC;
};
