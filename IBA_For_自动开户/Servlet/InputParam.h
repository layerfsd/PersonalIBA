#pragma once

#include <afxtempl.h>

namespace NS_SERVLET
{

namespace NS_SERVLET_HELPPER
{

enum EParamType
{
	EPT_STRING, EPT_NUMBER
};

enum EParamLengthType
{
	EPLT_FIXED, EPLT_VARIABLE, EPLT_OWN
};


class CInputParam
{
public :
	CInputParam();
	~CInputParam(void);

public :
	
	UINT GetLength() const;
	void SetLength(UINT newVal);	

	EParamLengthType GetLengthType() const;
	void SetLengthType(EParamLengthType newVal);

	EParamType GetType() const;
	void SetType(EParamType newVal);

	CString GetValue();
	void SetValue(CString newVal);

protected :

	CString m_strValue;

	UINT m_nLength;

	EParamType m_nType;

	EParamLengthType m_nLengthType;
};

class CInputParamList
{

public :
	CInputParamList(void);
	~CInputParamList(void);

	static void HandleError(CString& strError);

public :

	BOOL AddParam(CString strKey, EParamType nType, UINT nLength, EParamLengthType nLengthType = EPLT_FIXED);
	
	void SetValue(CString strKey, CString newVal);
	void SetValue(CString strKey, UINT newVal);
	void SetValue(CString strKey, INT newVal);
	
	void RemoveAll();
	
	CString GetValue(CString strKey);
	CString GetAllValue();

protected :

	BOOL AddParam(CString strKey, CInputParam & inputParam);

	INT GetIndexByKey(CString strKey);

protected :

	CArray<CInputParam, CInputParam&> m_ParamList;
	CArray<CString, CString&> m_KeyList;
};

}

}