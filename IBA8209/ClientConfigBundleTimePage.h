#pragma once

#include "NetBarConfig2.h"

// CClientConfigBundleTimePage 对话框

class CClientConfigBundleTimePage : public CIBAPage
{
	DECLARE_DYNAMIC(CClientConfigBundleTimePage)
	DECLARE_MESSAGE_MAP()
public:
	CClientConfigBundleTimePage();
	virtual ~CClientConfigBundleTimePage();

// 对话框数据
	enum { IDD = IDD_CONFIGCLIENT_BUNDLETIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

private:

	CNetBarConfig2 m_NetBarConfig2;

	BOOL m_bAllowBundTime;

	
public:
	UINT m_nBTLimitTime;
};
