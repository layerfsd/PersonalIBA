
#pragma once

#define WINNT							0x11

#define WIN9X							0x12

#define EXITWINDOWS_FAILED				0x13

#define EXITWINDOWS_SUCESS				0x14

#define ADJUST_TOCKEN_SUCESS			0x15

#define ADJUST_TOCKEN_FAILED			0x15

#define ADJUST_PRIVILEGE_FAILED			0x16

#define OPENING_PROCESS_TOKEN_FAILED	0x100


class L2_EXT_CLASS CExitWindows
{
private:

	UINT m_nVersionType;
	OSVERSIONINFO m_osvi;

public:
	static	BOOL SetItAsLastShutDownProcess();
	BOOL LogOff();
	BOOL Restart();
	BOOL ShutDown();
	OSVERSIONINFO GetOsVersionInfo()const;
	UINT GetOsVersion();
	BOOL ExitWindowsExt(UINT nFlag, DWORD dwType);
	BOOL ForceShutDown();
	BOOL ForceLogOff();
	BOOL ForceReStart();
	BOOL NTShutDown(void);

	CExitWindows();
	virtual ~CExitWindows();

protected:
	BOOL AdjustProcessTokenPrivilege();
public:
	
};


