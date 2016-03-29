// IScan.h: interface for the IScan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCAN_H__B1772FDD_0F9C_4E19_BA32_5B36F1694B99__INCLUDED_)
#define AFX_ISCAN_H__B1772FDD_0F9C_4E19_BA32_5B36F1694B99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IScan  
{
public:
	virtual bool IsDeviceValid() = 0;										//设备是否有效, 此处可以检查参数及环境
	virtual int OpenDevice() = 0;											//打开设备
	virtual int SetCallBack(CallBack pFunc) = 0;							//设置回调
	virtual int ScanImage(int idType,char* szImgFile) = 0;                  //扫描图片
	virtual int RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage) = 0; //识别图片
	virtual int ReadCard(IDENTITY_INFO &info,char* szHeadImage) =0;			//读取二代证
	virtual bool IsSupportReader() = 0;										//是否支持二代证读卡器，纯扫描仪不支持二代证读卡器
	virtual bool IsSupportScanner() = 0;									//是否支持证件扫描功能，纯二代证读卡器不支持扫描
	virtual bool IsSupportCallBack() = 0;									//是否支持回调
	virtual bool IsSupportRecognise(int idType) = 0;						//是否支持识别
	virtual bool IsSupportHeadImage(int idType) = 0;						//是否支持头像获取
	virtual int CloseDevice() = 0;                                          //关闭设备
	virtual void GetErrorMessage(int errCode,char* szErrMsg) = 0;           //获取出错信息
public:

};

#endif // !defined(AFX_ISCAN_H__B1772FDD_0F9C_4E19_BA32_5B36F1694B99__INCLUDED_)
