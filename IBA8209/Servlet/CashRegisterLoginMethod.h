#pragma once
#include "servletmethod.h"
#include "UserClassInfo.h"

namespace NS_SERVLET
{

class CCashRegisterLoginMethod : public CServletMethod
{
public :
	CCashRegisterLoginMethod(void);
	~CCashRegisterLoginMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual BOOL DecodeResp(CString strResp);		//!< 解析返回值，可重写

	CUserClassInfoArray m_UserClassInfoArray;

public :
	
	/**
	*当前收银台ID
	*/
	void SetCashRegisterId(UINT newVal);

	/**
	*收银台类型 0: 正常版本 1:无本地数据库版本
	*/
	void SetCashRegisterType(UINT newVal);

	/**
	*用户名称
	*/
	void SetUserId(CString newVal);

	/**
	*UNDES加密
	*/
	void SetPassword(CString newVal);

	/**
	*收银台版本号
	*/
	void SetCashRegisterVersion(CString newVal);

	void SetTimeChuo(UINT nTime);

	void SetCheckCode(CString strCheckCode);

	// 机器名，截取前面15位
	void SetPCName(CString strCheckCode);
public :

	/**
	*网吧域ID
	*/
	UINT GetDomainId();

	/**
	*中心主IP端口
	*/
	CString GetMainHost();

	/**
	*中心备份IP端口
	*/
	CString GetBackupHost();

	/**
	*用户级别个数
	*/
	UINT GetClassCount();

	CString GetClassInfo();

	/**
	*仅退款用户ID
	*/
	UINT GetReturnOnlyClassId();

	/**
	*DDN中心地址
	*/
	CString GetDodoNewHost();

	/**
	*DDN资源服务器地址
	*/
	CString GetDodonewResource();

	/**
	*资源更新间隔
	*/
	UINT GetUpdateInterval();

	/**
	*中心实名要求,0:不要求实名,1:允许多张卡,2:用户级别唯一,3:证件号码唯一,4:身份证且唯一,5:会员身份证且唯一
	*/
	UINT GetForeRealName();

	/**
	*当前班次ID，为0表示未上班，或班次过期(超过1个月).
	*/
	UINT GetDutyId();

	/**
	*上班时间，dutyId=0无意义
	*/
	CString GetOndutyTime();

	/**
	*系统查询机地址端口
	*/
	CString GetQueryHost();

	/**
	*备用系统查询机地址端口
	*/
	CString GetQueryHost2();

	/**
	*0:无实名 1:任子行 2:过滤王 3:摩根 4:桑地 5:网盾 6.任子行2008
	*/
	UINT GetAuditType();

	/**
	*弹出广告地址
	*/
	CString GetPopupURL();

	/**
	*针对ID卡系统有效,1:使用刷卡上机模式,0:使用插卡上机模式
	*/
	UINT GetNoIDCardIn();

	/**
	*可用网吧ID
	*/
	UINT GetIssueNetBarId();

	/**
	*中心系统时间
	*/
	CString GetServerTime();

	/**
	*1: 允许无卡上机 0:不允许
	*/
	UINT GetAllowNoCardLogin();

	/**
	*0: 不限制，1:强制扫描 2:实名插件
	*/
	UINT GetRealNameFrom();

	/**
	*收银台选项参数,多个参数用分号分隔
	*/
	CString GetControlOption();

	/**
	*1:启用IC卡永久绑定实名
	*/
	UINT GetCardRealName();

	/**
	*收银员权限，格式:[权限名称,权限名称…]
	*/
	CString GetUserPrivileges();

	/**
	*班次类型 1:早班 2:中班 3:晚班，没有上班则返回0
	*/
	UINT GetShiftType();

	/**
	*代理班值手机
	*/
	CString GetAgentMobilePhone();

	/**
	*代理固定电话
	*/
	CString GetAgentRegularTel();

	/**
	*服务代理名称
	*/
	CString GetAgentName();

	/**
	*
	软件类型 
	0：IC卡
	1: ID卡
	2: 牛盾V1
	3：软件
	4: 牛盾V2
	*/
	UINT GetSoftwareType();

	void GetUserClassInfoArray(CUserClassInfoArray & UserClassInfoArray);

	/**
	*取得通报内容
	*/
	CString GetNoticeInfo();


	/**
	*异地备份服务器地址
	*/
	CString GetStandbyHost();


	/**
	*是否允许账户余额进行足额预扣包时, 1：允许 0：不允许
	*/
	UINT GetBundleTimeUseBalance();


	/**
	*自动同步
	*/
	UINT GetMustSynchronize();


	/**
	*允许充值为负数
	*/
	UINT GetAllowCreditNegative();
};

}