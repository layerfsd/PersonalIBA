#pragma once

#include "resource.h"
//
//class:CSetPageOfMonitor 收银台设置->监控颜色设置对话框
//
class CSetPageOfMonitor : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfMonitor)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfMonitor();
	virtual ~CSetPageOfMonitor();

// 对话框数据
	enum { IDD = IDD_SET_MONITORCOLOR };

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

private:
	
	CBCGPColorButton m_CPOnLine;
	CBCGPColorButton m_CPOffLine;
	CBCGPColorButton m_CPThief;
	CBCGPColorButton m_CPLittleMoney;
	CBCGPColorButton m_CPLockScreen;
	CBCGPColorButton m_CPUnLock;
	CBCGPColorButton m_CPBlack;
	CBCGPColorButton m_CPVIP;
	CBCGPColorButton m_CPSuspend;
	CBCGPColorButton m_CPBirthDay;

	// 包时监控颜色
	CBCGPColorButton m_CPBundTimeColor;
};
