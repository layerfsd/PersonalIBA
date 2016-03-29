
#include "StdAfx.h"
#include "Memmap.h"
#include "DataXWnd.h"
#include ".\mdataxreceiver.h"

using namespace NS_DataX;

CDataXMReceiver::CDataXMReceiver(void)
{
	m_lpRevWnd = NULL;

	m_lpMemMapFile = NULL;
}

CDataXMReceiver::~CDataXMReceiver(void)
{
	if (m_lpRevWnd != NULL)
	{
		m_lpRevWnd->DestroyWindow();
		delete m_lpRevWnd;
		m_lpRevWnd = NULL;
	}

	if (m_lpMemMapFile != NULL)
	{
		delete m_lpMemMapFile;
		m_lpMemMapFile = NULL;
	}
}

BOOL CDataXMReceiver::Create(LPCTSTR lpszMemMapName, 
							 size_t nMemMapLen /*= 1024 * 4*/, UINT nTimeout /*= 10 * 1000*/)
{
	if (m_lpMemMapFile != NULL)
	{
		return FALSE;
	}

	CDataXData::Create(lpszMemMapName, nMemMapLen, nTimeout);
	
	//******************************************************************************

	//创建内存映射

	if (!CreateMemMap())
	{
		ASSERT(FALSE);

		return FALSE;
	}
	
	//******************************************************************************
	//创建接收窗口

	if (!CreateRevMechanism())
	{
		delete m_lpMemMapFile;
		m_lpMemMapFile = NULL;

		ASSERT(FALSE);

		return FALSE;
	}

	return TRUE;
}


INT CDataXMReceiver::OnDataArrival()
{
	DataX_Data_Header Data_Header = {0};

	CByteArray Buf;
	Buf.SetSize(GetMemMapLen() - Data_Header_Offset);

	if (ReadData(Data_Header, Buf.GetData(), Buf.GetSize()))
	{
		INT nRet = OnDataArrival(Data_Header.nAppId, 
			Data_Header.nMsg, Data_Header.nParam1, Data_Header.nParam2,
			Buf.GetData(), Data_Header.cbDataLen);

		if (Data_Header.bSync) //同步，需要写返回值
		{
			Data_Header.nResult = nRet;

			WriteData(Data_Header);
		}

		return nRet;
	}

	return -1;
}

INT CDataXMReceiver::OnDataArrival(INT nAppId, INT nMsg, INT nParam1, INT nParam2, 
								   LPVOID lpBuf, size_t cbLen)
{
	CString strTmp;

	strTmp.Format(_T("nAppId:%d, nMsg:%d, nParam1:%d, nParam1:%d"), nAppId, nMsg, nParam1, nParam2);

	AfxMessageBox(strTmp);

	return 0;
}

BOOL CDataXMReceiver::CreateRevMechanism()
{
	CDataXWnd* pWnd = new CDataXWnd;

	if (!pWnd->CreateRevWnd(this))
	{
		delete pWnd;

		return FALSE;
	}

	pWnd->SetWindowText(GetMemMapName());

	m_lpRevWnd = pWnd;

	DataX_Data_Header Data_Header = {0};
	Data_Header.hWnd = pWnd->GetSafeHwnd();

	return WriteData(Data_Header);
}

BOOL CDataXMReceiver::CreateMemMap()
{
	m_lpMemMapFile = new CMemMapFile;

	if (m_lpMemMapFile->MapExistingMemory(GetMemMapName(), GetMemMapLen()))
	{
		delete m_lpMemMapFile;
		m_lpMemMapFile = NULL;

		return FALSE;
	}

	//创建安全属性
	SECURITY_ATTRIBUTES sa = {0};
	SECURITY_DESCRIPTOR sd = {0};

	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = &sd;

	if (m_lpMemMapFile->MapMemory(GetMemMapName(), GetMemMapLen(), FALSE, &sa)) //这里是创建的
	{

		LPVOID pMemMapFileBuffer = m_lpMemMapFile->Open(GetTimeout());
		memset(pMemMapFileBuffer, 0x00, GetMemMapLen());

		m_lpMemMapFile->Close();
	}
	else
	{
		delete m_lpMemMapFile;
		m_lpMemMapFile = NULL;

		return FALSE;
	}

	return TRUE;
}

