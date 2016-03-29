#pragma once

#include "DdnDataX.h"
#include "RealNameDataX.h"

#include "..\NetBarUser.h"
#include "..\Servlet\BalanceMethod.h"
#include "..\Servlet\IDCheckCardMethod.h"

using namespace NS_SERVLET;

namespace NS_REALNAME
{
	
	typedef struct tag_CheckoutInfo
	{
		TCHAR szIP[50];
		TCHAR szTermId[50];
		TCHAR szNetId[100];
		TCHAR szName[100];
		int nCheckoutType;
		int nMoney;
		bool bClientCheckOut;		// 客户端发来的结账信息
		
		tag_CheckoutInfo()
		{
			ZeroMemory(szIP, sizeof(szIP));
			ZeroMemory(szTermId, sizeof(szTermId));
			ZeroMemory(szNetId, sizeof(szNetId));
			ZeroMemory(szName, sizeof(szName));
			nCheckoutType = 0;
			nMoney = 0;
			bClientCheckOut = false;
		};
	}CheckoutInfo;

class CRealNameBase
{
public:	
	virtual ~CRealNameBase(void);

	//!<实名初始化
	virtual BOOL InitRealName();
	
	//!<实名清理
	virtual BOOL UnInitRealName();

	//!<实名检查
	virtual BOOL CheckRealName();
	/**
	* 响应实名消息
	* @param nMsgType 0:刷卡消息，1:定时消息
	*/
	virtual INT  OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<用于二次执行刷卡消息
	virtual INT  OnExecuteMsg(UINT nExecuteType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<刷希之光的读卡器
	virtual void OnCardIn();

	//!<调用实名相关功能
	virtual INT  InvokeRealName(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	//!<扫描仪是否可用
	virtual BOOL IsScanEnabled();

	//!<扫描证件
	virtual BOOL ScanUserInfo(UINT nType, CNetBarUser& NetBarUser, UINT nOption);

	//!<扫描证件，并根据用户是否注册而显示注册或者充值退款对话框
	virtual BOOL DoScan(UINT nType);

	//!<上传照片
	virtual BOOL UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption);
	
	//!<激活用户
	virtual BOOL ActivationMember(const CNetBarUser& netBarUser);

	//!<是否允许开户
	virtual BOOL IsRegisterEnabled();

	//!<是否允许客户端登录
	virtual BOOL IsCheckInEnabled();

	//!<是否需要校验开户实名资料
	virtual BOOL NeedToCheckRegisterData();

	//!<是否允许实名菜单
	virtual BOOL IsMenuEnabled();

	//!<是否允许实名扩展菜单
	virtual BOOL IsRealNameExtendMenuEnabled();

	//!<刷卡后是否抢焦点
	virtual BOOL RobFocusAfterSwipingCard();

	//!<收银员登录
	virtual void OnCashierCheckIn();

	// 2011/12/02-8232-gxx: 
	void ExecuteRNS(BOOL bInit);

	// 2014-5-8 - qsc 
	void ExeSfreg(LPCTSTR szNation, LPCTSTR szDepart, LPCTSTR szAddress, LPCTSTR szTel);

	// 2014-5-23 - qsc
	void SendRegDataToProxy(LPCTSTR szNation, LPCTSTR szDepart, LPCTSTR szAddress, LPCTSTR szTel);

	// 2014-10-10 - qsc
	virtual void OnCheckOut(const CheckoutInfo* pCheckoutInfo){}
protected:

	CRealNameBase(void);

	virtual INT OnOldUser(CNetBarUser & NetBarUser);  //激活弹窗

	virtual INT OnOldUserInLocalMode(CNetBarUser & NetBarUser); //本地激活弹窗

	virtual INT OnNewUser(CNetBarUser & NetBarUser); //注册

	virtual BOOL CheckCard(LPCTSTR lpszNetId);//校验NetId. 并设置m_nCardId和m_nMemberId.

	virtual BOOL CheckCardInLocalMode(LPCTSTR lpszNetId); //本地模式下校验NetId.从Member表查找对应NetId和NebBarId,且可用的账户,并设置m_nCardId和m_nMemberId,设置m_NetBarUser

	virtual BOOL QueryBalance(); //使用m_nMemberId和m_nCardId向中心查余额，使用返回的SerialNum显示在工具栏中。

	//!<自动更新实名资料
	virtual BOOL AutoModifyMemberInfo();

	virtual BOOL AutoMatchIDCard(CNetBarUser & NetBarUser);
	
	//把二代证转换为上网号
	virtual BOOL ChangeNetIdToPersonalId(CString strNewNetId);

	//运行实名代理
	virtual BOOL ExecuteProxy();

	// 2011/08/01-8201-gxx: 
	virtual BOOL OnLocalRealName(UINT nMsgType, CString strNetID);
	virtual BOOL DoHasLocalRealName(UINT nMsgType,LPVOID lpData = NULL, DWORD dwLen = 0);

protected:

	virtual BOOL ReadUserInfo(CNetBarUser & NetBarUser);

protected:

	BOOL m_bOpenReader;

	UINT m_nMemberId;

	UINT m_nCardId;

public:
	CNetBarUser m_NetBarUser;

protected:

	CDataXMSender m_DataXSender;

	CDataXMReceiverT<CRealNameBase> m_DataXReceiver;

	INT OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
		LPVOID lpBuf, size_t cbLen);

	virtual BOOL OnCardInfo(DataX_UserInfo& dxUserInfo);

	virtual BOOL OnForceCheckOut(DataX_UserInfo& dxUserInfo);
	
	virtual BOOL OnAddMember(DataX_UserInfo& dxUserInfo);

	// 2011/06/15-gxx: 播放生日祝福语
	BOOL SpeakBirthday(CNetBarUser & NetBarUser);
};


class CNoneRealName : public CRealNameBase
{
public:	
	CNoneRealName(void);
	virtual ~CNoneRealName(void);

	//!<实名初始化
	virtual BOOL InitRealName();

	//!<实名检查
	virtual BOOL CheckRealName();

	//!<扫描仪是否可用
	virtual BOOL IsScanEnabled();
};

}