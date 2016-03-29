#pragma once


class CUserClassInfo
{
public:
	CUserClassInfo(void);
	~CUserClassInfo(void);

private:

	UINT m_ClassID;
	CString m_strClassName;
	UINT m_CostExpense;
	UINT m_Deposit;
	BOOL m_bIsMember;

public:

	void SetClassName(CString newVal);
	void SetCostExpense(UINT newVal);
	void SetDeposit(UINT newVal);
	void SetIsMember(BOOL newVal);
	void SetClassID(UINT newVal);
	/**
	*用户级别ID
	*/
	UINT GetClassID();

	/**
	*用户级别名称
	*/
	CString GetClassName();

	/**
	*工本费
	*/
	UINT GetCostExpense();

	/**
	*押金
	*/
	UINT GetDeposit();

	/**
	*是否会员
	*/
	BOOL GetIsMember();
};

typedef CArray<CUserClassInfo, CUserClassInfo&> CUserClassInfoArray;

