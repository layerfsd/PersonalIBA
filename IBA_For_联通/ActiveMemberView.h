#pragma once

#include "ActiveMember.h"
#include "ActiveMemberList.h"
#include "ActiveMemberGrid.h"

// CActiveMemberView 视图


class CActiveMemberView : public CBCGPGridView
{
	DECLARE_DYNCREATE(CActiveMemberView)
	DECLARE_MESSAGE_MAP()

public:

	CActiveMemberView();       
	virtual ~CActiveMemberView();

	void Refresh(BOOL bRedrawAll);
	
	BOOL GetCurActiveMember(CActiveMember& ActiveMember);

	void PayoutMember(CActiveMember &ActiveMember);

	// 2011/3/7 guoxuxing : 查找用户所在的行
	// return: -1(无效行),>=0有效
	// nKeyType:
	//    0: 上网账号的最后的4、5位
	//    1: 上网账号完全匹配
	int FindUserAt(UINT nKeyType,CString strKeyword);

	// 2011-03-21-gxx: 设置某个用户为选择状态
	void SetSelUser(int nIndex, BOOL bSetFocus=FALSE);

	// 2011-03-21-gxx: 获取用户列表
	CActiveMemberList* GetActiveMemberList();
	CActiveMemberGrid* GetActiveMemberGrid();

	afx_msg void OnRefresh();

	BOOL EnterActiveMember();

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCredit();
	afx_msg void OnUpdateCredit(CCmdUI *pCmdUI);
	afx_msg void OnPayout();
	afx_msg void OnConsumelist2();
	afx_msg void OnUpdateConsumelist2(CCmdUI* pCmdUI);
	
	afx_msg void OnBundtimeonline();
	afx_msg void OnUpdateBundtimeonline(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePayout(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnChangeComputer();
	afx_msg void OnUpdateChangeComputer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRetailonline(CCmdUI *pCmdUI);
	afx_msg void OnRetailonline();
	afx_msg void OnCheckin();
	afx_msg void OnUpdateCheckin(CCmdUI *pCmdUI);
	afx_msg void OnSendShortMessage();
	afx_msg void OnModifyRemark();

	afx_msg void OnCreditModify();
	afx_msg void OnUpdateCreditReverse(CCmdUI *pCmdUI);
	afx_msg void OnRebilling();
	afx_msg void OnUpdateRebilling(CCmdUI *pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnUpdatePrint(CCmdUI *pCmdUI);

public:


	
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


