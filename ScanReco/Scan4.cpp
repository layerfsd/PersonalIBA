// Scan4.cpp: implementation of the CScan4 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scan4.h"
#include <stdlib.h>
//#pragma comment(lib, "1/CardAll.lib")


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan4::CScan4()
{
	memset(&m_stProcessInfo, 0, sizeof(m_stProcessInfo));
	m_hCompleteEvent = ::CreateEvent(NULL, TRUE, FALSE, "CHWConnectDlg_SCAN_COMPLETE_FALG");
	if(m_hCompleteEvent == NULL)
	{
		MessageBox(NULL, "Create Event falied!", "Error:", 0);
		PostQuitMessage(0);
	}

	m_hScanPicCompleteEvent = ::CreateEvent(NULL, TRUE, FALSE, "CHWConnectDlg_SCAN_PIC_COMPLETE_FALG");
	if(m_hScanPicCompleteEvent == NULL)
	{
		MessageBox(NULL, "Create Scan PIC Event falied!", "Error:", 0);
		PostQuitMessage(0);
	}
}

CScan4::~CScan4()
{
	CloseDevice();
}

bool CScan4::IsDeviceValid()
{
	return true;
}

int CScan4::OpenDevice()
{
	m_hCompleteEvent = ::CreateEvent(NULL, TRUE, FALSE, "CHWConnectDlg_SCAN_COMPLETE_FALG");
	if(m_hCompleteEvent == NULL)
	{
		MessageBox(NULL, "Create Event falied!", "Error:", 0);
		PostQuitMessage(0);
	}

	m_hScanPicCompleteEvent = ::CreateEvent(NULL, TRUE, FALSE, "CHWConnectDlg_SCAN_PIC_COMPLETE_FALG");
	if(m_hScanPicCompleteEvent == NULL)
	{
		MessageBox(NULL, "Create Scan PIC Event falied!", "Error:", 0);
		PostQuitMessage(0);
	}

	if(StartupHWConnect())
	{
		Sleep(500);
		//if(SendHWCmd(CMD_CALIBRATE, 0))//校正扫描器
		{
			//Sleep(3000);
			//if(SendHWCmd(CMD_INIT, 0))//初始化扫描器
			{
				m_bOpened = TRUE;
				return ERROR_SCANNER_OK;
			}
		}
	}

	return ERROR_SCANNER_OTHER;
}

int CScan4::CloseDevice()
{
	if(m_bOpened)
	{
		DWORD dwPID = 0;
		HWND hSendHwnd = ::FindWindow(NULL, "HW.Connect..._...");
		if(IsWindow(hSendHwnd))
		{
			GetWindowThreadProcessId(hSendHwnd, &dwPID);
			HANDLE h = OpenProcess(PROCESS_TERMINATE, TRUE, dwPID);
			if(h != NULL)
			{
				TerminateProcess(h, 0);
			}
		}

		if(m_stProcessInfo.hProcess != NULL)
		{
			TerminateProcess(m_stProcessInfo.hProcess, 0);
			memset(&m_stProcessInfo, 0, sizeof(m_stProcessInfo));
		}

		ResetEvent(m_hCompleteEvent);
		CloseHandle(m_hCompleteEvent);
		m_hCompleteEvent = NULL;

		ResetEvent(m_hScanPicCompleteEvent);
		CloseHandle(m_hScanPicCompleteEvent);
		m_hScanPicCompleteEvent = NULL;
		m_bOpened = FALSE;
	}

	return ERROR_SCANNER_OK;
}

