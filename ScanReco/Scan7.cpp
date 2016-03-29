#include "StdAfx.h"
#include "Scan7.h"

#include <stdio.h>

CScan7 *CScan7::m_pThis = NULL;

CScan7::CScan7()
{
	m_bOpened = false;

	m_hDll = NULL;

	memset(m_cImageFilePath, 0, sizeof(m_cImageFilePath));

	memset(m_cColorImageFilePath, 0, sizeof(m_cColorImageFilePath));

	GetTempPath(MAX_PATH, m_cTempPath);

	sprintf(m_cImageFilePath, "%sSCANTEMP.BMP", m_cTempPath);

	sprintf(m_cHeadImageFilePath, "%sHEADTEMP.BMP", m_cTempPath);

	sprintf(m_cColorImageFilePath, "%sSCANCOLTEMP.BMP", m_cTempPath);

	sprintf(m_cColorHeadImageFilePath, "%sHEADCOLTEMP.BMP", m_cTempPath);

	m_pfHasScanner = NULL;

	// 图像识别相关函数
	m_pfCaptureImage = NULL;
	m_pfProcessImage = NULL;
	m_pfGetIDImage_Buff = NULL;
	m_pfGetIDImage_File = NULL;
	m_pfIdentifyCard_Buff = NULL;
	m_pfIdentifyCard_File = NULL;
	m_pfGetIDResult = NULL;

	// 芯片识别
	m_pfStartRFID = NULL;
	m_pfStopRFID = NULL;
	m_pfSearchCard = NULL;
	m_pfSelectCard = NULL;
	m_pfReadCard = NULL;
	m_pfSSReadCard = NULL;

	// 其他操作
	m_pfNotify = NULL;
	m_pfNotifyBeep = NULL;
	m_pfGetVersion = NULL;
	m_pfSetFTLight = NULL;
	m_pfCreateCalibrateData = NULL;
	m_pfUpdateCalibrateData = NULL;
	m_pfCoincideCamera = NULL;

	m_pThis = this;

}

CScan7::~CScan7()
{
	CloseDevice();
}

bool CScan7::IsDeviceValid()
{
	return true;
}

