//-------------------------------------------------------------------
// CCaptureVideo视频捕捉类实现文件CaptureVideo.cpp
//-------------------------------------------------------------------
// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <ImageHlp.h>
#include "CaptureVideo.h"
#include <afxmt.h>
#include "IBA_DEBUG.h"
#include "IBALog.h"
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Quartz.lib")
CEvent g_eCapEvent;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleGrabberCB::CSampleGrabberCB( )
{
	lstrcpy(m_szFileName, _T("c:\\jubao.bmp"));
	m_bOneShot = FALSE;
} 

STDMETHODIMP CSampleGrabberCB::QueryInterface(REFIID riid, void ** ppv)
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
	{ 
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CSampleGrabberCB::BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
{
	if( !m_bOneShot )return 0;

	
	IBA_LOG0(_T("Get snap request!"));
	if (!pBuffer)return E_POINTER;

	IBA_LOG0(_T("Start save bitmap!"));

	SaveBitmap(pBuffer, lBufferSize);

	m_bOneShot = FALSE;

	return 0;
}


BOOL CSampleGrabberCB::SaveBitmap(BYTE * pBuffer, long lBufferSize )
{
	HANDLE hf = CreateFile(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, NULL, NULL);

	if( hf == INVALID_HANDLE_VALUE )
	{
		DWORD dwError = GetLastError();

		CString strMsg;

		strMsg.Format(_T("Create bitmap file failed ErrorCode:%d"),dwError);

//		OutputEx(LPCTSTR(strMsg));

		return 0;
	}

	try
	{

		// 写文件头 
		BITMAPFILEHEADER bfh;

		memset( &bfh, 0, sizeof( bfh ) );

		bfh.bfType ='MB';

		bfh.bfSize = sizeof( bfh ) + lBufferSize + sizeof( BITMAPINFOHEADER );

		bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );

		DWORD dwWritten = 0;

		WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );

		// 写位图格式
		BITMAPINFOHEADER bih;

		memset( &bih, 0, sizeof( bih ) );

		bih.biSize = sizeof( bih );

		bih.biWidth = m_lWidth;

		bih.biHeight = m_lHeight;

		bih.biPlanes = 1;

		bih.biBitCount = 24;

		WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );

		// 写位图数据
		WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );

		FlushFileBuffers( hf );
	}
	catch (...)
	{
		IBA_LOG0(_T("write bitmap exception!"));
	}


	CloseHandle( hf );

	g_eCapEvent.SetEvent();

	return 0;
}

STDMETHODIMP CSampleGrabberCB::SampleCB( double SampleTime, IMediaSample * pSample )
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
////class CCaptureVideo

CCaptureVideo::CCaptureVideo()
{
	//COM Library Intialization
	if(FAILED(CoInitialize(NULL))) /*, COINIT_APARTMENTTHREADED)))*/
	{
		AfxMessageBox(_T("CoInitialize Failed!\r\n"));

		return;
	}

	m_bOpened = FALSE;

	m_hWnd = NULL;

	m_pVW = NULL;

	m_pMC = NULL;

	m_pGB = NULL;

	m_pCapture = NULL; 
}
CCaptureVideo::~CCaptureVideo()
{
	// Stop media playback
	if(m_pMC)m_pMC->Stop();
	if(m_pVW)
	{
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}


	//srelease(m_pCapture);

	//srelease(m_pMC);

	//srelease(m_pGB);

	//srelease(m_pBF);


	CoUninitialize( );
}

int CCaptureVideo::EnumDevices(CArray<CString, CString>& arList)
{
	int nCount = 0;
	//枚举视频扑捉设备
	ICreateDevEnum *pCreateDevEnum;

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	
	if (hr != NOERROR)return -1;
	
	CComPtr<IEnumMoniker> pEm;
	
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	
	if (hr != NOERROR)return -1;
	
	pEm->Reset();
	
	ULONG cFetched;
	
	IMoniker *pM;
	
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;

		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);

		if(SUCCEEDED(hr)) 
		{
			VARIANT var;

			var.vt = VT_BSTR;

			hr = pBag->Read(L"FriendlyName", &var, NULL);

			if (hr == NOERROR) 
			{
				char szStr[2048];

				nCount++;

				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, szStr, 2048, NULL, NULL);
				
				arList.Add( CString(szStr) );
				
				SysFreeString(var.bstrVal);
				
				pM->AddRef();

			}

			pBag->Release();
		}

		pM->Release();
	}

	return nCount;
}

