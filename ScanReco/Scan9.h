// Scan2.h: interface for the CScan9 class.
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
#include <atlstr.h>
#include <tchar.h>

//文通
// 文通的扫描仪到类销毁的时候才关闭扫描接口
// 提供给外部关闭设备的接口不关闭扫描接口
class CScan9  : public IScan
{
public:
	virtual bool IsDeviceValid();										//设备是否有效, 此处可以检查参数及环境
	virtual int OpenDevice();											//打开设备
	int CScan9::OpenScanDevice();
	int CScan9::OpenReadCardDevice();
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

	CScan9();
	virtual ~CScan9();

private:
	int m_iVer;
	HINSTANCE m_hDllScan;		// 扫描识别的
	HINSTANCE m_hDllRead;		// 读卡的
	HINSTANCE m_hDllGetBmp;		// 读卡获取图片
	bool m_bReadOpened;
	int	 m_nRetScanOpen;
	int  m_nRetReadCard;
	char m_cImageFilePath[MAX_PATH];
	char m_cHeadImageFilePath[MAX_PATH];
	char m_cColorImageFilePath[MAX_PATH];
	char m_cColorHeadImageFilePath[MAX_PATH];
	char m_cTempPath[MAX_PATH];

	/*************************扫描相关函数**************************************/
	typedef int   (WINAPI *FUN_InitIDCard)(LPCWSTR lpUserID, int nType,LPCWSTR lpDirectory);
	typedef void (WINAPI *FUN_FreeIDCard)();
	typedef bool (WINAPI *FUN_SetAcquireImageType)(int nLightType,int nImageType);
	FUN_InitIDCard m_pfInitIDCard;
	FUN_FreeIDCard m_pfFreeIDCard;
	FUN_SetAcquireImageType m_pfSetAcquireImageType;

	typedef bool (WINAPI *FUN_SetUserDefinedImageSize)(int nWidth, int nHeight);
	typedef bool (WINAPI *FUN_SetAcquireImageResolution)(int nResolutionX, int nResolutionY);
	typedef bool (WINAPI *FUN_CheckDeviceOnline)();
	FUN_SetUserDefinedImageSize m_pfSetUserDefinedImageSize;
	FUN_SetAcquireImageResolution m_pfSetAcquireImageResolution;
	FUN_CheckDeviceOnline m_pfCheckDeviceOnline;

	typedef int (WINAPI *FUN_AcquireImage)(int nImageSizeType);
	typedef int (WINAPI *FUN_SaveImage)(LPCWSTR lpFileName);
	typedef int (WINAPI *FUN_RecogIDCard)();
	FUN_AcquireImage m_pfAcquireImage;
	FUN_SaveImage m_pfSaveImage;
	FUN_RecogIDCard m_pfRecogIDCard;
 
	typedef int  (WINAPI *FUN_SetIDCardID)(int nMainID,int nSubID[],int nSubIdCount);
	typedef int (WINAPI *FUN_AddIDCardID)(int nMainID,int nSubID[],int nSubIdCount);
	typedef int (WINAPI *FUN_RecogIDCardEx)(int nMainID ,int nSubID);
	typedef int (WINAPI *FUN_GetSubId)();
	FUN_SetIDCardID m_pfSetIDCardID;
	FUN_AddIDCardID m_pfAddIDCardID;
	FUN_RecogIDCardEx m_pfRecogIDCardEx;
	FUN_GetSubId m_pfGetSubId;


	typedef int (WINAPI *FUN_GetRecogResult)(int nIndex, LPCWSTR lpBuffer, int &nBufferLen);
	typedef int (WINAPI *FUN_GetFieldName)(int nIndex, LPCWSTR lpBuffer, int &nBufferLen);
	FUN_GetRecogResult m_pfGetRecogResult;
	FUN_GetFieldName m_pfGetFieldName;

