#pragma once

#include <atlimage.h>

class CImageEx : public CImage
{
public:

	CImageEx(void);
	virtual ~CImageEx(void);

public:

	BOOL LoadFromOtherResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR lpszResType);

	void DoForPNG();

	HRESULT SaveAsJpeg(LPCTSTR lpszFileName, ULONG Quality = 100);
};
