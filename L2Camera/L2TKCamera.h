#pragma once


// CL2TKCamera

class L2_EXT_CLASS CL2TKCamera : public CWnd
{
	DECLARE_DYNAMIC(CL2TKCamera)
	DECLARE_MESSAGE_MAP()

protected:

	CL2TKCamera();

	virtual ~CL2TKCamera();

	virtual void ResizeVideoWindow() = 0;

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnDestroy();

	BOOL m_bOpened;

public:

	virtual BOOL Start(const UINT nDeviceID = 0) = 0;

	virtual BOOL Stop() = 0;

	virtual BOOL TakePicture(LPCTSTR lpszFile, DWORD nTimeOut = 5 * 1000) = 0;

	BOOL IsOpened();
};


