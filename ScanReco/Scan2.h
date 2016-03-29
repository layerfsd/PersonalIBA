// Scan2.h: interface for the CScan2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCAN2_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
#define AFX_SCAN1_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"
#include "2/IDRCore.h"

//鼎识 A2、A3
class CScan2  : public IScan
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

	CScan2();
	virtual ~CScan2();

private:
	int m_iVer;
	HINSTANCE m_hDll;
	bool m_bOpened;
	char m_cImageFilePath[MAX_PATH];
	char m_cHeadImageFilePath[MAX_PATH];
	char m_cColorImageFilePath[MAX_PATH];
	char m_cColorHeadImageFilePath[MAX_PATH];
	char m_cTempPath[MAX_PATH];
	
	Fun_Device_Open *Device_Open;
	Fun_Device_Close *Device_Close;
	Fun_Get_IdcPic *Get_IdcPic;
	Fun_Get_MiscPic *Get_MiscPic;
	Fun_Get_MiscData *Get_MiscData;
	Fun_Get_ColorPic *Get_ColorPic;
	Fun_Get_IdcData *Get_IdcData;
	Fun_Format_ErrMsg *Format_ErrMsg;
	Fun_BmpToJpg *BmpToJpg;
	Fun_Get_TermbData *Get_TermbData;

};

#endif // !defined(AFX_SCAN2_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
