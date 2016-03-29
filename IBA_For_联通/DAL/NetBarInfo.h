#pragma once

namespace NS_DAL
{

class CNetBarInfo
{
public:
	CNetBarInfo(void);
	~CNetBarInfo(void);

private :
	
	CString m_strName;

	CString m_strEnglistName;

	CString m_strAddress;

	CString m_Manager;
	
	CString m_strOpenDate;

	CString m_strParentNetBar;
	
	CString m_strTel;

	UINT m_nPCNumber;

	UINT m_nNetBarID;

	UINT m_nNetBarType;

public :

	UINT GetNetBarID() const { return m_nNetBarID; }
	void SetNetBarID(UINT newVal) { m_nNetBarID = newVal; }

	UINT GetPCNumber() const { return m_nPCNumber; }
	void SetPCNumber(UINT newVal) { m_nPCNumber = newVal; }

	CString GetParentNetBar() const { return m_strParentNetBar; }
	void SetParentNetBar(CString newVal) { m_strParentNetBar = newVal; }

	CString GetOpenDate() const { return m_strOpenDate; }
	void SetOpenDate(CString newVal) { m_strOpenDate = newVal; }

	CString GetManager() const { return m_Manager; }
	void SetManager(CString newVal) { m_Manager = newVal; }

	CString GetAddress() const { return m_strAddress; }
	void SetAddress(CString newVal) { m_strAddress = newVal; }

	CString GetEnglistName() const { return m_strEnglistName; }
	void SetEnglistName(CString newVal) { m_strEnglistName = newVal; }

	CString GetName() const { return m_strName; }
	void SetName(CString newVal) { m_strName = newVal; }

	CString GetTel() const { return m_strTel; }
	void SetTel(CString newVal) { m_strTel = newVal; }

	UINT GetNetBarType() const { return m_nNetBarType; }
	void SetNetBarType(UINT newVal) { m_nNetBarType = newVal; }
};

typedef CArray<CNetBarInfo, CNetBarInfo&> CNetBarInfoArray;

}