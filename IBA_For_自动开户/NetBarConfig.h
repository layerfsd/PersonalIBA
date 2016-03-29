#pragma once

#include "Singleton.h"
#include "IniConfig.h"
#include "NetbarBundleTime.h"
#include "dal\NetBarInfo.h"
#include "Servlet\UserClassInfo.h"
#include "dal\IDTypeInfo.h"

using namespace NS_DAL;

class CNetBarConfig : public CIniConfig
{
	SINGLETON_DECLARE(CNetBarConfig)

public :
	static CNetBarConfig* GetInstance();

	UINT GetNetBarId() const;
	CString GetNetBarName() const;
	CString GetNetBarKey() const { return m_NetBarKey; }

	CString GetLocalServerIP() const;
	void SetLocalServerIP(CString newVal);

	CString GetMainCenterIP() const;
	UINT GetMainCenterPort() const { return m_MainCenterPort; }
	CString GetMainCenterPath() const { return m_MainCenterPath; }

	CString GetBackupCenterIP() const { return m_BackupCenterIP; }
	UINT GetBackupCenterPort() const { return m_BackupCenterPort; }
	CString GetBackupCenterPath() const { return m_BackupCenterPath; }

	CString GetStandbyHost() const { return m_strStandbyHost; }
	void SetStandbyHost(CString newVal);

	CString GetBossPwd() const { return m_strBossPwd; }
	void SetBossPwd(CString newVal);

	CString GetUnLockPassword() const { return m_strUnLockPassword; }
	void SetUnLockPassword(CString newVal);

	CString GetProxyIP() const { return m_strProxyIP; }
	void SetProxyIP(CString newVal);
	
	CString GetNetbarNotice() const { return m_strNetbarNotice; }
	void SetNetbarNotice(CString newVal);
	
	void SetSystemConfig(LPCTSTR szFieldName, LPCTSTR szFieldValue, UINT value1, UINT value2 );

	// 是否允许开会员开
	BOOL IsEnableOpenMember();


public:
	
	BOOL LoadFromDB();
	void LoadFromIni();
	void SaveToIni();
	BOOL IsLoadFromDB() const { return m_bLoadFromDB; }

public :

	CNetbarBundleTime		m_NetbarBundleTime;	//包时列表信息
	CIDNameRecordArray		m_PCClassInfoArray; //机器区域
	CUserClassInfoArray		m_UserClassArray;	//用户级别列表
	CIDTypeInfoArray		m_IDTypeInfoArray;	//证件号码列表
	CNetBarInfoArray		m_NetBarInfoArray;	//网吧信息
	BOOL                    m_bRZXCardMember;   //任子行是否允许开会员卡

	CString GetNetBarName(UINT nNetbarId) const;
	CString GetPCClassName(UINT nPCClass);
	CString GetUserClassName(UINT nClassID);
	CString GetIDTypeName(UINT nIdType);

	BOOL IsUseDeposit();
	UINT GetDeposit(UINT nClassID);
	BOOL GetIsMember(UINT nClassID);//判断是否是会员
	UINT GetUserClassId(CString strClassName);//从名字得ID

private :

	BOOL m_bLoadFromDB; //是否使用数据库

	UINT m_NetBarId;
	UINT m_DomainId;
	CString m_NetBarKey;
	CString m_strNetBarName;

	CString m_LocalServerIP;
	CString m_strProxyIP;
	CString m_strBossPwd;

	CString m_strNetbarNotice;

	CString m_MainCenterIP;
	CString m_MainCenterPath;
	UINT m_MainCenterPort;

	CString m_BackupCenterIP;
	CString m_BackupCenterPath;
	UINT m_BackupCenterPort;
	
	CString m_strStandbyHost;

private:

	BOOL m_bReturnOnlyClassId;

	CString m_strDodoNewHost;

	CString m_strDodonewResource;

	UINT m_nUpdateInterval;

	UINT m_ForeRealName;

	CString m_strQueryHost;
	CString m_strQueryHost2;

	UINT m_nAuditType;

	UINT m_nSoftwareType;//软件计费类型

	BOOL m_bNoIDCardIn;

	UINT m_bIssueNetBarId;
	
	BOOL m_bAllowNoCardLogin;

	UINT m_nRealNameFrom;

	CString m_strUnLockPassword;

	UINT m_nICControlVerID;
	
	UINT m_nRoundType;
	
	BOOL m_nBundleTimeUseBalance; //现金包时

	BOOL m_bMustSynchronize;

	BOOL m_bCreditDirect; //!<是否允许直充

public :

	// 2011-4-15-gxx: 是否允许扫描
	GPropertyReadOnly(ScanEnabled, INT);

