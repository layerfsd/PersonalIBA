#pragma once

#include "NDDialog.h"
#include "ActiveMember.h"
#include "BCGPLabalButton.h"
#include "BCGPLinkStatic.h"
#include "afxwin.h"

// CNDReturnDlg 对话框

class CNDReturnDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CNDReturnDlg)
	DECLARE_MESSAGE_MAP()

public:
	CNDReturnDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNDReturnDlg();

// 对话框数据
	enum { IDD = IDD_ND_DREG };

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCredit();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonConsumelist();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonReturn2();
	afx_msg void OnBnClickedButtonBundletime();

	void OnGetBalanceOK(CBalanceMethod & Balance);
	void InitControls();
	BOOL PayoutMember();
	void DeregisterUser();
	void ShowBundleTimeInfo(CBalanceMethod & Balance);

	// 2011/04/29-gxx: 
	virtual void OnCenterDisconnect();
	
	// 2011/04/29-gxx: 处理和中心断开连接时的结账处理
	BOOL DoDisconnectReturn();  

	// 2011/07/12-8201-gxx: 本地结账查询
	virtual BOOL DoLocalModeQuery();

	// 2011/07/12-8201-gxx: 本地结账处理
	virtual BOOL DoLocalModeOperate();

	// 2011/10/26-8230-gxx: 
	BOOL ReturnRoomUser();

private:

	CActiveMember m_ActiveMember;

	BOOL m_bAutoQuery;

	BOOL m_bUserHasCard; //!<用户自带卡

	INT m_nAllMoney; //!<合计余额

	CString m_strPwd;//捆绑的控件密码

	CString m_strLastLogoutTime;
	CString m_strDeposit;
	CString m_strUserInfo;
	CString m_strBundleTimeInfo;

	CBCGPLabelButton m_btnConsumeList;
	CBCGPLabelButton m_btnCredit;
	CBCGPLabelButton m_btnReturn2;
	CBCGPLabelButton m_btnBundleTime;
	CBCGPLabelButton m_btnZSBConsumeList;
	CBCGPLabelButton m_btnCancelActivation;

	BOOL m_bPrintTicket;

	INT m_nConsumeMoney;

public:
	
	void SetNetId(CString newVal);
	
	
	afx_msg void OnBnClickedButtonConsumelist2();
	
	afx_msg void OnBnClickedButtonCancelactive();
};
