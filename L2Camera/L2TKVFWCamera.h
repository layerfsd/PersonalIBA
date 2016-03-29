#pragma once

#include "l2tkcamera.h"

class L2_EXT_CLASS CL2TKVFWCamera : public CL2TKCamera
{
public:

	CL2TKVFWCamera(void);

	virtual ~CL2TKVFWCamera(void);

	virtual BOOL Start(const UINT nDeviceID = 0);

	virtual BOOL Stop();

	virtual BOOL TakePicture(LPCTSTR lpszFile, DWORD nTimeOut = 5 * 1000);

	static  BOOL ShootPicture(UINT nDeviceID, LPCTSTR lpszFile);

protected:

	virtual void ResizeVideoWindow();

	HWND m_hWndCap;
};
