#pragma once

class CCaptureVideo;
class CIBACaptureImage
{
public:
	CIBACaptureImage(void);
	~CIBACaptureImage(void);

	bool Init(HWND hStaticShow, int nIndex = -1);

	// 设置图像的保存路径
	void SetImagesPath(LPCTSTR lpszPath);
	
	// 拍摄一张照片
	bool CaptureImage(LPCTSTR lpszName);
	bool GetCapArray(CArray<CString, CString> &arCapList);
	void Stop();
private:
	CCaptureVideo* m_pCap;
	CString m_strBmpFilePath1;
	CString m_strBmpFilePath2;
public:
	int m_nDeviceCount;
	bool m_bInitOK;
	CString m_strPath; 
};
