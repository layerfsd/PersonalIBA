// Scan4.h: interface for the CScan4 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Scan4_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
#define AFX_Scan4_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"

#define CMD_HOSTID	  0xFA	//所有消息必须有次标识，否则忽略

#define CMD_CALIBRATE 0x10	//校正扫描器
#define CMD_INIT	  0x11	//初始化
#define CMD_SCAN	  0x12	//调用时注意ucDataBuf中保存必须USHORT类型的证件类型
#define CMD_GetVALUE  0x13	//扩展使用，目前未用

//汉王扫描仪
class CScan4 : public IScan
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

	CScan4();
	virtual ~CScan4();

private:
	HANDLE m_hScanPicCompleteEvent;
	HANDLE m_hCompleteEvent;
	bool m_bOpened;
    PROCESS_INFORMATION m_stProcessInfo;

	BOOL SendHWCmd(USHORT usCmd, DWORD usIDType);
	BOOL StartupHWConnect();
};

#endif // !defined(AFX_Scan4_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
