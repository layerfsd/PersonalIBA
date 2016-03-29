#pragma once

#include "IBADialog.h"
#include "NetBarConfig2.h"
//
//class:CClientConfigCommonPage 客户端设置->基本设置
//
class CClientConfigCommonPage : public CIBAPage
{
	DECLARE_DYNAMIC(CClientConfigCommonPage)
	DECLARE_MESSAGE_MAP()
public:
	CClientConfigCommonPage();
	virtual ~CClientConfigCommonPage();

// 对话框数据
	enum { IDD = IDD_CONFIG_CLIENT };

private:
	
	CNetBarConfig2 m_NetBarConfig2;

	INT m_nCheckInPwdType;
	INT m_nAfterCheckOut;

	UINT m_nShutDownMinutes;

	// 临时用户自助换机
	BOOL m_bTmpUserChangeComputer;
	// 会员用户自助换机
	BOOL m_bMemberUserChangeComputer;
	// 客户端显示使用时间
	BOOL m_bShowClientUseTime;

	// 允许客户端挂机
	BOOL m_bAllowSuspend;

	//2011-03-15-gxx-允许客户端重启自动登录
	BOOL m_bAllowAutoCheckIn;

	UINT m_nNoEnoughTime;


protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

};
