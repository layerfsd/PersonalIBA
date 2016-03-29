#pragma once

#include ".\IBADialog.h"
#include "CurrentNetBarUser.h"

class CNDDialog : public CIBADialog
{
	DECLARE_MESSAGE_MAP()

protected:
	
	static const UINT TimeIdOfAutoQuery = 1000;
	static const UINT TimerIdOfRePosDialogs = 2;
	void RePosDialogs();

	static HWND hWndPre;
	void InitWindowPos();

protected:

	CNDDialog(UINT nIDTemplate, CWnd *pParent= NULL);
	~CNDDialog();

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScan1();
	afx_msg void OnScan2();
	afx_msg void OnRead2();

	void OnBnClickedButtonQuery();
	void OnBnClickedButtonScanreco();

	// 2011/07/08-8201-gxx: 获取本地用户状态, emUserStateNULL, emUserStateCenter,emUserStateLocal
	int GetUserState();

	// 2011/07/08-8201-gxx: 处理本地模式
	virtual BOOL DoLocalModeQuery();
	virtual BOOL DoLocalModeOperate();


protected:

	struct TFocusEnter
	{
		UINT nFocusID; //焦点所在控件ID
		UINT nEnterID; //要模拟单击事件的控件ID
	};

	CArray<TFocusEnter, TFocusEnter> m_FocusEnterMap;
	
	void AddFocusEnter(UINT nFocusID, UINT nEnterID);

	BOOL IsMeGotFocus();

	void SimulateClickMe();

protected:

	CImageList m_Images;

	BOOL m_bFromScan;
	
	UINT m_nMemberId;
	UINT m_nCardId;

	BOOL m_bIsMember; //!<会员
	BOOL m_bOnLine;   //!<是否在线

	CString m_strNetId;
	CString m_strName;
	CString m_strPersonalID;

	UINT    m_nUserClassID;
	CString m_strUserClass;

	CString m_strAllBalance;
	CString m_strMemberName;

	CString m_strTermId;
	CString m_strOpenNetBar;

	int m_nMoneyOnlineAmount;  //上网预留
	int m_nMoneyBalanceW; //无送金额
	int m_nMoneyBalanceY; //有送金额
	int m_nMoneyBalanceZ; //赠送

	UINT m_nCurrentPoint; //当前积分

	BOOL m_bAutoQuery;

	UINT m_nDlgTemplateID;

	//
	CActiveMember m_LocalActiveMember;

	// 2011/04/29-gxx: 
	BOOL m_bCenterDisconnect;

	CActiveMember m_ActiveMember;
	BOOL m_bNeedActivation;

protected:

	void CheckCard(LPCTSTR strNetId);
	virtual void OnOldCard(UINT nCardId, UINT nMemberId);
	virtual void OnNewCard(UINT nCardId);

	BOOL QueryBalance(UINT nMemberId, UINT nCardId);
	BOOL ReQuery();

	virtual void OnGetBalanceOK(CBalanceMethod & Balance);
	virtual void OnGetBalanceFailed(CBalanceMethod & Balance);

	virtual void ClearData();

	//2011-4-14-gxx-添加中心断开的处理
	virtual void OnCenterDisconnect();
	
	void ScanReco(UINT nType);

public:
	void SetNetId(CString newVal=_T(""), BOOL bAutoQuery = FALSE);  //设置网络ID开启自动查询，为空表示当前的网吧用户的ID
};