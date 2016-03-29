#pragma once

#include "NetBarUser.h"
#include "Cashier.h"
#include "NDDialog.h"
#include "BCGPLabalButton.h"
#include "DefineBundTimeDlg.h"

class CRegisterDlg : public CNDDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)
	DECLARE_MESSAGE_MAP()

public:
	CRegisterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegisterDlg();

// 对话框数据
	enum { IDD = IDD_ND_REGISTER };
	static HWND m_StaticPrehWnd;
	static BOOL m_StaticTmpMember;//保存上次操作，是否临时用户

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnSetfocusEditIdcardsno();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonBundletime();
	afx_msg void OnBnClickedButtonReadid2();
	afx_msg void OnScan1();
	afx_msg void OnRead2();
	afx_msg void OnScan2();
	afx_msg void OnScanPassport();
	afx_msg void OnScan2Back();
	afx_msg void OnScanOther();
	afx_msg LRESULT OnRealName(WPARAM wp, LPARAM lp);
	afx_msg void OnBnClickedCheckTempmember();
	afx_msg void OnBnClickedButtonReprint();
	afx_msg void OnBnClickedCheckPrint();
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnCbnSelchangeComboBtsel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnChangeEditPersonalid();
	afx_msg void OnCbnSelchangeComboUserclass();
	afx_msg void OnBnClickedRadioTempuser();
	afx_msg void OnBnClickedRadioMember();
	afx_msg void OnCbnSelchangeComboTempuserclass();

	void RegisterLocalUser();  // 注册本地用户
	void RegisterCenterUser(); // 注册中心用户

public:
	void SetPhoneNumberOpen();


private:

	static const UINT TimerIdOfDisableInput = 1002;

	CFont m_MoneyFont;
	
	CBCGPComboBox m_cboSex;
	CBCGPComboBox m_cboUserClass;
	CBCGPComboBox m_cboTempUserClass;
	CBCGPComboBox m_cboIDType;
	CBCGPComboBox m_cboBundTimeSel;	// 包时选择下拉控件
	CBCGPComboBox m_cboComputer;

	CEdit m_edtPersonalID; //需要绑定一个控件变量才能响应change事件
	CBCGPMaskEdit m_edtMoney;
	CBCGPButton m_chkTempMember;
	
	CBCGPButton m_btnTempUserClass;
	CBCGPButton m_btnMemberUserClass;
	CBCGPLabelButton m_btnSefDefine;

	BOOL m_bPrintTicket;
	BOOL m_bUserHasCard;
	BOOL m_bNoPresent;

	//用户资料
	INT m_nUserSex;
	CString m_strUserName;
	CString m_strPersonalID;
	CString m_strCardId;
	CString m_strPassword;
	CString m_strNetId;
	CString m_strNetId2; //备份上网账号
	CString m_strNation;
	CString m_strTel;
	CString m_strTelNum;
	CString m_strOfficer;
	BOOL	m_bIsTempUser; //是不是临时用户

	//
	CDefineBundleTimeDlg m_DefineBundleTimeDlg;

	// 2011/09/07-8201-gxx: 

private:

	CRegisterMethod UserRegister;

	INT	m_nDefClassIndex; //!<默认用户级别

	//第三方传过的来的临时变量
	
	BOOL m_bIsMember;		//!<是否为临时用户
	BOOL m_bFromScan;       //!<信息是否来自扫描
	BOOL m_bFromCard;		//!<信息是否来自刷卡
	BOOL m_bCanEditUserInfo;//!<是否可以编辑用户信息
	BOOL m_bNetIdFromPersonalId; //!<上网账号强制来自证件号码

	//开户过程中间变量

	UINT m_nCreditMoney;	//!<实收充值金额
	UINT m_nCreditPresent;  //!<充值赠送金额
	UINT m_nUserClassID;	//!<开户用户级别
	CString m_strUserClassName; //!<开户用户级别名称
	UINT m_nIdType;			//!<证件类型
	UINT m_nCostExpense;	//!<工本费
	UINT m_nGuarantyMoney;  //!<押金

	//注册成功后变量

	BOOL m_bAfterRegister;  //是否已经注册成功了
	UINT m_nMemberId;		//!<注册成功后的用户号

	//包时相关变量
	UINT	m_nBundleTimeId;
	UINT	m_nBundleTimeType;
	UINT    m_nBundTimeClass;//包时操作方式
	UINT	m_nBundTimeMoney;
	CString m_strBundleTimeStartTime;
	CString m_strBundleTimeEndTime;
	CString m_strBundleTimeArea;
	CString m_strBalanceAfterBundleTime;

	CNetBarUser m_userInfo;

	BOOL m_bPhoneNumberOpen;
	BOOL m_bWaitReg;		// 等待注册完成
	BOOL m_bClickRnReg;		// 点击了实名注册按钮
private:

	//生成临时用户资料
	void MakeTmpMemberInfo();
	void ScanReco(UINT nType);
	void ClearData();
	void InitData();
	void InitContrls();

	void ResetNetworkBrokenControl();
	void InitIdTypeComboBox();
	void InitUserClassComboBox();
	void InitBundleTimeComboBox();
	void InitComputerComboBox();
	void EnableUserInfoControls(BOOL bEnable);
	void SelectIDType();
	BOOL GetCardIdFromCenter();
	BOOL CheckRealNameData();
	BOOL CheckInput();
	BOOL CheckMoney();
	void MakeUserPwd();
	BOOL IsBundleTime();
	void AutoUploadImage();
	void AutoCheckIn();

	void PrintTicket();
	void PrintTicket2();

	void PrintTicketPos();		// 老的pos机打印小票的方式
	
	void PrintTicketA4();		// 打印A4纸方式
	
	void GetRegisterInfo();
	void SetRegisterInfo();
	void SaveRegisterInfo();
	void ShowRegisterResult(BOOL bLocalUser=FALSE);
	void ShowRegisterBundleTimeResult();

	// 网监要求
	void OpenRServer();

	void ExeSfreg();

	// 2014-4-25 - qsc
	void SendRegDataToProxy();
public:
	
	void SetNetId(CString newVal) { m_strNetId = newVal; }
	void SetCardId(CString newVal) { m_strCardId = newVal; }
	void SetUserInfo(CNetBarUser& NetBarUser);

	virtual INT_PTR DoModal();

	afx_msg void OnBnClickedBtnCustomBoundletime();
	afx_msg void OnBnClickedBtnInputNumber();
	afx_msg void OnBnClickedBtnCapphtoto();
	afx_msg void OnBnClickedBtnRnReg();
	afx_msg LRESULT OnCanClose(WPARAM wP,LPARAM lP);
	afx_msg void OnBnClickedButtonQueryTel();
	afx_msg void OnBnClickedBtnPhoto();
protected:
	virtual void OnCancel();
};