	// 2011/05/12-gxx: 是否强制自动退款时间为0,  1:表示强制
	GPropertyReadOnly(MustAutoReturn, INT);

	// 2011/06/10-gxx: 是否强制关闭"网吧卡户辅助"等程序,
	//                 KillCaptions是这些程序的标题的组合，用"|"分割
	GPropertyReadOnly(KillCaptions, CString );  
	
	// 2011/06/20-gxx: 包时的限制时间,即"离包时结束XXX分钟之内不允许包时"
	GPropertyNeedSet( BTLimitTime, UINT); 
	void SetBTLimitTime(const UINT nNewVal);

	// 2011/06/30-gxx: （中心参数）是否允许修改密码
	GPropertyReadOnly(ForbidModifyPwd, INT);

	// 2012/11/12-gxx: 是否监控实名代理已启动, 大于0表示监控
	// 目前此参数只对任子行实名有效
	GPropertyReadOnly(MonitorAuditProxy, INT);

	// 是否启用ID2Reader.exe, =1表示启用 其他 =0表示不启用
	GPropertyReadOnly(EnableID2Reader, INT);

	// 网监要求开户对身份证做18岁验证，gintidentitycard=18验证，=0不验证）
	GPropertyReadOnly(LimitAge, INT);

	// 强制重启IBA的参数: gstrForceQuit=IBA8260_1, _1表示如果版本低于8260,就强制重启收银台
	GPropertyReadOnly(ForceQuitVersion, CString);
	GPropertyReadOnly(ForceQuitParam, INT);

	// 是否强制使用手机号做为账号开户
	GPropertyReadOnly(MobileOpen, INT);
	
	// 2013-7-25 - qsc 是否显示任子行照片
	GPropertyReadOnly(IsShowRzxPhoto, INT);

	// 2013-9-11 - qsc 是否启用聚宝接口
	GPropertyReadOnly(EnableJuBao, INT);	
	
	// 2013-11-21- qsc是否允许自定义包时
	GPropertyReadOnly(CannotSelfBundleTime, INT);	

	// 2013-11-21- qsc启用A4纸开户
	GPropertyReadOnly(IsPrintA4, INT);	
	
	// 2013-10-25 - qsc 是否启用截断身份证后12作为账号
	GPropertyReadOnly(EnableAccount12, INT);	

	// 2011/08/10-8201-gxx: 应急模式的启用方式
	GPropertyNeedSet( LocalModeStartKind, INT );
	void SetLocalModeStartKind(INT nKind);

	// 2011/09/07-8201-gxx: 收到客户端余额不足消息后，是否做语音提示
	GPropertyNeedSet( ReminderBalance, BOOL );
	void SetReminderBalance(BOOL newVal);

	GPropertyNeedSet( ReminderAutoReturn, BOOL);
	void SetReminderAutoReturn(BOOL newVal);

	// 2011/12/12-8235-gxx: 是否禁用用户视图右键上机功能
	GPropertyNeedSet( UserRCheckIn, BOOL);
	void SetUserRCheckIn(BOOL newVal);

	GPropertyNeedSet( ForbidCaptureScreen, BOOL);
	void SetForbidCaptureScreen(BOOL newVal);

	// 2012/11/15-8250-gxx: 副收银台是否允许开户的标识
	BOOL AllowOpenUser;
	BOOL GetAllowOpenUser();
	void SetAllowOpenUser(BOOL newVal);


	//软件计费类型
	UINT GetSoftwareType() const;
	void SetSoftwareType(UINT newVal);

	BOOL GetMustSynchronize() const { return m_bMustSynchronize; }
	void SetMustSynchronize(BOOL newVal) { m_bMustSynchronize = newVal; }

	UINT GetRoundType() const { return m_nRoundType; }

	UINT GetICControlVerID() const { return m_nICControlVerID; }

	UINT GetRealNameFrom() const { return m_nRealNameFrom; }
	void SetRealNameFrom(UINT newVal);

	BOOL GetAllowNoCardLogin() const { return m_bAllowNoCardLogin; }
	void SetAllowNoCardLogin(BOOL newVal);

	UINT GetIssueNetBarId() const { return m_bIssueNetBarId; }
	void SetIssueNetBarId(UINT newVal);

	BOOL GetNoIDCardIn() const { return m_bNoIDCardIn; }
	void SetNoIDCardIn(BOOL newVal);

	UINT GetAuditType() const { return m_nAuditType; }
	void SetAuditType(UINT newVal);

	CString GetQueryHost2() const { return m_strQueryHost2; }
	void SetQueryHost2(CString newVal);

