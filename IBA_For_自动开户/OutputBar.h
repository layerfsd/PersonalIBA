
#pragma once

#include "LogListCtrl.h"
#include "operationinfo.h"
#include ".\Excel\ExportOutputListThread.h"

class CIBAOutputBar : public CBCGPDockingControlBar
{
	DECLARE_MESSAGE_MAP()
public:

	CIBAOutputBar();
	virtual ~CIBAOutputBar();


// Attributes
protected:
	//操作记录列表
	COutputListCtrl m_wndListInfo;
	//激活记录列表
	COutputListCtrl m_wndListActive;
	//消息记录
	COutputListCtrl m_wndMsgInfo;
	//退款记录
	COutputListCtrl m_wndConsumeInfo;

	// 2011/10/17-8206-gxx: 结账记录	
	COutputListCtrl m_listCheckOut;

	CFont m_Font;

// Generated message map functions
protected:
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnOperationLog(WPARAM, LPARAM);
	
	BOOL CreateOperationList();

	BOOL CreateActiveList();

	BOOL CreateMsgList();

	BOOL CreateConsumeList();

	// 2011/10/17-8206-gxx: 
	BOOL CreateCheckOutList();

	CString FormatNetId(CString strNetId);

	//从中心查询消费记录
	void QueryConsume(COperationInfo &OperationInfo, BOOL bNeedReturnRecord = FALSE, UINT nConsumeType=0);
	
	void SetCreditInfo(COperationInfo &OperationInfo);

	void AddActiveLog();

public:
	void AddMsgLog(CClientShortMessage& strMsgInfo);

public :
	//TAB控件的父类
	CBCGPTabWnd	m_wndTabs;

	void AddOperationLog(COperationInfo& OperationInfo);

	BOOL AddCheckOutLog(COperationInfo& OperationInfo);
	
	void UpdateMsgLog();

	// 2011/06/16-gxx: 
	void RefreshAllInfoList();

	// 2011/06/16-gxx: 
	void RefreshInfoList(CString strFileSuffix, COutputListCtrl& listCtrl);

	// 2011/03/10/gxx: 上班调用此函数，显示收银员保存的操作记录
	void RefreshOperationList();

	// 2011/06/16-gxx: 班次的刷卡信息，显示收银员保存的刷卡记录
	void RefreshCardList();

	void SaveToFile(CString strFileSuffix, COutputListCtrl& listCtrl);

	// 2011/06/14-gxx: 下班后，调用此函数，清除班次相关的文件
	void DeleteDutyFile();

	// 2011/06/16-gxx: 
	void DeleteListFile(CString strFileSuffix);

	// 2013-3-8 qsc
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	void OnExportExcel();

private:
	CExportOutputListThread m_ExpThread;
};

