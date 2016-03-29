/*
Module : MEMMAP.H
Purpose: Interface for an MFC class to wrap memory mapped files
Created: PJN / 30-07-1997

Copyright (c) 1997 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Macros /////////////////////////////////////

#ifndef __MEMMAP_H__
#define __MEMMAP_H__



/////////////////////////// Classes ///////////////////////////////////////////

class  CMemMapFile
{
public:
	//Constructors / Destructors
	CMemMapFile();
	virtual ~CMemMapFile();

	//Methods
	BOOL MapFile(LPCTSTR pszFilename, BOOL bReadOnly = FALSE,
		DWORD dwShareMode = 0, BOOL bNamed = FALSE, BOOL bGrowable = FALSE,
		const unsigned __int64& dwStartOffset = 0,
		const unsigned __int64& dwNumberOfBytesToMap = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);
	BOOL MapMemory(LPCTSTR pszName, const unsigned __int64& dwBytes,
		BOOL bReadOnly = FALSE,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL);
	BOOL MapExistingMemory(LPCTSTR pszName, const unsigned __int64& dwBytes,
		BOOL bReadOnly = FALSE, BOOL bInheritHandle = FALSE);
	void UnMap();
	LPVOID Open(DWORD dwTimeout = INFINITE);
	BOOL Close();
	BOOL Flush();

	//Accessors
	LPCTSTR GetMappingName() const
	{
		return m_sMappingName;
	};
	HANDLE GetFileHandle() const
	{
		return m_hFile;
	};
	HANDLE GetFileMappingHandle() const
	{
		return m_hMapping;
	};
	BOOL IsOpen() const
	{
		return m_bOpen;
	};
	unsigned __int64 GetLength() const
	{
		return m_dwLength;
	};

protected:
	//Methods
	BOOL MapHandle(HANDLE hHandle, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		const unsigned __int64& dwFileMappingSize,
		const unsigned __int64& dwStartOffset, LPCTSTR pszMutexName);
	virtual void CreateMappingName(LPCTSTR pszName, BOOL bNamed);
	virtual void CreateMutexName(LPCTSTR pszBaseName, LPTSTR pszMutextName,
		DWORD dwMutexNameSize, BOOL bReplaceDirectorySeparators);

	//Member variables
	HANDLE m_hFile;
	HANDLE m_hMapping;
	BOOL m_bReadOnly;
	LPVOID m_lpData;
	TCHAR m_sMappingName[_MAX_PATH];
	BOOL m_bOpen;
	HANDLE m_hMutex;
	unsigned __int64 m_dwLength;
};

#endif //__MEMMAP_H__
