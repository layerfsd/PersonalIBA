#pragma once

#include "CaptureVideo//CapVideoInf.h"
class CIBACaptureImage
{
public:
	CIBACaptureImage(void);
	~CIBACaptureImage(void);

	bool Init(HWND hStaticShow);

	// 设置图像的保存路径
	void SetImagesPath(LPCTSTR lpszPath) { m_strPath = lpszPath; }
	
	// 拍摄一张照片
	bool CaptureImage(LPCTSTR lpszName);

private:
	CCapVideoInf m_capVideo;

public:
	int m_nDeviceCount;
	bool m_bInitOK;
	CString m_strPath; 
};
