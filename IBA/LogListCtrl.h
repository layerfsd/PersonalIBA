#pragma once

//#include ".\Excel\ExportOutputListThread.h"
// COutputListCtrl
class CExportOutputListThread;
class COutputListCtrl : public CBCGPListCtrl
{
	DECLARE_DYNAMIC(COutputListCtrl)
	DECLARE_MESSAGE_MAP()

public:
	COutputListCtrl();
	virtual ~COutputListCtrl();

protected:

	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	//// 导出Excel
	//afx_msg void OnExportExcel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
public:
	void ActiveRemind(); // 激活提醒
public:
	afx_msg void OnTimer(UINT nIDEvent);
};


