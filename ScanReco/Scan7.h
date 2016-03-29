/////////////////////////////////////////////////////////////
///华视扫描仪接口

#ifndef AFX_SCAN7_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_
#define AFX_SCAN7_H__00A22144_FF5F_4FB0_B6A9_325BBC736A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define SCAN_COLOR_MODE		    1
#define SCAN_GRAY_MODE			2

#define FIRST_IDCARD				1 //一代身份证
#define SECOND_IDCARD			2 //二代身份证
#define CHINESE_PASSPORT			3 //中国护照
#define INTERNATION_PASSPORT	4 //国际护照
#define VISA_PAGE					5 //签证页
#define TAIBAO_CARD				6 //台胞证
#define GUANGAO_CARD			7 //港澳通行证

#define DRIVER_CARD				8 //驾驶证
#define XIANGGUANG_CARD			9 //香港居民身份证
#define SOLDER_CARD				10 //士兵证
#define CAP_CARD 					11 //军官证



#include "ScanReco.h"
#include "IScan.h"
#include <atlstr.h>

class CScan7: public IScan
{

public:

	typedef struct
	{
		char name [40];				//姓名
		char SurnameCH[100];		//中文姓
		char nameCH[100];			//中文名
		char sex[10];					//性别
		char Gender[10];				//性别（英文）
		char birthday[50];				//出生日期
		char people[20];				//民族
		char signdate[50];				//发证日期
		char validterm[50];			//有效期限
		char address[200];			//地址
		char number[40];				//身份证号码
		char organs[50];				//发证机关
		char SurnameEN[100];		//英文姓(汉语拼音)
		char nameEN[100];			//英文名(汉语拼音)
		char ENfullname[40];			//英文姓名	
		char Nationality[200];			//国籍
		char id[100];					//证件号码
		char Leavetime[50];			//离开时间
		char placeCH[100];			//签发地点中文
		char placeEN[100];			//签发地点英文
		char BirthplaceCH[100];		//出生地中文 
		char BirthplaceEN[100];		//出生地英文
		char szCodeOne[256];		//第一行代码识别结果
		char szCodeTwo[256];			//第二行代码识别结果
		char szCodeThree[256];		//第三行代码识别结果
		char Permitnumber_number[40]; //港澳证件号码
		char Vocational[200];			//职业
		char DocumentsCategory[100];	//证件类别
		char Other[200];				//其他
	}SYSCAN_RST;

	// 链接函数
	typedef long (PASCAL *FUN_IO_HasScanner)(LPSTR IVS_600DS, HANDLE hPnpWnd);
	
	// 图像识别相关函数
	typedef long (PASCAL *FUN_IO_CaptureImage)(BYTE *pIR, long lirSize, BYTE *pWh, long lwhSize);
	
	typedef long (PASCAL *FUN_IO_ProcessImage)(const unsigned char *pSrc, long lImageType,
		long lCardType, unsigned char *pRst, long *rst_w, long *rst_h);

	typedef long (PASCAL *FUN_IO_GetIDImage_Buff)(long lImageType, long lCardType,
		unsigned char *pRst, long *rst_w, long *rst_h);

	typedef long (PASCAL *FUN_IO_GetIDImage_File)(long lImageType, long lCardType, 
		long lCompressRet, char *szFileOutPut);

	typedef long (PASCAL *FUN_IO_IdentifyCard_Buff)(unsigned char* pImageBuff, long lWidth, 
		long lHeight, long lImageType, long lCardType, SYSCAN_RST * pResult);

	typedef long (PASCAL *FUN_IO_IdentifyCard_File)(char* szFileIn, long lCardType, 
		char* szPhotoOut, SYSCAN_RST* pResult);

	typedef long (PASCAL *FUN_IO_GetIDResult)(long lCardType, char* szOutPutFileName_IR, 
		char* szOutPutFileName_HR, char* szPhotoFileName_IR, char* szPhotoFileName_HR , 
		long lCompressRet, SYSCAN_RST* pstRst);

	// 芯片识别相关函数
	typedef BOOL (PASCAL *FUN_IO_StartRFID)(void);

	typedef BOOL (PASCAL *FUN_IO_StopRFID)(void);

	typedef long (PASCAL *FUN_IO_SearchCard)(void);

	typedef long (PASCAL *FUN_IO_SelectCard)(void);

