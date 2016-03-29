#pragma once

#include <map>

class L2_EXT_CLASS CTimer
{
public :
	CTimer(void);
	virtual ~CTimer(void);

public :

	BOOL StartTimer(UINT uTimerID, UINT uElapse, BOOL bOneShot = FALSE);

	BOOL StopTimer(UINT uTimerID);
	
	void StopAllTimers();

private :

	static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
	
	static const UINT INVALID_TIMERID = 0xFFFFFFFF;

private :

	void HandlerTimer(const UINT uID);

	UINT GetTimerId(const UINT uID);

	virtual void OnTimer(const UINT uTimerID) = 0;

private :	

	typedef std::map<UINT, UINT> TimerIDMap;
	
	TimerIDMap m_TimerIDMap;

	CCriticalSection m_cs;
	
};
