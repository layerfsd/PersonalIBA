#pragma once
/**************************************************************************

CIBARealName：

 1.维护第三方实名控制
 2.实名对象的创建，销毁

**************************************************************************/


#include "RealNameBase.h"



class CIBARealName
{
public:
	CIBARealName(void);

	~CIBARealName(void);

	BOOL InitRealName();

	BOOL UnInitRealName();

	BOOL CheckRealName();

	INT  OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	INT  OnExecuteMsg(UINT nExecuteType, LPVOID lpData = NULL, DWORD dwLen = 0);

	void OnCardIn();

	INT  InvokeRealName(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	BOOL IsScanEnabled();

	BOOL ScanUserInfo(UINT nType, CNetBarUser& NetBarUser, UINT nOption = 0);

	BOOL DoScan(UINT nType);

	BOOL UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption = 0);

	BOOL ActivationMember(UINT nMemberID);

	BOOL IsRegisterEnabled();

	BOOL IsCheckInEnabled();

	BOOL IsMenuEnabled();

	BOOL NeedToCheckRegisterData();

	BOOL IsRealNameExtendMenuEnabled();

	//!<刷卡后是否抢焦点
	BOOL RobFocusAfterSwipingCard();

	//!<收银员登录
	void OnCashierCheckIn();

	// 2011/10/27-8210-gxx: 通知实名用户下机
	void AuditUserCheckOut(CString strTermID,CString strIP,UINT nMemberID);

public:

	BOOL CreateRealName();
	void DestoryRealName();

	NS_REALNAME::CRealNameBase* m_lpRealName;

};

