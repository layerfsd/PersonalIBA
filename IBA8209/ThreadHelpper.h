#pragma  once

class CThreadHelpper
{
public:
	static void StartThead(IN LPTHREAD_START_ROUTINE lpStartAddress);
	static DWORD CALLBACK GetSysConfig(LPVOID pParam);
};