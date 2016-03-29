// Scan3.cpp: implementation of the CScan3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <process.h>
#include "Scan3.h"
#include <stdlib.h>
//#pragma comment(lib, "1/CardAll.lib")


CScan3 *CScan3::m_pThis = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan3::CScan3()
{
	m_pThis = this;
	m_bOpened = false;
	memset(m_cImageFilePath, 0, sizeof(m_cImageFilePath));
	m_hDll = NULL;
	m_hQuitEvent = ::CreateEvent(NULL, TRUE, FALSE, "ScanReco_dll_CScan3_CScan3_Stop_Flag");
	m_pfnCallBack = NULL;

	LoadIdcardLibrary = NULL;
	FreeIdcardLibrary = NULL;
	ScanImageEx = NULL;
	RecogIdcardExALL = NULL;
	RecogIdcardOnlyALL = NULL;
	RecogDPOnlyALL = NULL;
	RecogDPALL = NULL;
	RecogPassPortOnlyALL = NULL;
	RecogPassPortALL = NULL;
	RecogNewIdcardOnlyALL = NULL;
	RecogNewIdcardALL = NULL;
}

CScan3::~CScan3()
{
	CloseDevice();
}

bool CScan3::IsDeviceValid()
{
	return true;
}

int CScan3::OpenDevice()
{
	int iError = ERROR_SCANNER_OTHER;

	if(m_hDll == NULL)
	{
		char cLibDir[MAX_PATH]={0};
		GetPrivateProfileString("SCAN_3", "LIB_DIR", "", cLibDir, sizeof(cLibDir), g_cIniFile);
		int pathLen=strlen(cLibDir);
		if(pathLen > 0 && cLibDir[pathLen-1]!='\\')
		{
			cLibDir[pathLen]='\\';
			pathLen++;
		}
		strcpy(cLibDir+pathLen,"idcarddll.dll");
		m_hDll = LoadLibrary(cLibDir);
	}

	if(m_hDll == NULL)
	{
		return ERROR_SCANNER_LOADDLL;
	}
	
	LoadIdcardLibrary = (Fun_V3_LoadIdcardLibrary *)GetProcAddress(m_hDll, "LoadIdcardLibrary");
	FreeIdcardLibrary = (Fun_V3_FreeIdcardLibrary *)GetProcAddress(m_hDll, "FreeIdcardLibrary");
	ScanImageEx = (Fun_V3_ScanImageEx *)GetProcAddress(m_hDll, "ScanImageEx");
	RecogIdcardOnlyALL = (Fun_V3_RecogIdcardOnlyALL *)GetProcAddress(m_hDll, "RecogIdcardOnlyALL");
	RecogNewIdcardOnlyALL = (Fun_V3_RecogNewIdcardOnlyALL *)GetProcAddress(m_hDll, "RecogNewIdcardOnlyALL");
	RecogPassPortOnlyALL = (Fun_V3_RecogPassPortOnlyALL *)GetProcAddress(m_hDll, "RecogPassPortOnlyALL");
	GetButtonDownType = (Fun_V3_GetButtonDownType *)GetProcAddress(m_hDll, "GetButtonDownType");

	if(IsBadCodePtr( (FARPROC)LoadIdcardLibrary ) || 
		IsBadCodePtr( (FARPROC)FreeIdcardLibrary ) || 
		IsBadCodePtr( (FARPROC)ScanImageEx ) || 
		IsBadCodePtr( (FARPROC)RecogIdcardOnlyALL ) || 
		IsBadCodePtr( (FARPROC)RecogNewIdcardOnlyALL ) || 
		IsBadCodePtr( (FARPROC)RecogPassPortOnlyALL ))
	{
		FreeLibrary(m_hDll);
		m_hDll=NULL;
		return ERROR_SCANNER_LOADDLL;
	}

	if(!m_bOpened)
	{
		if(LoadIdcardLibrary() == 0)
		{
			iError=ERROR_SCANNER_OK;
			m_bOpened = true;

			if(GetButtonDownType != NULL) //E验通FS531
			{
				SetTimer(NULL, 0x01, 100, (void (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,unsigned long))CheckButtonTimerProc);
			}
		}
	}

	return iError;
}

