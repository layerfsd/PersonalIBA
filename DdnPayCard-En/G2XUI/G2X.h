#pragma once

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib") ////请修改为你的.lib文件路径
using namespace Gdiplus;

#define G2XUI_BEGIN namespace G2XUI{
#define G2X_END   }
#define USING_G2XUI using namespace G2XUI;
#define G2X_FRAME namespace G2XUIFrame{




#ifdef _ASSERT_BASE
#define _G2X_ASSERT(expr,msg) \
	_ASSERT_BASE(expr,msg)
#else
#define _G2X_ASSERT(expr,msg) \
	void(0)
#endif

#define G2X_ASSERT _G2X_ASSERT

typedef long					HG2XWND;

#include "G2XUIException.h"
#include "..\G2XUtility\G2XUtility.h"
#include "G2XUI.h"

typedef G2XUI::CG2XDrawImpl*	LPG2XDrawImpl;
typedef G2XUI::CG2XWnd*			LPG2XWnd;
typedef Gdiplus::Image* LPG2XImage;
typedef CWnd*                  CWND;
typedef const G2XUI::GDI_ACCELERATOR GDIC_ACCELERATOR;
typedef G2XUI::GDI_ACCELERATOR* LPGDI_ACCELERATOR;

BOOL			G2X_CLASSES		G2XStartup();
void			G2X_CLASSES		G2XShutdown();

LPG2XDrawImpl	G2X_CLASSES		G2XGetDrawImpl();
void			G2X_CLASSES		G2XSetDrawImpl(LPG2XDrawImpl lpDrawImpl);

LPG2XWnd		G2X_CLASSES     G2XFromHandle(HG2XWND);

BOOL			G2X_CLASSES		G2XIsWindow(HG2XWND);

BOOL            G2X_CLASSES     G2XCreateWnd(
									LPG2XWnd pWnd, 
									LPG2XWnd pParentWnd, 
									CWnd* pSuperWnd, 
									LPCTSTR lpszWndText,
									LPCRECT lpRect, 
									UINT nID);

void			G2X_CLASSES		G2XDeleteWnd(HG2XWND hG2XWnd);

BOOL			G2X_CLASSES		SendGdiMessage(
									HG2XWND hGdiWnd,
									UINT nGMsgType,
									UINT nID,
									WPARAM wParam=0,
									LPARAM lParam=0);

CWND			G2X_CLASSES		G2XGetSuperWnd(HG2XWND hG2XWnd);

LPG2XWnd		G2X_CLASSES G2XGetMainG2XWnd(HG2XWND hG2XWnd);
CRect			G2X_CLASSES G2XGetWindowRect(HG2XWND hG2XWnd);
CString			G2X_CLASSES G2XGetWindowText(HG2XWND hG2XWnd);
VOID			G2X_CLASSES G2XSetWindowText(HG2XWND hG2XWnd,LPCTSTR lpszText,BOOL bRepaint);
CRect			G2X_CLASSES G2XGetClientRect(HG2XWND hG2XWnd);
LPG2XWnd		G2X_CLASSES G2XGetFocus(HG2XWND hG2XWnd);
BOOL			G2X_CLASSES G2XSetFocus(HG2XWND hG2XMain,LPG2XWnd pWnd);
VOID			G2X_CLASSES G2XMoveWindow(HG2XWND hG2XWnd, LPCRECT lpRect, BOOL bRepaint);
UINT			G2X_CLASSES G2XGetControlID(HG2XWND);
UINT			G2X_CLASSES G2XGetChildCout(HG2XWND);

LPG2XWnd		G2X_CLASSES G2XGetParentWnd(HG2XWND hG2XWnd);

LPG2XWnd		G2X_CLASSES G2XGetOwner(HG2XWND hG2XWnd);
VOID			G2X_CLASSES G2XSetOwner(HG2XWND hG2XWnd, LPG2XWnd pG2XWnd);
BOOL			G2X_CLASSES G2XTranslateAccelator(HG2XWND hG2XMain, MSG* pMsg);
BOOL			G2X_CLASSES G2XRegisterAccelerator(HG2XWND hG2XMain, HG2XWND hG2X);
BOOL			G2X_CLASSES G2XDiregisterAccelerator(HG2XWND hG2XMain, HG2XWND hG2X );
BOOL			G2X_CLASSES G2XSetAccelerator(HG2XWND hG2X, LPGDI_ACCELERATOR pAccel, UINT nCount);
BOOL			G2X_CLASSES	G2XAddAccelator(HG2XWND hG2XWnd, GDIC_ACCELERATOR& Accel);
BOOL			G2X_CLASSES G2XRemoveAccelator(HG2XWND hG2XWnd, DWORD dwAccFlag);
BOOL			G2X_CLASSES G2XIsExistAccelator(HG2XWND hG2XWnd, DWORD dwAccFlag);

//-------------------------------------------------------------------------
//- 枚举和迭代相关函数
//-------------------------------------------------------------------------

// 获取子窗口的首位置
POSITION        G2X_CLASSES G2XGetHeadChild(HG2XWND hG2XWnd);

// 获取下一个子窗口，只能枚举hG2XWnd的第一层子窗口
LPG2XWnd		G2X_CLASSES G2XGetNext(HG2XWND hG2XWnd, POSITION& pos);

// 获取子窗口的尾位置
POSITION		G2X_CLASSES G2XGetTailChild(HG2XWND hG2XWnd);

// 获取前一个子窗口，只能枚举hG2XWnd的第一层子窗口
LPG2XWnd		G2X_CLASSES G2XGetPrev(HG2XWND hG2XWnd, POSITION& pos);

typedef BOOL (CALLBACK* G2XWNDENUMPROC)(HG2XWND, LPARAM);
BOOL			G2X_CLASSES G2XEnumChildWnd(HG2XWND hG2XWnd, G2XWNDENUMPROC enumProc, LPARAM lParam);

//-------------------------------------------------------------------------
//- 常用API
//-------------------------------------------------------------------------
LPG2XImage G2X_CLASSES G2XImageFromResource(HINSTANCE hInstance, UINT nResID, LPCTSTR lpszResType);



