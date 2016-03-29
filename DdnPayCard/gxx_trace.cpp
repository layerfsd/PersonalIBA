#include "stdafx.h"
#include "gxx_trace.h"
#include <Windows.h>
#include <AtlBase.h>
#include <AtlConv.h>

#ifdef __GXX_TRACE
#ifdef _DEBUG

#pragma warning(disable: 4100)
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)

__gxx_trace__:: __gxx_trace__()
    { 
        __index = 0;
        ::InitializeCriticalSection( &__cs);
        __bFinished = TRUE;
        __count = 0;
        __dwID = 0;
        __bWChar = FALSE;
		_outNameA[0] = '\0';
		_outNameW[0] = '\0';
    }
__gxx_trace__::~__gxx_trace__()
    {
        ::DeleteCriticalSection(&__cs);
        _tracePre(0);
		sprintf(_buf,"\r\n========================================== Tracing End!===============================================\r\n",
			_outNameA);
		_traceA(_buf);
    }
void __gxx_trace__::operator ()(const char *lpszOut,...)
    {
        ::EnterCriticalSection(&__cs);
        _tracePre(0);
        va_list  args;
        va_start(args,lpszOut);
        _vsnprintf(_buf,BUF_SIZE,lpszOut,args);
		va_end(args);
		
		if( _outNameA[0] == '\0' )
			sprintf(__buf,"%5d>  %s\r\n",++__index,_buf);
		else
			sprintf(__buf,"[%s] %5d>  %s\r\n",_outNameA,++__index,_buf);

		_traceA(__buf);
   
        ::LeaveCriticalSection(&__cs);
    }
void __gxx_trace__::operator ()(const wchar_t *lpszOut,...)
    {
        ::EnterCriticalSection(&__cs);

        _tracePre(0);
        va_list  args;
        va_start(args,lpszOut);
        vswprintf(_wbuf,lpszOut,args);
		va_end(args);

		if( _outNameW[0] == L'\0' )
			swprintf(__wbuf,BUF_SIZEW,L"%5d>  %s\r\n",++__index,_wbuf);
		else
			swprintf(__wbuf,BUF_SIZEW,L"[%s] %5d>  %s\r\n",_outNameW,++__index,_wbuf);
    
        _traceW(__wbuf);

        ::LeaveCriticalSection(&__cs);
    }
void __gxx_trace__::operator()(BYTE traceFlag,const char *lpszOut)
    {
        ::EnterCriticalSection(&__cs);
        if( _tracePre((DWORD)lpszOut) == TRUE )  //开始新的累计
        {
            __dwID = (DWORD)lpszOut;
            __bWChar = FALSE;
            __bFinished = FALSE;
        }
        __count ++;
        ::LeaveCriticalSection(&__cs);
    }
void __gxx_trace__::operator()(BYTE traceFlag,const wchar_t *lpszOut)
    {
        ::EnterCriticalSection(&__cs);
        if( _tracePre((DWORD)lpszOut) == TRUE )   
        {
            __dwID = (DWORD)lpszOut;
            __bWChar = TRUE;
            __bFinished = FALSE;
        }
        __count ++;
        ::LeaveCriticalSection(&__cs);
    }
BOOL __gxx_trace__::_tracePre(DWORD dwID)
    {
        if( dwID == __dwID)  //同一个累计还没结束，返回
        {
            return FALSE;
        }
        if( __bWChar == TRUE && __bFinished == FALSE )  //上一个wchar_t型的字符串跟踪没结束
        {
            if(__dwID != 0)
            {
                wchar_t *pWC = (wchar_t*)__dwID;
                swprintf(__wbuf,BUF_SIZEW,L"[%s] %5d>  %s (count= %d)\r\n",_outNameW,++__index,pWC,__count);    
                _traceW(__wbuf);

            }
            __bFinished = TRUE;
            __count = 0;
            __dwID = 0;
        }
        else if(  __bWChar == FALSE && __bFinished == FALSE )
        {
            if(__dwID != 0)
            {
                char *pC = (char*)__dwID;
                sprintf(__buf,"[%s] %5d>  %s (count= %d)\r\n",_outNameA,++__index,pC,__count);
                _traceA(__buf);
            }
            __bFinished = TRUE;
            __count = 0;
            __dwID = 0;
        }
        return TRUE;
    }
void __gxx_trace__::_traceW(wchar_t * wc)
    {
        ::WideCharToMultiByte( _AtlGetConversionACP(), 0, wc, (int)wcslen(wc)+1, _buf, 1024, NULL, NULL );
        _traceA(_buf);
    }
