#pragma once

class CCreditCard
{
public:
	CCreditCard(void);
	~CCreditCard(void);

	BOOL NextCreditCard();

	BOOL DeleteCreditCard();

	INT GetCount();

private :

	BOOL LoadFile();

	BOOL SaveFile();

	CStringArray m_StringArray;

	INT m_nCurPos;

	UINT m_nType;

	UINT m_nType2;

	CString m_strId;

	CString m_strPwd;

	UINT m_nMoney;

	COleDateTime m_Date;

public:
	
	UINT GetCreditCardType();

	CString GetCreditCardID();

	CString GetCreditCardPassword();

	UINT GeCreditCardMoney(); 
	
	UINT GeCreditCardFun(); 

	BOOL IsCreditCardValid();
};
