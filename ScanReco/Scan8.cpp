

#include "StdAfx.h"
#include "Scan8.h"
#include <stdio.h>
#include   <fstream>   
#include   <string>   
#include <conio.h>

CScan8 *CScan8::m_pThis = NULL;
TCHAR	szExePath[MAX_PATH];
CScan8::CScan8()
{
	m_bOpened = false;
	m_hDll = NULL;
	m_pThis = this;
	m_iRetUSB = 0;

}

CScan8::~CScan8()
{
	CloseDevice();
}


int CScan8::OpenDevice()
{
	int iError = ERROR_SCANNER_OTHER;
	
	if(m_hDll == NULL)
	{

		CString strPath(g_cIniFile);
		int indexr = strPath.ReverseFind('\\');
		strPath.Delete(indexr+1,strPath.GetLength());
		strPath +=_T('\\');
		strPath += "termb.dll";
		m_hDll = LoadLibrary(strPath);
	}
	if (m_hDll!=NULL)
	{
		CVR_InitComm=(lpCVR_InitComm)::GetProcAddress(m_hDll,"CVR_InitComm");				//初始化连接
		CVR_CloseComm=(lpCVR_CloseComm)::GetProcAddress(m_hDll,"CVR_CloseComm");			//关闭连接
		CVR_Authenticate=(lpCVR_Authenticate)::GetProcAddress(m_hDll,"CVR_Authenticate");  //卡认证
		CVR_Read_Content=(lpCVR_Read_Content)::GetProcAddress(m_hDll,"CVR_Read_Content");  //读卡操作
		CVR_ReadBaseMsg=(lpCVR_ReadBaseMsg)GetProcAddress(m_hDll,"CVR_ReadBaseMsg"); 
	}
	int iPort = 1001;
	while(iPort <= 1016)
	{
		m_iRetUSB=CVR_InitComm(iPort);
		if (m_iRetUSB==1)
		{
			m_bOpened = true;
			break;
		}

		iPort++;
	}

	/*int iPort;
	for (iPort=1001; iPort<=1016; iPort++)
	{
		m_iRetUSB=CVR_InitComm(iPort);
		if (m_iRetUSB==1)
		{
			m_bOpened = true;
			break;
		}
	}*/

	return 1;
}


int CScan8::CloseDevice()
{
	static bool bFlag = false;
	m_bOpened = false;
	if(!bFlag && m_bOpened)
	{
		int nRet = CVR_CloseComm();
		if(nRet) bFlag = true;
	}

	if(m_hDll != NULL)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
	
	return ERROR_SCANNER_OK;
}


int CScan8::ScanImage(int idType, char *szImgFile)
{
	CString strImagePath(g_cIniFile);
	int ind = strImagePath.ReverseFind('\\');
	strImagePath.Delete(ind+1,strImagePath.GetLength());
	strImagePath += "\\zp.bmp";
	int nRet = MoveFile(strImagePath,szImgFile);
	return 0;
}

int CScan8::RecogniseImage(int idType, IDENTITY_INFO &info, char *szHeadImage)
{
	return ReadCard(info,szHeadImage);

}

bool CScan8::IsSupportHeadImage(int /*idType*/)
{
	return true;
}

bool CScan8::IsSupportRecognise(int idType)
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

	//return 	bSupport;
	return 0;
}

bool CScan8::IsSupportCallBack()
{
	return false;
}

bool CScan8::IsSupportScanner()
{
	return true;
}
bool CScan8::IsDeviceValid()
{
	return true;
}
int CScan8::SetCallBack(CallBack pFunc)
{
	return 8;
}

void CScan8::GetErrorMessage(int errCode,char* szErrMsg)
{
	if(szErrMsg)
	{
		CString strMsg;
		//FormatErrorMsg(errCode, strMsg);
		strcpy(szErrMsg,strMsg);
	}
}

bool CScan8::IsSupportReader()
{
	return true;
}
void CScan8::HandleFile()
{
	CString strPath(g_cIniFile);
	int indexr = strPath.ReverseFind('\\');
	strPath.Delete(indexr+1,strPath.GetLength());
	strPath += "\\wz.txt";
	DeleteFile(strPath);


	CString strImagePath(g_cIniFile);
	indexr = strImagePath.ReverseFind('\\');
	strImagePath.Delete(indexr+1,strImagePath.GetLength());
	strImagePath += "\\zp.bmp";
	DeleteFile(strImagePath);

}