int CScan3::CloseDevice()
{
	if(m_bOpened)
	{
		KillTimer(NULL, 0x01);
		SetEvent(m_hQuitEvent);
		Sleep(300);
		FreeIdcardLibrary();
		m_bOpened = false;
	}
	if(m_hDll != NULL)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}

	return ERROR_SCANNER_OK;
}

int CScan3::ScanImage(int idType, char *szImgFile)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	short iType = 3;
	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
			iType = 3;
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{
			iType = 6;
		}
		break;

	case IDENTITY_STUDENT:// 15 学生证
		{
		}
		break;

	case IDENTITY_OFFICER:// 90 军官证
		{
		
		}
		break;

	case IDENTITY_POLICE:// 91 警官证
		{
		
		}
		break;

	case IDENTITY_SOLDIER:// 92 士兵证
		{
		
		}
		break;

	case IDENTITY_HUKOUBU:// 93 户口簿
		{
		
		}
		break;

	case IDENTITY_PASSPORT:// 94 护照
		{
			iType = 4;
		}
		break;

	case IDENTITY_TAIBAO:// 95 台胞证
		{
		
		}
		break;

	case IDENTITY_OTHER:// 99 其它证件
		{
		
		}
		break;

	default:
		break;
	}

	if(ScanImageEx(szImgFile, iType) == 0)
	{
		strncpy(m_cImageFilePath, szImgFile, strlen(szImgFile));
		return ERROR_SCANNER_OK;
	}

	return ERROR_SCANNER_OTHER;
}

int CScan3::RecogniseImage(int idType, IDENTITY_INFO &info, char *szHeadImage)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	int iError = -1;
	IDCARD_ALL3 stIDCARD_ALL = {0};
	
	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
			iError = RecogIdcardOnlyALL(
				m_cImageFilePath,
				stIDCARD_ALL,
				TRUE,
				0,
				0x003f,
				szHeadImage);
		}
		break;
		
	case IDENTITY_ID2:// 11 二代身份证
		{
			iError = RecogNewIdcardOnlyALL(
				m_cImageFilePath,
				stIDCARD_ALL,
				TRUE,
				0x003f,
				szHeadImage);
		
		}
		break;
		
	case IDENTITY_STUDENT:// 15 学生证
		{
		}
		break;
		
	case IDENTITY_OFFICER:// 90 军官证
		{
			
		}
		break;
		
	case IDENTITY_POLICE:// 91 警官证
		{
			
		}
		break;
		
	case IDENTITY_SOLDIER:// 92 士兵证
		{
			
		}
		break;
		
	case IDENTITY_HUKOUBU:// 93 户口簿
		{
			
		}
		break;
		
	case IDENTITY_PASSPORT:// 94 护照
		{
			iError = RecogPassPortOnlyALL(
				m_cImageFilePath,
				stIDCARD_ALL,
				TRUE,
				0x003f,
				szHeadImage);
		}
		break;
		
	case IDENTITY_TAIBAO:// 95 台胞证
		{
		}
		break;
		
	case IDENTITY_OTHER:// 99 其它证件
		{
			
		}
		break;
		
	default:
		break;
	}
	
	if(iError == 0)
	{
		strncpy(info.name, stIDCARD_ALL.name, 20);     //姓名
		strncpy(info.sex, stIDCARD_ALL.sex, 2);        //性别
		strncpy(info.number, stIDCARD_ALL.number, 40);   //证件编码
		strncpy(info.nation, stIDCARD_ALL.people, 20);    //民族
		strncpy(info.birthday, stIDCARD_ALL.birthday, 10);  //出生日期
		strncpy(info.address, stIDCARD_ALL.address, 200);  //地址
		strncpy(info.signdate, stIDCARD_ALL.signdate, 10);  //签发日期
		strncpy(info.validterm, stIDCARD_ALL.validterm, 30);  //有效期
		strncpy(info.department, "", 40); //发证机关
		return ERROR_SCANNER_OK;
	}
	
	return ERROR_SCANNER_OTHER;
}

