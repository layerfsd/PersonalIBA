#include "StdAfx.h"
#include ".\hooks.h"
#include <Wtsapi32.h>

#pragma comment(lib, "Wtsapi32.lib")

HHOOK CHooks::gLLKeyBoardHook = NULL;
HHOOK CHooks::gKeyBoardHook = NULL;

HHOOK CHooks::gLLMouseHook = NULL;
HHOOK CHooks::gMouseHook = NULL;

BOOL  CHooks::gIsInRemoteDesktop = FALSE;

HINSTANCE CHooks::gModuleHandle = NULL;

BOOL CHooks::gEnableRemoteInput = FALSE;

CHooks::CHooks(void)
{
}

LRESULT CALLBACK CHooks::LowLevelKeyboardProc(int nCode,
									  WPARAM wParam,
									  LPARAM lParam
									  )
{
	PKBDLLHOOKSTRUCT pKBHS = (PKBDLLHOOKSTRUCT)lParam;

	if (HC_ACTION == nCode && (pKBHS->flags & LLKHF_INJECTED))
	{
		MarkRemoteInputMessage();
	}

	return CallNextHookEx(gKeyBoardHook, nCode, wParam, lParam);

}

LRESULT CALLBACK CHooks::KeyboardProc(int nCode,
									  WPARAM wParam,
									  LPARAM lParam
									  )
{
	if (HC_ACTION == nCode && IsRemoteInputMessage())
	{
		return 1;
	}

	return CallNextHookEx(gKeyBoardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK CHooks::LowLevelMouseProc(int nCode,
								   WPARAM wParam,
								   LPARAM lParam
									  )
{

	PMSLLHOOKSTRUCT pMHS = (PMSLLHOOKSTRUCT)lParam;

	if (HC_ACTION == nCode && (pMHS->flags & LLMHF_INJECTED))
	{
		MarkRemoteInputMessage();
	}

	return CallNextHookEx(gKeyBoardHook, nCode, wParam, lParam);

}

LRESULT CALLBACK CHooks::MouseProc(int nCode, 
								   WPARAM wParam, 
								   LPARAM lParam)
{
	if (HC_ACTION == nCode && IsRemoteInputMessage())
	{
		return 1;
	}

	return CallNextHookEx(gKeyBoardHook, nCode, wParam, lParam);
}


BOOL CHooks::Hook()
{
#ifdef _DEBUG
	return TRUE;
#endif
	gKeyBoardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, NULL, GetCurrentThreadId());

	gMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, NULL, GetCurrentThreadId());

	gLLKeyBoardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, gModuleHandle, 0);

	gLLMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, gModuleHandle, 0);

	return (gKeyBoardHook != NULL) 
		&& (gMouseHook != NULL) 
		&& (gLLKeyBoardHook != NULL)  
		&& (gLLMouseHook != NULL);
}

BOOL CHooks::UnHook()
{
#ifdef _DEBUG
	return TRUE;
#endif
	if (gKeyBoardHook != NULL)
	{
		UnhookWindowsHookEx(gKeyBoardHook);
		gKeyBoardHook = NULL;
	}

	if (gLLKeyBoardHook != NULL)
	{
		UnhookWindowsHookEx(gLLKeyBoardHook);
		gLLKeyBoardHook = NULL;
	}

	if (gMouseHook != NULL)
	{
		UnhookWindowsHookEx(gMouseHook);
		gMouseHook = NULL;
	}

	if (gLLMouseHook != NULL)
	{
		UnhookWindowsHookEx(gLLMouseHook);
		gLLMouseHook = NULL;
	}

	return TRUE;
}

void CHooks::MarkRemoteInputMessage()
{
	LONG_PTR ExtraInfo = 2011;

	::SetMessageExtraInfo(ExtraInfo);
}

BOOL CHooks::IsRemoteInputMessage()
{
	if (gEnableRemoteInput)
	{
		return FALSE;
	}

	if (gIsInRemoteDesktop) //Ô¶³Ì×ÀÃæ
	{
		return TRUE;
	}

	LONG_PTR ExtraInfo = GetMessageExtraInfo();

	return ExtraInfo == 2011;
}

BOOL CHooks::IsInRemoteDesktop()
{
	BOOL bRet = FALSE;

	DWORD dwSectionId = 0, dwBytesReturned = 0;

	DWORD dwProcessId = GetCurrentProcessId();

	ProcessIdToSessionId(dwProcessId, &dwSectionId);

	LPTSTR pBuffer = NULL;

	DWORD dwRet = WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, 
		dwSectionId, WTSClientName, &pBuffer, &dwBytesReturned);

	if (dwRet != 0 && dwBytesReturned != 0 && pBuffer != NULL)
	{
		bRet = lstrlen(pBuffer) > 0;

		WTSFreeMemory(pBuffer);
	}

	return bRet;
}

void CHooks::CheckIsInRemoteDesktop()
{
	gIsInRemoteDesktop = IsInRemoteDesktop();
}