void __gxx_trace__::_traceA(char * ca)
    {
		if( __index == 1 )
		{
			char tempBuf[1024];
			sprintf(tempBuf,"\r\n==========================================%s Tracing Start! ===============================================\r\n",_outNameA);
			ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, tempBuf, NULL);
		}

        int len = (int)strlen(ca);
        const int  LEN = 512;
		bool bHaveBai = false;
		for(int i = 0 ; i < len ; i ++)
		{
			if( ca[i] == '%' )
			{
				// 孤独无用的%会导致输出时出现无法预料的错误
				// %%双百分号不会出现错误
				if( i < len-1 && ca[i+1] !='%')
				{
					ca[i] = '_'; //将百分号换成 _
					bHaveBai = true;
				}
			}
		}
		if( bHaveBai )
		{
			//GXX_TRACE("warning：通过trace输出信息，但信息中包含非法的‘%’");
		}
        if( len < LEN )
        {
            ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, ca, NULL);
            return;
        }
        //输出过长就分段输出
        char* pC = ca;
        int nCount = len / LEN;
        char *tempC = new char[nCount+1];
        for(int i = 1; i <= nCount ; i ++)
        {
            tempC[i] = ca[i*LEN];
            ca[i*LEN] = '\0';
        }
        for(int i = 1 ; i <= nCount ; i ++)
        {
            ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, pC, NULL);
            pC = & ca[i*LEN];
            ca[i*LEN] = tempC[i];
        }
        if( nCount * LEN != len )    //不是LEN的整数倍
        {
            ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, pC, NULL);
        }
        delete []tempC;
    }

void __gxx_trace__::SetOutName( const wchar_t *lpszName/*=NULL*/ )
{
	if( lpszName == NULL || lpszName[0]==L'\0')
	{
		return;
	}
	memcpy(_outNameW,lpszName,sizeof(WCHAR)*(wcslen(lpszName)+1));
}

void __gxx_trace__::SetOutName( const char* lpszName /*=NULL*/ )
{
	if( lpszName == NULL || lpszName[0]=='\0')
	{
		return;
	}
	memcpy(_outNameA,lpszName,sizeof(char)*(strlen(lpszName)+1));
}
//////
__gxx_trace__  __gxx_trace;
/////
long __gxx_auto_trace__::s_levels = 0;
__gxx_auto_trace__::__gxx_auto_trace__ (char* strFormat,...)
{
	_bChar = true;
	va_list  args;
	va_start(args,strFormat);

	_vsnprintf(_buf,1024,strFormat,args);
	
	for(int i = 0 ; i < s_levels ; i ++)
	{
		for(int j = 0 ; j < _iRepeat ; j++)
		{
			_spaces[i*_iRepeat + j] = _space;
		}
	}
	_spaces[ s_levels * _iRepeat ] = '\0';
	//GXX_TRACE("%s ==========%s=========",_spaces,_buf);
	GXX_TRACE("%s{ BEGIN--> %s",_spaces,_buf);
	GXX_TRACE("");
	InterlockedCompareExchange (&s_levels,s_levels+1,s_levels);

}
__gxx_auto_trace__::__gxx_auto_trace__ (wchar_t* lpszOut,...)
{
	_bChar = false;
	va_list  args;
	va_start(args,lpszOut);
	
	vswprintf(_wBuf,1024,lpszOut,args);

	for(int i = 0 ; i < s_levels ; i ++)
	{
		for(int j = 0 ; j < _iRepeat ; j++)
		{
			_wspaces[i*_iRepeat + j] = _wspace;
		}
	}
	_wspaces[ s_levels * _iRepeat ] = L'\0';
	//GXX_TRACE(L"%s ==========%s=========",_wspaces,_wBuf);
	GXX_TRACE(L"%s{ BEGIN--> %s",_wspaces,_wBuf);
	GXX_TRACE("");
	InterlockedCompareExchange (&s_levels,s_levels+1,s_levels);
}
__gxx_auto_trace__::~__gxx_auto_trace__ ()
{
	InterlockedCompareExchange (&s_levels,s_levels-1,s_levels);
	GXX_TRACE("");
	if( _bChar )
	{
		GXX_TRACE("%s} END--> %s",_spaces,_buf);
		//GXX_TRACE("%s ====================",_spaces);
	}
	else
	{
		GXX_TRACE(L"%s} END--> %s",_wspaces,_wBuf);
		//GXX_TRACE(L"%s ====================",_wspaces);
	}
}
///////////////////////////////////////////////
__gxx_auto_timer__::__gxx_auto_timer__( const char* szOut,... )
{
	_bChar = true;
	va_list  args;
	va_start(args,szOut);
	_vsnprintf(_spaces,1024,szOut,args);

	_dwTick = ::GetTickCount();
}

