
#pragma once

#include "OutputBar.h"
#include "TaskPane.h"
#include "IBAMenuBar.h"
#include "ToolBarLabel.h"
#include "DlgNotice.h"
#include "SeatStatusBar.h"
#include "BCGPLabalButton.h"
#include "BCGPStaticTips.h"

//const WORD STATUSTOOLTIPWIDTH = 30; //2015-0825 liyajun 状态栏提示框单行宽度 

class CJuBao;
class CProxyThead;
class CMainFrame : public CBCGPFrameWnd
{
public: // create from serialization only
	CMainFrame();
	virtual ~CMainFrame();

	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()

// Attributes

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnShowPopupMenu(CBCGPPopupMenu* pMenuPopup);
	BOOL OnMenuButtonToolHitTest(CBCGPToolbarButton* pButton, TOOLINFO* pTI);

public:  // control bar embedded members

	CIBAMenuBar			m_wndMenuBar;//菜单栏
	CBCGPStatusBar		m_wndStatusBar;//状态栏
	CBCGPToolBar		m_wndToolBar;//工具栏
	CIBAFastLane		m_wndFastLane;//便捷通道
	CIBAOutputBar		m_wndOutput;//日志输出
	CSeatStatusBar		m_wndSeatStatusBar;
	CIBATaskPane		m_wndTaskPane;//左边的用户信息显示（任务模板）
	CBCGPToolBarImages  m_UserImages;

	CString m_strLastIdInToolbar;

	static HWND m_hOperationLog;

	CDlgNotice m_dlgNotice;

	BOOL m_bDiankaSellCash; // 是否为现金点卡销售

	CJuBao* GetJubaoInf()	{ return m_pJuBao; }
// Generated message map functions
protected:
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnStartIBAEnd();		// 2014-4-9 - qsc update
	afx_msg void OnDestroy();
	afx_msg void OnViewCustomize();
	afx_msg void OnViewOutlookBar();
	afx_msg void OnViewCaptionBar();
	afx_msg void OnViewFastLane();
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnTimeChange();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOutlookBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewFastLane(CCmdUI* pCmdUI);

	afx_msg LRESULT OnToolbarReset(WPARAM, LPARAM);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM, LPARAM);
	afx_msg LRESULT OnReplyMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnChildClose(WPARAM wp, LPARAM);
	afx_msg LRESULT OnRealName(WPARAM, LPARAM);
	afx_msg LRESULT OnCardIn(WPARAM, LPARAM);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnExecuteRealName(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnToolbarSetLabelText(WPARAM wP,LPARAM lP);
	afx_msg LRESULT OnUpdateIBAStatus(WPARAM wP,LPARAM lP);
	afx_msg LRESULT OnCanClose(WPARAM wP,LPARAM lP);	

	// 2011/11/15-8210-gxx: 
	afx_msg LRESULT OnClientDianka(WPARAM, LPARAM);

	afx_msg LRESULT OnHotKey(WPARAM, LPARAM);

	afx_msg void OnReadCard();

private :
	void DestroyJXCDlg();
	BOOL CreateTaskPane();
	BOOL CreateOutPut();
	BOOL CreateCommonBar();
	void ChangeSkin(UINT nSkinId);

private:

	UINT m_nAppLook;
	CJuBao *m_pJuBao;

public:
	afx_msg void OnCreditReverse();

protected:

	afx_msg void OnCalc();
	afx_msg void OnNotepad();
	afx_msg void OnCredit();
	afx_msg void OnNewUser();
	afx_msg void OnAppAbout();
	afx_msg void OnDutyOff();
	
	afx_msg void OnClose();
	afx_msg void OnIbaSet();
	afx_msg void OnNetbarSet();
	afx_msg void OnModifyPwd();
	afx_msg void OnModifyUserinfo();
	afx_msg void OnBoundTime();
	afx_msg void OnChangeSN();
	afx_msg void OnConsumeList();
	afx_msg void OnTermConsume();
	afx_msg void OnUpgrade();
	afx_msg void OnCardCredit();
	afx_msg void OnShiftCash();
	afx_msg void OnOperation();
	afx_msg void OnSell();

	afx_msg void OnRnOpencard();
	afx_msg void OnRnLose();
	afx_msg void OnRnModifypwd();
	afx_msg void OnRnAddmember();
	afx_msg void OnLogPath();
	afx_msg void OnAllLog();
	afx_msg void OnRealnameSet();
	afx_msg void OnClientSet();

	afx_msg void OnPointExchange();
	afx_msg void OnAgent();
	afx_msg void OnSeatoperation();
	afx_msg void OnServerweb();
	afx_msg void OnShortmsgquery();
	afx_msg void OnCardderegister();
	afx_msg void OnCardopenconsume();
	afx_msg void OnScan(UINT nId);
	
	afx_msg void OnMergemember();
	afx_msg void OnUpdateMergemember(CCmdUI *pCmdUI);
	afx_msg void OnMuticheckout();
	afx_msg void OnUpdateMuticheckout(CCmdUI *pCmdUI);
	afx_msg void OnPrintTemplate();
	afx_msg void OnSpeechTemplate();
	afx_msg void OnBoundtime2();

	afx_msg void OnUpdateAllLog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateScan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSeatoperation(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePointexchange(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewUser(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModifyPwd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModifyUserinfo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBoundTime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChangeSN(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConsumeList(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTermConsume(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUpgrade(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCardCredit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShiftCasht(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOperation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCreditReverse(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRealNameSet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNetbarSet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIbaSet(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCredit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSell(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRealName(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRealNameExtendMenu(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCardDeregister(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBoundtime2(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOffDuty(CCmdUI *pCmdUI); // 2011/07/13-8201-gxx: 
	afx_msg void OnCreditDirect();
	afx_msg void OnZengshoubao();
	afx_msg void OnUpdateDianka(CCmdUI *pCmdUI);
	afx_msg void OnDianKa();
	afx_msg void OnLabelClick(); // 2011/05/26-gxx: 点击上网账号，对应到用户视图

	afx_msg void OnUpdateCardopenconsume(CCmdUI *pCmdUI);

	afx_msg void OnRoom(); // 包房
	afx_msg void OnUpdateRoom(CCmdUI *pCmdUI);
	afx_msg void OnLockScreen();

	//afx_msg void OnNcPaint();
	//virtual LRESULT DefWindowProc(UINT Message,WPARAM wParam,LPARAM lParam);
public:
	
	void StartIBA();
	void UpdateStatusText(CString strStatInfo = _T(""));

	// 2011/09/08-8201-gxx: 显示公告
	void ShowNotice(LPCTSTR lpTitle,LPCTSTR lpInfo, LPCTSTR lpLink, BOOL bShouldSign);

	//2010-3-5-gxx, 添加刷新工具栏右边的ID文本
	void UpdateToolbarLastID(const CString&, BOOL bError=FALSE);
	void OnUpdateCreditDirect(CCmdUI* pCmdUI);
	
	void SelAndShowDlgByNetId(CString strTmp);
	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

/////////////////////////////////////////////////////////////////////////////
