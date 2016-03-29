#pragma once

namespace NS_GXX
{

//
//把数据写入到缓冲区
//最后将缓存区的数据装入剪贴板
class CClipBoardTextBuffer
{
public:
	CClipBoardTextBuffer()
	{
		m_hGlobalMem = NULL;
		m_pCurMem = NULL;
		m_iAppendAt = 0;
	}
	~CClipBoardTextBuffer(){
		if( m_hGlobalMem != NULL)
		{
			GlobalFree(m_hGlobalMem);
		}
	}
public:
	static const size_t  MAX_SIZE = 0x200000;
	
	BOOL Lock()
	{
		if( m_hGlobalMem != NULL )
		{
			GlobalFree(m_hGlobalMem);
		}
		m_hGlobalMem  = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,MAX_SIZE);
		ASSERT( m_hGlobalMem != NULL );
		if( m_hGlobalMem == NULL )
		{
			return FALSE;
		}
		m_pCurMem = (CHAR*)GlobalLock(m_hGlobalMem);
		*m_pCurMem = '\0';
		*(m_pCurMem+1) = '\0';
		m_iAppendAt = 0;
		return TRUE;
	}
	
	BOOL AppendData(const CHAR* pszData)
	{
		size_t nLen = strlen(pszData);
		if( nLen == 0)
		{
			return TRUE;
		}
		LONG nMaxSize = (long)GlobalSize(m_hGlobalMem);
		if( nLen + m_iAppendAt > nMaxSize -2)
		{
			GlobalUnlock(m_hGlobalMem);
			LONG nNewAdding = ((nLen / 4096)+1)*4096;   //采用4k的整数倍递增
			HANDLE hTempMem = GlobalReAlloc(m_hGlobalMem,(SIZE_T)nMaxSize+nNewAdding,GMEM_MOVEABLE | GMEM_ZEROINIT);
			if(hTempMem == NULL)
			{
				return FALSE;
			}
			m_hGlobalMem = hTempMem;

			m_pCurMem = (CHAR*)GlobalLock(m_hGlobalMem);
			if(m_pCurMem == NULL)
			{
				return FALSE;
			}
			m_pCurMem += m_iAppendAt;
		}
		memcpy(m_pCurMem,pszData,nLen);
		m_pCurMem += nLen;
		m_iAppendAt += nLen;
		return TRUE;
	}

	BOOL AppendData(const WCHAR* pwszData)
	{
		return AppendData(CW2A(pwszData));
	}
	//解锁将数据写入剪切板
	BOOL UnLock()
	{
		if( !::OpenClipboard(NULL) )
		{
			return FALSE;
		}
		if( !::EmptyClipboard() )
		{
			CloseClipboard();
			return FALSE;
		}
		LONG nRes = GlobalUnlock(m_hGlobalMem) ;
		if( nRes != 0)
		{
			CloseClipboard();
			return FALSE;
		}
		*m_pCurMem = '\0';
		*(m_pCurMem+1) = '\0';
		SetClipboardData(CF_TEXT,m_hGlobalMem);
		CloseClipboard();
		m_hGlobalMem = NULL;
		return TRUE;
	}
protected:
	HGLOBAL m_hGlobalMem;
	CHAR* m_pCurMem;
	LONG  m_iAppendAt;
};


}