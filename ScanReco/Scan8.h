/////////////////////////////////////////////////////////////
///华视扫描仪接口

#ifndef AFX_SCAN7_H__00A22144_FF5F_4FB0_B6A9_325BBC736A85__INCLUDED_
#define AFX_SCAN7_H__00A22144_FF5F_4FB0_B6A9_325BBC736A85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 

#include "stdio.h"
#include "ScanReco.h"
#include "IScan.h"
#include <atlstr.h>

class CScan8: public IScan
{

public:
	CScan8();
	virtual ~CScan8();
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
	void HandleFile();



	typedef int(PASCAL *lpCVR_InitComm)(int port);			//指向dll库中CVR_InitComm(int port)函数的指针
	typedef int(PASCAL *lpCVR_CloseComm)();					//指向dll库中CVR_CloseComm()函数的指针
	typedef int(PASCAL *lpCVR_Authenticate)();				//指向dll库中CVR_Authenticate()函数的指针
	typedef int(PASCAL *lpCVR_Read_Content)(int active);    //指向dll库中CVR_Read_Content(int active)函数的指针
	typedef int(PASCAL *lpCVR_ReadBaseMsg)(unsigned char *pucCHMsg, unsigned int *puiCHMsgLen, 								
									   unsigned char *pucPHMsg, unsigned int *puiPHMsgLen, 
								       int nMode
                                       );    //读卡操作(读入内存)
	lpCVR_InitComm CVR_InitComm;
	lpCVR_CloseComm CVR_CloseComm;
	lpCVR_Authenticate CVR_Authenticate;
	lpCVR_Read_Content CVR_Read_Content;
	lpCVR_ReadBaseMsg CVR_ReadBaseMsg;

private:
	static CScan8 *m_pThis;
	HINSTANCE m_hDll;
	
	bool m_bOpened ;
	int m_iRetUSB;

};


#endif