int CScan7::OpenDevice()
{
	int iError = ERROR_SCANNER_OTHER;
	
	if(m_hDll == NULL)
	{
		char cLibDir[MAX_PATH]={0};

		GetPrivateProfileString("SCAN_7", "LIB_DIR", "", cLibDir, sizeof(cLibDir), g_cIniFile);

		int pathLen=strlen(cLibDir);

		if(pathLen > 0 && cLibDir[pathLen-1]!='\\')
		{
			cLibDir[pathLen]='\\';
			pathLen++;
		}

		strcpy(cLibDir+pathLen,"ScanDll.dll");

		m_hDll = LoadLibrary(cLibDir);
	}

	DWORD dwError;
	
	if(m_hDll == NULL)
	{
		dwError = GetLastError();
		return ERROR_SCANNER_LOADDLL;
	}

	m_pfHasScanner = (FUN_IO_HasScanner)::GetProcAddress(m_hDll, "IO_HasScanner");

	// 图像识别相关函数
	m_pfCaptureImage =  (FUN_IO_CaptureImage)::GetProcAddress(m_hDll, "IO_CaptureImage");
	m_pfProcessImage =  (FUN_IO_ProcessImage)::GetProcAddress(m_hDll, "IO_ProcessImage");
	m_pfGetIDImage_Buff =  (FUN_IO_GetIDImage_Buff)::GetProcAddress(m_hDll, "IO_GetIDImage_Buff");
	m_pfGetIDImage_File =  (FUN_IO_GetIDImage_File)::GetProcAddress(m_hDll, "IO_GetIDImage_File");
	m_pfIdentifyCard_Buff = (FUN_IO_IdentifyCard_Buff) ::GetProcAddress(m_hDll, "IO_IdentifyCard_Buff");
	m_pfIdentifyCard_File =  (FUN_IO_IdentifyCard_File)::GetProcAddress(m_hDll, "IO_IdentifyCard_File");
	m_pfGetIDResult =  (FUN_IO_GetIDResult)::GetProcAddress(m_hDll, "IO_GetIDResult");

	// 芯片识别
	m_pfStartRFID =  (FUN_IO_StartRFID)::GetProcAddress(m_hDll, "IO_StartRFID");
	m_pfStopRFID =  (FUN_IO_StopRFID)::GetProcAddress(m_hDll, "IO_StopRFID");
	m_pfSearchCard =  (FUN_IO_SearchCard)::GetProcAddress(m_hDll, "IO_SearchCard");
	m_pfSelectCard =  (FUN_IO_SelectCard)::GetProcAddress(m_hDll, "IO_SelectCard");
	m_pfReadCard =  (FUN_IO_ReadCard)::GetProcAddress(m_hDll, "IO_ReadCard");
	m_pfSSReadCard =  (FUN_IO_SSReadCard)::GetProcAddress(m_hDll, "IO_SSReadCard");

	// 其他操作
	m_pfNotify =  (FUN_IO_Notify)::GetProcAddress(m_hDll, "IO_Notify");
	m_pfNotifyBeep =  (FUN_IO_NotifyBeep)::GetProcAddress(m_hDll, "IO_NotifyBeep");
	m_pfGetVersion =  (FUN_O_GetVersion)::GetProcAddress(m_hDll, "IO_GetVersion");
	m_pfSetFTLight =  (FUN_IO_SetFTLight)::GetProcAddress(m_hDll, "IO_SetFTLight");
	m_pfCreateCalibrateData =  (FUN_IO_CreateCalibrateData)::GetProcAddress(m_hDll, "IO_CreateCalibrateData");
	m_pfUpdateCalibrateData = (FUN_IO_UpdateCalibrateData) ::GetProcAddress(m_hDll, "IO_UpdateCalibrateData");
	m_pfCoincideCamera = (FUN_IO_CoincideCamera) ::GetProcAddress(m_hDll, "IO_CoincideCamera");

	if(IsBadCodePtr( (FARPROC)m_pfHasScanner ) || 
		IsBadCodePtr( (FARPROC)m_pfCaptureImage) || 
		IsBadCodePtr( (FARPROC)m_pfProcessImage) || 
		IsBadCodePtr( (FARPROC)m_pfGetIDImage_Buff) || 
		IsBadCodePtr( (FARPROC)m_pfGetIDImage_File) || 
		IsBadCodePtr( (FARPROC)m_pfGetIDResult) ||
		IsBadCodePtr( (FARPROC) m_pfStartRFID)||
		IsBadCodePtr( (FARPROC) m_pfStopRFID)||
		IsBadCodePtr( (FARPROC) m_pfSearchCard)||
		IsBadCodePtr( (FARPROC) m_pfSelectCard)||
		IsBadCodePtr( (FARPROC) m_pfReadCard)||
		IsBadCodePtr( (FARPROC) m_pfSSReadCard)||
		IsBadCodePtr( (FARPROC) m_pfNotify)||
		IsBadCodePtr( (FARPROC) m_pfNotifyBeep)||
		IsBadCodePtr( (FARPROC) m_pfGetVersion)||
		IsBadCodePtr( (FARPROC) m_pfSetFTLight)||
		IsBadCodePtr( (FARPROC) m_pfCreateCalibrateData)||
		IsBadCodePtr( (FARPROC) m_pfUpdateCalibrateData)||
		IsBadCodePtr( (FARPROC) m_pfCoincideCamera)
		)
	{
		return ERROR_SCANNER_LOADDLL;
	}

	
	HMODULE hUser32 = GetModuleHandle("user32");

	if(NULL == hUser32) return iError;

	GetTaskmanWindow = (PROCGETTASKMANWND)
		GetProcAddress(hUser32,"GetTaskmanWindow");

	if(NULL == GetTaskmanWindow) return iError;

	HWND m_hMainWnd = GetParent(GetTaskmanWindow());

	if(NULL == m_hMainWnd) return iError;

	if(!m_bOpened)
	{
		if(m_pfHasScanner("IDCapture",m_hMainWnd) == 0)
		{
			iError=ERROR_SCANNER_OK;
			m_bOpened = true;
		}
	}

	return iError;
}

int CScan7::CloseDevice()
{

	m_bOpened = false;

	if(m_hDll != NULL)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}

	return ERROR_SCANNER_OK;
}


