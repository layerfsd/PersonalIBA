#pragma once


class COperationInfo 
{
public:
	COperationInfo();
	virtual ~COperationInfo();

private:
	//时间
	COleDateTime m_OperationTime;
	//密码，临时用户才有，会员为空
	CString m_strPassWord;
	//姓名
	CString m_strUserName;
	//上网账号
	CString m_strNetId;
	//附加信息(就是除去原先的信息又额外的附加)
	CString m_strDescription;
	//操作类型,0是无此类操作，1为充值，2为退款，3为开户，4为换机
	UINT m_nOperationType;
	//操作金额（退款金额，充值金额, 开户金额）（不含押金）
	UINT m_nOperationAmount;
	//用户号
	UINT m_nMemberId;
	//押金
	UINT m_nDeposit;
	//充值赠送金额
	UINT m_nPresentAmount;
	//退款原账号余额
	UINT m_nAllAmount;
	//是否有包时操作
	BOOL m_bIsBundTime;
	//包时金额
	UINT m_nBundTimeAmount;
	
	BOOL m_bIsMember;

	GProperty(IsResumeData, BOOL); // 是否为续传断网数据时的操作
	GProperty(ClassId, UINT); // 用户级别
	GProperty(TermId, CString); // 终端号
	GProperty(CheckInTime, COleDateTime); // 上机时间
	GProperty(IdNum, CString); // 证件号码
	GProperty(ConsumeMoney, INT); // 消费金额

public:
	COleDateTime GetOperationTime() const { return m_OperationTime; }
	void SetOperationTime(COleDateTime NewValue) { m_OperationTime = NewValue; }

	CString GetPassWord() const { return m_strPassWord; }
	void SetPassWord(CString NewValue) { m_strPassWord = NewValue; }

	CString GetUserName() const { return m_strUserName; }
	void SetUserName(CString NewValue) { m_strUserName = NewValue; }

	CString GetNetId() const { return m_strNetId; }
	void SetNetId(CString NewValue) { m_strNetId = NewValue; }

	CString GetDescription() const { return m_strDescription; }
	void SetDescription(CString val) { m_strDescription = val; }

	UINT GetOperationType() const { return m_nOperationType; }
	void SetOperationType(UINT NewValue) { m_nOperationType = NewValue; } 

	UINT GetOperationAmount() const { return m_nOperationAmount; }
	void SetOperationAmount(UINT NewValue) { m_nOperationAmount = NewValue; } 

	UINT GetDeposit() const { return m_nDeposit; }
	void SetDeposit(UINT NewValue) { m_nDeposit = NewValue; } 

	UINT GetAllAmount() const { return m_nAllAmount; }
	void SetAllAmount(UINT NewValue) { m_nAllAmount = NewValue; } 

	UINT GetMemberId() const { return m_nMemberId; }
	void SetMemberId(UINT NewValue) { m_nMemberId = NewValue; } 

	BOOL GetIsBundTime() const { return m_bIsBundTime; }
	void SetIsBundTime(BOOL NewValue) { m_bIsBundTime = NewValue; } 

	UINT GetBundTimeAmount() const { return m_nBundTimeAmount; }
	void SetBundTimeAmount(UINT NewValue) { m_nBundTimeAmount = NewValue; } 

	UINT GetPresentAmount() const { return m_nPresentAmount; }
	void SetPresentAmount(UINT NewValue) { m_nPresentAmount = NewValue; } 

	BOOL GetIsMember() const { return m_bIsMember; }
	void SetIsMember(BOOL newVal) { m_bIsMember = newVal; }
};


