#pragma once

namespace NS_SERVLET
{

class CLastTranInfo
{
public:
	CLastTranInfo(void);
	~CLastTranInfo(void);

public :

	CString GetLastTranTime() const { return m_strLastTranTime; }
	void SetLastTranTime(CString newVal) { m_strLastTranTime = newVal; }

	UINT GetLastTranType() const { return m_nLastTranType; }
	void SetLastTranType(UINT newVal) { m_nLastTranType = newVal; }

	UINT GetLastRefNo() const { return m_nLastRefNo; }
	void SetLastRefNo(UINT newVal) { m_nLastRefNo = newVal; }

	void Load();

	void Save();

private :

	CString m_strLastTranTime;

	UINT m_nLastTranType;

	UINT m_nLastRefNo;

};

}
