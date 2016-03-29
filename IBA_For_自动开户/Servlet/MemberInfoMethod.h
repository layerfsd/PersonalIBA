#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CMemberInfo
{
public :

	CMemberInfo();

	/**
	*用户开户网吧
	*/
	UINT GetNetBarId();

	/**
	*用户ID
	*/
	UINT GetMemberId();

	/**
	*用户状态,0正常1挂失 2注销
	*/
	UINT GetAvailable();

	/**
	*
	*/
	UINT GetCardId();

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
	*发证单位
	*/
	CString GetIdDepart();

	/**
	*ISO3166
	*/
	UINT GetCountry();

	/**
	*卡序列号
	*/
	CString GetSerailNum();

	/**
	*任子行接口身份ID
	*/
	CString GetRzxId();

	/**
	*过滤王接口身份ID
	*/
	CString GetGlwId();

	/**
	*摩根接口身份ID
	*/
	CString GetMgId();

	/**
	*其他接口身份ID
	*/
	CString GetOtherId();

	/**
	*充值账户可用余额
	*/
	UINT GetBalance();

	/**
	*用户地址
	*/
	CString GetAddress();

	/**
	*EMAIL地址
	*/
	CString GetEmail();

	/**
	*联系电话
	*/
	CString GetTel();

	/**
	*工作
	*/
	CString GetJob();

	/**
	*用户当前积分
	*/
	UINT GetCurrentPort();

protected :

	CRespFieldList  m_RespFields;	//!< 返回值列表
	
	friend class CMemberInfoMethod;
};

class CMemberInfoMethod : public CServletMethod
{
public :
	CMemberInfoMethod(void);
	virtual ~CMemberInfoMethod(void);

protected :

	void AddParams();
	void AddRespFields();

	virtual CString GetMethodName();
	virtual BOOL    DecodeResp(CString strResp);		//!< 解析返回值，可重写

	CArray<CMemberInfo, CMemberInfo> m_MemberInfos;

public :
	
	/**
	*会员ID，读卡时填写
	*/
	void SetMemberId(UINT newVal);

	/**
	*业务发生网吧
	*/
	void SetNetBarId(UINT newVal);

	/**
	*用户完整登录名，可以不填
	*/
	void SetMemberName(CString newVal);

	/**
	*用户完整姓名，可以不填
	*/
	void SetUsername(CString newVal);

	/**
	*用户完整的证件号码，可以不填
	*/
	void SetPersonalId(CString newVal);

	/**
	*卡序列号
	*/
	void SetSerialNum(CString newVal);

	/**
	*当服务器查询到的信息有多条时，只返回用户号大于该值的用户
	*/
	void SetMinMemberId(UINT newVal);

	/**
	*内部卡号
	*/
	void SetCardId(UINT newVal);

	/**
	*是否进行舍入(0：不舍入 1:舍入)
	*/
	void SetRound(UINT newVal);

	/**
	*0:全部，1:正常 2:已挂失 3:已退款
	*/
	void SetAvailable(UINT newVal);


public :

	/**
	*用户信息条数，
	*/
	UINT GetMemberCount();

	/**
	*用户开户网吧
	*/
	UINT GetNetBarId(INT nIndex);

	/**
	*用户ID
	*/
	UINT GetMemberId(INT nIndex);

	/**
	*用户状态,0正常1挂失 2注销
	*/
	UINT GetAvailable(INT nIndex);

	/**
	*
	*/
	UINT GetCardId(INT nIndex);

	/**
	*用户级别，0表示临时用户
	*/
	UINT GetUserClass(INT nIndex);

	/**
	*押金
	*/
	UINT GetGuarantyMoney(INT nIndex);

	/**
	*用户结算网吧
	*/
	UINT GetAccountNetbarId(INT nIndex);

	/**
	*用户登录名
	*/
	CString GetMemberName(INT nIndex);

	/**
	*姓名
	*/
	CString GetUserName(INT nIndex);

	/**
	*0：man 1：woman
	*/
	UINT GetSex(INT nIndex);

	/**
	*证件号码
	*/
	CString GetPersonalId(INT nIndex);

	/**
	*证件类型ID
	*/
	UINT GetIdType(INT nIndex);

	/**
	*发证单位
	*/
	CString GetIdDepart(INT nIndex);

	/**
	*ISO3166
	*/
	UINT GetCountry(INT nIndex);

	/**
	*卡序列号
	*/
	CString GetSerailNum(INT nIndex);

	/**
	*任子行接口身份ID
	*/
	CString GetRzxId(INT nIndex);

	/**
	*过滤王接口身份ID
	*/
	CString GetGlwId(INT nIndex);

	/**
	*摩根接口身份ID
	*/
	CString GetMgId(INT nIndex);

	/**
	*其他接口身份ID
	*/
	CString GetOtherId(INT nIndex);

	/**
	*充值账户可用余额
	*/
	UINT GetBalance(INT nIndex);

	/**
	*用户地址
	*/
	CString GetAddress(INT nIndex);

	/**
	*EMAIL地址
	*/
	CString GetEmail(INT nIndex);

	/**
	*联系电话
	*/
	CString GetTel(INT nIndex);

	/**
	*工作
	*/
	CString GetJob(INT nIndex);

	/**
	*用户当前积分
	*/
	UINT GetCurrentPort(INT nIndex);

};


}