#include "stdafx.h"
#include ".\ibacaptureimage.h"
#include "IBALog.h"
#include "resource.h"
#include "CaptureVideo.h"
#include <gdiplus.h>
#include "CapImageFunc.h"
#include "IBA.h"
using namespace Gdiplus;
extern CEvent g_eCapEvent;

CIBACaptureImage::CIBACaptureImage(void)
{
	m_pCap = new CCaptureVideo;
	m_nDeviceCount = 0;
	m_bInitOK = false;
}

CIBACaptureImage::~CIBACaptureImage(void)
{
	Stop();
}


bool CIBACaptureImage::Init(HWND hStaticShow, int nIndex)
{
	CArray<CString, CString> arCapList;
	m_pCap->EnumDevices (arCapList);
	m_nDeviceCount = arCapList.GetCount();
	if(0 == m_nDeviceCount)
	{
		IBA_LOG0(_T("没有可用的摄像头设备！"));
		theApp.IBAMsgBox2(_T("没有可用的摄像头设备！"));
		return false;
	}
	
	bool bOk = false;
	if(-1 == nIndex)
	{
		for (int i=0; i < m_nDeviceCount; i++)
		{
			HRESULT hr =  m_pCap->Init(i, hStaticShow); 
			if (SUCCEEDED(hr))
			{
				bOk = true;
				break;
			}
		}
	}
	else
	{		
		HRESULT hr =  m_pCap->Init(nIndex, hStaticShow); 
		if (SUCCEEDED(hr))
		{
			bOk = true;
		}
	}
	
	if (!bOk)
	{
		MessageBox(NULL, LOAD_STRING(IDS_VEDIO_NO_DEVICE), NULL, MB_OK);
		IBA_LOG0(_T("CIBACaptureImage::init...打开摄像头设备失败"));
		return false;
	}
	
	m_bInitOK = true;

	m_pCap->SetSaveFilePath(m_strBmpFilePath1);
	return true;
}

bool CIBACaptureImage::GetCapArray(CArray<CString, CString> &arCapList)
{
	m_pCap->EnumDevices (arCapList);
	if(0 == arCapList.GetCount())
	{
		IBA_LOG0(_T("摄像头初始化失败 摄像头个数为零"));
		return false;
	}
	return true;
}

bool CIBACaptureImage::CaptureImage(LPCTSTR lpszName)
{
	if (!m_bInitOK || !m_pCap->IsOpened())
	{
		IBA_LOG0(_T("CIBACaptureImage::CaptureImage(采集图片失败-未初始化)"));
		return false;
	}

	g_eCapEvent.ResetEvent();
	m_pCap->GrabOneFrame(TRUE);
	if( WaitForSingleObject( g_eCapEvent, 5 * 1000 ) != WAIT_OBJECT_0 )
	{
		IBA_LOG0(_T("设备超时"));
		return false;
	}


	bool bFlag = false;
	//try
	//{
	GdiplusStartupInput gdiplusStartupInput; 
	ULONG_PTR gdiplusToken = NULL;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 

	BMPScale( m_strBmpFilePath2, m_strBmpFilePath1, 320, 240 );

	BMP2JPG( m_strBmpFilePath2, m_strPath + lpszName + _T(".jpg"));

	// 2014-7-7 - qsc
	DeleteFile(m_strBmpFilePath1);
	DeleteFile(m_strBmpFilePath2);

	GdiplusShutdown(gdiplusToken);

	Sleep( 50 );
	bFlag = true;
	//}
	//catch (...)
	//{
	//	bFlag = false;
	//	IBA_LOG0(_T("照片转换成jpg出现错误！"));
	//}
	return bFlag;
}

void CIBACaptureImage::SetImagesPath( LPCTSTR lpszPath )
{
	m_strPath = lpszPath;
	m_strBmpFilePath1 = m_strPath + _T("tmp1.bmp");
	m_strBmpFilePath2 = m_strPath + _T("tmp2.bmp");
}

void CIBACaptureImage::Stop()
{
	if (m_bInitOK && m_pCap->IsOpened())
	{
		m_pCap->Stop();
	}
}
