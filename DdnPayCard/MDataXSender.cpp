
#include "StdAfx.h"
#include "DataXWnd.h"
#include ".\mdataxsender.h"

using namespace NS_DataX;

CDataXMSender::CDataXMSender(void)
{
	m_nAppId = 0;

	m_hRevWnd = NULL;
}

CDataXMSender::~CDataXMSender(void)
{
}

BOOL CDataXMSender::Create(INT nAppId, LPCTSTR lpszMemMapName, 
						   size_t nMemMapLen /*= 1024 * 4*/, UINT nTimeout /*= 10 * 1000*/)
{
	m_nAppId = nAppId;

	return CDataXData::Create(lpszMemMapName, nMemMapLen, nTimeout);
}

INT CDataXMSender::SendData(INT nMsg, INT nParam1, INT nParam2, 
							LPVOID lpBuf, size_t cbLen)
{
	INT nRet = -1;

	if (PrepareData(true, nMsg, nParam1, nParam2, lpBuf, cbLen))
	{
		nRet = SendToRev(true);
	}
	
	return nRet;
}

INT CDataXMSender::PostData(INT nMsg, INT nParam1, INT nParam2, 
							LPVOID lpBuf, size_t cbLen)
{
	INT nRet = -1;

	if (PrepareData(false, nMsg, nParam1, nParam2, lpBuf, cbLen))
	{
		nRet = SendToRev(false);
	}

	return nRet;
}

BOOL CDataXMSender::PrepareData(bool bSync, INT nMsg, INT nParam1, INT nParam2, 
								LPVOID lpBuf, size_t cbLen)
{
	DataX_Data_Header Data_Header = {0};

	if (ReadData(Data_Header))
	{
		m_hRevWnd = Data_Header.hWnd;

		Data_Header.nAppId = m_nAppId;
		Data_Header.bSync = bSync;

		Data_Header.nMsg = nMsg;
		Data_Header.nParam1 = nParam1;
		Data_Header.nParam2 = nParam2;
		Data_Header.cbDataLen = cbLen;

		if (WriteData(Data_Header, lpBuf, cbLen))
		{
			return TRUE;
		}
	}

	return FALSE;
}

INT CDataXMSender::SendToRev(bool bSync)
{
	INT nRet = -1;

	if (bSync)
	{
		nRet = (INT)SendMessage(m_hRevWnd, CDataXWnd::WM_DATAX, NULL, NULL);
	}
	else
	{
		nRet = PostMessage(m_hRevWnd, CDataXWnd::WM_DATAX, NULL, NULL);
	}

	return nRet;
}