#pragma once
#include "IBADialog.h"


// CClearDataSetPage 对话框

class CClearDataSetPage : public CIBAPage 
{
	DECLARE_DYNAMIC(CClearDataSetPage)

public:
	CClearDataSetPage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClearDataSetPage();

// 对话框数据
	enum { IDD = IDD_SET_CLEARDATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	// 开始时间
	CBCGPDateTimeCtrl m_StartTime;
	// 结束时间
	CBCGPDateTimeCtrl m_EndTime;

	CFont m_Font;
	
	CPPToolTip m_ToolTip;
	
	CRect m_ToolTipRect;
	afx_msg void OnBnClickedClearShortMsg();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	void InitTimeCtrl(void);
	void SetToolTipPos(UINT uID);
};
