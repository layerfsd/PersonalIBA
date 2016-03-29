#pragma once

#include "Memmap.h"

class L2_EXT_CLASS CL2Instance
{
public :

	CL2Instance(LPCTSTR lpszInstanceName = NULL, size_t MemMapSize = 4096);
	
	virtual ~CL2Instance(void);

protected :

	BOOL m_bIsFirstInstance;

	HANDLE m_hMutex;

	CString m_strInstanceName;

	CString m_strMemMapName;

	size_t  m_MemMapSize;

	typedef struct TInstanceData
	{
		HWND  hMainWnd;
		UINT  nCommand;
		UINT  nFlag;
	}; 

	TInstanceData m_InstanceData;

	CMemMapFile m_MemMapFile;

protected :
	
	BOOL WriteInstanceData();
	BOOL GetInstanceData();
	BOOL CreateMemMap();
	
	virtual void OnNextInstance();
	virtual void OnFirstInstance();
	
public :

	BOOL CreateInstance();

	BOOL UpdateInstanceData(BOOL bSave = TRUE);

	BOOL WriteOtherData(const LPVOID lpBuf, size_t nCount);
	
	BOOL ReadOtherData(LPVOID lpBuf, size_t nCount);

	BOOL IsFirstInstance() const { return m_bIsFirstInstance; }
	
	BOOL ActiveMainWnd();

public :

	HWND GetMainWnd() const { return m_InstanceData.hMainWnd; }

	void SetMainWnd(HWND newVal) { m_InstanceData.hMainWnd = newVal; }

	UINT GetCommand() const { return m_InstanceData.nCommand; }

	void SetCommand(UINT newVal) { m_InstanceData.nCommand = newVal; }

	UINT GetFlag() const { return m_InstanceData.nFlag; }

	void SetFlag(UINT newVal) { m_InstanceData.nFlag = newVal; }
	
};