int CScan7::ScanImage(int idType, char *szImgFile)
{

	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	SYSCAN_RST Rst = {0};

	int iError = -1;

	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
		    iError = m_pfGetIDResult(FIRST_IDCARD,m_cImageFilePath,szImgFile,
			   m_cHeadImageFilePath,m_cColorHeadImageFilePath,100,&Rst);
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{

			iError = m_pfGetIDResult(SECOND_IDCARD,m_cImageFilePath,szImgFile,
									m_cHeadImageFilePath,m_cColorHeadImageFilePath,100,&Rst);

		}
		break;

	case IDENTITY_PASSPORT:// 94 护照
		{
			//iError = Get_MiscPic(3, m_cImageFilePath);
			//if(iError == 0)
			//{
			//	BmpToJpg(m_cImageFilePath, szImgFile);
			//}
		}

		break;

	case IDENTITY_TAIBAO:// 95 台胞证
		{
			//iError = m_pfGetIDResult(TAIBAO_CARD,m_cImageFilePath,szImgFile,
			//	m_cHeadImageFilePath,m_cColorHeadImageFilePath,100,&Rst);
		}
		break;

	default:

		break;
	}

	return iError==0 ? ERROR_SCANNER_OK : ERROR_SCANNER_OTHER;
}

int CScan7::RecogniseImage(int idType, IDENTITY_INFO &info, char *szHeadImage)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	int iError = -1;

	SYSCAN_RST Rst = {0};


	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{

            iError = m_pfGetIDResult(FIRST_IDCARD,m_cImageFilePath,szHeadImage,
				m_cHeadImageFilePath,m_cColorHeadImageFilePath,100,&Rst);
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{
			iError = m_pfGetIDResult(SECOND_IDCARD,m_cImageFilePath,szHeadImage,
				m_cHeadImageFilePath,m_cColorHeadImageFilePath,100,&Rst);
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
		if(idType == IDENTITY_ID1 || idType == IDENTITY_ID2)
		{
			strncpy(info.name, Rst.name, 20);     //姓名
			strncpy(info.sex,  Rst.sex, 2);        //性别
			strncpy(info.number, Rst.number, 40);   //证件编码
			strncpy(info.nation, Rst.people, 20);    //民族
			strncpy(info.birthday, Rst.birthday, 10);  //出生日期
			strncpy(info.address, Rst.address, 200);  //地址
			strncpy(info.signdate, Rst.signdate, 10);  //签发日期
			strncpy(info.validterm, Rst.validterm, 30);  //有效期
			strncpy(info.department, Rst.organs, 40); //发证机关
		}

		if(m_pfNotify)
			m_pfNotify(500);

		return ERROR_SCANNER_OK;
	}

	return ERROR_SCANNER_OTHER;
}

bool CScan7::IsSupportHeadImage(int /*idType*/)
{
	return true;
}

