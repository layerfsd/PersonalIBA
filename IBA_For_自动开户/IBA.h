
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "ibalog.h"
#include "netbarconfig.h"
#include "IBAConfig.h"
#include "IBAView.h"
#include "OperationInfo.h"
#include "ActiveMemberView.h"

class CCashier;
class CIBARealName;
class CDialogManager;

class CIBAApp : public CWinApp,
				public CBCGPWorkspace
{
public:
	CIBAApp();

	virtual void PreLoadState ();
	virtual BOOL LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd);
	
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CString GetWorkPath() const;

	void UpdateToolbarLastID(const CString&, BOOL bError=FALSE);
	void UpdateIBAStatus(BOOL bTryingConnection=FALSE);

	CString GetLastNetID()const;

public:

	INT IBAMsgBox(LPCTSTR lpszPrompt, UINT nType = MB_OK | MB_ICONINFORMATION, BOOL bFormat=TRUE);
	INT IBAMsgBox(UINT nResID, UINT nType = MB_OK | MB_ICONINFORMATION, BOOL bFormat=TRUE);
	INT IBAMsgBox2(LPCTSTR lpszPrompt, UINT nType = MB_OK | MB_ICONINFORMATION, BOOL bFormat=TRUE);

	CCashier* GetCurCashier();
	CIBARealName* GetRealName();
	CDialogManager* GetDialogManager();

	CIBAView* GetIBAView() const { return m_lpIBAView; }
	void SetIBAView(CIBAView* newVal) { m_lpIBAView = newVal; }
	
	//2011-03-21-gxx 
	CActiveMemberView* GetActiveMemberView();

	void UpdateIBA();
private:

	
	BOOL InitIBAEnv();
	void InitIBA();
	void OnLoginOK();

private :

	CCashier*		m_lpCurCashier;
	CIBAView*		m_lpIBAView;
	CIBARealName*	m_lpRealName;
	CDialogManager* m_lpDialogManager;

	CL2Instance m_IBAInstance;
	HMODULE m_hinstBCGCBRes;
	CString m_strWorkPath;

public:
	// 2011/06/24-gxx: 自动重新启动IBA
	void AutoRestart();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};


extern CIBAApp theApp;

class CIBAGlobal
{
public:
	static const UINT CLASSID_LOCALUSER = 9999;  // 本地用户的级别ID

	// 实名类型定义
	enum emRealnameType
	{
		emRealnameNone = 0,
		emRealnameRZX = 1, // 任子行
		emRealnameGLW = 2, // 过滤王
		emRealnameMoGen = 3, // 摩根
		emRealnameSangDi = 4, // 桑地
		emRealnameWD = 5, // 网盾
		emRealnameRZX2008 = 6, // 任子西安
		emRealnameHengXin = 9, // 恒信
	};

	// 密码类型
	enum emPasswordType
	{
		emPwdTypeRandom  = 0,   // 随机密码
		emPwdTypeIDTail6 = 1,   // 证件后6位数
		emPwdTypeFixed   = 2,   // 固定密码
		emPwdTypeInput   = 3,   // 用户输入
		emPwdTypeEmpty   = 4    // 空密码
	};

	// 操作类型
	enum emOperationType
	{
		emOpTypeNULL = 0,
		emOpTypeCredit = 1,          // 充值
		emOpTypeReturn = 2,          // 退款
		emOpTypeOpen = 3,            // 开户
		emOpTypeChangeMachine = 4,   // 换机
		emOpTypeAutoReturn = 5,      // 自动退款
		emOpTypeVipReturn = 6,       // 会员结账
		emOpTypeBundleTime = 7,      // 包时
		emOpTypeReturnBalance = 8,   // 退余额
		emOpTypeCreditDelete = 9,    // 充值冲正
		emOpTypeCreditModify = 10,   // 充值修正	

		emOpTypeOpenLocal ,          // 本地开户
		emOpTypeLocalReturn,         // 本地用户退款
		emOpTypeLocalCredit,         // 本地充值
		emOpTypeBrokenReturn,        // 中心临时用户断网退款
		emOpTypeBrokenReturnVip,     // 中心会员用户断网退款
		emOpTypeRoomReturn,          // 包房结账

		emOpTypeCount
	};

	static CString GetOperationName(int nOptType);

	// 与中心的网络状态
	enum emNetworkStatus
	{
		emNetworkNormal,             // 中心网络连接正常, 处于正常状态
		emNetworkBroken,             // 网络中断，处于应急模式状态
		emNetworkRestore             // 恢复状态， 从网络中断到网络连接完全正常的过渡过程
	};

	// 用户状态类型
	enum emUserState
	{   
		emUserStateNULL,             // 用户不存在
		emUserStateCenter,           // 中心用户
		emUserStateLocal             // 本地用户
	};

	// 应急模式启用方式
	enum emLocalModeStartUpKind
	{
		emLocalModeForbid,           // 禁用
		emLocalModeManual,           // 手动
		emLocalModeAuto              // 自动
	};

	// 系统动作之后，用户视图弹出界面的反应
	enum emOperationAfter
	{
		emOperationAfterNull,        // 回车/鼠标双击后不弹出任何界面
		emOperationAfterCredit,      // 弹出充值界面
		emOperationAfterRefund       // 弹出结账界面
	};

	// 计费方式
	enum emPayType
	{
		emPayTypeNormal = 0,         // 普通计费
		emPayTypeAccumulative = 1,   // 累计封顶
		emPayTypeWithholding = 2     // 足额预扣
	};

	enum emAfterCheckOutFlag
	{
		emAfterCheckOutLock,     // 锁屏
		emAfterCheckOutShutdown, // 关机
		emAfterCheckOutRestart   // 重启
	};

	// 99
	static const UINT RZX_SPE_DOMAIN1 = 4; // 任子行，江门地区任子行实名域ID

public:
	static emNetworkStatus NetworkStatus;

	static DWORD s_dwStartUpTickcount;

	static HWND m_hCurRegisterDlg;		// 当前开户对话框的句柄
	static HWND m_hCurCreditDlg;			// 充值对话框句柄
};