CString GetPath()
{
	CString strPath(g_cIniFile);
	int indexr = strPath.ReverseFind('\\');
	strPath.Delete(indexr+1,strPath.GetLength());
	return strPath;
}
int CScan8::ReadCard(IDENTITY_INFO &info, char *szHeadImage)  //网吧那边调用这个接口函数读取用户数据
{
	if(!m_bOpened) return 0;
	
	HandleFile();// 删除原来的身份证信息

	//char *pBufText = new char[1024];
	//memset(pBufText,1024,0);
	//char *pBufBmp = new  char[1024];
	//unsigned int nLen1=256, nLen2=1024;
	if  (m_iRetUSB == 1 )
	{
		if (1==CVR_Authenticate())
		{
			if (1!=CVR_Read_Content(4) )
			//if (1!=CVR_ReadBaseMsg((unsigned char*)pBufText, &nLen1, (unsigned char*)pBufBmp, &nLen2, 4))
			{
				//delete []pBufText;
				//delete []pBufBmp;
				//return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	} 


	//MessageBox(NULL,pBufText,"fffff",MB_OK);
	
	CString strPath = GetPath();
	strPath += "wz.txt";
	FILE* fp = _tfopen(strPath,"rb+");
	if(NULL == fp) return 0;
	char buff[1024];
	char temp[1024];
	memset(temp,0,1024);
	memset(buff,0,1024);
	fread(buff,1,1024,fp);
	fclose(fp);
	
	CString str("");
	char szUserInfo[9][50];
	ZeroMemory(szUserInfo,sizeof(szUserInfo));
	ZeroMemory(&info,sizeof(IDENTITY_INFO));
	int j = 0;
	int index = 0;
	for(int i=0;i<1024;i++)
	{
		if(buff[i] == 10)
		{
			int n = strlen(temp);
			strncpy(szUserInfo[index],temp,strlen(temp));
			//szUserInfo[index][strlen(temp)-1]='\0';
			index++;
			memset(temp,0,1024);
			j =0;
		}
		if(buff[i] == 32) continue;
		if(buff[i] == 10) continue;
		if(buff[i] == 13) continue;
		temp[j] = buff[i];
		j++;
	}

	CString strBirthday(szUserInfo[3]);
	strBirthday.Replace("年","");
	strBirthday.Replace("月","");
	strBirthday.Replace("日","");
	_tcscpy(info.birthday,/*szUserInfo[3]*/strBirthday);
	strncpy(info.name,szUserInfo[0],strlen(szUserInfo[0]));
	if(strcmp(szUserInfo[1],"男") == 0) strcpy(info.sex, "0");
	else strcpy(info.sex,"1");
	strncpy(info.nation,szUserInfo[2],strlen(szUserInfo[2]));
	//strncpy(info.birthday,szUserInfo[3],strlen(szUserInfo[3]));
	strncpy(info.address,szUserInfo[4],strlen(szUserInfo[4]));
	strncpy(info.number,szUserInfo[5],strlen(szUserInfo[5]));
	strncpy(info.department,szUserInfo[6],strlen(szUserInfo[6]));
	CString strData(szUserInfo[7]);
	CString strStartDay(strData.Left(strData.Find('-')));
	strStartDay.Replace(".","");
	strStartDay.Replace(".","");
	strStartDay.Replace(".","");
	strData.Delete(0,strData.Find('-')+1);
	CString strEndDay(strData);
	char szStarDay[16],szEndDay[16];
	memcpy(szStarDay,strStartDay,16);
	memcpy(szEndDay,strEndDay,16);
	strncpy(info.signdate,szStarDay,strlen(szStarDay));
	strncpy(info.validterm,szEndDay,strlen(szEndDay));


	// 不再传送头像信息
	CString strImagePath(g_cIniFile); 
	int ind = strImagePath.ReverseFind('\\');
	strImagePath.Delete(ind+1,strImagePath.GetLength());
	strImagePath += "zp.bmp";
	DeleteFile(szHeadImage);


	int nRet = MoveFile(strImagePath,szHeadImage);

	HandleFile();// 删除原来的身份证信息

	return 1;
	
}