__gxx_auto_timer__::__gxx_auto_timer__( const wchar_t* szOut,... )
{
	_bChar = false;
	va_list  args;
	va_start(args,szOut);
	vswprintf(_wspaces,1024,szOut,args);
	
	_dwTick = ::GetTickCount();
}

__gxx_auto_timer__::~__gxx_auto_timer__()
{
	_dwTick = ::GetTickCount() - _dwTick;

	if(_bChar)
	{
		int nlen = (int)strlen(_spaces);
		sprintf(&_spaces[nlen],":%d",_dwTick);

		GXX_TRACE(_spaces);
	}
	else
	{
		int nlen = (int)wcslen(_wspaces);
		wsprintfW(&_wspaces[nlen],L":%d",_dwTick);

		GXX_TRACE(_wspaces);
	}
}

#endif // -end-DEBUG
#endif // -end-__GXX_TRACE

#ifdef __GXX_LOG
__gxx_log__::__gxx_log__()
{
    m_hFile = INVALID_HANDLE_VALUE ;
	m_bInitPah = FALSE;
	m_chFilePath[0] = L'\0';
    InitializeCriticalSection(&m_cs);

	// 加密相关
	m_pPSK = NULL;
	SetEncrypt(_T("dqdwzxc"));
}
__gxx_log__::~__gxx_log__()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    DeleteCriticalSection(&m_cs);
}
void __gxx_log__::operator ()(const wchar_t* tcszLogFilePath)
{
    EnterCriticalSection(&m_cs);
	if( tcszLogFilePath == NULL )
	{
		LeaveCriticalSection(&m_cs);
		return ;
	}
	int nLen = (int)wcslen(tcszLogFilePath);
	memcpy(m_chFilePath,tcszLogFilePath,nLen*2);
	m_chFilePath[nLen] = L'\0';
	m_bInitPah = TRUE;
    LeaveCriticalSection(&m_cs);
}
void __gxx_log__::operator ()(int iWay,const char* szOutFormat,...)
{
    EnterCriticalSection(&m_cs);
	LogCreate();
    if( m_hFile == INVALID_HANDLE_VALUE )
    {
        LeaveCriticalSection(&m_cs);
        return;
    }
    char *cOut = NULL;
    switch(iWay)
    {
        case __GXX_HIT:
            {
                cOut = "提示";
            }
            break;
        case __GXX_WARNING:
            {
                cOut = "警告";
            }
            break;
        case __GXX_ERROR:
            {
                cOut = "错误";
            }
            break;
        case __GXX_BAD:
            {
                cOut = "严重";
            }
            break;
        default:
            LeaveCriticalSection(&m_cs);
            return;
    }
    int strLen = (int)strlen(szOutFormat) ;
    if( strLen <= 0 )
    {
        LeaveCriticalSection(&m_cs);
        return;
    }

    va_list  args;
    va_start(args,szOutFormat);
    char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
    va_end(args);

    //得到系统时间
    SYSTEMTIME stTime;
    GetLocalTime(&stTime);
    char outbuf[1024];

	sprintf(outbuf,"%s! [ %d-%d-%d %d:%02d:%02d  ]: %s\r\n",
                          cOut,
                          stTime.wYear,stTime.wMonth,stTime.wDay,
                          stTime.wHour,stTime.wMinute,stTime.wSecond,
                          _buf);
    LONG longH = 0;
    if( ::SetFilePointer(m_hFile,0,&longH,FILE_END) >=0 )
    {
		// 2014-4-11 - qsc
		if(IsNeedEncrypt())
		{
			Encrypt((BYTE*)outbuf,strlen(outbuf));
		}
        DWORD written = 0;
        ::WriteFile(m_hFile,outbuf,strlen(outbuf),&written,NULL);
    }
    FlushFileBuffers(m_hFile);
    LeaveCriticalSection(&m_cs);
}
void __gxx_log__::operator () (int iWay,const wchar_t * tcszOutFormat,...)
{
    EnterCriticalSection(&m_cs);
	LogCreate();
    if( m_hFile == INVALID_HANDLE_VALUE )
    {
        return;
    }
    wchar_t *tOut = NULL;
    switch(iWay)
    {
    case __GXX_HIT:
        {
            tOut = L"提示";
        }
        break;
    case __GXX_WARNING:
        {
            tOut = L"警告";
        }
        break;
    case __GXX_ERROR:
        {
            tOut = L"错误";
        }
        break;
    case __GXX_BAD:
        {
            tOut = L"严重";
        }
        break;
    default:
        LeaveCriticalSection(&m_cs);
        return;
    }
    int strLen = (int)wcslen(tcszOutFormat) ;
    if( strLen <= 0 )
    {
        LeaveCriticalSection(&m_cs);
        return;
    }
    wchar_t _wBuf[512];
    va_list  args;
    va_start(args,tcszOutFormat);
    vswprintf(_wBuf,tcszOutFormat,args);
    va_end(args);


    SYSTEMTIME stTime;
    GetLocalTime(&stTime);
    wchar_t outbuf[1024];

    swprintf(outbuf,L"%s! [ %d-%d-%d %d:%02d:%02d  ]: %s\r\n",
        tOut,
        stTime.wYear,stTime.wMonth,stTime.wDay,
        stTime.wHour,stTime.wMinute,stTime.wSecond,
        _wBuf);
    //写入buf完毕
    LONG longH = 0;
    if( ::SetFilePointer(m_hFile,0,&longH,FILE_END)>=0)
    {
        char _buf[2048];
        ::WideCharToMultiByte( _AtlGetConversionACP(), 0, outbuf, wcslen(outbuf)+1, _buf, 2048, NULL, NULL );
		// 2014-4-11 - qsc
		if(IsNeedEncrypt())
		{
			Encrypt((BYTE*)_buf,strlen(_buf));
		}
        DWORD written = 0;
        ::WriteFile(m_hFile,_buf,strlen(_buf),&written,NULL);
    }
    FlushFileBuffers(m_hFile);
    LeaveCriticalSection(&m_cs);
}

