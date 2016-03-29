#pragma once

#include "NDDialog.h"
#include "BCGPLabalButton.h"

//
//class:CCreditDlg 充值退款对话框,在响应工具栏命令时弹出
//
class CCreditDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CCreditDlg)
	DECLARE_MESSAGE_MAP()

public:
	CCreditDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreditDlg();
	//静态
	static HWND m_StaticPrehWnd;//用来接收本窗体的指针，标识窗口是否已经弹出（因为是模态的）

// 对话框数据
	enum { IDD = IDD_ND_CREDITANDDEREG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCancelactive();
	afx_msg void OnBnClickedButtonCreditprint();
	afx_msg void OnBnClickedButtonRefundment();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCheckout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnRealName(WPARAM wp, LPARAM);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonConsumelist();
	afx_msg void OnBnClickedButtonReprint();
	afx_msg void OnBnClickedButtonBundtime();
	afx_msg void OnEnChangeEditIdcardsno();

	void OnGetBalanceOK(CBalanceMethod & Balance);
	void OnOldCard(UINT nCardId, UINT nMemberId);
	BOOL CheckReturnAction();//判断回车操作，符合执行要求返回真

	//执行充值操作
	BOOL DoCredit();
	BOOL CheckInput();
	void InitControls();
	void ClearData();
	BOOL MakeSureCredit(UINT nMoney);
	BOOL MakeSureDReg();

	void PrintTicket(CString strCreditMoney);
	void PrintCreditBTimeTicket();

	void DeregisterUser(BOOL bAutoRefund=FALSE);
	//执行中心包时处理
	BOOL DoBundleTime();
	//取消包时
	BOOL DoCancelBundleTime(CString strMemberID);
	//在线包时
	void DoBundTimeOnLine();

	// 选择包时
	void SelectBundTime();

	//显示用户信息
	void ShowUserInfo(CBalanceMethod & Balance);
	void ShowBundleTimeInfo(CBalanceMethod & Balance);
	void ShowControls(CBalanceMethod & Balance);
	//!<自动退款
	void AutoDoRefundment(CBalanceMethod & Balance); 

public:
	
	void SetForReturn(BOOL newVal = TRUE){ m_bForReturn = newVal; }

	void SetFormCard(BOOL newVal = TRUE){ m_bFromCard = newVal; }

	virtual INT_PTR DoModal();

	// 2011/07/28-8201-gxx: 
	virtual BOOL DoLocalModeQuery();

	// 2011/07/28-8201-gxx: 
	virtual BOOL DoLocalModeCredit(UINT nCreditMoney);
	
private:

	BOOL m_bUserHasCard; //!<用户自带卡
	BOOL m_bNoPresent;   //!<是否赠送
	BOOL m_bPrint;		//是否打印

	INT m_nAllMoney; //!<合计余额
	//先前的充值金额
	UINT m_nCreditMoney; 
	UINT m_nPresentMoney;

	CString m_strCreditMoney;
	CString m_strPwd;//捆绑的控件密码
	CString m_strTmpUserPwd;//保存的临时用户密码
	CString m_strGiveBack;
	CString m_strMemberID;
	CString m_strNetBar;
	CString m_strDeposit;
	CString m_strBalanceW;
	CString m_strBalanceY;
	CString m_strBalanceZ;
	CString m_strResult;
	CString m_strPoint;
	CString m_strOnlineAmount;// 上网预留

	//包时相关变量
	BOOL    m_bAlreadyBundTime;//是否已经包时
	UINT	m_nBundleTimeId;//包时ID，初始为0，为无包时状态
	UINT	m_nBundleTimeType;//包时付费类型
	UINT    m_nBundTimeClass;//包时操作类型

	CString m_strBundleTimeStartTime;
	CString m_strBundleTimeEndTime;
	CString m_strBundleTimeArea;//包时地点区域
	CString m_strBalanceAfterBundleTime;
	// 包时时段
	CString m_strBundTimeLimit;
	// 包时金额
	CString m_strBundTimeMoney;
	// 包时剩余时间
	CString m_strBTRemainTime;
	
	CBCGPMaskEdit m_edtCreditMoney;
	// 上网账号
	CEdit m_edtNetId;
	
	CBCGPLabelButton m_btnCancelActivation;
	CBCGPLabelButton m_btnConsumeList;

	BOOL m_bFromCard;  //!<信息是否来自刷卡

	BOOL m_bForReturn; //!<为了退款

	BOOL m_bHasLocalQuery; // 2011/07/28-8201-gxx: 是否为本地查询的结果

	// 2011/10/21-8208-gxx: 
	CActiveMember m_ActiveMember;

	bool m_bMem;
public:
	afx_msg void OnBnClickedWxCredit();
};
