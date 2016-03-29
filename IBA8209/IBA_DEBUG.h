#pragma once

#define __GXX_TRACE    //开启跟踪
#include "gxx_trace.h"

#define IBA_TRACE   GXX_TRACE
#define IBA_ASSERT  ASSERT
#define IBA_ASSERT2 GXX_ASSERT

//-------------------------------------------------------
#define __UNIT_TRACE   // 开启单元跟踪

#ifdef __UNIT_TRACE

#define UNIT_TRACE                    GXX_TRACE
#define UNIT_AUTO_TRACE               GXX_AUTO_TRACE

#else

#define UNIT_TRACE(lpFmtStr,...)      void(0)
#define UNIT_AUTO_TRACE(lpFmtStr,...) void(0)

#endif
//-------------------------------------------------------

// 当把一个负数形式的UINT隐式转为double时，转换成的这个double值将是一个很大的值，
// 程序会出现无法预料的结果. 此宏进行无符号数据的负数形式检查

#ifdef DEBUG
#define IBA_CHECK_UINT(nUintNum) {\
	IBA_ASSERT(sizeof(nUintNum)==4);\
	int nIntN = (int)nUintNum;\
	if (nIntN<0)\
	{\
		IBA_ASSERT2(0,"此处无符号整数代表的是一个负数,请注意代码安全!");\
	}\
}
#else
#define IBA_CHECK_UINT(nUintNum) 
#endif

// 安全的转换(UINT to Double)
#define IBA_U_TO_D(nUintNum) (double((int)nUintNum))

//-------------------------------------------------------

#define IBA_LOG0 CIBALog::GetInstance()->Write
#define IBA_LOG  CIBALog::GetInstance()->WriteFormat
