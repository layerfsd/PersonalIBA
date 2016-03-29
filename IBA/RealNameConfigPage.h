#pragma once

#include "IBADialog.h"
#include "NetBarConfig2.h"

class CConfigOfRealName : public CIBADialog
{
	DECLARE_DYNAMIC(CConfigOfRealName)
	DECLARE_MESSAGE_MAP()

public:
	CConfigOfRealName(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigOfRealName();

// 对话框数据
	enum { IDD = IDD_REALNAMECONFIG };

protected:
	
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	
private:

	CNetBarConfig2 m_NetBarConfig2;

	CString m_ProxyIP;
	CString m_WWMIP;

	CString m_strWWMIPTIP;

	BOOL m_bNetIdentityManage;

};
