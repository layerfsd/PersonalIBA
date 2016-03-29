
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>
#include <afxsock.h>
#include <afxtempl.h>
#include <afxdlgs.h>

#include "..\BCGCBPro\BCGCBProInc.h"			// BCGControlBar Pro

#define L2_SAFE_DELETE(p) { if (p != NULL) { delete p;  p = 0; } }

//******************************************************************************

#define L2_EXT_CLASS __declspec(dllimport)

#include "..\L2Camera\L2CameraPro.h"
#include "..\L2ADOKit\ADODBConnPool.h"
#include "..\L2ToolKit\L2ToolKitPro.h"
#include "..\L2HTMLKit\L2HTMLKitPro.h"

#include "IBA_DEBUG.h"
#include "gxx_property.h"
#include <afxhtml.h>

//*******************界面文字资源的取得方式控制,如果定义，那么就使用默认资源里的字符串.否则使用第三方字符串资源.
#define __USING_INTER_RESOURCE

#ifndef __USING_INTER_RESOURCE
#pragma message("                                           警告:正在编译多语言版本...")
#endif

#include ".\common\StrProviderLoader.h"
typedef CStringLoaderPage<CBCGPPropertyPage> CIBAPage;

#ifdef __USING_INTER_RESOURCE
#define CIBAString CString
#else
#include "IBAString.h"
#endif



//********************


//******************************************************************************

#define WM_REPLYMESSAGE			WM_APP + 1
#define WM_RECEIVECARD			WM_APP + 2
#define WM_CHILDCLOSE			WM_APP + 3
#define WM_EXECUTEREALNAME		WM_APP + 4
#define WM_OPERATIONLOG			WM_APP + 5
#define WM_TOOLBAR_LABEL_TEXT   WM_APP + 6
#define WM_UPDATE_IBA_STATUS    WM_APP + 7
#define WM_CLIENT_DIANKA        WM_APP + 8
#define WM_COMPVIEW_REFRESH		WM_APP + 9

#define WM_UPDATESEAT			(WM_APP + 11)	// 更新场景视图
#define WM_UPDATETERMINALINFO	(WM_APP + 12)	//更新终端信息
const int C_CANCLOSE		= 888;		//是否允许进程被关闭,收银台没有收到该消息时,会监控
//SFREG,保证它的运行
const int C_SFREGSTARTED	= 889;		//SFREG启动成功

#ifdef _DEBUG
#define OPEN_GLW_CARD_TESTING
#endif

#ifdef OPEN_GLW_CARD_TESTING
#pragma message("                    Debug，开启GLW开卡...")
#endif
