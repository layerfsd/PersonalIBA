#pragma once

#include "NetBarConfigPage.h"
#include "NetBarInfoPage.h"
#include "NetBarOtherInfoPage.h"
#include "RealNameConfigPage.h"

// CNetBarPropertySheet

class CNetBarPropertySheet : public CBCGPPropertySheet
{
	DECLARE_DYNAMIC(CNetBarPropertySheet)
	DECLARE_MESSAGE_MAP()

public:

	CNetBarPropertySheet(CWnd* pParentWnd = NULL);
	virtual ~CNetBarPropertySheet();
	
private:
	
	CNetBarInfoPage m_NetBarInfo;
	CNetBarConfigPage m_NetBarConfig;
	CNetBarOtherInfoPage m_NetBarOtherInfo;

public:

	virtual BOOL OnInitDialog();
	LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
};