	typedef long (PASCAL *FUN_IO_ReadCard)(SYSCAN_RST * pResult); 

	typedef long (PASCAL *FUN_IO_SSReadCard)(SYSCAN_RST * pResult);

	// 其他操作
	typedef void  (PASCAL *FUN_IO_Notify)(unsigned long time);

	typedef void  (PASCAL *FUN_IO_NotifyBeep)(unsigned long time);

	typedef long  (PASCAL *FUN_O_GetVersion)(BYTE *pVersion,DWORD nLen);

	typedef long  (PASCAL *FUN_IO_SetFTLight)(int lType, int lSwitch);

	typedef long  (PASCAL *FUN_IO_CreateCalibrateData)(BOOL bBK);

	typedef long  (PASCAL *FUN_IO_UpdateCalibrateData)();

	typedef void  (PASCAL *FUN_IO_CoincideCamera)(BOOL bCoincide);

	typedef HWND (WINAPI *PROCGETTASKMANWND)();



	CScan7();

	virtual ~CScan7();

	virtual bool IsDeviceValid();										//设备是否有效, 此处可以检查参数及环境
	
	virtual int OpenDevice();											//打开设备
	
	virtual int SetCallBack(CallBack pFunc);							//设置回调
	
	virtual int ScanImage(int idType,char* szImgFile);                  //扫描图片
	
	virtual int RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage); //识别图片
	
	virtual int ReadCard(IDENTITY_INFO &info,char* szHeadImage);        //读取二代证
	
	virtual bool IsSupportReader();						//是否支持二代证读卡器，纯扫描仪不支持二代证读卡器
	
	virtual bool IsSupportScanner();						//是否支持证件扫描功能，纯二代证读卡器不支持扫描
	
	virtual bool IsSupportCallBack();						//是否支持回调
	
	virtual bool IsSupportRecognise(int idType);						//是否支持特定证件识别
	
	virtual bool IsSupportHeadImage(int idType);						//是否支持特定头像获取
	
	virtual int CloseDevice();                                          //关闭设备
	
	virtual void GetErrorMessage(int errCode,char* szErrMsg);           //获取出错信息

protected:

	PROCGETTASKMANWND GetTaskmanWindow;

	// 链接函数
    FUN_IO_HasScanner m_pfHasScanner;

	// 图像识别相关函数
	FUN_IO_CaptureImage m_pfCaptureImage;
	FUN_IO_ProcessImage m_pfProcessImage;
	FUN_IO_GetIDImage_Buff m_pfGetIDImage_Buff;
	FUN_IO_GetIDImage_File m_pfGetIDImage_File;
	FUN_IO_IdentifyCard_Buff m_pfIdentifyCard_Buff;
	FUN_IO_IdentifyCard_File m_pfIdentifyCard_File;
	FUN_IO_GetIDResult m_pfGetIDResult;

	// 芯片识别
	FUN_IO_StartRFID m_pfStartRFID;
	FUN_IO_StopRFID  m_pfStopRFID;
	FUN_IO_SearchCard m_pfSearchCard;
	FUN_IO_SelectCard m_pfSelectCard;
	FUN_IO_ReadCard   m_pfReadCard;
	FUN_IO_SSReadCard m_pfSSReadCard;

	// 其他操作
	FUN_IO_Notify m_pfNotify;
	FUN_IO_NotifyBeep m_pfNotifyBeep;
	FUN_O_GetVersion  m_pfGetVersion;
	FUN_IO_SetFTLight m_pfSetFTLight;
	FUN_IO_CreateCalibrateData m_pfCreateCalibrateData;
	FUN_IO_UpdateCalibrateData m_pfUpdateCalibrateData;
	FUN_IO_CoincideCamera  m_pfCoincideCamera;

	//int SaveImg(long iCardType);


private:

	static CScan7 *m_pThis;

	int m_iVer;

	HINSTANCE m_hDll;

	bool m_bOpened;

	HWND m_hMainWnd;

	char m_cImageFilePath[MAX_PATH];

	char m_cHeadImageFilePath[MAX_PATH];

	char m_cColorImageFilePath[MAX_PATH];

	char m_cColorHeadImageFilePath[MAX_PATH];

	char m_cTempPath[MAX_PATH];

	void FormatErrorMsg(int iErrorCode,CString strErrorMsg);

	void CreateMyWindow();

};


#endif

