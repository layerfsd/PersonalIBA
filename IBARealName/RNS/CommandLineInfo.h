#pragma once

class CCommandLine : public CCommandLineInfo
{
public:
	CCommandLine(void);
	~CCommandLine(void);

	void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	void StartRZX();

	void StopRZX();
	
public:

	UINT m_nDomainId;

	UINT m_nRealNameType;

};
