// ScanReco.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ScanReco.h"
#include "IScan.h"
#include "Log.h"
#include "Scan0.h" 
#include "Scan1.h"
#include "Scan2.h"
#include "Scan3.h"
#include "Scan4.h"
#include "Scan5.h"
#include "Scan6.h"
#include "Scan7.h"
#include "Scan8.h"
#include "Scan9.h"



const char *g_cErrorDict[][2] = 
{
	{"0", "其它未知错误"},
	{"1", "成功"},
	{"2", "加载识别核心失败"},
	{"3", "硬件加密校验失败"},
	{"4", "扫描发生错误"},
	{"5", "用户输入的文件路径名不符合要求"},
	{"6", "识别失败"},
	{"7", "目前扫描仪没有在线"},
	{"8", "不支持的操作"},

	{0, 0}
};

IScan* g_pObjScan = NULL;
//CLog g_objCLog;
char g_cIniFile[MAX_PATH] = {0};

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID /*lpReserved*/
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			if(g_pObjScan!=NULL) break;  //可能会产生多DLL_PROCESS_ATTACH

			//g_objCLog.Open();

			GetModuleFileName((HINSTANCE)hModule, g_cIniFile, MAX_PATH);
			char *p = strrchr(g_cIniFile, '.');
			if(p != NULL)
			{
				strncpy(p, ".INI", 4);
			}
			else
			{
				strcat(g_cIniFile,".INI");
			}
			
			int load_id = GetPrivateProfileInt("SCAN_LOAD", "LOAD_ID", 0, g_cIniFile);

			switch(load_id)
			{
				case 1:
					{
						char cLibDir[MAX_PATH]={0};
						GetPrivateProfileString("SCAN_1", "LIB_DIR", "", cLibDir, sizeof(cLibDir), g_cIniFile);
						int pathLen=strlen(cLibDir);
						if(pathLen > 0 && cLibDir[pathLen-1]!='\\')
						{
							cLibDir[pathLen]='\\';
							pathLen++;
						}
						strcpy(cLibDir+pathLen,"CardAll.dll");
						if (GetFileAttributes(cLibDir) != INVALID_FILE_ATTRIBUTES)
							g_pObjScan=new CScan1(); //FS533
						else
							g_pObjScan=new CScan3(); //FS531
						break;
					}
				case 2:
					g_pObjScan=new CScan2();
					break;
				case 3:
					g_pObjScan=new CScan3();
					break;
				case 4:
					g_pObjScan=new CScan4();
					break;
				case 5:
					g_pObjScan=new CScan5();
					break;
				case 6:
					g_pObjScan=new CScan6();
					break;
				case 7:
					g_pObjScan=new CScan7();
					break;
				case 8:
					g_pObjScan= new CScan8();
					break;
				case 9:
					g_pObjScan= new CScan9();
					break;
				default:
					g_pObjScan=new CScan0();
					break;
			}
			break;
		}
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
		{
			//g_objCLog.Close();
			delete g_pObjScan;
			g_pObjScan=NULL;
			break;
		}
    }
    return TRUE;
}


SCANRECO_API int WINAPI OpenDevice()
{
	int errCode=g_pObjScan ? g_pObjScan->OpenDevice() : ERROR_SCANNER_OTHER;

	
	return errCode;
}  

SCANRECO_API int WINAPI SetCallBack(CallBack pFunc)
{
	return g_pObjScan ? g_pObjScan->SetCallBack(pFunc) : ERROR_SCANNER_OTHER;
}

SCANRECO_API int WINAPI ScanImage(int idType,char* szImgFile)
{
	int errCode=g_pObjScan ? g_pObjScan->ScanImage(idType, szImgFile) : ERROR_SCANNER_OTHER;



	return errCode;
}

SCANRECO_API int WINAPI RecogniseImage(int idType, IDENTITY_INFO &info,char* szHeadImage)
{
	int errCode=g_pObjScan ? g_pObjScan->RecogniseImage(idType, info, szHeadImage) : ERROR_SCANNER_OTHER;

	return errCode;
}

SCANRECO_API int WINAPI IsSupportCallBack()
{
	bool rc = g_pObjScan ? g_pObjScan->IsSupportCallBack() : false;

	return rc ? 1 : 0;
}

SCANRECO_API int WINAPI IsSupportRecognise(int idType)
{
	bool rc = g_pObjScan ? g_pObjScan->IsSupportRecognise(idType) : false;

	return rc ? 1 : 0;
}

SCANRECO_API int WINAPI IsSupportHeadImage(int idType)
{
	bool rc = g_pObjScan ? g_pObjScan->IsSupportHeadImage(idType) : false;

	return rc ? 1 : 0;
}

SCANRECO_API int WINAPI IsSupportReader()
{
	bool rc = g_pObjScan ? g_pObjScan->IsSupportReader() : false;

	return rc ? 1 : 0;
}

SCANRECO_API int WINAPI IsSupportScanner()
{
	bool rc = g_pObjScan ? g_pObjScan->IsSupportScanner() : false;

	return rc ? 1 : 0;
}

SCANRECO_API void WINAPI GetErrorMessage(int errCode,char* szErrMsg)
{
	if(g_pObjScan) g_pObjScan->GetErrorMessage(errCode,szErrMsg);
}

SCANRECO_API int WINAPI CloseDevice()
{
	return g_pObjScan ? g_pObjScan->CloseDevice() : ERROR_SCANNER_OTHER;
}

SCANRECO_API int WINAPI ReadCard(IDENTITY_INFO &info,char* szHeadImage)
{
	int errCode=g_pObjScan ? g_pObjScan->ReadCard(info, szHeadImage) : ERROR_SCANNER_OTHER;
	
	//g_objCLog.Write("ReadCard: %u", errCode);

	return errCode;
}

SCANRECO_API int WINAPI IsDeviceValid()
{
	bool rc = g_pObjScan ? g_pObjScan->IsDeviceValid() : false;
	
	return rc ? 1 : 0;
}
