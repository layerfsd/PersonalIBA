#pragma once
#include "afxwin.h"
#include ".\common\StaticRollText.h"
#include "bcgptoolbarctrlcommon.h"
#include "..\L2ToolKit\GdipButton.h"

//
//class:CDlgIBAFastlane 快速通道栏对话框
//
#define WM_UPDATEROLLTEXT WM_USER+100

class CDlgIBAFastlane : public CStringLoaderDlg<CBCGPDialog>
{
	DECLARE_DYNAMIC(CDlgIBAFastlane)
	DECLARE_MESSAGE_MAP()

public:
	CDlgIBAFastlane(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgIBAFastlane();

// 对话框数据
	enum { IDD = IDD_DLG_IBA_FAST };

	static const UINT EVENT_TIME_BUGLE = 1;   // 控制喇叭交替的定时器事件
	static const UINT TIME_BUGLE = 1*1000;    // 1S中，喇叭闪一次
	static const int COUNT_TURN = 1*60;      // 交替次数(1分钟)
       

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK(){ return ; }
	virtual void OnCancel(){ return; }
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboFast();
	afx_msg void OnCbnSelendokComboFast();
	afx_msg void OnCbnSelendcancelComboFast();
	afx_msg LRESULT OnUpdateRollText(WPARAM wParam, LPARAM lParam);

	// 2011/08/24-8201-gxx: 0:正常模式,1:应急模式,2:恢复模式,3:正在尝试网络连接
	afx_msg LRESULT OnUpdateIBAStatus(WPARAM wParam, LPARAM lParam);

public:
	void UpdateBugle(); // 刷新喇叭的显示
	void StartBugleTurn(BOOL bStartActive=TRUE); // 开始喇叭交替
	void StopBugleTurn(BOOL bLastActive=FALSE);  // 停止喇叭交替


private:
	CVCenterStatic m_static1;
	CVCenterStatic m_static2;

	BOOL m_bBugleActive; // 喇叭是否为活动状态,(喇叭默认为消极状态)
	int m_nTurnCount; 
public:

	CBCGPComboBox m_combFast;
	//CComboBoxEx m_combFast;
	NS_COMMON::CStaticRollText m_stcContent;
	CStatic m_stcPicStatus;
	CImageEx m_imgStatus;
	INT m_nIBAStatus;

	CPPToolTip m_ToolTip;


	
	afx_msg void OnTimer(UINT nIDEvent);
	
	afx_msg void OnStnClickedPicStatus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
