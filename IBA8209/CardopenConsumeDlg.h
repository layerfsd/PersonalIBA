#pragma once

#include "IBAFormView.h"

//
//class:CCardopenConsumeView 开户未消费用户信息查询
//
class CCardopenConsumeView : public CIBAFormView
{
	DECLARE_DYNCREATE(CCardopenConsumeView)
	DECLARE_MESSAGE_MAP()

protected:
	CCardopenConsumeView();   // 标准构造函数
	virtual ~CCardopenConsumeView();

// 对话框数据
	enum { IDD = IDV_CARDOPENCONSUME };

public:
	virtual void OnInitialUpdate();

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkListInfo(NMHDR *pNMHDR, LRESULT *pResult);

private:
	// 结果显示
	CString m_strNoConsumeResult;
	// 开始时间
	CBCGPDateTimeCtrl m_StartTime;
	// 结束时间
	CBCGPDateTimeCtrl m_EndTime;
	// 查询结果
	CStatic m_STATICResult;
	// 信息列表
	CBCGPListCtrl m_InfoList;
	// 初始化时间控件
	void InitTimeCtrl(void);
	// 设置时间控件的数据
	void InitDate(void);

	// 初始化列表控件
	void InitList(void);
	// 查询同时为列表赋值
	void FillList(void);
	// 用户开户查询
	void QueryRegister(void);
	// 查询当前上机记录和上机记录
	BOOL QueryActiveConsume(CString strMemberId);
	// 取得账户状态的文字字串
	CString GetCardStateAsString(BOOL bNewCardState = FALSE);
	// 查询记录
	BOOL QueryHistotyConsume(CString strMemberId, CString strCardOpenTime);

	// 从中心取得普通用户级别ID并组合成SQL字串
	CString GetCommonClassIdAsString(void);

};