bool CScan3::IsSupportHeadImage(int /*idType*/)
{
	return true;
}

bool CScan3::IsSupportRecognise(int idType)
{
	bool bSupport = false;

	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
			bSupport = TRUE;
		}
		break;
		
	case IDENTITY_ID2:// 11 二代身份证
		{
			bSupport = TRUE;
		}
		break;
		
	case IDENTITY_STUDENT:// 15 学生证
		{
		}
		break;
		
	case IDENTITY_OFFICER:// 90 军官证
		{
			
		}
		break;
		
	case IDENTITY_POLICE:// 91 警官证
		{
			
		}
		break;
		
	case IDENTITY_SOLDIER:// 92 士兵证
		{
			
		}
		break;
		
	case IDENTITY_HUKOUBU:// 93 户口簿
		{
			
		}
		break;
		
	case IDENTITY_PASSPORT:// 94 护照
		{
			bSupport = TRUE;
		}
		break;
		
	case IDENTITY_TAIBAO:// 95 台胞证
		{
		}
		break;
		
	case IDENTITY_OTHER:// 99 其它证件
		{
			
		}
		break;
		
	default:
		break;
	}

	return 	bSupport;
}

bool CScan3::IsSupportCallBack()
{
	return GetButtonDownType != NULL ? true : false;
}

bool CScan3::IsSupportReader()
{
	return false;
}

bool CScan3::IsSupportScanner()
{
	return true;
}

int CScan3::ReadCard(IDENTITY_INFO& /*info*/, char* /*szHeadImage*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

void CScan3::GetErrorMessage(int errCode,char* szErrMsg)
{
	if(szErrMsg)
	{
		for(int i = 0; i < 1024 && g_cErrorDict[i][0] != 0; i++)
		{
			if(errCode == atol(g_cErrorDict[i][0]))
			{
				strcpy(szErrMsg, g_cErrorDict[i][1]);
				return;
			}
			
		}
		strcpy(szErrMsg, g_cErrorDict[7][1]);
	}
}

VOID CALLBACK CScan3::CheckButtonTimerProc(HWND hWnd,
										   UINT /*uMsg*/,
										   UINT_PTR idEvent,
										   DWORD /*dwTime*/
										   )
{

	if(!m_pThis->m_bOpened || IsBadCodePtr( (FARPROC)m_pThis->GetButtonDownType ))
	{
		ResetEvent(m_pThis->m_hQuitEvent);
		KillTimer(hWnd, idEvent);
		return;
	}

	if(WaitForSingleObject(m_pThis->m_hQuitEvent, 0) == WAIT_OBJECT_0)
	{
		ResetEvent(m_pThis->m_hQuitEvent);
		KillTimer(hWnd, idEvent);
		return;
	}

	if(!IsBadCodePtr( (FARPROC)m_pThis->m_pfnCallBack ))
	{
		int iButton = m_pThis->GetButtonDownType();
		switch(iButton)
		{
		case 1:
			((CallBack)m_pThis->m_pfnCallBack)(IDENTITY_ID1);
			break;

		case 2:
			((CallBack)m_pThis->m_pfnCallBack)(IDENTITY_ID2);
			break;

		case 3:
			((CallBack)m_pThis->m_pfnCallBack)(IDENTITY_OTHER);
			break;

		default:
			;
		}
	}


}

int CScan3::SetCallBack(CallBack pFunc)
{
	if(GetButtonDownType==NULL)
	{
		return ERROR_SCANNER_UNSUPPORT;
	}

	if(IsBadCodePtr( (FARPROC)pFunc ))
	{
		return ERROR_SCANNER_OTHER;
	}

	m_pfnCallBack = (CallBack)pFunc;

	return ERROR_SCANNER_OK;
}