bool CScan7::IsSupportRecognise(int idType)
{
	bool bSupport = false;

	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
			bSupport = true;
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{
			bSupport = true;
		}
		break;

	case IDENTITY_STUDENT:// 15 学生证
		{
		}
		break;

	case IDENTITY_OFFICER:// 90 军官证
		{
			//bSupport = true;
		}
		break;

	case IDENTITY_POLICE:// 91 警官证
		{

		}
		break;

	case IDENTITY_SOLDIER:// 92 士兵证
		{
			//bSupport = true;
		}
		break;

	case IDENTITY_HUKOUBU:// 93 户口簿
		{

		}
		break;

	case IDENTITY_PASSPORT:// 94 护照
		{
		}
		break;

	case IDENTITY_TAIBAO:// 95 台胞证
		{
			//bSupport = true;
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

bool CScan7::IsSupportCallBack()
{
	return false;
}

bool CScan7::IsSupportScanner()
{
	return true;
}

int CScan7::SetCallBack(CallBack /*pFunc*/)
{
	return 8;
}

void CScan7::GetErrorMessage(int errCode,char* szErrMsg)
{
	if(szErrMsg)
	{
		CString strMsg;
		FormatErrorMsg(errCode, strMsg);
		strcpy(szErrMsg,strMsg);
	}
}

bool CScan7::IsSupportReader()
{

	return true;
}

int CScan7::ReadCard(IDENTITY_INFO &info, char *szHeadImage)
{
	if(!IsSupportReader())
	{
		return ERROR_SCANNER_HARDWARE;
	}

	if (NULL == m_pfSSReadCard || NULL == m_pfStartRFID || NULL == m_pfStopRFID ||
		NULL == m_pfGetIDResult)
	{
		return ERROR_SCANNER_OTHER;
	}

			
	SYSCAN_RST Rst = {0};

	int iRes = -1;

	if(m_pfStartRFID())
	{
		iRes = m_pfSSReadCard(&Rst);

		m_pfStopRFID();

		if(0 == iRes)
		{
			//拷贝信息
			strncpy(info.name, Rst.name, 20);     //姓名
			strncpy(info.sex,  Rst.sex, 2);        //性别
			strncpy(info.number, Rst.number, 40);   //证件编码
			strncpy(info.nation, Rst.people, 20);    //民族
			strncpy(info.birthday, Rst.birthday, 10);  //出生日期
			strncpy(info.address, Rst.address, 200);  //地址
			strncpy(info.signdate, Rst.signdate, 10);  //签发日期
			strncpy(info.validterm, Rst.validterm, 30);  //有效期
			strncpy(info.department, Rst.organs, 40); //发证机关


			//删除临时照片
			try
			{
				DeleteFile(szHeadImage);

				//拷贝照片到指定目录
				MoveFile(".\\Picture.bmp",szHeadImage);

				Sleep(500);

				DeleteFile(".\\Picture.bmp");
			}
			catch (...)
			{

			}

			if(m_pfNotify)
			    m_pfNotify(500);

			return ERROR_SCANNER_OK;
		}
	}


	return ERROR_SCANNER_OTHER;
}

void CScan7::FormatErrorMsg(int iErrorCode,CString strErrorMsg)
{
	switch(iErrorCode)
	{
	case -2000:
		strErrorMsg="链接失败/设备未链接";
		break;
	case -2001:
		strErrorMsg="WHITE或者IR缓冲区长度不够";
		break;
	case -2002:
		strErrorMsg="获取图像数据失败";
		break;
	case -2003:
		strErrorMsg="处理失败";
		break;
	case -2004:
		strErrorMsg="寻卡失败";
		break;
	case -2005:
		strErrorMsg="选卡失败";
		break;
	case -2006:
		strErrorMsg="读卡失败";
		break;
	case -2007:
		strErrorMsg="前置灯设置失败";
		break;
	case -2008:
		strErrorMsg="获取设备信息失败";
		break;
	case -2009:
		strErrorMsg="从读取到的RFID数据中提取图像失败";
		break;
	case -2010:
		strErrorMsg="初始化OCR模块失败";
		break;
	case -2011:
		strErrorMsg="OCR识别失败";
		break;
	case -2012:
		strErrorMsg="装载失败";
		break;
	case -2013:
		strErrorMsg="生成校准用模板失败";
		break;
	case -3000:
		strErrorMsg="未知错误";
		break;
	case -0:
		strErrorMsg="操作成功";
		break;
	default:
		strErrorMsg = "错误代码未定义";
	}

}


//
//int CScan7::SaveImg(long iCardType)
//{
//	if(m_pfCaptureImage == NULL || NULL == m_pfProcessImage ) return -1;
//
//	BYTE *pWh = new BYTE [1024*1024*10];
//	long lwhSize =1024*1024*10;
//	memset(pWh,0,lwhSize);
//
//	BYTE *pIR = new BYTE [1024*1024*10];
//	long lIRSize =1024*1024*10;
//	memset(pIR,0,lIRSize);
//
//	BYTE *pImgBuf = new BYTE[1024*1024*10];
//	long lImgSize = 1024*1024*10;
//	memset(pImgBuf,0,lImgSize);
//
//	int iRes = -1;
//
//	try
//	{
//		iRes = m_pfCaptureImage(pIR,lIRSize,pWh,lwhSize);
//		if(0 == iRes)
//		{
//			long lWidth = 1280;
//			long lHeigth =1024;
//            iRes = m_pfProcessImage(pIR,SCAN_COLOR_MODE,iCardType,pImgBuf,&lWidth,&lHeigth);
//			if (0 == iRes)
//			{
//				iRes = m_pfGetIDImage_File(SCAN_COLOR_MODE,iCardType,100,m_cColorImageFilePath);
//				if (0 == iRes)
//				{
//					return 0;
//				}
//			}
//		}
//
//	}
//	catch (...)
//	{
//
//	}
//
//	delete pWh;
//
//	delete pIR;
//
//	delete pImgBuf;
//
//	return -1;
//
//}

