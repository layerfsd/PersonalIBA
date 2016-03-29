////////////////////////////////////////////////////////////////////////////////////////
// 
//	Project:    Class ComAccess
//              Overlapped serial IO communication class
//  System:     Win9x WinNT
//	File:       comaccess.h
//	Start date:	17.11.1997
//	Update:     31.07.1998
//	Version:    1.2
//	Author:     Patrick Feuser pat@das-netz.de Germany
//	Copyright ?1997. Alle Rechte vorbehalten
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef _COMACCESS_H_
#define _COMACCESS_H_


#include <windows.h>

#define  COMACCESS GetInstance();
class ComAccess 
{
private:

	HANDLE      m_hCom; // Device handle 

	OVERLAPPED  m_ov;   // A structure that contains informations which are
	                    // used for asynchronous input and output operations

	CHAR       m_lpszErrorMessage[256];
	string      m_MyComName;

	static ComAccess* GetInstance();
private:
	static ComAccess* m_Instance;
public:

	ComAccess(VOID);
	ComAccess(LPCSTR lpszPortNum);

	~ComAccess() ;
			                        // For more definitions see <winbase.h>
	BOOL	Open(LPCSTR lpszPortNum	= "com1",
			     DWORD  dwBaudRate  = CBR_115200, 
			     BYTE   byParity    = NOPARITY,
			     BYTE   byStopBits  = ONESTOPBIT,
			     BYTE   byByteSize  = 8);

	VOID	Close(VOID);
	
	DWORD	WriteData(LPCVOID pdata, DWORD len);
	DWORD	ReadData(LPVOID  pdest, DWORD len, DWORD dwMaxWait = 500);

	LPSTR	GetErrorMessage(VOID) { return m_lpszErrorMessage; }
public:

	BOOL CheckIsMyDevice(vector<string> vec);
	void SendMoneyCommand(char* csOrderNumBer, char* csMoney, char* csPayUrl);
	void SendLogoCommand();

private:
	VOID	ErrorToString(LPCSTR lpszMessage);
	BOOL	IsNT(VOID);

};


#endif // _COMACCESS_H_
