#pragma once

#include "ITipsShow.h"
#include "TipsWnd.h"

class CTipsShow: public ITipsShow 
{
public:
	CTipsShow();
	~CTipsShow(void);

	static BOOL IsFullScreenMode();

public:

	virtual void SetShowParam(Tips_ShowParam tips_ShowParam);
	virtual int ShowTips(LPCTSTR szURL);
	virtual int ShowTips(LPCTSTR szTitle, LPCTSTR szContent);
	virtual void SetExternal(LPDISPATCH lpExternal);
	virtual void HideTips();

private:
	
	CTipsWnd* CreateTipsByWndMode();

	Tips_ShowParam m_TipsParam;

	LPDISPATCH m_lpExternal;

	HWND m_hWndLastTips;
};
