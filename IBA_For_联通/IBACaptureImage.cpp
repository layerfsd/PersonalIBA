#include "stdafx.h"
#include ".\ibacaptureimage.h"
#include "IBALog.h"

CIBACaptureImage::CIBACaptureImage(void)
{
	m_nDeviceCount = 0;
	m_bInitOK = false;
}

CIBACaptureImage::~CIBACaptureImage(void)
{
	if (m_bInitOK && m_capVideo.IsOpened())
	{
		m_capVideo.Stop();
	}
}

void EnumDevice(const char* lpstr,void* pParam)
{
	CIBACaptureImage* pCap = (CIBACaptureImage*) pParam;
	pCap->m_nDeviceCount ++;
}

bool CIBACaptureImage::Init(HWND hStaticShow)
{
	m_capVideo.EnumDevices(EnumDevice, this);
	if (m_nDeviceCount == 0)
	{
		MessageBox(NULL, _T("没有一个可用的摄像头设备"), NULL, MB_OK);
		IBA_LOG0(_T("CIBACaptureImage::init...没有可用的摄像头设备"));
		return false;
	}

	bool bOk = false;
	for (int i=0; i < m_nDeviceCount; i++)
	{
		HRESULT hr = m_capVideo.Init(i, hStaticShow); 
		if (SUCCEEDED(hr))
		{
			bOk = true;
			break;
		}
	}
	if (!bOk)
	{
		MessageBox(NULL, _T("没有一个可用的摄像头设备"), NULL, MB_OK);
		IBA_LOG0(_T("CIBACaptureImage::init...打开摄像头设备失败"));
		return false;
	}
	
	m_bInitOK = true;

	m_capVideo.SetBitmapSavePath(CT2A(m_strPath));
	return true;
}

bool CIBACaptureImage::CaptureImage(LPCTSTR lpszName)
{
	if (!m_bInitOK || !m_capVideo.IsOpened())
	{
		IBA_LOG0(_T("CIBACaptureImage::CaptureImage(采集图片失败-未初始化)"));
		return false;
	}
	if (!m_capVideo.CapOneImage(CT2A(lpszName)))
	{
		IBA_LOG0(_T("CIBACaptureImage::CaptureImage(采集图片失败)"));
		return false;
	}
	return true;
}