void __gxx_log__::LogCreate()
{
	if( !m_bInitPah )
	{
		return;
	}
	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE ;
	}
	//创建今天的文件
	SYSTEMTIME stToday;
	GetLocalTime(&stToday);

	WCHAR chBuf[512];
	swprintf(chBuf,L"%s%04d%02d%02d.log",m_chFilePath,stToday.wYear,stToday.wMonth,stToday.wDay);
	
	m_hFile = ::CreateFileW(chBuf,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
}

void __gxx_log__::log_out( int iWay,const char* szOutFormat,... )
{
	EnterCriticalSection(&m_cs);
	LogCreate();
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		LeaveCriticalSection(&m_cs);
		return;
	}
	char *cOut = NULL;
	switch(iWay)
	{
	case __GXX_HIT:
		{
			cOut = "提示";
		}
		break;
	case __GXX_WARNING:
		{
			cOut = "警告";
		}
		break;
	case __GXX_ERROR:
		{
			cOut = "错误";
		}
		break;
	case __GXX_BAD:
		{
			cOut = "严重";
		}
		break;
	default:
		LeaveCriticalSection(&m_cs);
		return;
	}
	int strLen = strlen(szOutFormat) ;
	if( strLen <= 0 )
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	va_list  args;
	va_start(args,szOutFormat);
	char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
	va_end(args);

	//得到系统时间
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	char outbuf[1024];
	sprintf(outbuf,"%s! [ %d-%d-%d %d:%02d:%02d  ]: %s\r\n",
		cOut,
		stTime.wYear,stTime.wMonth,stTime.wDay,
		stTime.wHour,stTime.wMinute,stTime.wSecond,
		_buf);
	LONG longH = 0;
	if( ::SetFilePointer(m_hFile,0,&longH,FILE_END) >=0 )
	{
		// 2014-4-11 - qsc
		if(IsNeedEncrypt())
		{
			Encrypt((BYTE*)outbuf,strlen(outbuf));
		}
		DWORD written = 0;
		::WriteFile(m_hFile,outbuf,strlen(outbuf),&written,NULL);
	}
	FlushFileBuffers(m_hFile);
	LeaveCriticalSection(&m_cs);
}

