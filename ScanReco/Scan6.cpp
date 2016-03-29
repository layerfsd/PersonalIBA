// Scan0.cpp: implementation of the CScan6 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <process.h>
#include "Scan6.h"

//#pragma comment(lib, "6/CardAll.lib")


CScan6 *CScan6::m_pThis = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan6::CScan6()
{
	m_pThis = this;
	m_bOpened = false;
	memset(m_cImageFilePath, 0, sizeof(m_cImageFilePath));
	m_hDll = NULL;
	m_hQuitEvent = ::CreateEvent(NULL, TRUE, FALSE, "ScanReco_dll_CScan6_CScan6_Stop_Flag");
	m_pfnCallBack = NULL;

	GetVersionInfo = NULL;
	LoadIdcardLibrary = NULL;
	FreeIdcardLibrary = NULL;
	CalibrateScanner = NULL;
	GetButtonDownType = NULL;
	ScanImageEx = NULL;
	ScanImageEx_NoBack = NULL;
	ScanImageOther = NULL;
	FeedBackScanner = NULL;
	RecogIdcardExALL = NULL;
	RecogIdcardOnlyALL = NULL;
	RecogIdcardRe = NULL;
	RecogDPOnlyALL = NULL;
	RecogDPALL = NULL;
	RecogPassPortOnlyALL = NULL;
	RecogPassPortALL = NULL;
	RecogNewIdcardOnlyALL = NULL;
	RecogNewIdcardALL = NULL;
	RecogPassPortOnlyALL_Region = NULL;
	RecogPassPortALL_Region = NULL;
	RecogTBZOnlyALL = NULL;
	RecogTBZALL = NULL;
	RecogFXZOnlyALL = NULL;
	RecogFXZALL = NULL;
	RecogHXZOnlyALL = NULL;
	RecogHXZALL = NULL;
	RecogMacaoHKFaceALL = NULL;
	RecogMacaoHKFaceOnlyALL = NULL;
}

CScan6::~CScan6()
{
	CloseDevice();
}

bool CScan6::IsDeviceValid()
{
	return true;
}

int CScan6::OpenDevice()
{
	int iError = ERROR_SCANNER_OTHER;

	if(m_hDll == NULL)
	{
		char cLibDir[MAX_PATH]={0};
		GetPrivateProfileString("SCAN_6", "LIB_DIR", "", cLibDir, sizeof(cLibDir), g_cIniFile);
		GetPrivateProfileString("SCAN_1", "LIB_DIR", "", cLibDir, sizeof(cLibDir), g_cIniFile);
		int pathLen=strlen(cLibDir);
		if(pathLen > 0 && cLibDir[pathLen-1]!='\\')
		{
			cLibDir[pathLen]='\\';
			pathLen++;
		}
		strcpy(cLibDir+pathLen,"CardAll.dll");
		m_hDll = LoadLibrary(cLibDir);
	}
	if(m_hDll == NULL)
	{
		return ERROR_SCANNER_LOADDLL;
	}
	
	LoadIdcardLibrary = (Fun_LoadIdcardLibrary *)GetProcAddress(m_hDll, "LoadIdcardLibrary");
	FreeIdcardLibrary = (Fun_FreeIdcardLibrary *)GetProcAddress(m_hDll, "FreeIdcardLibrary");
	CalibrateScanner = (Fun_CalibrateScanner *)GetProcAddress(m_hDll, "CalibrateScanner");
	ScanImageEx = (Fun_ScanImageEx *)GetProcAddress(m_hDll, "ScanImageEx");
	RecogIdcardOnlyALL = (Fun_RecogIdcardOnlyALL *)GetProcAddress(m_hDll, "RecogIdcardOnlyALL");
	RecogNewIdcardOnlyALL = (Fun_RecogNewIdcardOnlyALL *)GetProcAddress(m_hDll, "RecogNewIdcardOnlyALL");
	RecogPassPortOnlyALL = (Fun_RecogPassPortOnlyALL *)GetProcAddress(m_hDll, "RecogPassPortOnlyALL");
	RecogTBZOnlyALL = (Fun_RecogTBZOnlyALL *)GetProcAddress(m_hDll, "RecogTBZOnlyALL");
	GetButtonDownType = (Fun_GetButtonDownType *)GetProcAddress(m_hDll, "GetButtonDownType");
	RecogNewIdcardALL = (Fun_RecogNewIdcardALL *)GetProcAddress(m_hDll, "RecogNewIdcardALL");

	if(IsBadCodePtr( (FARPROC)LoadIdcardLibrary ) || 
		IsBadCodePtr( (FARPROC)FreeIdcardLibrary ) || 
		IsBadCodePtr( (FARPROC)CalibrateScanner ) || 
		IsBadCodePtr( (FARPROC)ScanImageEx ) || 
		IsBadCodePtr( (FARPROC)RecogIdcardOnlyALL ) || 
		IsBadCodePtr( (FARPROC)RecogNewIdcardOnlyALL ) || 
		IsBadCodePtr( (FARPROC)RecogPassPortOnlyALL ) ||
		IsBadCodePtr( (FARPROC)RecogTBZOnlyALL ) ||
		IsBadCodePtr( (FARPROC)GetButtonDownType ))
	{
		return ERROR_SCANNER_LOADDLL;
	}

	if(!m_bOpened)
	{
		if(LoadIdcardLibrary() == 0)
		{
			iError = ERROR_SCANNER_OK;

			//CalibrateScanner();
			m_bOpened = true;
			
			//UINT uiRet = 0;
			//_beginthreadex(NULL, 0, CheckButtonThread, this, 0, &uiRet);
			::SetTimer(NULL, 0x01, 300, (void (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,unsigned long))CheckButtonTimerProc);
		}
	}

	return iError;
}

