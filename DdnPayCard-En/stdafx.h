// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#include <afxhtml.h>
#include <afxdhtml.h>


#include <MsXml2.h>
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")

#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4267)

#define __GXX_LOG
#include "gxx_trace.h"

#include "G2XUI\\G2XIncS.h"
#ifdef DEBUG
#pragma comment(lib, "G2XUI\\G2XLibSSD.lib")
#else
#pragma comment(lib, "G2XUI\\G2XLibSS.lib")
#endif

using namespace G2XUI;
using namespace G2XUI::G2XUIFrame;

