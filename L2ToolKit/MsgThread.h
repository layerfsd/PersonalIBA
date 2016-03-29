#pragma once

#include "thread.h"

class L2_EXT_CLASS CMsgThread : public CThread
{
public :

	CMsgThread(void);
	virtual ~CMsgThread(void);

	BOOL PostMessage(UINT Msg, WPARAM wParam = NULL, LPARAM lParam = NULL);
	void PostQuitMessage(int nExitCode = 0);

private :

	virtual int Run();

	virtual bool OnInit();
	virtual void OnExit(INT nExitCode);
	virtual void OnMesseage(UINT Msg, WPARAM wParam, LPARAM lParam);

	CMsgThread(const CMsgThread &rhs);
	CMsgThread &operator=(const CMsgThread &rhs);
	
};
