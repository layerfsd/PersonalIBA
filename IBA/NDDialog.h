#pragma once

#include ".\IBADialog.h"
#include "CurrentNetBarUser.h"
#include "ActiveMember.h"

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

	CImageList m_Images;             //netbaruser.bmp中的图样
	BOOL m_bFromScan;				 //信息是否来自扫描
	UINT m_nMemberId;
	UINT m_nCardId;
	//BOOL m_bNDHasLocalQuery;	     //是否进行过本地查询
	BOOL m_bAutoQuery;               //是否在生成窗口前要进行自动查询
	UINT m_nDlgTemplateID;			 //对话框资源ID
	/*BOOL m_bCenterDisconnect;   */     //是否已与中心断开.CheckCard动作返回-1时,或直接调用DoLocalModeQuery或OnCenterDisconnect时设置为TRUE

	CActiveMember m_ActiveMember;      //用户 - 正常模式下使用
	CActiveMember m_LocalActiveMember; //用户 - 本地模式下使用

	BOOL m_bIsMember;                  //会员
	BOOL m_bOnLine;                    //是否在线.=传给BalanceMethod的OnlineInFact?需要上报时间在10分钟内,才算在线
	CString m_strNetId;                //NetId
	CString m_strMemberName;           //用户名
	CString m_strName;                 //用户名(性别)
	CString m_strPersonalID;           //用户证件ID
	UINT    m_nUserClassID;            //用户级别ID
	CString m_strUserClass;            //用户级别
	CString m_strTermId;               //终端号
	CString m_strOpenNetBar;           //开户网吧	
	
	int m_nMoneyAllInAccount;          //账户的总金额(包括金额,押金,预扣)
	CString m_strAllBalance;           //账户的总金额的字符串形式
	int m_nMoneyBalanceW;              //无送充值余额
	int m_nMoneyBalanceY;              //有送充值余额
	int m_nMoneyBalanceZ;              //赠送账户余额
	UINT m_nCurrentPoint;              //当前积分
	int m_nMoneyOnlineAmount;          //预扣金额
	int m_nMoneyGuaranty;              //押金 - 只有临时用户有

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
	void SetLocalActiveMember(CActiveMember& activeMember);
};