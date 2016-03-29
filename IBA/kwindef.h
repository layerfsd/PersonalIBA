#ifndef KWINDEF_H_
#define KWINDEF_H_


/*
* you'll find a list of NTSTATUS status codes in the DDK header
* ntstatus.h (\WINDDK\2600.1106\inc\ddk\wxp\)
*/
#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

/*
*************************************************************************
* ntddk.h
*/
typedef LONG    NTSTATUS;
typedef ULONG    ACCESS_MASK;
/*
* ntdef.h
*************************************************************************
*/

/*
*************************************************************************
* <<Windows NT/2000 Native API Reference>> - Gary Nebbett
*/

typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

/*
*Information Class 16
*/
typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG            ProcessId;
    UCHAR            ObjectTypeNumber;
    UCHAR            Flags;
    USHORT            Handle;
    PVOID            Object;
    ACCESS_MASK        GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _MEMORY_CHUNKS {
    ULONG Address;
    PVOID Data;
    ULONG Length;
}MEMORY_CHUNKS, *PMEMORY_CHUNKS;

typedef struct _THREAD_INFO
{
	LARGE_INTEGER CreateTime;
	DWORD dwUnknown1;
	DWORD dwStartAddress;
	DWORD StartEIP;
	DWORD dwOwnERPID;
	DWORD dwThreadId;
	DWORD dwCurrentPriority;
	DWORD dwBasePriority;
	DWORD dwContextSwitches;
	DWORD Unknown;
	DWORD WaitReason;

}THREADINFO, *PTHREADINFO;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaxLength;
	PWSTR Buffer;
} UNICODE_STRING;


typedef struct _PROCESS_INFO
{
	DWORD dwOffset;
	DWORD dwThreadsCount;
	DWORD dwUnused1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;

	DWORD dwBasePriority;
	DWORD dwProcessID;
	DWORD dwParentProcessId;
	DWORD dwHandleCount;
	DWORD dwUnused3[2];

	DWORD dwVirtualBytesPeak;
	DWORD dwVirtualBytes;
	ULONG dwPageFaults;
	DWORD dwWorkingSetPeak;
	DWORD dwWorkingSet;
	DWORD dwQuotaPeakPagedPoolUsage;
	DWORD dwQuotaPagedPoolUsage;
	DWORD dwQuotaPeakNonPagedPoolUsage;
	DWORD dwQuotaNonPagedPoolUsage;
	DWORD dwPageFileUsage;
	DWORD dwPageFileUsagePeak;

	DWORD dCommitCharge;
	THREADINFO ThreadSysInfo[1];

} PROCESSINFO, *PPROCESSINFO;

typedef NTSTATUS ( __stdcall *ZWQUERYSYSTEMINFORMATION ) ( IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
														  IN OUT PVOID SystemInformation,
														  IN ULONG SystemInformationLength, 
														  OUT PULONG ReturnLength OPTIONAL );

typedef NTSTATUS ( __stdcall *NTSYSTEMDEBUGCONTROL ) ( IN int nCmd,
														  IN PVOID pInBuf,
														  IN ULONG nInLen, 
														  OUT PVOID pOutBuf,
														  IN ULONG nOutLen,
														  OUT PULONG nRetLen OPTIONAL);

typedef struct _WTS_PROCESS_INFO { 
	DWORD SessionId; 
	DWORD ProcessId; 
	LPTSTR pProcessName; 
	PSID pUserSid;
} WTS_PROCESS_INFO, *PWTS_PROCESS_INFO;

typedef HANDLE (WINAPI* WTSOPENSERVER)(LPTSTR pServerName);

typedef HANDLE (WINAPI* WTSCLOSESERVER)(HANDLE hServer);

typedef void (WINAPI* WTSFREEMEMORY)(PVOID pMemory);


typedef BOOL (WINAPI* WTSENUMERATEPROCESSES)(HANDLE hServer,DWORD Reserved,DWORD Version,\
											 PWTS_PROCESS_INFO* ppProcessInfo,DWORD* pCount);



#endif