int CScan4::ScanImage(int idType, char *szImgFile)
{
	ResetEvent(m_hCompleteEvent);

	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	switch(idType)
	{
	case IDENTITY_ID1:// 10 一代身份证
		{
		}
		break;

	case IDENTITY_ID2:// 11 二代身份证
		{
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

	SendHWCmd(CMD_INIT, (unsigned long)szImgFile);//初始化扫描器
	Sleep(100);
	if(SendHWCmd(CMD_SCAN, idType))//扫描
	{
		Sleep(4000);
		//char cBuf[128] = "\0";
		WaitForSingleObject(m_hScanPicCompleteEvent, 20000);
		//sprintf(cBuf, "EventHandle: %X Ret:%X Error:%d", m_hScanPicCompleteEvent, dw, GetLastError());
		//MessageBox(NULL, cBuf, "", 0);
		return ERROR_SCANNER_OK;
	}

	return ERROR_SCANNER_OTHER;
}

int CScan4::RecogniseImage(int /*idType*/, IDENTITY_INFO &info, char* /*szHeadImage*/)
{
	if(!m_bOpened)
	{
		return ERROR_SCANNER_HARDWARE;
	}

	WaitForSingleObject(m_hCompleteEvent, 20000);

	char cPath[MAX_PATH+1] = "\0", cTemp[128] = "\0";
	GetModuleFileName((HINSTANCE)NULL, cPath, MAX_PATH);
	char *p = strrchr(cPath, '\\');
	if(p != NULL)
	{
		strcpy(p + 1, "HWConnect.INI");
	}
//MessageBox(NULL, cPath, "path:", 0);
	GetPrivateProfileString("ScanValue", "ID", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.number, cTemp, 40);   //证件编码
//MessageBox(NULL, info.number, "Num:", 0);

	GetPrivateProfileString("ScanValue", "Name", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.name, cTemp, 20);     //姓名
	
	GetPrivateProfileString("ScanValue", "Sex", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.sex, cTemp, 2);        //性别
	
	GetPrivateProfileString("ScanValue", "Nation", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.nation, cTemp, 20);    //民族
	
	GetPrivateProfileString("ScanValue", "BirthDay", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.birthday, cTemp, 10);  //出生日期
	
	GetPrivateProfileString("ScanValue", "Address", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.address, cTemp, 200);  //地址
	
	GetPrivateProfileString("ScanValue", "CarType", "", cTemp, sizeof(cTemp), cPath);
	
	GetPrivateProfileString("ScanValue", "IDSignDate", "", cTemp, sizeof(cTemp), cPath);
	strncpy(info.signdate, cTemp, 10);  //签发日期

	strncpy(info.validterm, cTemp, 30);  //有效期
	strncpy(info.department, "", 40); //发证机关
	
	ResetEvent(m_hCompleteEvent);

	return ERROR_SCANNER_OK;
}

bool CScan4::IsSupportHeadImage(int /*idType*/)
{
	return false;
}

bool CScan4::IsSupportRecognise(int idType)
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

int CScan4::SetCallBack(CallBack /*pFunc*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

bool CScan4::IsSupportCallBack()
{
	return false;
}

bool CScan4::IsSupportReader()
{
	return false;
}

bool CScan4::IsSupportScanner()
{
	return true;
}

int CScan4::ReadCard(IDENTITY_INFO& /*info*/, char* /*szHeadImage*/)
{
	return ERROR_SCANNER_UNSUPPORT;
}

BOOL CScan4::StartupHWConnect()
{
    STARTUPINFO si;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &m_stProcessInfo, sizeof(m_stProcessInfo) );

    // Start the child process. 
    if( !CreateProcess(NULL, // No module name (use command line). 
        "HWConnect.exe", // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &m_stProcessInfo )             // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
		return FALSE;
    }

    // Wait until child process exits.
    //WaitForSingleObject( m_stProcessInfo.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( m_stProcessInfo.hProcess );
    CloseHandle( m_stProcessInfo.hThread );

	return TRUE;
}

BOOL CScan4::SendHWCmd(USHORT usCmd, DWORD usIDType)
{
	typedef struct tag_CMD
	{
		unsigned short usCMD;
		unsigned char  ucDataBuf[512];
		
	}TAG_CMD;
	COPYDATASTRUCT stSend = {0};


	TAG_CMD stCmd = {0};
	stCmd.usCMD = usCmd;

	switch(usCmd)
	{
	case CMD_CALIBRATE://校正扫描器
		{
		}
		break;

	case CMD_INIT://初始化
		{
			strcpy((char*)&stCmd.ucDataBuf, (char*)usIDType);
		}
		break;

	case CMD_SCAN://调用时注意ucDataBuf中保存必须USHORT类型的证件类型
		{
			memcpy(&stCmd.ucDataBuf, &usIDType, sizeof(USHORT));
		}
		break;

	case CMD_GetVALUE://扩展使用，目前未用
		{
		}
		break;

	default:
		;
	}

	stSend.cbData = sizeof(TAG_CMD);
	stSend.dwData = CMD_HOSTID;
	stSend.lpData = &stCmd;
	
	HWND hSendHwnd = ::FindWindow(NULL, "HW.Connect..._...");
	if(IsWindow(hSendHwnd))
	{
		::SendMessage( hSendHwnd,
			WM_COPYDATA,
			(WPARAM)(HWND)NULL,//m_hWnd,
			(LPARAM) (LPVOID) &stSend );
		return TRUE;
	}

	return FALSE;
}

void CScan4::GetErrorMessage(int errCode,char* szErrMsg)
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
