#pragma once

#include "NetBarConfigPage.h"
#include "ClientCOnfigUnlockPage.h"
#include "ClientConfigBundleTimePage.h"
#include "IBAStringLoader.h"
// CClientConfigPropertySheet

class CClientConfigPropertySheet : public CIBASheet
{
	DECLARE_DYNAMIC(CClientConfigPropertySheet)
	DECLARE_MESSAGE_MAP()

public:
	CClientConfigPropertySheet(CWnd* pParentWnd = NULL);
	virtual ~CClientConfigPropertySheet();

protected:

	virtual BOOL OnInitDialog();

	CClientConfigCommonPage m_CommonConfig;
	CClientCOnfigUnlockPage m_UnlockPage;
	CClientConfigBundleTimePage m_BundleTimePage;
};


