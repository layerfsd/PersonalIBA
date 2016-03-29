// Scan3.h: interface for the CScan3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Scan3_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
#define AFX_Scan3_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"
#include "3/CardAll.h"

//清华文通、E验通FS531
class CScan3  : public IScan
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

	CScan3();
	virtual ~CScan3();

private:
	static CScan3 *m_pThis;

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

	Fun_V3_LoadIdcardLibrary *LoadIdcardLibrary;
	Fun_V3_FreeIdcardLibrary *FreeIdcardLibrary;
	Fun_V3_ScanImageEx *ScanImageEx;
	Fun_V3_RecogIdcardExALL *RecogIdcardExALL;
	Fun_V3_RecogIdcardOnlyALL *RecogIdcardOnlyALL;
	Fun_V3_RecogDPOnlyALL *RecogDPOnlyALL;
	Fun_V3_RecogDPALL *RecogDPALL;
	Fun_V3_RecogPassPortOnlyALL *RecogPassPortOnlyALL;
	Fun_V3_RecogPassPortALL *RecogPassPortALL;
	Fun_V3_RecogNewIdcardOnlyALL *RecogNewIdcardOnlyALL;
	Fun_V3_RecogNewIdcardALL *RecogNewIdcardALL;
	Fun_V3_GetButtonDownType *GetButtonDownType;
};

#endif // !defined(AFX_Scan3_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
