#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CBalanceMethod : public CServletMethod
{
public :
	CBalanceMethod(void);
	~CBalanceMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual CString GetMethodName();

	UINT m_nMemberId;
	
public :
	
	UINT GetMemberId() const { return m_nMemberId; }

	/**
	*用户号
	*/
	void SetMemberId(UINT newVal);

	/**
	*网吧编号
	*/
	void SetNetBarId(UINT newVal);

	/**
	*内部卡号
	*/
	void SetCardId(UINT newVal);

	/**
	*是否进行舍入(0:不舍入 1:舍入)
	*/
	void SetRound(UINT newVal);

	//2011/3/11//gxx-是否实际在线
	void SetOnlineInFact(UINT newVal);


	UINT GetCardId();

public :

	/**
	*1：在线 2：不在线
	*/
	UINT GetOnline();

	/**
	*充值账户可用余额
	*/
	INT GetBalance();

	/**
	*用户在线网吧
	*/
	UINT GetOnlineNetBarId();

	/**
	*用户在线终端流水
	*/
	UINT GetOnlineSerialNo();

	/**
	*用户预扣金额
	*/
	UINT GetOnlineAmount();

	/**
	*存在赠送的网吧数，固定为1
	*/
	UINT GetNetBarCount();

	/**
	*存在赠送的网吧ID(当前网吧ID)
	*/
	UINT GetNetBarId();

	/**
	*可用“赠送账户”余额
	*/
	INT GetCreditPresent();

	/**
	*可用“有送充值”余额
	*/
	INT GetUsePresent();

	/**
	*用户级别，0表示临时用户
	*/
	UINT GetUserClass();

	/**
	*押金
	*/
	UINT GetGuarantyMoney();

	/**
	*用户结算网吧
	*/
	UINT GetAccountNetbarId();

	/**
	*用户登录名
	*/
	CString GetMemberName();

	/**
	*姓名
	*/
	CString GetUserName();

	/**
	*0：man 1：woman
	*/
	UINT GetSex();

	/**
	*证件号码
	*/
	CString GetPersonalId();

	/**
	*证件类型ID
	*/
	UINT GetIdType();

	/**
	*卡序列号
	*/
	CString GetSerailNum();

	/**
	*用户当前积分
	*/
	UINT GetCurrentPoint();

	/**
	*无送充值余额
	*/
	INT GetCashBalance();

	/**
	*应退余额
	*/
	INT GetReturnAmount();

	/**
	*=1 重点用户
	*/
	UINT GetImportantMember();

	/**
	*在线的终端号
	*/
	CString GetTermId();

	/**
	*实名ID
	*/
	CString GetAuditId();

	/**
	*总积分
	*/
	UINT GetTotalPoint();

	/**
	*是否嘟嘟牛会员 1: 是 0: 否
	*/
	UINT GetIsMember();

	/**
	*是否禁止上机 1: 是 0:否
	*/
	UINT GetDisallowLogin();

	/**
	*yyyymmddhhmmss
	*/
	CString GetLogoutTime();

	/**
	*用户PSK
	*/
	CString GetEpsk();

	/**
	*消费类型
	*/
	UINT GetPayType();

	/**
	*包时开始时间
	*/
	CString GetStartTime();

	/**
	*包时结束时间
	*/
	CString GetEndTime();

	/**
	*包时金额
	*/
	UINT GetPrice();

	/**
	*开户网吧
	*/
	UINT GetOpenNetbarId();

	/**
	*0:不允许 1:允许 2:只退押金
	*/
	UINT GetAllowReturn();

	/**
	*0:允许自助下机 1:不允许自助下机
	*/
	UINT GetDisallowLogout();

	/**
	*上次消费时长(秒)，在线值为0
	*/
	UINT GetLastConsumeTime();

	/**
	*上次消费金额(分)，在线值为0
	*/
	INT GetLastConsumeAmount();

	/**
	*帐户密码
	*/
	CString GetPassword();
	
	/**
	*1:需要激活 0:不需激活
	*/
	UINT GetNeedActivation();

	/**
	*总金额
	*/
	INT GetTotalBalacne();


	/**
	*服务器时间
	*/
	CString GetServerTime();

	/**
	*是否为网吧会员
	*/
	BOOL IsNetbarMember();
};

}