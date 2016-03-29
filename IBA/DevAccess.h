////////////////////////////////////////////////////////////////////////////////////////
// 
//	Project:    Class DevAccess
//              Used for getting devices on the terminal and write&read specified device
//  System:     WinNT
//	File:       DevAccess.h
//	Start date:	2016.03.06
//	Version:    1.0
//	Author:     liyajun91@yahoo.com
//	Copyright 
//  Note:       Need import "setupapi.lib" , Need library "setupapi.lib"&"hid.lib", Headfiles "setupapi.h"&"hidsdi.h"&"hidusage.h"
//
////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <iomanip>
#include <vector>

extern "C" 
{  
#include "setupapi.h" 
#include "hidsdi.h" 
//#include <iosfwd>
}
using namespace std;

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CDevAccess
{
public:
	CDevAccess::CDevAccess();
	CDevAccess::~CDevAccess();

public:
	//获取某类设备(接口类)
	BOOL GetClassDevs(const GUID *InterfaceClassGUID, std::vector<std::tstring>* pvDevPaths/*,std::vector<std::tstring>* pvHardwareIDs*/);
	//对串口设备进行初始化.初始化成功返回TRUE,失败返回FALSE
	BOOL InitPort(const TCHAR* lpszDevPath,UINT baud = 115200, char parity = 'N',UINT databits = 8,UINT stopbits = 1);
	void WriteToDev(const TCHAR* lpszCmd,const TCHAR* lpszDevPath);
	//BOOL ReadDev(TCHAR* lpszOutbuffer,const TCHAR* lpszDevPath);

public:
	BOOL GetReadyState();
	BOOL Reset();
	void ShowLogo();
	void ShowSuccess();
	void CleanDisplay();
	void ShowQrInfo(const TCHAR *lpNumber,const TCHAR *lpPrice,const TCHAR *lpLink);

private:
	string unicode_to_multibyte(const wchar_t* lpUnicode);
	wstring multibyte_to_unicode(const char* lpMultiByte);

private:
	CRITICAL_SECTION m_csSynCommDevice; //控制对设备的访问
	HANDLE m_hDev;//设备句柄
	BOOL m_bDevReady;//设备是否准备好
	std::tstring m_sDevPath;//设备路径
	std::tstring m_sDevLocation;//设备地址(或者设备描述)
};

