#pragma once

class CHooks
{
public:
	
	static BOOL Hook();
	static BOOL UnHook();
	
	static void CheckIsInRemoteDesktop();

	static HINSTANCE gModuleHandle;
	static BOOL gEnableRemoteInput;

private:
	CHooks(void);

	static BOOL  gIsInRemoteDesktop;

	static HHOOK gLLKeyBoardHook;
	static HHOOK gKeyBoardHook;

	static HHOOK gLLMouseHook;
	static HHOOK gMouseHook;

	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	static void MarkRemoteInputMessage();
	static BOOL IsRemoteInputMessage();

	static BOOL IsInRemoteDesktop();
};
