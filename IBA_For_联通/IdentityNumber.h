#pragma once

class CIdentityNumber
{
public:
	CIdentityNumber(LPCTSTR lpszIdentityNumber);
	~CIdentityNumber(void);

private:

	UINT m_nProvince;

	UINT m_nCity;

	UINT m_nCounty;

	UINT m_nSex;

	UINT m_nAge;

	COleDateTime m_BirthDay;

	CString m_strIdentityNumber;

public:
	BOOL IsBirthDay();

public:

	UINT GetProvince() const { return m_nProvince; }
	UINT GetCounty() const { return m_nCounty; }
	UINT GetCity() const { return m_nCity; }
	UINT GetSex() const { return m_nSex; }
	UINT GetAge() const { return m_nAge; }

public:

	void Attach(LPCTSTR lpszIdentityNumber);

	BOOL IsValid();

	TCHAR MakeVerifyCode();
};