	typedef BOOL (WINAPI *FUN_GetCurrentDevice)(LPCWSTR, int);
	typedef void  (WINAPI *FUN_GetVersionInfo)(LPCWSTR ,int );
	typedef int (WINAPI *FUN_GetButtonDownType)();
	typedef int (WINAPI *FUN_SaveHeadImage)(LPCWSTR );
	FUN_GetCurrentDevice m_pfGetCurrentDevice;
	FUN_GetVersionInfo m_pfGetVersionInfo;
	FUN_GetButtonDownType m_pfGetButtonDownType;
	FUN_SaveHeadImage m_pfSaveHeadImage;

	///////////////////////////////////
	typedef int (WINAPI *FUN_GetGrabSignalType)();
	//typedef int (WINAPI *FUN_SetIDCardID)(int nMainID,int nSubID[],int nSubIdCount);
	//typedef int (WINAPI *FUN_AddIDCardID)(int nMainID,int nSubID[],int nSubIdCount);
	typedef int (WINAPI *FUN_SetSpecialAttribute)(int nType,int nSet);
	FUN_GetGrabSignalType m_pfGetGrabSignalType;
	//FUN_SetIDCardID	m_pfSetIDCardID;
	//FUN_AddIDCardID m_pfAddIDCardID;
	FUN_SetSpecialAttribute m_pfSetSpecialAttribute;
	
	BOOL m_bIsScanInitSuccess;
	int m_nIdCardType;

	/*************************芯片识别的函数*************************/
	typedef int (WINAPI *FUN_SDT_OpenPort)(int iPort);
	typedef int (WINAPI *FUN_SDT_ClosePort)(int iPort);
	typedef int (WINAPI *FUN_SDT_StartFindIDCard)(int iPort,unsigned char *pRAPDU, int iIfOpen);
	typedef int (WINAPI *FUN_SDT_SelectIDCard)(int iPort,unsigned char *pRAPDU, int iIfOpen);
	typedef int (WINAPI *FUN_SDT_ReadBaseMsgToFile)(int iPort,char *pcCHMsgFileName,unsigned int* puiCHMsgFileLen,char *pcPHMsgFileName,unsigned int* puiPHMsgFileLen, int iIfOpen);
	typedef int (WINAPI *FUN_SDT_ReadBaseMsg)(int iPort,unsigned char *pcCHMsg,unsigned int* puiCHMsgLen,unsigned char *pcPHMsg,unsigned int* puiPHMsgLen, int iIfOpen);
	typedef int (WINAPI *FUN_SDT_ReadNewAppMsg)(int iPortID,unsigned char * pucAppMsg,unsigned int *puiAppMsgLen,int iIfOpen);
	typedef int (WINAPI *FUN_GetBmp)(char * wltfile,int nType);//wlt--bmp
	FUN_SDT_OpenPort m_pfSDT_OpenPort;
	FUN_SDT_ClosePort m_pfSDT_ClosePort;
	FUN_SDT_StartFindIDCard m_pfSDT_StartFindIDCard;
	FUN_SDT_SelectIDCard m_pfSDT_SelectIDCard;
	FUN_SDT_ReadBaseMsgToFile m_pfSDT_ReadBaseMsgToFile; 
	FUN_SDT_ReadBaseMsg m_pfSDT_ReadBaseMsg; 
	FUN_SDT_ReadNewAppMsg m_pfSDT_ReadNewAppMsg;
	FUN_GetBmp m_pfGetBmp;

	unsigned char m_readCardPucCHMsg[512];	//文字信息
	unsigned char m_readCardPucPHMsg[1024];	//照片信息

	unsigned int m_readCardUiCHMsgLen;
	unsigned int m_readCardUiPHMsgLen;
	bool m_OPort;
	int m_nOpenPort;

	CString GetNation(void);
	CString GetBirthday(void);
	CString GetAddress(void);
	CString GetAuthority(void);
	CString GetIDCode(void);
	CString GetExpityDay(void);
	CString GetIssueDay(void);
	CString GetName(void);
	CString GetSex(void);
	void CloseReadCard();
	void CloseScan();
	int SavePhoto(TCHAR* retFileName, int nType);
	int GetBmp(char * photopath, int nType);
};

#endif // !defined(AFX_SCAN2_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_)
