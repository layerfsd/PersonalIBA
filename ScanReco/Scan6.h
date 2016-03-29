// Scan0.h: interface for the CScan6 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCAN0_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
#define AFX_SCAN0_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"
#include "6/CardAll.h"
#include "HX_FDX3CardReader.h"

//矽感
class CScan6   : public IScan, public CHX_FDX3CardReader
{
public:
	virtual bool IsDeviceValid();										//设备是否有效, 此处可以检查参数及环境
	virtual int OpenDevice();											//打开设备
	virtual int SetCallBack(CallBack pFunc);							//设置回调
	virtual int ScanImage(int idType,char* szImgFile);                  //扫描图片
	virtual int RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage); //识别图片
	virtual int ReadCard(IDENTITY_INFO &info,char* szHeadImage);        //读取二代证
	virtual bool IsSupportReader();										//是否支持二代证读卡器，纯扫描仪不支持二代证读卡器
	virtual bool IsSupportScanner();									//是否支持证件扫描功能，纯二代证读卡器不支持扫描
	virtual bool IsSupportCallBack();									//是否支持回调
	virtual bool IsSupportRecognise(int idType);						//是否支持特定证件识别
	virtual bool IsSupportHeadImage(int idType);						//是否支持特定头像获取
	virtual int CloseDevice();                                          //关闭设备
	virtual void GetErrorMessage(int errCode,char* szErrMsg);           //获取出错信息

	CScan6();
	virtual ~CScan6();

private:
	static CScan6 *m_pThis;

	CallBack m_pfnCallBack;
	HANDLE m_hQuitEvent;
	static VOID CALLBACK CheckButtonTimerProc(HWND hwnd,
		UINT uMsg,
		UINT_PTR idEvent,
		DWORD dwTime
		);

	bool m_bOpened;
	char m_cImageFilePath[MAX_PATH];
	HINSTANCE m_hDll;

	Fun_GetVersionInfo *GetVersionInfo;
	Fun_LoadIdcardLibrary *LoadIdcardLibrary;
	Fun_FreeIdcardLibrary *FreeIdcardLibrary;
	Fun_CalibrateScanner *CalibrateScanner;
	Fun_GetButtonDownType *GetButtonDownType;
	Fun_ScanImageEx *ScanImageEx;
	Fun_ScanImageEx_NoBack *ScanImageEx_NoBack;
	Fun_ScanImageOther *ScanImageOther;
	Fun_FeedBackScanner *FeedBackScanner;
	Fun_RecogIdcardExALL *RecogIdcardExALL;
	Fun_RecogIdcardOnlyALL *RecogIdcardOnlyALL;
	Fun_RecogIdcardRe *RecogIdcardRe;
	Fun_RecogDPOnlyALL *RecogDPOnlyALL;
	Fun_RecogDPALL *RecogDPALL;
	Fun_RecogPassPortOnlyALL *RecogPassPortOnlyALL;
	Fun_RecogPassPortALL *RecogPassPortALL;
	Fun_RecogNewIdcardOnlyALL *RecogNewIdcardOnlyALL;
	Fun_RecogNewIdcardALL *RecogNewIdcardALL;
	Fun_RecogPassPortOnlyALL_Region *RecogPassPortOnlyALL_Region;
	Fun_RecogPassPortALL_Region *RecogPassPortALL_Region;
	Fun_RecogTBZOnlyALL *RecogTBZOnlyALL;
	Fun_RecogTBZALL *RecogTBZALL;
	Fun_RecogFXZOnlyALL *RecogFXZOnlyALL;
	Fun_RecogFXZALL *RecogFXZALL;
	Fun_RecogHXZOnlyALL *RecogHXZOnlyALL;
	Fun_RecogHXZALL *RecogHXZALL;
	Fun_RecogMacaoHKFaceALL *RecogMacaoHKFaceALL;
	Fun_RecogMacaoHKFaceOnlyALL *RecogMacaoHKFaceOnlyALL;
};

#endif // !defined(AFX_SCAN0_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
