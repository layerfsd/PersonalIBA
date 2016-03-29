#pragma once

#include "InputParam.h"

namespace NS_SERVLET
{

namespace NS_SERVLET_HELPPER
{

class CRespField
{
public:
	CRespField(void);
	~CRespField(void);

public :

	UINT GetLength() const;
	void SetLength(UINT newVal);	

	BOOL GetFixedLength() const;
	void SetFixedLength(BOOL newVal);

	EParamType GetType() const;
	void SetType(EParamType newVal);

	CString GetValue();
	void SetValue(CString newVal);

	BOOL GetLoopBody() const;
	void SetLoopBody(BOOL newVal);

protected :

	CString m_strValue;

	UINT m_nLength;

	EParamType m_nType;

	BOOL m_bFixedLength;

	BOOL m_bLoopBody;

};

class CRespFieldList
{

public :
	CRespFieldList(void);
	~CRespFieldList(void);

public :

	BOOL AddRespField(CString strKey, EParamType nType, UINT nLength, BOOL bFixedLength = TRUE, BOOL bLoopBody = FALSE);
	
	void SetValue(CString strKey, CString newVal);

	void RemoveAll();
	void ResetAllValue();

	CString GetValueAsString(CString strKey);
	UINT GetValueAsUINT(CString strKey);
	INT GetValueAsINT(CString strKey);

	BOOL DecodeResp(CString strResp);

	UINT GetAllLength() const { return m_nAllLength; }

	CArray<CRespField, CRespField&> m_RespFieldList;

protected :

	BOOL AddRespField(CString strKey, CRespField & respField);

	INT GetIndexByKey(CString strKey);

protected :

	CArray<CString, CString&> m_KeyList;

	UINT m_nAllLength;
	
};

}

}