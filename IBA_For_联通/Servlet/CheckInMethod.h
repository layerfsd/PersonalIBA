#pragma once
#include "servletmethod.h"

namespace NS_SERVLET
{

class CCheckInMethod : public CServletMethod
{
public :
	CCheckInMethod(void);
	~CCheckInMethod(void);

protected :

	void AddParams();
	void AddRespFields();
	
	virtual BOOL	OnBegin();							//!< 执行请求前

	CString m_strPwd;

public :
	
	void SetUserPwd(CString newVal);

	/**
	*网吧ID，采用16位整数表示
	*/
	void SetNetbarId(UINT newVal);

	/**
	*包时ID，第一次申请时填写，其他填0
	*/
	void SetTimeId(UINT newVal);

	/**
	*机器标识，限制最长为8个字符
	*/
	void SetTermId(CString newVal);

	/**
	*加密方式 1: DES 2:TEA
	*/
	void SetEncryptMode(UINT newVal);

	/**
	*ID卡号
	*/
	void SetCardSerialNum(CString newVal);

	/**
	*读卡器标识
	*/
	void SetReaderSerialNum(CString newVal);

	/**
	*客户端机器MAC地址
	*/
	void SetMacAddress(CString newVal);

	/**
	*客户端机器IP地址
	*/
	void SetIpAddress(CString newVal);

	/**
	*客户端当前版本
	*/
	void SetClientVersion(CString newVal);

	/**
	*1: 自动登入 0:用户输入账号登入
	*/
	void SetAutoLogin(UINT newVal);


public :

	/**
	*系统参考号, 服务器为每个网吧维护refNo，refNo从0-999999循环递增，可确保当天唯一。
	*/
	UINT GetRefNo();

	/**
	*冻结金额（分）
	*/
	UINT GetFreezeMoney();

	/**
	*单位扣费时间（分钟），普通资费方式有效，多次扣费的包时段方式也可采用，对一次性扣费方式（如包时长）无效
	*/
	UINT GetUnitTime();

	/**
	*下次登入时间
	*/
	UINT GetNextCheckinTime();

	/**
	*累计封顶动态封顶额
	*/
	UINT GetDynamicMaxAmount();

	/**
	*用psk加密pdk后的密文
	*/
	CString GetEpdk();

	/**
	*可用余额（分）
	*/
	UINT GetAvailableBalance();

	/**
	*0-23小时的扣率（分/每小时），
	*/
	UINT GetPolicyInfo();

	/**
	*姓名
	*/
	CString GetUsername();

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
	*0:普通 1:包时段 2:包时长
	*/
	UINT GetPayType();

	/**
	*0:无实名 1:任子行 2:过滤王 3:摩根 4:桑地 5:网盾 6:任子行2008
	*/
	UINT GetAuditType();

	/**
	*服务器当前时间，用于时间同步
	*/
	CString GetServerTime();

	/**
	*用户当前积分
	*/
	UINT GetCurrentPoint();

	/**
	*用户级别
	*/
	UINT GetClassId();

	/**
	*是否会员
	*/
	UINT GetIsMember();

	/**
	*域ID
	*/
	UINT GetDomainId();

	/**
	*支撑主IP端口
	*/
	CString GetMainHost();

	/**
	*支撑备IP端口
	*/
	CString GetBackupHost();

	/**
	*增值IP端口
	*/
	CString GetDodonewHost();

	/**
	*ID卡卡号
	*/
	UINT GetCardId();

	/**
	*ID卡用户密钥
	*/
	CString GetEpsk();

	/**
	*包时类型
	*/
	UINT GetBundleType();

	/**
	*包时开始时间
	*/
	UINT GetBeginBundleTime();

	/**
	*包时结束时间
	*/
	UINT GetEndBundleTime();

	/**
	*用户号
	*/
	UINT GetMemberId();

	/**
	*开户网吧号
	*/
	UINT GetNetBarId();

	/**
	*完整账号
	*/
	CString GetSerialNum();

	/**
	*无送充值余额
	*/
	UINT GetBalance();

	/**
	*1:强制使用探针 0:不强制
	*/
	UINT GetForceAudit();

	/**
	*民族
	*/
	CString GetNation();

	/**
	*电话号码
	*/
	CString GetTel();

	/**
	*用macKey加密的readerPSK, 供简易牛盾用
	*/
	CString GetEReaderPsk();

	/**
	*用macKey加密的dodonewKey，供客户端调用dodonew页面用
	*/
	CString GetEDodonewKey();

	/**
	*当前终端所在机器区域
	*/
	UINT GetPcClass();

	/**
	*0:允许自助下机 1：不允许自助下机
	*/
	UINT GetDisallowLogout();

	/**
	*计费模式:1,2,3,4,5
	*/
	UINT GetPayMode();

	/**
	*上次多扣金额
	*/
	UINT GetLastRemainAmount();

	/**
	*单位扣费金额
	*/
	UINT GetUnitAmount();

	/**
	*最低消费
	*/
	UINT GetMinConsume();

	/**
	*登入后免费使用时间
	*/
	UINT GetBeginFreeTime();

	/**
	*结账免费时间
	*/
	UINT GetNextPayFreeTime();

	/**
	*首次登入时间
	*/
	CString GetFirstCheckinTime();

	/**
	*本次登入前累计消费金额
	*/
	UINT GetConsumeAmount();

	/**
	*本次当入时间(不换机、计费模式不变时这个值一般不变)
	*/
	CString GetCurrCheckinTime();


};

}