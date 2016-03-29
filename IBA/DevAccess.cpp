#include "stdafx.h"
#include "DevAccess.h"
#include <tchar.h>
#include <winbase.h>
#include <iostream>
#include <winioctl.h>
#include "IBA_DEBUG.h"
#include "ibalog.h"
using namespace std;

//构造函数 - 1.获取设备 2.判断是否是想要的串口设备 3.初始化串口设备
CDevAccess::CDevAccess()
{
	vector<tstring> DevPaths;
	//vector<tstring> HardwareIDs;
	m_hDev = NULL;
	m_bDevReady = FALSE;
	UINT nDevIdx = 0;

	//获取所有COMPORT接口类设备	
	if(GetClassDevs(&GUID_DEVINTERFACE_COMPORT,&DevPaths/*,&HardwareIDs*/))
	{
		for(int i = 0;i < DevPaths.size(); ++i)
		{
			//判断pid和vid是否相同
			TCHAR lpDevPath[256];
			_tcscpy(lpDevPath,DevPaths[i].c_str());
			_tcsupr(lpDevPath);
			IBA_LOG(_T("Find:%d:DevPath:%s"),i,DevPaths[i].c_str());
			TCHAR *p = _tcsstr(lpDevPath,_T("VID_067B&PID_2303"));			
			if(NULL != p)
			//if(0 == _tcscmp(lpHardwareID,_T("USB\\Vid_067b&Pid_2303&Rev_0400")))//与USB-Serial Controller D比较
			{
				IBA_LOG(_T("Matched"));
				//判断发送特定字符后的接收字符是否一样
				//先初始化
				if(InitPort(DevPaths[i].c_str()))
				{
					IBA_LOG(_T("InitCommOk"));
					//暂时增加这一行.因为硬件有时候不发送数据
					m_bDevReady = TRUE;
					nDevIdx = i;
					break;

					/*OVERLAPPED ov;
					RtlZeroMemory(&ov,sizeof(OVERLAPPED));
					ov.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

					EnterCriticalSection(&m_csSynCommDevice);

					//写入特定字符
					//char lpWriteBuffer[] = "Mutil:\tA1\t总价100\thttp://www.dodonew.com";
					char lpWriteBuffer[] = "Logo";
					DWORD dwSize = 0;
					BOOL bSuccess = WriteFile(m_hDev,lpWriteBuffer,(DWORD)(strlen(lpWriteBuffer)),&dwSize,&ov);
					if(FALSE == bSuccess)
					{
						if(ERROR_IO_PENDING == GetLastError())
						{
							WaitForSingleObject(ov.hEvent,1000);
							IBA_LOG(_T("WriteWaitedObject"));
						}
					}
					//读回字符
					OVERLAPPED ov_read;
					RtlZeroMemory(&ov_read,sizeof(OVERLAPPED));
					ov_read.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
					char lpRecvBuffer[100];
					RtlZeroMemory(lpRecvBuffer,sizeof(lpRecvBuffer));
					dwSize = 0;
					if(FALSE == ReadFile(m_hDev,lpRecvBuffer,100,&dwSize,&ov_read))
					{
						if(ERROR_IO_PENDING == GetLastError())
						{
							WaitForSingleObject(ov_read.hEvent,1000);
							IBA_LOG(_T("ReadwaitedObject"));
						}
					}
					IBA_LOG(_T("Recved:%s"),lpRecvBuffer);
					if(0 == strcmp(lpRecvBuffer,lpWriteBuffer)) //字符相同则判断为所需设备
					{
						IBA_LOG(_T("Recved reponse.Ready!"));
						m_bDevReady = TRUE;
					}*/

				/*	OVERLAPPED ov_write;
					RtlZeroMemory(&ov_write,sizeof(OVERLAPPED));
					ov_write.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
					char lpTmp[] = "Logo";
					dwSize = 0;
					bSuccess = WriteFile(m_hDev,lpTmp,(DWORD)(strlen(lpTmp)),&dwSize,&ov_write);
					if(FALSE == bSuccess)
					{
						if(ERROR_IO_PENDING == GetLastError())
						{
							WaitForSingleObject(ov_write.hEvent,1000);
						}
					}
					LeaveCriticalSection(&m_csSynCommDevice);*/
				}
			}
		}

	}

	if(FALSE == m_bDevReady && NULL != m_hDev)
	{
		CloseHandle(m_hDev);
	}
	if(GetReadyState())
	{
		m_sDevPath = DevPaths[nDevIdx];
		//m_sDevLocation = HardwareIDs[0];
	}
}

//析构函数
CDevAccess::~CDevAccess()
{
	if(NULL != m_hDev)
	{
		CloseHandle(m_hDev);
	}
}

