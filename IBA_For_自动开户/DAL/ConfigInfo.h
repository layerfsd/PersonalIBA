#pragma once


namespace NS_DAL
{

class CConfigInfo
{
public :
	CConfigInfo(void);
	~CConfigInfo(void);

public :

	void Clear();

	UINT GetKeyID() const;
	void SetKeyID(UINT newVal);
	CString GetKeyType() const;
	void SetKeyType(CString newVal);
	CString GetKeyName() const;
	void SetKeyName(CString newVal);
	CString GetKeyString() const;
	void SetKeyString(CString newVal);
	UINT GetKeyValue1() const;
	void SetKeyValue1(UINT newVal);
	UINT GetKeyValue2() const;
	void SetKeyValue2(UINT newVal);
	CString GetKeyValue3() const;
	void SetKeyValue3(CString newVal);
	CString GetDescription() const;
	void SetDescription(CString newVal);

private :
	
	UINT m_KeyID;

	CString m_KeyType;

	CString m_KeyName;

	CString m_KeyString;

	UINT m_KeyValue1;

	UINT m_KeyValue2;

	CString m_KeyValue3;

	CString m_Description;

};

typedef CArray<CConfigInfo, CConfigInfo&> CSystemConfigArray;

}
