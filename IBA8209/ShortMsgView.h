#pragma once

#include "IBAFormView.h"

// CShortMsgView 窗体视图

class CShortMsgView : public CIBAFormView
{
	DECLARE_DYNCREATE(CShortMsgView)
	DECLARE_MESSAGE_MAP()

protected:
	CShortMsgView();           // 动态创建所使用的受保护的构造函数
	virtual ~CShortMsgView();

public:
	enum { IDD = IDV_SHORTMSG };
	virtual void OnInitialUpdate();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedButtonQuery();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnNMDblclkListMsg(NMHDR *pNMHDR, LRESULT *pResult);

private:
	// 消息列表
	CBCGPListCtrl m_MsgList;
	// 开始时间
	CBCGPDateTimeCtrl m_btnStartTime;
	// 结束时间按钮控件
	CBCGPDateTimeCtrl m_btnEndTime;
	
	CString m_strResult;

	void InitListView();
	// 初始化时间控件
	BOOL InitTimeCtrl(void);
	// 初始化时间控件的数据
	BOOL InitTimeDate(void);
	// 添加消息列表
	INT FillMsgList(void);

public:

	// 查询消息
	void QueryShortMsgList(void);

};


