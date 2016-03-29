#pragma once
#include "bcgptoolbarctrlcommon.h"

class CIdentityToolbarButton :
	public CBCGPToolbarCtrlCommon
{
	DECLARE_SERIAL(CIdentityToolbarButton)
public:
	CIdentityToolbarButton(void);
	CIdentityToolbarButton(UINT nID,CBCGPToolBar *pToolbar);

	virtual ~CIdentityToolbarButton(void);

	virtual void CopyFrom(const CBCGPToolbarButton& src);

protected:
	virtual BOOL CreateCtrlWnd();

	CFont m_hFont;
public:
	static const int  DEFAULT_WIDTH = 280;
};
