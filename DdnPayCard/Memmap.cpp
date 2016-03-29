/*
Module : MEMMAP.CPP
Purpose: Implementation for an MFC class to wrap memory mapped files
Created: PJN / 19-03-1997
History: PJN / 31-03-1998 1) Class now avoids trying to lock the mutex if only read access is required
						  2) User now has the option of specifying whether a file should be mapped with 
							 A Null terminator at the end. Can prove helpful when you want to use some
							 of the "C" runtime functions on the pointer returned.
		 PJN / 20-04-1998 1) Now uses GetFileSize SDK call instead of GetFileInformationByHandle as a
							 more "reliable" way to determine file length.
						  2) Included TRACE statements to call GetLastError in all places where 
							 SDK functions fail
		 PJN / 29-05-1998 1) Mapping a file now has the option of making it named or not.
		 PJN / 22-10-1998 1) Fixed a bug in a number of calls to CreateMappingName when the classes were 
							 being used to share memory.
						  2) Tidy up of the demo app including:
							 a) Made the amount of text being shared a constant of MAX_EDIT_TEXT instead
							 of hardcoding it to 20 everywhere in the sample.
							 b) Changed where the timer is being created to OnInitDialog
							 c) Tidied up the initialisation sequence in OnInitDialog
							 d) Now using _tcscpy instead of _tcsncpy to ensure array is null terminated
							 e) Fixed resource.h which was causing the resources to fail to compile
							 f) Removed unnecessary symbols from resource.h
							 g) Optimized the way the OnTimer code works to only update the text when it
							 has changed in the MMF. This means that you can type continuously into the
							 edit control.
						  3) New documentation in the form of a HTML file.
						  4) Sample now ships as standard with VC 5 workspace files
		 PJN / 30-03-1999  1) Code is now unicode compliant
						  2) Code now supports growable MMF's
						  3) Addition of accessor functions for file handle and file mapping handle
		 PJN / 21-04-1999  1) Works around a Window bug where you try to memory map a zero length file on
							 Windows 95 or 98.
		 PJN / 24-04-2000  1) Fixed a simple typo problem in a TRACE statement when compiled for UNICODE
		 PJN / 07-03-2001  1) Updated copyright information   
						  2) Fixed problem where mutex was not being locked when read only access to 
						  the memory mapped file was desired. Access to the MMF should be synchronised
						  irrespective of the access mode.
		 PJN / 02-04-2001  1) Now supports passing in a security descriptor to MapFile and MapMemory
						  2) Now supports opening a specified portion of a file, rather than always mapping
						  all of the file
		 PJN / 05-10-2003 1) Updated copyright details.
						  2) Fixed a small typo in the description of the FSCTL_SET_SPARSE IOCTL. Thanks to 
						  amores perros for reporting this.
						  3) Fixed a minor tab indentation problem at the start of the MapFile method. Again
						  thanks to amores perros for reporting this.
						  4) Removed the unnecessary AssertValid function. Again thanks to amores perros for 
						  reporting this.
		 PJN / 17-11-2003 1) Fixed a memory leak in UnMap as reported by Bart Duijndam using 
						  "Memory Validator". The memory leak may in fact not be real, but the code change 
						  avoids the reported problem.
		 PJN / 06-06-2004 1) Fixed an issue in MapHandle where the wrong value was sent to CreateFileMapping. 
						  This issue only occurs when you are not mapping the whole of a file, but instead
						  decide to perform the mapping a chunk at a time. Thanks to Nicolas Stohler for
						  reporting this problem.
						  2) Removed the AppendNull option as it is incompatible with general use of memory 
						  mapped files.
						  3) Reviewed all the TRACE statements throughout the class
						  4) Added ASSERT validation at the top of functions which modify member variables
						  5) Failing to create mutex in MapHandle and MapExistingMemory not fails the function
		 PJN / 23-12-2004 1) Removed unnecessary include of winioctl.h header file.
						  2) Fixed a problem with the declaration of the FSCTL_SET_SPARSE macro which is used
						  in the support of growable MMF. With the VC6 WinIOCTL.h, the macro generates the 
						  value 0x000980C4, but the correct value for the DeviceIoControl function to enable 
						  the sparse file option is 0x000900C4. Thanks to a posting on CodeProject for pointing
						  out this problem.
						  3) Optimized CMemMapFile::CreateMappingName by now using CString::Replace.
						  4) Addition of a bInheritHandle parameter to MapExistingMemory.
						  5) Fixed a bug in the handling of calls to GetFileSize()
						  6) Removed unnecessary check to verify that mapping size is greater than 4GB since
						  CreateFileMapping will do this for us.
						  7) Updated sample app to use a sample "input.txt" file for demonstration purposes
						  instead of using "c:\config.sys"
						  8) Code now uses unsigned __int64 for specifying indexes and lengths for memory 
						  mapping. This should allow the code to be easily used to map > 4GB on 64 bit 
						  versions of Windows.
						  9. Tidied up some of the ASSERT code at the top of each key function in the class.
		 PJN / 30-04-2005 1. Removed derivation from CObject MFC class. In fact the class can now operate entirely
						  independently of MFC.
						  2. Destructor is now virtual.
						  3. Fixed a bug where the mutex name which is used to synchronize access to the MMF's data
						  could by default have the same name for multiple MMF's.
						  4. CreateMappingName and CreateMutexName methods are now virtual
		 PJN / 02-05-2006 1. Updated the copyright details in the modules.
						  2. Updated the documentation to use the same style as the web site.
						  3. Addition of a CMEMMAPFILE_EXT_CLASS macro to allow the class to be easily incorporated
						  into an extension dll.
						  4. The sample app now uses filenames without "(" or ")" characters in their filenames. 
						  Thanks to Andrew MacGinitie for reporting this issue.
						  5. Fixed a bug in the sample app where it reports the wrong filename when doing the lowercase
						  conversion. Again thanks to Andrew MacGinitie for reporting this issue.
						  6. Fixed an issue in the download where the sample file called "input.txt" is now provided
						  out of the box. Again thanks to Andrew MacGinitie for reporting this issue.
						  7. Fixed an issue in the sample app when the code is compiled with /Wp64
		 PJN / 07-07-2006 1. Code now uses newer C++ style casts instead of C style casts.
						  2. The code now requires the Platform SDK if compiled using VC 6.
						  3. Updated code to compile cleanly using VC 2005.

Copyright (c) 1997 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

/////////////////////////////////  Includes / Macros / Defines ////////////////

#include "stdafx.h"
#include "memmap.h"

//Define taken from the Platform SDK, means that CMemMapFile will compile correctly even on systems 
//which do not define _WIN32_WINNT >= 0x0500
#define FSCTL_MMF_SET_SPARSE CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 49, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#ifdef _AFX
#define MEMMAP_ASSERT ASSERT
#else
#define MEMMAP_ASSERT assert
#include <assert.h>
#endif

#ifndef _INC_TCHAR
#pragma message("to avoid this message, you should put TCHAR.h in your PCH (normally stdafx.h)")
#include <TCHAR.h>
#endif
#include <WinIoCtl.h>
using namespace NS_DataX;

///////////////////////////////// Implementation //////////////////////////////

CMemMapFile::CMemMapFile() : m_hFile(INVALID_HANDLE_VALUE), m_hMapping(NULL),
	m_bReadOnly(TRUE), m_lpData(NULL), m_bOpen(FALSE), m_hMutex(NULL),
	m_dwLength(0)
{
}

CMemMapFile::~CMemMapFile()
{
	UnMap();
}

BOOL CMemMapFile::MapFile(LPCTSTR pszFilename, BOOL bReadOnly,
	DWORD dwShareMode, BOOL bNamed, BOOL bGrowable,
	const unsigned __int64& dwStartOffset,
	const unsigned __int64& dwNumberOfBytesToMap,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	//Validate our state
	MEMMAP_ASSERT(m_hFile == INVALID_HANDLE_VALUE); //Should not be already open
	MEMMAP_ASSERT(m_hMapping == NULL);
	MEMMAP_ASSERT(m_lpData == NULL);
	MEMMAP_ASSERT(m_hMutex == NULL);

	//Work out the file access flags
	m_bReadOnly = bReadOnly;
	DWORD dwDesiredFileAccess = GENERIC_READ;
	if (!m_bReadOnly)
		dwDesiredFileAccess |= GENERIC_WRITE;

	//Open the real file on the file system
	m_hFile = CreateFile(pszFilename, dwDesiredFileAccess, dwShareMode, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
#ifdef TRACE  
		TRACE(_T("CMemMapFile::MapFile, Failed in call to CreateFile, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}

	//Make the file sparse, if requested to make the memory mapped file growable
	if (bGrowable)
	{
		DWORD dw;
		if (!DeviceIoControl(m_hFile, FSCTL_MMF_SET_SPARSE, NULL, 0, NULL, 0,
				&dw, NULL))
		{
#ifdef TRACE
			TRACE(_T("CMemMapFile::MapFile, Failed in call to make file sparse, You need Windows 2000 and an NTFS 5 volume for this !!, Error:%d\n"),
				GetLastError());
#endif
			UnMap();
			return FALSE;
		}
	}

	//Get the size of the file we are mapping
	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSizeLow = GetFileSize(m_hFile, &dwFileSizeHigh);
	if (dwFileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
	{
		//There was an error calling GetFileSize
#ifdef TRACE
		TRACE(_T("CMemMapFile::MapFile, Failed in call to GetFileSize, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}
	unsigned __int64 dwFileMappingSize = ((static_cast<unsigned __int64>(dwFileSizeHigh)) <<
		32) +
		dwFileSizeLow;

	//Fail if file is 0 length in size, calling CreateFileMapping on a 
	//zero length file on 95/98 can cause problems, plus you should implement client logic to detect 
	//for 0 byte files before you memory map them as documented in the help for CreateFileMapping
	if (dwFileMappingSize == 0)
	{
#ifdef TRACE  
		TRACE(_T("CMemMapFile::MapFile, File size is 0, not attempting to memory map the file\n"));
#endif
		UnMap();
		return FALSE;
	}

	//Now work out the size of the mapping we will be creating
	if (dwNumberOfBytesToMap)
		m_dwLength = dwNumberOfBytesToMap;
	else
		m_dwLength = dwFileMappingSize;

	//Create the mapping name
	CreateMappingName(pszFilename, bNamed);

	//Create the mutex name
	TCHAR sMutexName[512];
	CreateMutexName(pszFilename, sMutexName,
		sizeof(sMutexName) / sizeof(TCHAR), TRUE);

	//Do the actual mapping
	return MapHandle(m_hFile, lpSecurityAttributes, dwFileMappingSize,
			dwStartOffset, sMutexName);
}

BOOL CMemMapFile::MapMemory(LPCTSTR pszName, const unsigned __int64& dwBytes,
	BOOL bReadOnly, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	//Validate our state
	MEMMAP_ASSERT(m_hMapping == NULL);
	MEMMAP_ASSERT(m_lpData == NULL);
	MEMMAP_ASSERT(m_hMutex == NULL);

	m_dwLength = dwBytes;
	m_bReadOnly = bReadOnly;

	//Create the mapping name
	CreateMappingName(pszName, TRUE);

	//Create the mutex name
	TCHAR sMutexName[512];
	CreateMutexName(m_sMappingName, sMutexName,
		sizeof(sMutexName) / sizeof(TCHAR), FALSE);

	return MapHandle(INVALID_HANDLE_VALUE, lpSecurityAttributes, dwBytes, 0,
			sMutexName);
}

BOOL CMemMapFile::MapExistingMemory(LPCTSTR pszName,
	const unsigned __int64& dwBytes, BOOL bReadOnly, BOOL bInheritHandle)
{
	//Validate our state
	MEMMAP_ASSERT(m_hMapping == NULL);
	MEMMAP_ASSERT(m_lpData == NULL);
	MEMMAP_ASSERT(m_hMutex == NULL);

	m_dwLength = dwBytes;
	m_bReadOnly = bReadOnly;

	DWORD dwDesiredAccess = (!bReadOnly) ? FILE_MAP_WRITE : FILE_MAP_READ;

	CreateMappingName(pszName, TRUE);

	CString str = m_sMappingName; 

	m_hMapping = OpenFileMapping(dwDesiredAccess, bInheritHandle,
					m_sMappingName);
	if (m_hMapping == NULL)
	{
#ifdef _AFX
		TRACE(_T("CMemMapFile::MapExistingMemory, Failed in call to OpenFileMapping, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}

	//Map the view
	m_lpData = MapViewOfFile(m_hMapping, dwDesiredAccess, 0, 0,
				static_cast<SIZE_T>(dwBytes));

	//Create the mutex to sync access
	TCHAR sMutexName[512];
	CreateMutexName(m_sMappingName, sMutexName,
		sizeof(sMutexName) / sizeof(TCHAR), FALSE);
	m_hMutex = CreateMutex(NULL, FALSE, sMutexName);
	if (m_hMutex == NULL)
	{

#ifdef _AFX
		TRACE(_T("CMemMapFile::MapExistingMemory, Failed in call to CreateMutex, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}

	return (m_lpData != NULL);
}

BOOL CMemMapFile::MapHandle(HANDLE hHandle,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	const unsigned __int64& dwFileMappingSize,
	const unsigned __int64& dwStartOffset, LPCTSTR pszMutexName)
{
	//Validate our state
	MEMMAP_ASSERT(m_hMapping == NULL);
	MEMMAP_ASSERT(m_lpData == NULL);
	MEMMAP_ASSERT(m_hMutex == NULL);

	//Create the file mapping object
	DWORD flProtect = (!m_bReadOnly) ? PAGE_READWRITE : PAGE_READONLY;

	
	m_hMapping = CreateFileMapping(hHandle, lpSecurityAttributes, flProtect,
					static_cast<DWORD>((dwFileMappingSize & 0xFFFFFFFF00000000) >>
		32),
					static_cast<DWORD>(dwFileMappingSize & 0xFFFFFFFF),
					m_sMappingName);
	if (m_hMapping == NULL)
	{
#ifdef _AFX
		TRACE(_T("CMemMapFile::MapHandle, Failed in call to CreateFileMapping, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}

	//Map the view
	DWORD dwDesiredAccess = (!m_bReadOnly) ? FILE_MAP_WRITE : FILE_MAP_READ;
	m_lpData = MapViewOfFile(m_hMapping, dwDesiredAccess,
				static_cast<DWORD>((dwStartOffset & 0xFFFFFFFF00000000) >> 32),
				static_cast<DWORD>(dwStartOffset & 0xFFFFFFFF),
				static_cast<SIZE_T>(m_dwLength));

	//Create the mutex to sync access
	m_hMutex = CreateMutex(lpSecurityAttributes, FALSE, pszMutexName);
	if (m_hMutex == NULL)
	{
#ifdef _AFX
		TRACE(_T("CMemMapFile::MapHandle, Failed in call to CreateMutex, Error:%d\n"),
			GetLastError());
#endif
		UnMap();
		return FALSE;
	}

	return (m_lpData != NULL);
}

LPVOID CMemMapFile::Open(DWORD dwTimeout)
{
	if (m_lpData == NULL)
		return NULL;

	//Synchronise access to the MMF using the named mutex
	DWORD dwResult = WaitForSingleObject(m_hMutex, dwTimeout);
	if (dwResult == WAIT_OBJECT_0)
	{
		m_bOpen = TRUE;
		return m_lpData;
	}

	return NULL;
}

BOOL CMemMapFile::Close()
{
	//Release our interest in this MMF
	if (!m_bOpen)
		return FALSE;

	MEMMAP_ASSERT(m_hMutex); //Mutex should be valid
	m_bOpen = FALSE;
	ReleaseMutex(m_hMutex);

	return TRUE;
}

BOOL CMemMapFile::Flush()
{
	//No mapping open, so nothing to do
	if (m_lpData == NULL)
		return FALSE;

	return FlushViewOfFile(m_lpData, 0);
}

#if (_MSC_VER >= 1400)
void CMemMapFile::CreateMutexName(LPCTSTR pszBaseName, LPTSTR pszMutexName,
	DWORD dwMutexNameSize, BOOL bReplaceDirectorySeparators)
#else
void CMemMapFile::CreateMutexName(LPCTSTR pszBaseName, LPTSTR pszMutexName,
	DWORD /*dwMutexNameSize*/, BOOL bReplaceDirectorySeparators)
