#pragma once

// COutputListCtrl

class COutputListCtrl : public CBCGPListCtrl
{
	DECLARE_DYNAMIC(COutputListCtrl)
	DECLARE_MESSAGE_MAP()

public:
	COutputListCtrl();
	virtual ~COutputListCtrl();

protected:

	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	void ActiveRemind(); // º§ªÓÃ·–—
public:
	afx_msg void OnTimer(UINT nIDEvent);
};


