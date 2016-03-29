
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SCANRECO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SCANRECO_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SCANRECO_EXPORTS
#define SCANRECO_API __declspec(dllexport)
#else
#define SCANRECO_API __declspec(dllimport)
#endif

#ifndef _SCANRECO_H
#define _SCANRECO_H

enum IDENTITY_TYPE
{
		IDENTITY_ID1   = 10, //一代身份证
		IDENTITY_ID2   = 11, //二代身份证
		IDENTITY_STUDENT   = 15, //学生证
		IDENTITY_OFFICER   = 90, //军官证
		IDENTITY_POLICE   = 91, //警官证
		IDENTITY_SOLDIER   = 92, //士兵证
		IDENTITY_HUKOUBU   = 93, //户口簿
		IDENTITY_PASSPORT   = 94, //护照
		IDENTITY_TAIBAO   = 95, //台胞证
		IDENTITY_GANGHAO  = 96,	// 港澳通行证
		IDENTITY_OTHER   = 99, //其它证件
		IDENTITY_ID2_BACK  = 200	// 二代证背面
};	

//2.2	身份信息结构 
typedef struct
{
	char name[20];     //姓名
	char sex[2];        //性别
	char number[40];   //证件编码
	char nation[20];    //民族
	char birthday[10];  //出生日期
	char address[200];  //地址
	char signdate[10];  //签发日期
	char validterm[30];  //有效期
	char department[40]; //发证机关
	
}IDENTITY_INFO; 


//typedef struct
//{
//	char name[30];     //姓名
//	char sex[4];        //性别
//	char nation[4];    //民族
//	char birthday[16];  //出生日期
//	char address[70];  //地址
//	char number[36];   //证件编码
//	char department[30]; //发证机关
//	char signdate[16];  //签发日期
//	char validterm[40];  //有效期
//}IDENTITY_INFO; 

#define ERROR_SCANNER_OTHER 0
#define ERROR_SCANNER_OK 1
#define ERROR_SCANNER_LOADDLL 2
#define ERROR_SCANNER_HARDWARE 3
#define ERROR_SCANNER_PROCESS 4
#define ERROR_SCANNER_FILEPATH 5
#define ERROR_SCANNER_RECOGNISE 6
#define ERROR_SCANNER_OFFLINE   7
#define ERROR_SCANNER_UNSUPPORT 8

extern "C" 
{
	typedef VOID (CALLBACK *CallBack)(int idType);
	
	
	SCANRECO_API int WINAPI OpenDevice();
	SCANRECO_API int WINAPI SetCallBack(CallBack pFunc);
	SCANRECO_API int WINAPI ScanImage(int idType,char* szImgFile);
	SCANRECO_API int WINAPI RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage);
	SCANRECO_API int WINAPI IsSupportCallBack();
	SCANRECO_API int WINAPI IsSupportRecognise(int idType);
	SCANRECO_API int WINAPI IsSupportHeadImage(int idType);
	SCANRECO_API void WINAPI GetErrorMessage(int errCode,char* szErrMsg);
	SCANRECO_API int WINAPI CloseDevice();
	SCANRECO_API int WINAPI IsDeviceValid();
	SCANRECO_API int WINAPI IsSupportReader();
	SCANRECO_API int WINAPI IsSupportScanner();
	SCANRECO_API int WINAPI ReadCard(IDENTITY_INFO &info,char* szHeadImage);
	
}

extern char g_cIniFile[MAX_PATH];
extern const char *g_cErrorDict[][2];

#endif//_SCANRECO_H