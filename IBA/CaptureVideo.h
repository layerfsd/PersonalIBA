/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// CaptureVideo.h : header file
#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  

#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#include <Qedit.h>
#include <ImageHlp.h>
#include <afxtempl.h>


#ifndef srelease
#define srelease(x) \
if ( NULL != x ) \
{ \
  x->Release( ); \
  x = NULL; \
}
#endif

class CSampleGrabberCB : public ISampleGrabberCB 
{
public:

	long m_lWidth;

	long m_lHeight;

	BOOL m_bOneShot;

	CSampleGrabberCB();

	STDMETHODIMP_(ULONG) AddRef() { return 2; }

	STDMETHODIMP_(ULONG) Release() { return 1; }

	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);

	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );

	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );

	void SetSaveFilePath(LPCTSTR szPath) { _tcsncpy(m_szFileName, szPath, 4 * MAX_PATH); }
	void GetSaveFilePath(CString& strPath) { strPath = m_szFileName; }
private:

	TCHAR m_szFileName[4 * MAX_PATH];// 位图文件名称

	//创建位图文件
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize );

};

class CCaptureVideo:public CWnd 
{

friend class CSampleGrabberCB;

public:

    void GrabOneFrame(BOOL bGrab);

    HRESULT Init(int iDeviceID,HWND hWnd);

	int EnumDevices(CArray<CString, CString>& arList);

    CCaptureVideo();

    virtual ~CCaptureVideo();

	BOOL IsOpened(){ return m_bOpened; };

	BOOL Stop();

	void SetSaveFilePath(LPCTSTR szPath) { m_CB.SetSaveFilePath(szPath); }
	void GetSaveFilePath(CString& strPath) { m_CB.GetSaveFilePath(strPath); }
private:

	bool m_bOpened;

    HWND m_hWnd;

    IGraphBuilder* m_pGB;

    ICaptureGraphBuilder2* m_pCapture;

    IBaseFilter* m_pBF;

    IMediaControl* m_pMC;//

    IVideoWindow* m_pVW;//

    CComPtr <ISampleGrabber> m_pGrabber;

	CSampleGrabberCB m_CB;

protected:

	void FreeMediaType(AM_MEDIA_TYPE& mt);

	bool BindFilter(int deviceId, IBaseFilter **pFilter);

	void ResizeVideoWindow();

	HRESULT SetupVideoWindow();

	HRESULT InitCaptureGraphBuilder();

};

#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