HRESULT CCaptureVideo::Init(int iDeviceID, HWND hWnd)
{
	HRESULT hr;

	hr = InitCaptureGraphBuilder();

	if (FAILED(hr))
	{
		IBA_LOG0(_T("Failed to get video interfaces!"));
		return hr;
	}

	// Bind Device Filter. We know the device because the id was passed in
	if(!BindFilter(iDeviceID, &m_pBF))
		return S_FALSE;
	
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");

	m_pGrabber = NULL;

	hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber );

	if( !m_pGrabber )
	{
		IBA_LOG0(_T("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
		return hr;
	}

	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );
	
	//设置视频格式
	AM_MEDIA_TYPE mt;
	
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = m_pGrabber->SetMediaType(&mt);

	if( FAILED( hr ) )
	{
		IBA_LOG0(_T("Fail to set media type!"));
		return hr;
	}
	hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
	
	if( FAILED( hr ) )
	{
		IBA_LOG0(_T("Fail to put sample grabber in graph"));
		return hr;
	}

	// try to render preview/capture pin
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	
	if( FAILED( hr ) )
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);

	if( FAILED( hr ) )
	{
//		OutputEx(ToString("Can’t build the graph"));
		return hr;
	}

	hr = m_pGrabber->GetConnectedMediaType( &mt );

	if ( FAILED( hr) )
	{
		IBA_LOG0(_T("Failt to read the connected media type"));
		return hr;
	}

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;

	m_CB.m_lWidth = vih->bmiHeader.biWidth;

	m_CB.m_lHeight = vih->bmiHeader.biHeight;

	FreeMediaType(mt);

	hr = m_pGrabber->SetBufferSamples( FALSE );

	hr = m_pGrabber->SetOneShot( FALSE );

	hr = m_pGrabber->SetCallback( &m_CB, 1 );

	//设置视频捕捉窗口
	m_hWnd = hWnd ;

	SetupVideoWindow();

	hr = m_pMC->Run();//开始视频捕捉

	if(FAILED(hr))
	{
		IBA_LOG0(_T("Couldn’t run the graph!"));
		return hr;
	}

	m_bOpened = true;

	return S_OK;
}

BOOL CCaptureVideo::Stop()
{
	m_bOpened = false;

	if (m_pMC)
	{
		m_pMC->Stop();;
		m_pMC->Release();
		m_pMC = NULL;
	}

	if(m_pVW)
	{
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
		m_pVW->Release();
		m_pVW = NULL;
	}

	srelease(m_pCapture);
//	srelease(m_pBF);
	m_pGrabber = NULL;
	srelease(m_pGB);

	return TRUE;
}

bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0) return false;

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
	IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	if (hr != NOERROR) return false;

	CComPtr<IEnumMoniker> pEm;

	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);

	if (hr != NOERROR) return false;

	pEm->Reset();

	ULONG cFetched;

	IMoniker* pM;

	int index = 0;

	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
	{
		IPropertyBag* pBag;

		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);

		if(SUCCEEDED(hr)) 
		{
			VARIANT var;

			var.vt = VT_BSTR;

			hr = pBag->Read(L"FriendlyName", &var, NULL);

			if (hr == NOERROR) 
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}

			pBag->Release();

		}

		pM->Release();

		index++;
	}
	return true;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;

	// 创建IGraphBuilder接口
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
								IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))return hr;

	m_pCapture->SetFiltergraph(m_pGB);

	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

	if (FAILED(hr))return hr;

	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);

	if (FAILED(hr))return hr;

	return hr;
}
HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;

	hr = m_pVW->put_Owner((OAHWND)m_hWnd);

	if (FAILED(hr))return hr;

	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);

	if (FAILED(hr))return hr;

	ResizeVideoWindow();

	hr = m_pVW->put_Visible(OATRUE);

	return hr;
}
void CCaptureVideo::ResizeVideoWindow()
{
	if (m_pVW)
	{
		//让图像充满整个窗口
		CRect rc;

		::GetClientRect(m_hWnd,&rc);

		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	} 
}
void CCaptureVideo::GrabOneFrame(BOOL bGrab)
{
	m_CB.m_bOneShot = bGrab;
}
void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) 
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
} 