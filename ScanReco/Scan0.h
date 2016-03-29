// Scan0.h: interface for the CScan0 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCAN0_H__1D85AD8C_3CD6_46B8_B4C3_8054EA17CBF3__INCLUDED_)
#define AFX_SCAN0_H__1D85AD8C_3CD6_46B8_B4C3_8054EA17CBF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"

//空设备类
class CScan0  : public IScan
{
public:

	virtual bool IsDeviceValid();										//设备是否有效, 此处可以检查参数及环境
	virtual int OpenDevice();											//打开设备
	virtual int SetCallBack(CallBack pFunc);							//设置回调
	virtual int ScanImage(int idType,char* szImgFile);                  //扫描图片
	virtual int RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage); //识别图片
	virtual int ReadCard(IDENTITY_INFO &info,char* szHeadImage);        //读取二代证
	virtual bool IsSupportReader();									//是否支持二代证读卡器，纯扫描仪不支持二代证读卡器
	virtual bool IsSupportScanner();									//是否支持证件扫描功能，纯二代证读卡器不支持扫描
	virtual bool IsSupportCallBack();									//是否支持回调
	virtual bool IsSupportRecognise(int idType);						//是否支持特定证件识别
	virtual bool IsSupportHeadImage(int idType);						//是否支持特定头像获取
	virtual int CloseDevice();                                          //关闭设备
	virtual void GetErrorMessage(int errCode,char* szErrMsg);           //获取出错信息

	CScan0();
	virtual ~CScan0();

};

#endif // !defined(AFX_SCAN0_H__1D85AD8C_3CD6_46B8_B4C3_8054EA17CBF3__INCLUDED_)
