//**********************************************************************
//>gxx_trace.h  
//__________________________________________________________________________________________________
//>自定义跟踪( 在_DEBUG模式下才有跟踪输出 )
//__________________________________________________________________________________________________
//>描述:包含 __gxx_trace__ 的声明，该类实现跟踪的输出和跟踪的累计,在gxx_trace.cpp中定义了
//        __gxx_trace__的一个全局对象 __gxx_trace,尽量避免直接使用__gxx_trace.

//>使用说明: 在使用 自定义跟踪之前必须有 __GXX_TRACE的定义，就是说再包含 gxx_trace.h之前
//       必须定义__GXX_TRACE来开启 自定义跟踪功能，不需要跟踪功能时，只需要取消__GXX_TRACE
//       的定义即可（无需改动项目中的源代码）。用 GXX_TRACE 来进行及时跟踪，用GXX_TRACE_COUNT
//       来进行累计跟踪，使用累计跟踪时，一直要等到下一个不同跟踪到来才会输出。通过这两个宏
//       可以重定义多个开关，以达到选择不同的输出目的.
//>注：gxx_trace.h必须包含在 stdafx.h中，在包含 gxx_trace.h之前 通过是否定义__GXX_TRACE来
//       决定是否开启跟踪. 如果定义开关，一定要注意关闭跟踪功能时，要关闭相应的开关。
//       不要直接使用__gxx_trace,不要在项目其他地方定义__gxx_trace__对象.
//       使用GXX_TRACE_COUNT时，由于计数的结束点是未知的，所以必须再有GXX_TRACE调用，GXX_TRACE_COUNT的内容才会输出
//>关键字: 
//		__GXX_TRACE: 是所有GXX跟踪宏的开关，如果在包含gxx_trace.h之前定义此宏表示打开的所有的跟踪功能
//      GXX_TRACE_SETTING_NAME: 设置在输出时显示在一行最前面的行标题，为不同的模块设置不同的名称，那么
//                       在output窗口中可以很容易地看到输出的内容是属于哪个模块
//      GXX_TRACE:  带行标题和行号的基本输出,每调用一次该宏，就会换行一次
//      GXX_TRACE_COUNT: 在无法预知一个循环要循环多少次或一个函数要连续调用多少此，可以调用
//                       此宏来统计次数。注：在循环范围内不能有其他GXX宏的使用，在想看到统计次数
//                       的地方任意调用一个GXX跟踪宏都可以触发统计次数的输出。例如：
//                       char * p = "abcdferer.........";
//						 //无法预知p的长度
//                       char * t = p;
//                       while(*t != '\0')
//                       { ++t; GXX_TRACE_COUNT("字符串的长度为:"); }
//                       GXX_TRACE("统计结束"); //触发
//                       // 输出结果为:
//                       // 字符串的长度为：...
//                       // 统计结束
//      GXX_AUTO_TRACE: 在一个作用域内有作用，用于输出的这个作用域的起点和结束点，用"{"开始，"}"结束.
//                      例如：
//                      int (int a)
//                      {
//							GXX_AUTO_TRACE("fun1对一个整数进行运算");
//                          if( a %2 = 0 )
//                          {   GXX_TRACE("a是一个偶数");   return a/2; }
//                          else
//                          {   GXX_TRACE("a是一个基数");   return a+1; }
//                      }
//                      //掉用此函数 fun1(10);输出结果为
//                      // =============fun1对一个整数进行运算=============
//                      // {{{
//                      // a是一个偶数
//                      // }}}
//      GXX_AUTO_TIMER: 统计在一个做域内内，从使用此宏开始，到这个作用域结束经过的毫秒数
//		GXX_FILE_LINE:  输出当前的代码文件和代码行号
//      GXX_ASSERT: 如果表达式为false，就弹出中断对话框

//      __GXX_LOG: 日志信息输出的开关宏
//      GXX_LOG_CREATE: 调用此宏创建日志文件,每天自动创建一个文件(这个宏指定的名称为前缀+日期 作为文件名)
//      GXX_LOG_HIT:  输出日志类型为提示的日志信息
//      GXX_LOG_WARNING: 输出日志类型为警告的日志信息
//      GXX_LOG_ERROR: 输出日志类型为错误的日志信息
//      GXX_LOG_BAD:  输出日志类型为严重的日志信息
//      
//>作者：Guoxuxing
//>Date：2009/05/25
//>LastTime:2011/6/29
//>新增
//	GXX_ASSERT
//__________________________________________________________________________________________________
//>Log日志输出到文件(在_DEBUG和Release模式下都可使用)
//__________________________________________________________________________________________________
//__gxx_log__类，实现日志的输出
//>注：必须定义 __GXX_LOG来开启日志功能
#pragma  once

#include <Windows.h>
#include <stdio.h>

//_DEBUG状态下才有输出,必须预先定义 __GXX_TRACE才能开启自定义跟踪
using namespace std;

#ifndef _DEBUG
#ifdef __GXX_TRACE
#undef __GXX_TRACE
#endif
#endif

#ifdef __GXX_TRACE

#define __GXX_COUNT            0           //表示开始统计，如果索引发生变化，直接显示统计计数的结果
class __gxx_trace__;
class __gxx_auto_trace__;
extern __gxx_trace__  __gxx_trace;

