#pragma once
#include "G2XDesktopWnd.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XDropList : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XDropList)
	DECLARE_GDI_MESSAGE_MAP()

public:
	CG2XDropList();
	virtual ~CG2XDropList();

public:
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);
	virtual void DrawTextBox(Graphics* pGraphics, LPRECT lpRect, LPCTSTR lpText, G2XControlState ctrlState);
	virtual void DrawButton(Graphics* pGraphics, LPRECT lpRect, G2XControlState ctrlState);

protected:
	g_msg void OnCreate();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg LONG OnListBoxClosing(WPARAM,LPARAM);
	g_msg LONG OnListBoxItemChanging(WPARAM,LPARAM);

	g_msg void OnDestroy();

public:
	INT GetCount();
	CString GetLBText() { return m_strLBText; }
	BOOL SetLBText(LPCTSTR lptText);
	void ShowDropListBox(BOOL bShow);
	CG2XListBox* GetListBox();
	void SetDropListWidth(INT nWidth);
	void SetDropListHeight(INT nHeight);
	void AddString(LPCTSTR, INT iImage=-1);
	void InsertString(INT iIndex, LPCTSTR, INT iImage=-1);
	void SetSortStyle(G2XSortStyle style, BOOL bResort=TRUE);
	void SetCurSel(INT iSel);
	INT GetCurSel();
	CString GetString(INT iIndex);
	void SetData(INT iIndex, DWORD dwData);
	DWORD GetData(INT iIndex);
protected:
	CString m_strLBText;
	CG2XDesktopWnd m_listboxWnd;
	INT m_nDropListWidth;
	INT m_nDropListHeight;
private:
	DWORD m_dwCloseTickCount;
};

G2X_END