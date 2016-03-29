
#pragma once

#include <dshow.h>
#include <Qedit.h>
#include <afxmt.h>
#include "L2TKCamera.h"

class L2_EXT_CLASS CL2TKDirectShowCamera : public CL2TKCamera
{
public :

	CL2TKDirectShowCamera();

	virtual ~CL2TKDirectShowCamera();
	
	virtual BOOL Start(const UINT nDeviceID = 0);

	virtual BOOL Stop();

	virtual BOOL TakePicture(LPCTSTR lpszFile, DWORD nTimeOut = 5 * 1000);
	
	static BOOL ShootPicture(UINT nDeviceID, LPCTSTR lpszFile, DWORD nTimeOut = 5 * 1000);
	
	static INT EnumDevices(CStringArray& Devices);

protected:

	virtual void ResizeVideoWindow();

private:

    IGraphBuilder *m_pGB;

    ICaptureGraphBuilder2* m_pCapture;

    IBaseFilter* m_pBF;

    IMediaControl* m_pMC;

    IVideoWindow* m_pVW;

    CComPtr<ISampleGrabber> m_pGrabber;

	BOOL InitCaptureGraphBuilder();

	BOOL BindFilter(const UINT nDeviceID, IBaseFilter **pFilter);
	
	void SetupVideoWindow();
	
	void FreeMediaType(AM_MEDIA_TYPE& mt);
	
private :

	class CSampleGrabberCB : public ISampleGrabberCB 
	{
	public:

		CSampleGrabberCB(); 

		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
		STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample );
		STDMETHODIMP BufferCB(double dblSampleTime, BYTE* pBuffer, long lBufferSize );

		LONG m_lWidth;

		LONG m_lHeight;

		BOOL m_bOneShot;

		CString m_strFileName;

		CEvent m_CapEvent;

		BOOL m_bPhotoResult;

	private:

		BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize);
	};

	CSampleGrabberCB m_CB;
	
};
