
#pragma once

#include "ColorGridCtrl.h"
#include "DdnDataX.h"

class CComputerListView : public CBCGPGridView
{
	DECLARE_DYNCREATE(CComputerListView)
	DECLARE_MESSAGE_MAP()

public: // create from serialization only

	CComputerListView();
	~CComputerListView();

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSnapScreen();
	afx_msg void OnGetLog();
	afx_msg void OnPowerOff();
	afx_msg void OnPowerOn();
	afx_msg void OnSendShortMessage();
	afx_msg void OnCheckClient(CCmdUI *pCmdUI);
	afx_msg void OnCheckNotClient(CCmdUI *pCmdUI);
	afx_msg void OnCheckUser(CCmdUI *pCmdUI);
	afx_msg void OnCreditOnline();
	afx_msg void OnCheckPayOut(CCmdUI *pCmdUI);
	afx_msg void OnCheckCredit(CCmdUI *pCmdUI);
	afx_msg void OnCheckUnusal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateColumnSet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConsumelist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTermConsume(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSendShortMessage(CCmdUI *pCmdUI);
	afx_msg void OnShowColumn(UINT id);
	afx_msg void OnColumnSet();
	afx_msg void OnPayout();
	afx_msg void OnChangeComputer();
	afx_msg void OnReboot();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRefresh();
	afx_msg void OnConsumelist();
	afx_msg void OnTermConsume();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBundtimeonline();
	afx_msg void OnUpdateBundtimeonline(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRefresh(CCmdUI *pCmdUI);
	afx_msg void OnRetailonline();
	afx_msg void OnUpdateRetailonline(CCmdUI *pCmdUI);
	afx_msg void OnPing();
	afx_msg void OnUnlockScreen();
	afx_msg void OnUpdateUnlockScreen(CCmdUI *pCmdUI);
	
	LRESULT OnCompRefresh(WPARAM wParam, LPARAM lParam);

	BOOL GetComputerInfoFromRow(CComputerInfo & ComputerInfo, INT RowIndex);

	INT_PTR MsgBox(LPCTSTR lpszText, UINT nType = MB_OK | MB_ICONINFORMATION, LPCTSTR lpszCaption = NULL);

private :

	CComputerGrid* m_lpComputerGird;
	
	CUIntArray m_SelectRows;

	CString m_strDefaultTermID;
	// 接受点卡动态库发送的消息（点卡销售成功的返回信息）
	CDataXMReceiverT<CComputerListView>* m_lpDataXMReceiver; 

public:
	INT OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

private :

	static const UINT TimeIdOfPing = 1000;
	static const UINT TimeIdOfSetFocus = 1001;
	static const UINT IntervalOfPing = 1000 * 10;

public:

	void AdjustLayout();
	
	void Refresh(BOOL bRedrawAll = FALSE);

	void OnClientShortMessage(SHORT_MESSAGE_INFO &shortMsgInfo);
	
	/**
	* 生成统计文本
	*/
	CString MakeStatText();

	BOOL FindComputer(UINT nKeyType, LPCTSTR lpszKeyValue);
	
	void SetCurSel(int nIndex);

	void SetNewUserDefaultTerm(LPCTSTR szTermID);

	//取得选中行的终端信息
	BOOL GetCurComputerInfo(CComputerInfo & ComputerInfo);
};