#endif
{
#if (_MSC_VER >= 1400)
	_tcscpy_s(pszMutexName, dwMutexNameSize, pszBaseName);
#else
	_tcscpy(pszMutexName, pszBaseName);
#endif  
	if (bReplaceDirectorySeparators)
	{
		size_t nLen = _tcslen(pszMutexName);
		for (size_t i = 0; i < nLen; i++)
		{
			if (pszMutexName[i] == _T('\\'))
				pszMutexName[i] = _T('_');
		}
	}
#if (_MSC_VER >= 1400)
	_tcscat_s(pszMutexName, dwMutexNameSize, _T("MUTEX"));
#else  
	_tcscat(pszMutexName, _T("MUTEX"));
#endif
}

void CMemMapFile::UnMap()
{
	//Close any views which may be open
	Close();

	//unmap the view
	if (m_lpData != NULL)
	{
		FlushViewOfFile(m_lpData, 0);
		UnmapViewOfFile(m_lpData);
		m_lpData = NULL;
	}

	//remove the file mapping
	if (m_hMapping != NULL)
	{
		CloseHandle(m_hMapping);
		m_hMapping = NULL;
	}

	//close the file system file if its open
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	//Close the mutex we have been using
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}

	//Reset the remaining member variables
	m_bReadOnly = TRUE;