	CString GetQueryHost() const { return m_strQueryHost; }
	void SetQueryHost(CString newVal);

	UINT GetForeRealName() const { return m_ForeRealName; }
	void SetForeRealName(UINT newVal);

	UINT GetUpdateInterval() const { return m_nUpdateInterval; }
	void SetUpdateInterval(UINT newVal);

	CString GetDodonewResource() const { return m_strDodonewResource; }
	void SetDodonewResource(CString newVal);

	CString GetDodoNewHost() const { return m_strDodoNewHost; }
	void SetDodoNewHost(CString newVal);

	BOOL GetReturnOnlyClassId() const { return m_bReturnOnlyClassId; }
	void SetReturnOnlyClassId(BOOL newVal);

	BOOL GetBundleTimeWithCash(BOOL bIsMember) const ;
	void SetBundleTimeWithCash(BOOL newVal) { m_nBundleTimeUseBalance = newVal; }

	BOOL GetCreditDirect() const { return m_bCreditDirect; }
	void SetCreditDirect(BOOL newVal) { m_bCreditDirect = newVal; }


	// 存储班次	早班中班晚班
	void SetShiftType(UINT newVal) { m_nShiftType = newVal; }
	UINT GetShitfType()	{ return m_nShiftType; }

	// 2013-12-23 - qsc启用手机号码验证
	void SetTelVerify(UINT newVal) { m_nTelVerify = newVal; }
	UINT GetTelVerify()	{ return m_nTelVerify; }	

public:

	void SetDomainId(UINT nDomainId);
	UINT GetDomainId(){ return m_DomainId; }
	void SetNetBarInfo(UINT nNetBarId, LPCTSTR lpNetBarKey);
	void SetMainHost(CString strMainHost, CString strPath = _T(""));
	void SetBackUpHost(CString strBackUpHost, CString strPath = _T(""));

private :
	//中心控制参数获取
	CStringArray m_strControlOptionArray;//控制参数队列

	INT m_nShenzhen;

	INT m_nMustScan;//强制扫描
	
	INT m_nTempMember;//临时用户
	
	INT m_nNeedActivation;//必须激活
	
	INT m_nUploadImage;//上传照片

	INT m_nSoftSfreg;	// 上传照片-软件版

	INT m_nNoModifyMember;//验证用户信息

	INT m_nRealNamePassword;//是否采用过滤王强制密码

	INT m_nAutoMatchIDCard; //自动匹配身份证号码

	// 1为禁止锁屏，0为允许锁屏
	INT m_nDisallowUnlock;

	INT m_nInputActivation; //输入激活
	
	UINT m_nShiftType;

	UINT m_nTelVerify;
public :

	BOOL GetValue(CString strSrc, LPCTSTR strKey, INT & nValue);
	BOOL GetValue(CString strSrc, LPCTSTR strKey, CString & strValue);

	BOOL PraseControlOption(CString strInfo);

	INT GetShenzhen() const {return m_nShenzhen;}
	INT GetMustScan() const;
	INT GetTempMember() const { return m_nTempMember; }
	INT GetNeedActivation() const { return m_nNeedActivation; }
	INT GetUploadImage() const { return m_nUploadImage; }
	INT GetSoftSfreg()	const { return m_nSoftSfreg; }
	INT GetNoModifyMember() const { return m_nNoModifyMember; }
	INT GetAutoMatchIDCard() const { return m_nAutoMatchIDCard; }
	INT GetInputActivation() const { return m_nInputActivation; }

	INT GetRealNamePassword() const  { return m_nRealNamePassword; }
	void SetRealNamePassword(INT val);

	INT GetDisallowUnlock() const { return m_nDisallowUnlock; }
	void SetDisallowUnlock(INT val);

private:

	CString m_strAgentMobilePhone;//代理商移动电话

	CString m_strAgentName;//代理商名字

	CString m_strAgentregularTel;//代理商固定电话

public:
	//代理商信息
	CString GetAgentName() const { return m_strAgentName; } //代理商名字
	void SetAgentName(CString newVal) { m_strAgentName = newVal; }

	CString GetAgentMobilePhone() const { return m_strAgentMobilePhone; } //代理商移动电话
	void SetAgentMobilePhone(CString newVal) { m_strAgentMobilePhone = newVal; }

	CString GetAgentregularTel() const { return m_strAgentregularTel; } //代理商固定电话
	void SetAgentregularTel(CString newVal) { m_strAgentregularTel = newVal; }

private:

	UINT m_nStoreInAmount; //入库金额

public:

	UINT GetStoreInAmount();
	void SetStoreInAmount(UINT val); 
	
};
