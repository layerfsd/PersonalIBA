
#pragma once

#include "IBADoc.h"

class CActiveMemberView;
class CComputerListView;

class CIBAView : public CBCGPTabView
{
	DECLARE_DYNCREATE(CIBAView)
	DECLARE_MESSAGE_MAP()

protected: // create from serialization only
	CIBAView();

// Attributes
public:
	CIBADoc* GetDocument();
	virtual ~CIBAView();

// Generated message map functions
protected:

	virtual void OnInitialUpdate();
	virtual void OnActivateView (CView* pView);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnChildClose(WPARAM wp, LPARAM lp);

private:

	static  const COMPUTERLIST_VIEW = 0;   // 机器视图的tab索引
	static  const ACTIVEMEMBER_VIEW = 1;   // 用户视图的tab索引

	HWND m_hTermConsumeView;
	HWND m_hComsumeListView;
	HWND m_hOperationView;
	HWND m_hShiftCashView;
	HWND m_hSeatView;
	HWND m_hShortMsgView;
	HWND m_hCardOpenConsumeView;
public:
	HWND m_hDodonewView;

public:

	void ShowOperationView();
	void ShowShiftCashView();
	void ShowSeatLayoutView();
	// 开户未消费查询显示
	void ShowCardOpenConsumeView(void);

	void ShowTermConsumeView();
	void QueryTermConsumeList(LPCTSTR lpszTermId);

	void ShowComsumeListView();
	void QueryUserComsumeList(LPCTSTR lpszNetId, UINT nMemberId = 0);

	BOOL FindComputer(UINT nKeyType, LPCTSTR lpszKeyValue);
	// 显示客户端消息
	void ShowShortMsgView(void);
	void QueryShortMsgList();

	// 2011/05/06-gxx: 添加嘟嘟牛增收宝视图
	void ShowDodonewView();

	// 2011/05/23-gxx: 获取用户视图
	CActiveMemberView* GetActiveMemberView();

	// 2011/05/23-gxx: 获取机器视图
	CComputerListView* GetComputerListView();

	// 2011/05/23-gxx: 判断当前视图是否为机器视图
	BOOL IsCurrentComputerView();

	//{ 2011/05/23-gxx: 几个定位方法
	BOOL GotoActiveMember(CString strNetID,BOOL bActiveView=TRUE);  // 根据上网账号定位到用户(用户视图)
	// 2011/07/05-gxx:  根据终端号定位到用户, 返回最佳用户对应的终端ID
	BOOL GotoActiveMemberEx(CString strTermID, CString* pBestTernID=NULL);

	void GotoComputer(CString strTermID,BOOL bActiveView=TRUE); // 定位到机器
	void GotoComputerByIP(CString strIP,BOOL bActiveView=TRUE); // IP 定位到机器
	void GotoComputerByNetID(CString strNetId,BOOL bActiveView=TRUE); // 上网账号 定位到机器

	// 2014-10-27 - qsc
	LRESULT OnUpdateSeat(WPARAM wParam, LPARAM lParam);
	//}
	
	
};
