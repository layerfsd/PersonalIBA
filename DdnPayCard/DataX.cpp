#include "StdAfx.h"
#include "Memmap.h"
#include ".\datax.h"

using namespace NS_DataX;

CDataXData::CDataXData(void)
{
	m_nMemMapLen = 0;

	m_nTimeout = 0;
}

CDataXData::~CDataXData(void)
{
}

BOOL CDataXData::Create(LPCTSTR lpszMemMapName, 
						size_t nMemMapLen /*= 1024 * 4*/, UINT nTimeout /*= 10 * 1000*/)
{

	m_strMemMapName = lpszMemMapName;
	m_nMemMapLen = nMemMapLen;
	m_nTimeout = nTimeout + Data_Header_Offset;

	return TRUE;
}

BOOL CDataXData::WriteData(DataX_Data_Header& Data_Header, LPVOID lpBuf, size_t cbLen)
{
	ASSERT((m_nMemMapLen - Data_Header_Offset) > cbLen);

	if ((m_nMemMapLen - Data_Header_Offset) < cbLen)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	CMemMapFile memMapFile;

	if (memMapFile.MapExistingMemory(m_strMemMapName, m_nMemMapLen))
	{
		LPVOID pMemMapFileBuffer = memMapFile.Open(GetTimeout());

		if (pMemMapFileBuffer != NULL)
		{
			memset(pMemMapFileBuffer, 0x00, m_nMemMapLen);

			memcpy(pMemMapFileBuffer, &Data_Header, sizeof DataX_Data_Header);

			if (lpBuf != NULL && cbLen != 0)
			{
				memcpy((char*)pMemMapFileBuffer + Data_Header_Offset, lpBuf, cbLen);
			}

			memMapFile.Close();

			bRet = TRUE;
		}
	}

	return bRet;
}


BOOL CDataXData::ReadData(DataX_Data_Header& Data_Header, LPVOID lpBuf, size_t cbLen)
{
	BOOL bRet = FALSE;

	CMemMapFile memMapFile;

	if (memMapFile.MapExistingMemory(m_strMemMapName, m_nMemMapLen))
	{
		LPVOID pMemMapFileBuffer = memMapFile.Open(GetTimeout());

		if (pMemMapFileBuffer != NULL)
		{
			memcpy(&Data_Header, pMemMapFileBuffer, sizeof DataX_Data_Header);

			if (lpBuf != NULL && cbLen != 0)
			{
				ASSERT(Data_Header.cbDataLen < cbLen);

				memcpy(lpBuf, (char*)pMemMapFileBuffer + Data_Header_Offset, Data_Header.cbDataLen);
			}

			memMapFile.Close();

			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CDataXData::WriteData(LPVOID lpBuf, size_t cbLen)
{
	DataX_Data_Header Data_Header = {0};

	if (ReadData(Data_Header))
	{
		Data_Header.cbDataLen = cbLen;

		return WriteData(Data_Header, lpBuf, cbLen);
	}

	return FALSE;
}

BOOL CDataXData::ReadData(LPVOID lpBuf, size_t cbLen)
{
	DataX_Data_Header Data_Header = {0};

	return ReadData(Data_Header, lpBuf, cbLen);

}