//获取某个设备接口类中所有可用设备的路径，该路径可用于CreateFile
BOOL CDevAccess::GetClassDevs(const GUID *InterfaceClassGUID, std::vector<std::tstring>* pvDevPaths/*,std::vector<std::tstring>* pvHardwareIDs*/)
{
	if(NULL == InterfaceClassGUID || NULL == pvDevPaths /*|| NULL == pvHardwareIDs*/){return FALSE;}
	pvDevPaths->clear();
	//pvHardwareIDs->clear();

	//获取device information set
	HDEVINFO hDevInfo =  SetupDiGetClassDevs(InterfaceClassGUID,NULL,NULL,DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if(INVALID_HANDLE_VALUE == hDevInfo)
	{
		return FALSE;
	}

	//枚举device interfaces
	DWORD nMemberIndex = 0;
	SP_DEVICE_INTERFACE_DATA device_interface;
	RtlZeroMemory(&device_interface,sizeof(SP_DEVICE_INTERFACE_DATA));
	device_interface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	while(SetupDiEnumDeviceInterfaces(hDevInfo,NULL,InterfaceClassGUID,nMemberIndex,&device_interface))
	{
		nMemberIndex++;

		TCHAR DevPath[256];
		RtlZeroMemory(DevPath,sizeof(DevPath));

		//获取device_interface_detail所需空间
		SP_DEVINFO_DATA device_information_element;
		RtlZeroMemory(&device_information_element,sizeof(SP_DEVINFO_DATA));
		device_information_element.cbSize = sizeof(SP_DEVINFO_DATA);

		DWORD nSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo,&device_interface,NULL,NULL,&nSize,NULL);

		//第二次获取detail
		PSP_DEVICE_INTERFACE_DETAIL_DATA pdev_interface_detail;
		pdev_interface_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)calloc(1,nSize);
		pdev_interface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if(FALSE == SetupDiGetDeviceInterfaceDetail(hDevInfo,&device_interface,pdev_interface_detail,nSize,NULL,&device_information_element)){break;} 

		_tcscpy(DevPath,pdev_interface_detail->DevicePath);
		free(pdev_interface_detail); //千万记得释放空间
		std::tstring sDevPath = DevPath;

		IBA_LOG(_T("Get:%d:%s"),(nMemberIndex-1),sDevPath.c_str());
		//从注册表获取地址信息
		/*TCHAR lpszTmp[512];
		RtlZeroMemory(lpszTmp,sizeof(lpszTmp));
		DWORD dataType;
		DWORD dwBuffersize = sizeof(lpszTmp);
		if (FALSE == SetupDiGetDeviceRegistryProperty(hDevInfo, &device_information_element, SPDRP_HARDWAREID, &dataType, (LPBYTE)lpszTmp, dwBuffersize, 0))
		{
			break;
		}
		std::tstring sHardwareID = lpszTmp;
		SetupDiDestroyDeviceInfoList(hDevInfo);*/

		//保存到vector中
		pvDevPaths->push_back(sDevPath);
		//pvHardwareIDs->push_back(sHardwareID);
	}

	//获取到设备,返回true.否则返回false
	if(pvDevPaths->size()){return TRUE;}

	return FALSE;
}

BOOL CDevAccess::InitPort(const TCHAR* lpszDevPath,UINT baud, char parity,UINT databits,UINT stopbits)
{
	//先打开设备
	HANDLE hDevice = CreateFile(lpszDevPath,           // pointer to name of the file
		GENERIC_READ|GENERIC_WRITE, // 读写
		0,                     // 独占模式comm devices must be opened w/exclusive-access 
		NULL,                  // no security attributs 
		OPEN_EXISTING,         // 只打开现有存在的
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,  // 异步方式打开(若是同步就不需要overlapped结构)
		NULL);                 // open_existing时需为NULL
	if(INVALID_HANDLE_VALUE == hDevice){return FALSE;}

	InitializeCriticalSection(&m_csSynCommDevice);
	EnterCriticalSection(&m_csSynCommDevice);

	//设置超时参数
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 1000;        
	CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	CommTimeouts.ReadTotalTimeoutConstant = 1000;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	CommTimeouts.WriteTotalTimeoutConstant = 1000;
	if (FALSE == SetCommTimeouts(hDevice, &CommTimeouts)){return FALSE;}

	//设置监控事件
	DWORD dwCommEvents = EV_RXCHAR;//设备读取到字符的事件
	if(FALSE == SetCommMask(hDevice,dwCommEvents)){return FALSE;}

	//设置其他
	DCB dcb;
	RtlZeroMemory(&dcb,sizeof(DCB));
	if(FALSE == GetCommState(hDevice,&dcb)){return FALSE;}
	dcb.EvtChar = 'q';
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	char szBaud[128];
	RtlZeroMemory(szBaud,sizeof(szBaud));
	sprintf(szBaud,"baud=%d parity=%c data=%d stop=%d",baud, parity, databits, stopbits);
	if(FALSE == BuildCommDCBA(szBaud,&dcb)){return FALSE;}

	if(FALSE == SetCommState(hDevice,&dcb)){return FALSE;}

	//flush设备缓冲和等待读写操作
	if(FALSE == PurgeComm(hDevice,PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR)){return FALSE;}

	//完成初始化,释放CS
	LeaveCriticalSection(&m_csSynCommDevice);
	
	m_hDev = hDevice;

	return TRUE;
}

BOOL CDevAccess::GetReadyState()
{
	return m_bDevReady;
}

