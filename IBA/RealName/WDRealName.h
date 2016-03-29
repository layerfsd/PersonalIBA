#pragma once

#include "RealNameBase.h"

namespace NS_REALNAME
{

//class CWDRealName : public CRealNameBase, public CThread
	class CWDRealName : public CRealNameBase
{
public:
	CWDRealName(void);
	virtual ~CWDRealName(void);

	virtual BOOL InitRealName();

	virtual BOOL UnInitRealName();
	
	virtual BOOL CheckRealName();

	virtual BOOL IsScanEnabled();

	virtual BOOL ScanUserInfo(UINT nType, CNetBarUser& NetBarUser, UINT nOption);

	//!<扫描证件，并根据用户是否注册而显示注册或者充值退款对话框
	virtual BOOL DoScan(UINT nType);

	virtual INT  OnRealNameMsg(UINT nMsgType, LPVOID lpData = NULL, DWORD dwLen = 0);

	virtual BOOL UploadImage(UINT nType, CNetBarUser& NetBarUser, UINT nOption = 0);

	//!<是否允许开户
	virtual BOOL IsRegisterEnabled();

	//!<是否允许客户端登录
	virtual BOOL IsCheckInEnabled();

	//!<是否需要校验开户实名资料
	virtual BOOL NeedToCheckRegisterData();

public:
	BOOL m_bNeedUpload;

private:

	void Load();

	void UnLoad();

	void GetWDState();

	BOOL SendToProxy(CByteArray &buffer);

	BOOL SendImage(UINT nType);

	BOOL IsWdWorking();

	virtual int Run();

private:

	//!<自动更新实名资料
	virtual BOOL AutoModifyMemberInfo();

	INT m_nPort;

	/*HMODULE m_hIDReader;
	HMODULE m_hIDRSUCore;
	HMODULE m_hWdCheck;*/
	HMODULE m_hRealname;

	//INT m_nWDState;
public:

	typedef struct WDUserInfo
	{
		char Name[40]; 			// 姓名
		char Sex[10];			// 性别
		char Nation[20];		// 民族
		char BirthDate[50];		// 出生年月日（1985-08-16）
		char Address[200];		// 地址
		char Id[40];			// 证件号码
		char Type[4];			// 证件类型(“011”表示身份证;“099”表示其它证件)
		char FromDepart[200];	// 发证机关
		char StartDate[50];		// 有效期限（起始日期）
		char EndDate[50];		// 有效期限（截止日期）
	}*pWDUserInfo;

	typedef struct WDParam
	{
		char ConsumeVer[24]; // 计费版本号							// 必须传
		char ConsumeNo[50];  // 计费注册序列号		
		int	 ConsumeCount;    // 网吧注册终端数
		int  ConsumeType;    // 计费类型 (1希之光 2 pubwin  3万象)		// 必须传
		char Expiration[12]; // 计费软件使用截止日期  格式：年月日（20201022）
		char MacAddr[16];    // 与注册号绑定的机器MAC 格式：00E04C47105A
		char HostIp[16];     // 与注册号绑定的机器IP 格式: 202.119.23.54
	}*pWDParam;
	static CNetBarUser s_NetBarUser;
private:

	/*typedef INT (__cdecl *pfnIDReaderInit)(INT, HWND);
	typedef INT (__cdecl *pfnIDReaderUninit)();
	typedef INT (__cdecl *pfnIDReaderSetEventWnd)(HWND, HWND);

	pfnIDReaderInit m_pfnIDReaderInit;
	pfnIDReaderUninit m_pfnIDReaderUninit;
	pfnIDReaderSetEventWnd m_pfnIDReaderSetEventWnd;*/

	//struct ID_CARD_INFO
	//{
	//	char szISN[20]; //内码  （以内码为准）
	//	char szPSN[20]; //印刷码（暂时不使用）
	//};

	WDParam m_param;
	BOOL m_bInitSuccessed;
	typedef void (__cdecl* PFunOnUserScan)(WDUserInfo *pUserInfo);
	typedef int (__cdecl* FUN_Init)(WDParam *pParam,PFunOnUserScan pfUserScan);
	typedef void (__cdecl* FUN_Stop)();

	FUN_Init m_pfunInit;
	FUN_Stop m_pfunStop;

	//******************************************************************************

	//struct WDUserInfo
	//{
	//	char szName[40];
	//	char szSex[10];
	//	char szNation[20];
	//	char szBirthDate[50];
	//	char szAddress[200];
	//	char szID[40];
	//	char szFromDepart[200];
	//	char szStartDate[50];
	//	char szEndDate[50];
	//	char szNewAddress[200];
	//};

	//typedef INT (__stdcall *pfnIDRSUCoreInit)();

	//pfnIDRSUCoreInit m_pfnIDRSUCoreInit;


	//typedef INT (__stdcall *pfnGetInfoFirstType)(WDUserInfo* lpUserInfo);

	//pfnGetInfoFirstType m_pfnGetInfoFirstType;

	//typedef INT (__stdcall *pfnGetInfoSecondType)(WDUserInfo* lpUserInfo);

	//pfnGetInfoSecondType m_pfnGetInfoSecondType;

	//typedef INT (__stdcall *pfnGetInfoSecondTypeMachine)(WDUserInfo* lpUserInfo);

	//pfnGetInfoSecondTypeMachine m_pfnGetInfoSecondTypeMachine;

	//typedef INT (__stdcall *pfnGetOtherType)();

	//pfnGetOtherType m_pfnGetOtherType;

	////******************************************************************************

	//typedef INT (__stdcall *pfnInitCheck)(char* lpWdHostIp, INT TypeId);
	//
	//pfnInitCheck m_pfnInitCheck;

	//typedef void (__stdcall *pfnUnInitCheck)();

	//pfnUnInitCheck m_pfnUnInitCheck;

};

}