int CScan6::CloseDevice()
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

int CScan6::ScanImage(int idType, char *szImgFile)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	short iRecoType = 3;

	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
			iRecoType = 3;
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{
			iRecoType = 6;
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
			iRecoType = 4;
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

	if(ScanImageEx(szImgFile, iRecoType) == 0)
	{
		strncpy(m_cImageFilePath, szImgFile, strlen(szImgFile));

		return ERROR_SCANNER_OK;
	}

	return ERROR_SCANNER_OTHER;
}

int CScan6::RecogniseImage(int idType, IDENTITY_INFO &info, char *szHeadImage)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	int iError = -1;
	IDCARD_ALL stIDCARD_ALL = {0};
	
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
			iError = RecogTBZOnlyALL(
				m_cImageFilePath,
				stIDCARD_ALL,
				TRUE,
				0x003f,
				szHeadImage);
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

bool CScan6::IsSupportHeadImage(int /*idType */)
{
	return true;
}

bool CScan6::IsSupportRecognise(int idType)
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
			bSupport = TRUE;
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

bool CScan6::IsSupportCallBack()
{
	return true;
}

VOID CALLBACK CScan6::CheckButtonTimerProc(HWND hWnd,
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

int CScan6::SetCallBack(CallBack pFunc)
{
	if(IsBadCodePtr( (FARPROC)pFunc ))
	{
		return ERROR_SCANNER_OTHER;
	}

	m_pfnCallBack = (CallBack)pFunc;
	
	return ERROR_SCANNER_OK;
}

bool CScan6::IsSupportScanner()
{
	return true;
}

bool CScan6::IsSupportReader()
{
	return true;
}

int CScan6::ReadCard(IDENTITY_INFO &info, char *szHeadImage)
{
	int iOpen = 0, iError = 0;
	UINT i = 0;
	unsigned char pucManaInfo[4] = {0};
	CHX_FDX3CardReader::TAG_TEXT_INFO stTAG_TEXT_INFO = {0};
	unsigned char pucPHMsg[1024]; //照片信息 
	unsigned char ucTemp[1024];
	unsigned int uiCHMsgLen = 0, uiPHMsgLen = 0;

	if(!OpenPort())
	{
		ClosePort();
		return ERROR_SCANNER_OTHER;		
	}

	do
	{
		iError = StartFindIDCard(1001, pucManaInfo, iOpen);
		if(0X9F == iError)
		{
			iError = SelectIDCard(1001, pucManaInfo, iOpen);
			if(iError == 0x90)
			{
				break;
			}
		}

		Sleep(20);
		i ++;
	}while(i < 100);

	if(i >= 100)
	{
		if(iOpen == 0x00)
			ClosePort();
		return ERROR_SCANNER_LOADDLL;
	}

	iError = ReadBaseMsg(1001, (unsigned char*)ucTemp, &uiCHMsgLen, pucPHMsg, &uiPHMsgLen, iOpen);
//	iError = ReadBaseMsg(1001, (unsigned char*)&stTAG_TEXT_INFO, &uiCHMsgLen, pucPHMsg, &uiPHMsgLen, iOpen);
	if(iError != 0x90)
	{
		if(iOpen == 0x00)
			ClosePort();
		return ERROR_SCANNER_HARDWARE;
	}

	if(iOpen == 0x00)
		ClosePort();

	memcpy((char*)&stTAG_TEXT_INFO, ucTemp, sizeof(stTAG_TEXT_INFO));

	
	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucAddress, -1,  (char*)info.address, sizeof(stTAG_TEXT_INFO.ucAddress) - 1, NULL, NULL );
	for(i = 0; i < strlen((char*)info.address); i++)
	{
		if(info.address[i] == 0x20)
		{
			info.address[i] = 0;
			break;
		}
	}


	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucBirthDay, -1,  (char*)info.birthday, 8, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucExpirEnd, -1,  (char*)info.validterm, 8, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucExpirStart, -1,  (char*)info.signdate, 8, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucID, -1,  (char*)info.number, 18, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucIssue, -1,  (char*)info.department, sizeof(stTAG_TEXT_INFO.ucIssue) - 1, NULL, NULL );
	for(i = 0; i < strlen((char*)info.department); i++)
	{
		if(info.department[i] == 0x20)
		{
			info.department[i] = 0;
			break;
		}
	}

	WideCharToMultiByte( CP_ACP, 0, (USHORT*)stTAG_TEXT_INFO.ucName, -1,  (char*)info.name, sizeof(info.name) - 1, NULL, NULL );
	for(i = 0; i < strlen((char*)info.name); i++)
	{
		if(info.name[i] == 0x20)
		{
			info.name[i] = 0;
			break;
		}
	}

	ucTemp[0] = stTAG_TEXT_INFO.ucNation[0];
	ucTemp[1] = stTAG_TEXT_INFO.ucNation[2];
	ucTemp[2] = 0;
	strcpy((char*)info.nation, GetNationStr((char*)ucTemp));

	if(stTAG_TEXT_INFO.ucSex[0] == '1')
	{
		strncpy((char*)info.sex, "男", 2);
	}
	else
	{
		strncpy((char*)info.sex, "女", 2);
	}


	memset(ucTemp, 0, sizeof(ucTemp));
	strncpy((char*)ucTemp, szHeadImage, strlen(szHeadImage));
	for(i = strlen((char*)ucTemp); i > 0; i--)
	{
		if(szHeadImage[i] == '.')
		{
			ucTemp[i + 1] = 'W';
			ucTemp[i + 2] = 'L';
			ucTemp[i + 3] = 'T';
			ucTemp[i + 4] = 0;
			break;
		}
	}

	FILE *pFile = fopen((char*)ucTemp, "wb");
	if(pFile != NULL)
	{
		fwrite(pucPHMsg, 1024, 1, pFile);
		fclose(pFile);


		try
		{
			GetBmp((char*)ucTemp);

			////////////////强制修改扩展名为JPG，内容未改，给Sfreg使用/////////////////////////////
			char cNewName[1024] = "\0";
			strcpy(cNewName, (const char*)ucTemp);
			char *p = strrchr((const char*)cNewName, '.');
			if(p != NULL)
			{
				strcpy(p, ".JPG");
			}
			
			p = strrchr((const char*)ucTemp, '.');
			if(p != NULL)
			{
				strcpy(p, ".BMP");
			}
			
			
			MoveFile((const char*)ucTemp, cNewName);
			////////////////强制修改扩展名为JPG，内容未改，给Sfreg使用 end/////////////////////////
		}
		catch(...)
		{
		}

	}

	return ERROR_SCANNER_OK;
}

void CScan6::GetErrorMessage(int errCode,char* szErrMsg)
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
