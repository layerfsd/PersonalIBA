/************************************************************************/
/* @ desc: 微信充值提示                                                 */
/* @ date: 2015/5/5														*/
/* @ author: tmelody			                                        */
/* @ note:						                                        */
/*		如果想设置成最顶层窗口，那么收银台窗口将处于非激活状			*/
/*						                                                */
/************************************************************************/

#pragma once

#include "HoverButton.h"

//////////////////////////////////////////////////////////////////////////
// 仅仅实现一个功能，鼠标进入变手型

class CSpecStatic : public CStatic
{
	DECLARE_DYNAMIC(CSpecStatic)
	DECLARE_MESSAGE_MAP()

public:
	CSpecStatic();

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

protected:
	HCURSOR m_hCursor;
};


//////////////////////////////////////////////////////////////////////////
// 

class CWxMsgTips : public CWnd
{
	DECLARE_DYNAMIC(CWxMsgTips)
	DECLARE_MESSAGE_MAP()

public:
	CWxMsgTips();
	virtual ~CWxMsgTips();

public:
	BOOL Create();

	void SetUserID(LPCTSTR lpszID);				// 账号
	void SetPayAmount(LPCTSTR lpszAmount);		// 支付金额
	void SetGiveAmount(LPCTSTR lpszAmount);		// 赠送
	void SetTermID(LPCTSTR lpszTermID);			// 机器号
	void SetTime(LPCTSTR lpszTime);				// 时间

	void ShowRBCorner();

protected:
	void InitResource();
	void AdjustPosition();
	BOOL IsMouseIn();
	void ConstructorInfoText();

protected:
	// 
	virtual BOOL Create(LPCTSTR lpszClassName, 
		LPCTSTR lpszWindowName, 
		DWORD dwStyle, 
		const RECT& rect, 
		CWnd* pParentWnd, 
		UINT nID, 
		CCreateContext* pContext /* = NULL */);
	virtual void PostNcDestroy();

protected:
	// message
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnCloseClicked();
	afx_msg void OnBtnDetailClicked();

protected:
	static const UINT TimerIDOfPerSec			= 3000;
	static const UINT TimerIDOfPerSecInterval	= 1000;

	static const UINT TextItemCount = 5;
	static const UINT NormalStaticCount = 4;
	static const UINT TextItemSpace = 3;

private:
	BOOL			m_bCreated;		// 是否已经创建
	CHoverButton	m_BtnClose;		// 关闭按钮
	CStatic			m_StcTitle;		// 标题
	CFont			m_FntText;		// 字体
	CImageEx		m_ImgBack;		// 背景图

	RECT			m_ScreenRect;	// 桌面大小
	CRect			m_WndRect;		// 窗口大小，以图片大小为准

	int				m_nShowTimes;	// 显示总时间
	int				m_nCurTimes;	// 当前时间

	BOOL			m_bMouseTrack;
	BOOL			m_bMouseIn;

	CStatic			m_aTexts[NormalStaticCount];	// 文字控件
	CFont			m_FntDetailText;				// 显示详情的字体
	CSpecStatic		m_StcDetail;					// 点击详情

	CString			m_strUserID;			// 账号
	CString			m_strPayAmount;			// 支付金额
	CString			m_strGiveAmount;		// 赠送
	CString			m_strTermID;			// 机器号
	CString			m_strTime;				// 时间
};