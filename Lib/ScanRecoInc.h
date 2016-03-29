
#pragma once

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
struct IDENTITY_INFO
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
}; 

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

int WINAPI OpenDevice();
int WINAPI SetCallBack(CallBack pFunc);
int WINAPI ScanImage(int idType, char* szImgFile);
int WINAPI RecogniseImage(int idType, IDENTITY_INFO &info, char* szHeadImage);
int WINAPI IsSupportCallBack();
int WINAPI IsSupportRecognise(int idType);
int WINAPI IsSupportHeadImage(int idType);
void WINAPI GetErrorMessage(int errCode, char* szErrMsg);
int WINAPI CloseDevice();
int WINAPI IsDeviceValid();
int WINAPI IsSupportReader();
int WINAPI IsSupportScanner();
int WINAPI ReadCard(IDENTITY_INFO &info, char* szHeadImage);

}