void __gxx_log__::log_out( int iWay,const wchar_t* tcszOutFormat,... )
{
	EnterCriticalSection(&m_cs);
	LogCreate();
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		return;
	}
	wchar_t *tOut = NULL;
	switch(iWay)
	{
	case __GXX_HIT:
		{
			tOut = L"提示";
		}
		break;
	case __GXX_WARNING:
		{
			tOut = L"警告";
		}
		break;
	case __GXX_ERROR:
		{
			tOut = L"错误";
		}
		break;
	case __GXX_BAD:
		{
			tOut = L"严重";
		}
		break;
	default:
		LeaveCriticalSection(&m_cs);
		return;
	}
	int strLen = wcslen(tcszOutFormat) ;
	if( strLen <= 0 )
	{
		LeaveCriticalSection(&m_cs);
		return;
	}
	wchar_t _wBuf[512];
	va_list  args;
	va_start(args,tcszOutFormat);
	vswprintf(_wBuf,tcszOutFormat,args);
	va_end(args);


	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	wchar_t outbuf[1024];
	swprintf(outbuf,L"%s! [ %d-%d-%d %d:%02d:%02d  ]: %s\r\n",
		tOut,
		stTime.wYear,stTime.wMonth,stTime.wDay,
		stTime.wHour,stTime.wMinute,stTime.wSecond,
		_wBuf);
	//写入buf完毕
	LONG longH = 0;
	if( ::SetFilePointer(m_hFile,0,&longH,FILE_END)>=0)
	{
		char _buf[2048];
		::WideCharToMultiByte( _AtlGetConversionACP(), 0, outbuf, wcslen(outbuf)+1, _buf, 2048, NULL, NULL );
		// 2014-4-11 - qsc
		if(IsNeedEncrypt())
		{
			Encrypt((BYTE*)_buf,strlen(_buf));
		}
		DWORD written = 0;
		::WriteFile(m_hFile,_buf,strlen(_buf),&written,NULL);
	}
	FlushFileBuffers(m_hFile);
	LeaveCriticalSection(&m_cs);
}

void __gxx_log__::log_error( const char* szOutFormat,... )
{
	va_list  args;
	va_start(args,szOutFormat);
	char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
	va_end(args);
	log_out(__GXX_ERROR,_buf);
}
void __gxx_log__::log_hit( const char* szOutFormat,... )
{
	va_list  args;
	va_start(args,szOutFormat);
	char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
	va_end(args);
	log_out(__GXX_HIT,_buf);
}

void __gxx_log__::log_warning( const char* szOutFormat,... )
{
	va_list  args;
	va_start(args,szOutFormat);
	char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
	va_end(args);
	log_out(__GXX_WARNING,_buf);
}

void __gxx_log__::log_bad( const char* szOutFormat,... )
{
	va_list  args;
	va_start(args,szOutFormat);
	char _buf[1024];
	vsprintf(_buf,szOutFormat,args);
	va_end(args);
	log_out(__GXX_BAD,_buf);
}


void __gxx_log__::log_error( const wchar_t* tcszOutFormat,... )
{
	wchar_t _wBuf[2048];
	va_list  args;
	va_start(args,tcszOutFormat);
	vswprintf(_wBuf,2048,tcszOutFormat,args);
	va_end(args);
	log_out( __GXX_ERROR,_wBuf);
}
void __gxx_log__::log_hit( const wchar_t* tcszOutFormat,... )
{
	wchar_t _wBuf[2048];
	va_list  args;
	va_start(args,tcszOutFormat);
	vswprintf(_wBuf,2048,tcszOutFormat,args);
	va_end(args);
	log_out( __GXX_HIT,_wBuf);
}
void __gxx_log__::log_warning( const wchar_t* tcszOutFormat,... )
{
	wchar_t _wBuf[2048];
	va_list  args;
	va_start(args,tcszOutFormat);
	vswprintf(_wBuf,2048,tcszOutFormat,args);
	va_end(args);
	log_out( __GXX_WARNING,_wBuf);
}
void __gxx_log__::log_bad( const wchar_t* tcszOutFormat,... )
{
	wchar_t _wBuf[2048];
	va_list  args;
	va_start(args,tcszOutFormat);
	vswprintf(_wBuf,2048,tcszOutFormat,args);
	va_end(args);
	log_out( __GXX_BAD,_wBuf);
}

bool __gxx_log__::IsNeedEncrypt()
{
	if(NULL == m_pPSK || m_iPSKLen<=0 || strlen(m_pPSK) != m_iPSKLen)
		return FALSE;

	return TRUE;
}

void __gxx_log__::SetEncrypt(CString strPSK)
{
	if(strPSK == "") return;

	if(m_pPSK != NULL)
	{
		delete m_pPSK;
		m_pPSK = NULL;
	}

	m_iPSKLen = strPSK.GetLength();

	m_pPSK = new char[m_iPSKLen+1];

	memset(m_pPSK,0,m_iPSKLen+1);

	strcpy(m_pPSK, CT2A(strPSK));
}

void __gxx_log__::Encrypt(BYTE  *buffer,DWORD bufsize)
{
	if(NULL == buffer|| !IsNeedEncrypt()) return;

	DWORD num=0;

	while(num<bufsize)
	{	
		for(DWORD i=0;i<m_iPSKLen;i++)
		{
			buffer[num]^= m_pPSK[i];
		}

		num++;
	}

}

__gxx_log__  __gxx_log;

#endif