BOOL CDevAccess::Reset()
{
	if(GetReadyState())
	{
		CloseHandle(m_hDev);
		TCHAR lpBuffer[256];
		_tcscpy(lpBuffer,m_sDevPath.c_str());
		if(InitPort(lpBuffer))
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CDevAccess::ShowLogo()
{
	if(GetReadyState())
	{
		WriteToDev(_T("Logo"),m_sDevPath.c_str());
	}	
}

void CDevAccess::ShowSuccess()
{
	if(GetReadyState())
	{
		WriteToDev(_T("Success"),m_sDevPath.c_str());
	}
}

void CDevAccess::CleanDisplay()
{
	if(GetReadyState())
	{
		WriteToDev(_T("Clean"),m_sDevPath.c_str());
	}
}

void CDevAccess::ShowQrInfo(const TCHAR *lpNumber,const TCHAR *lpPrice,const TCHAR *lpLink)
{
	if(GetReadyState())
	{
		TCHAR lpBuff[256];
		_stprintf(lpBuff,_T("Mutil:\tNO.:%s\t充值:%s\t%s"),lpNumber,lpPrice,lpLink);
		WriteToDev(lpBuff,m_sDevPath.c_str());
	}
}

void CDevAccess::WriteToDev(const TCHAR* lpszCmd,const TCHAR* lpszDevPath)
{
	if(FALSE == GetReadyState())
	{
		return;
	}

	OVERLAPPED ov;
	RtlZeroMemory(&ov,sizeof(OVERLAPPED));
	ov.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	EnterCriticalSection(&m_csSynCommDevice);
	//PurgeComm(m_hDev,PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR);

	char lpBuffer[120];
	RtlZeroMemory(lpBuffer,sizeof(lpBuffer));
	//先转换字符串
#ifdef _UNICODE
	string sTmp = unicode_to_multibyte(lpszCmd);
	strcpy(lpBuffer,sTmp.c_str());
	lpBuffer[119] = '\0'; //在120位置处截断，防止写入数据太长
#else
	strcpy(lpBuffer,lpszCmd);
#endif
	DWORD dwSize = 0;
	BOOL bSuccess = WriteFile(m_hDev,lpBuffer,(DWORD)(strlen(lpBuffer)),&dwSize,&ov);
	if(FALSE == bSuccess)
	{
		if(ERROR_IO_PENDING == GetLastError())
		{
			WaitForSingleObject(ov.hEvent,1000);
		}
	}	

	LeaveCriticalSection(&m_csSynCommDevice);

	return;
}

//BOOL CDevAccess::ReadDev(TCHAR* lpszOutbuffer,const TCHAR* lpszDevPath)
//{
//	//缓冲区需大于120，如何判断？
//
//	//先打开设备
//	HANDLE hDevice = CreateFile(lpszDevPath,           // pointer to name of the file
//		GENERIC_READ|GENERIC_WRITE, // 读写
//		0,                     // 独占模式comm devices must be opened w/exclusive-access 
//		NULL,                  // no security attributs 
//		OPEN_EXISTING,         // 只打开现有存在的
//		FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/,  // 异步方式打开(若是同步就不需要overlapped结构)
//		NULL);                 // open_existing时需为NULL
//	if(INVALID_HANDLE_VALUE == hDevice){return FALSE;}
//	
//	EnterCriticalSection(&m_csSynCommDevice);
//
//	char lpBuffer[256];
//	RtlZeroMemory(lpBuffer,sizeof(lpBuffer));
//	DWORD dwSize = 0;
//	BOOL bSuccess = ReadFile(hDevice,lpBuffer,100,&dwSize,NULL);
//
//	LeaveCriticalSection(&m_csSynCommDevice);
//	CloseHandle(hDevice);
//
//	if(bSuccess)
//	{
//#ifdef _UNICODE
//		wstring sTmp = multibyte_to_unicode(lpBuffer);
//		wcscpy(lpszOutbuffer,sTmp.c_str());
//#else
//		strcpy(lpszOutbuffer,sTmp.c_str());
//#endif
//		return TRUE;
//	}
//
//	return FALSE;
//}

//转换unicode到multibyte
std::string CDevAccess::unicode_to_multibyte(const wchar_t* lpUnicode)
{
	wstring str(lpUnicode);

	int  MultiLen = ::WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);  

	char * lpMulti = new  char[MultiLen+1];  

	memset(lpMulti,0,(MultiLen+1)*sizeof(char));  

	::WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPSTR)lpMulti,
		MultiLen ,
		NULL,
		NULL); 

	string strRet(lpMulti);

	delete[] lpMulti;

	return strRet;
}

//转换unicode到multibyte
wstring CDevAccess::multibyte_to_unicode(const char* lpMultiByte)
{
	string str(lpMultiByte);

	int  UnicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		//0,
		//NULL,
		NULL);  

	wchar_t * lpUnicode = new  wchar_t[UnicodeLen+1];  

	memset(lpUnicode,0,(UnicodeLen+1)*sizeof(wchar_t));  

	::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)lpUnicode,
		UnicodeLen /*,
		NULL,
		NULL*/); 

	wstring strRet(lpUnicode);

	delete[] lpUnicode;

	return strRet;
}