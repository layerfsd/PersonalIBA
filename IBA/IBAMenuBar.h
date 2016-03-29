
#pragma once

class CIBAMenuBar : public CBCGPMenuBar
{
public:
	CIBAMenuBar();
	~CIBAMenuBar();

	BOOL CreateMenuBar(CWnd* pParentWnd);

	virtual BOOL SaveState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual BOOL LoadState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual void OnDefaultMenuLoaded(HMENU hMenu);
};