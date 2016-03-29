// Scan5.cpp: implementation of the CScan5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scan5.h"
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan5::CScan5()
{

}

CScan5::~CScan5()
{
	CloseDevice();
}

bool CScan5::IsDeviceValid()
{
	return true;
}

int CScan5::ReadCard(IDENTITY_INFO &info, char *szHeadImage)
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
			
			MoveFileEx((const char*)ucTemp, cNewName, MOVEFILE_REPLACE_EXISTING);
			CopyFile(cNewName, (const char *)ucTemp, FALSE);
			////////////////强制修改扩展名为JPG，内容未改，给Sfreg使用 end/////////////////////////
		}
		catch(...)
		{
		}

	}

	return ERROR_SCANNER_OK;
}

int CScan5::OpenDevice()
{
	return ERROR_SCANNER_OK;
}

int CScan5::CloseDevice()
{
	return ERROR_SCANNER_OK;
}

int CScan5::ScanImage(int /*idType*/, char* /*szImgFile*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

int CScan5::RecogniseImage(int /*idType*/, IDENTITY_INFO& /*info*/, char* /*szHeadImage*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

bool CScan5::IsSupportHeadImage(int /*idType*/)
{
	return true;
}

bool CScan5::IsSupportRecognise(int /*idType*/)
{
	return false;
}

bool CScan5::IsSupportCallBack()
{
	return false;
}

bool CScan5::IsSupportScanner()
{
	return false;
}

int CScan5::SetCallBack(CallBack /*pFunc*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

bool CScan5::IsSupportReader()
{
	return true;
}

void CScan5::GetErrorMessage(int errCode,char* szErrMsg)
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