/*在外界调用以下两个宏来进行跟踪*/
#define GXX_TRACE_SETTING_NAME(name) __gxx_trace.SetOutName(name)
#define GXX_TRACE  __gxx_trace                  
#define GXX_TRACE_COUNT(strOut)  __gxx_trace(__GXX_COUNT,strOut);/*该宏用来累计跟踪计数，因为某些地方重复的次数太多，如果都显示出来，观察会显得很吃力*/
#define GXX_AUTO_TRACE __gxx_auto_trace__  gxx_auto_trace
#define GXX_AUTO_TIMER __gxx_auto_timer__  gxx_atuo_timer
#define GXX_FILE_LINE() GXX_TRACE("file:%s",__FILE__);GXX_TRACE("line:%d",__LINE__)
#define GXX_ASSERT(expr,_string)	{if(!(expr)){GXX_TRACE(#_string);GXX_FILE_LINE();_ASSERTE((_string,0));}}
class  __gxx_trace__ 
{
public:
    __gxx_trace__();
    ~__gxx_trace__();
	void SetOutName(const wchar_t *lpszName=NULL);
	void SetOutName(const char* lpszName =NULL);
    void operator ()(const char *lpszOut,...);
    void operator ()(const wchar_t *lpszOut,...);
    void operator()(BYTE traceFlag,const char *lpszOut);
    void operator()(BYTE traceFlag,const wchar_t *lpszOut);
private:
	static const long BUF_SIZE = 10240;  //一次最大输出字节数
	static const long BUF_SIZEW = BUF_SIZE/2;
	char _outNameA[256];  //输出的每行的前缀名称,不宜过长，越短输出的结果越清晰
	WCHAR _outNameW[256];

    char _buf[BUF_SIZE];
    char __buf[BUF_SIZE];
    wchar_t _wbuf[BUF_SIZEW];
    wchar_t __wbuf[BUF_SIZEW];
    int __index;
    CRITICAL_SECTION __cs;
    BOOL __bFinished;
    BOOL __bWChar;
    DWORD __dwID;
    UINT __count;
    BOOL _tracePre(DWORD dwID);
    void _traceW(wchar_t * wc);
    void _traceA(char * ca);
};
class __gxx_auto_trace__
{
public:
	__gxx_auto_trace__(char* szOut,...);
	__gxx_auto_trace__(wchar_t* szOut,...);
	~__gxx_auto_trace__();
private:
	static long s_levels;

	const static char _space = '-'; 
	const static wchar_t _wspace = L'-';
	const static int _iRepeat = 2;

	char _spaces[1024];
	wchar_t _wspaces[1024];
	char _buf[1024];
	wchar_t _wBuf[1024];
	bool _bChar;
};
class __gxx_auto_timer__
{
public:
	__gxx_auto_timer__(const char* szOut,...);
	__gxx_auto_timer__(const wchar_t* szOut,...);
	~__gxx_auto_timer__();

	char _spaces[1024];
	wchar_t _wspaces[1024];
	bool _bChar;
	DWORD _dwTick;
};

#else
//如果没有开启跟踪，用一下方法把跟踪作废
#define GXX_TRACE_SETTING_NAME __noop
#define GXX_TRACE __noop
#define GXX_TRACE_COUNT __noop
#define GXX_AUTO_TRACE __noop
#define GXX_AUTO_TIMER __noop
#define GXX_FILE_LINE __noop
#define GXX_ASSERT __noop
#endif  // end #ifdef __GXX_TRACE


///----------------------------------------------定义跟踪结束

///----------------------------------------------定义日志输出
#define __GXX_HIT            0           //提示信息
#define __GXX_WARNING    1           //警告信息
#define __GXX_ERROR        2           //错误信息
#define __GXX_BAD           3           //严重信息

#ifdef __GXX_LOG
class __gxx_log__;
extern __gxx_log__  __gxx_log;    //全局的log申明

#define GXX_LOG_CREATE(logFilePath) __gxx_log(logFilePath) 

/*在外界调用以下两个宏来进行日志输出*/
#define GXX_LOG_HIT				__gxx_log.log_hit
#define GXX_LOG_WARNING			__gxx_log.log_warning
#define GXX_LOG_ERROR			__gxx_log.log_error 
#define GXX_LOG_BAD				__gxx_log.log_bad 
                         
#define GXX_LOG  __gxx_log

typedef __gxx_log__  CGxxLog;

class __gxx_log__
{
public:
    __gxx_log__();
    ~__gxx_log__();

    void operator ()(const wchar_t *tcszLogFilePath);
    void operator ()(int iWay,const char* szOutFormat,...);
    void operator ()(int iWay,const wchar_t * tcszOutFormat,...);

	void log_out(int iWay,const char* szOutFormat,...);
	void log_out(int iWay,const wchar_t* tcszOutFormat,...);
	void log_error(const char* szOutFormat,...);
	void log_error(const wchar_t* tcszOutFormat,...);
	void log_hit(const char* szOutFormat,...);
	void log_hit(const wchar_t* tcszOutFormat,...);
	void log_warning(const char* szOutFormat,...);
	void log_warning(const wchar_t* tcszOutFormat,...);
	void log_bad(const char* szOutFormat,...);
	void log_bad(const wchar_t* tcszOutFormat,...);
private:
	//每天创建一个文件
	void LogCreate();
    HANDLE m_hFile;
    CRITICAL_SECTION m_cs;
	WCHAR m_chFilePath[512];
	BOOL m_bInitPah;
};
#else
#define GXX_LOG_CREATE(closed) ;
#define GXX_LOG_HIT(closed) ;
#define GXX_LOG_WARNING(closed) ;
#define GXX_LOG_ERROR(closed);
#define GXX_LOG_BAD(closed) ;
//#define GXX_LOG(i,closed,...) void(0)
#endif

///功能宏----------------------------------------------
#define MY_ERROR    GXX_TRACE
#define MY_TRACE    GXX_TRACE

#define TRACE_ERROR GXX_TRACE