#if (_MSC_VER >= 1400)
	_tcscpy_s(m_sMappingName, sizeof(m_sMappingName) / sizeof(TCHAR), _T(""));
#else  
	_tcscpy(m_sMappingName, _T(""));
#endif
	m_dwLength = 0;
}

void CMemMapFile::CreateMappingName(LPCTSTR pszName, BOOL bNamed)
{
	//Start with an empty string
#if (_MSC_VER >= 1400)
	_tcscpy_s(m_sMappingName, sizeof(m_sMappingName) / sizeof(TCHAR), _T(""));
#else  
	_tcscpy(m_sMappingName, _T(""));
#endif
	if (bNamed)
	{
#if (_MSC_VER >= 1400)
		_tcscpy_s(m_sMappingName, sizeof(m_sMappingName) / sizeof(TCHAR),
			pszName);
#else
		_tcscpy(m_sMappingName, pszName);
#endif
		OSVERSIONINFO osv = {0};

		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osv);

		INT index = -1;

		size_t nLen = _tcslen(m_sMappingName);
		for (size_t i = 0; i < nLen; i++)
		{
			if (m_sMappingName[i] == _T('\\'))
			{
				index = (INT)i;
			}
		}

		if (osv.dwMajorVersion < 5 || (osv.dwMajorVersion == 5 && osv.dwMinorVersion == 1))
		{
			if (index != -1) // 存在'\\'
			{
				m_sMappingName[index] = _T('_');
			}
		}
		else //2003以上版本
		{
			if (index != -1) // 存在'\\'
			{
				return;
			}
		}


#if (_MSC_VER >= 1400)
		_tcsupr_s(m_sMappingName, sizeof(m_sMappingName) / sizeof(TCHAR));
#else  
		_tcsupr(m_sMappingName);
#endif


	}
}
