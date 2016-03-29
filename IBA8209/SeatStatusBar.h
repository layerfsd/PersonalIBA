
#pragma once

#include "LogListCtrl.h"
#include "operationinfo.h"
#include <memory>

class CSeatStatusBar : public CBCGPDockingControlBar
{
	DECLARE_MESSAGE_MAP()
public:

	CSeatStatusBar();
	virtual ~CSeatStatusBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	Gdiplus::Bitmap *m_pSeatBitmap;
};

