#pragma once
#include "WndFinder.h"
#include "capturemanager.h"

#define WM_REALNAME (WM_USER + 100)

BOOL SendCardInfo(HWND hIBAWnd, LPCTSTR lpszNetid, LPCTSTR lpszIDNumber, LPCTSTR lpszName);

class CCaptureBase
{
public:
	CCaptureBase(void);
	~CCaptureBase(void);

	virtual BOOL Create(BOOL bDialog, LPCTSTR lpszCaption, LPCTSTR lpszClassName);

	virtual BOOL CreateFrom(const CaptureItem& item);
	virtual BOOL CreateFromEx(const CaptureItemEx& item);

	void AddNetidID(UINT nID);
	void AddIdNumberID(UINT nID);
	void AddNameID(UINT nID);

	BOOL CaptureAndSend(HWND hIBAWnd);
	BOOL CaptureAndSendEx(HWND HIBAWnd);

	CWndFinder m_wndFinder;

public:
	UINT m_nNetidID;
	UINT m_nIdNumberID;
	UINT m_nNameID;

	BOOL m_bEx;
};
