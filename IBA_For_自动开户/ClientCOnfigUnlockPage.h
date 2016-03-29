#pragma once

#include "NetBarConfig2.h"

// CClientCOnfigUnlockPage 对话框

class CClientCOnfigUnlockPage : public CIBAPage
{
	DECLARE_DYNAMIC(CClientCOnfigUnlockPage)
	DECLARE_MESSAGE_MAP()

public:
	CClientCOnfigUnlockPage();
	virtual ~CClientCOnfigUnlockPage();

// 对话框数据
	enum { IDD = IDD_CLIENTCONFIG_UNLOCK };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:

	CString m_strPwd;
	// 软件维护时间
	UINT m_nMaxKeepTime;
	// 硬件维护时间
	UINT m_nMaxKeepTimeOfUKey;

	CNetBarConfig2 m_NetBarConfig2;
	
};
