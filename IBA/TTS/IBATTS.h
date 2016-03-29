#pragma once

#include "..\Singleton.h"
#include "..\IniConfig.h"
#include "SpeechVoice.h"

#define _USETTS

namespace NS_TTS
{

class CIBATTS : private CIniConfig
{
	SINGLETON_DECLARE(CIBATTS)

public :

	static CIBATTS* GetInstance();

	BOOL CreateSpeechVoiceObject();

	void SpeakText(LPCTSTR lpszText);

	void SpeakRegisterResult(INT nCredit, INT nPresent);
	void SpeakBundleTimeRegisterResult(INT nBundleTime, INT nBlance);
	void SpeakCreditResult(INT nCredit, INT nPresent, INT nBalance);
	void SpeakReturnResult(INT nReturned);
	void SpeakBundleTimeResult(INT nBundleTimeMoney);
	void SpeakActivation(INT nBalance, BOOL bHaveBirthday=FALSE);
	void SpeakBalance(INT nBalance);
	void SpeakReturn2Result(INT nReturned);
	//充值包时发声
	void SpeakCreditBundTimeResult(INT nCredit, INT nPresent, INT nBalance, INT nBundleTimeMoney);

	//解屏声音
	void SpeakUnlock();

	// 2011/07/05-gxx: 转包时补差价
	void SpeakBundleTimePatch(INT nMoney);

	// 2011/09/07-8201-gxx: 余额不足
	void SpeakReminderBalance(CString strTermID);

private :

	void LoadString();
	
	CSpeechVoice m_SpeechVoice;

	CString m_strCreditResult;

	CString m_strCreditResultWithPresent;

	CString m_strRegisterResult;

	CString m_strRegisterResultWithPresent;

	CString m_strBundleTimeRegisterResult;

	CString m_strReturnResult;

	CString m_strReturnResult2;

	CString m_strBundleTimeResult;

	CString m_strActivation;
	
	//充值包时
	CString m_strCreditResultWithBundTime;

	CString m_strBalance;

	// 2011/06/17-gxx: 增加节日快乐的激活语音
	CString m_strActivation2;

	// 
	CString m_strBundleTimePatchMoney; 

	// 余额不足语音提醒： XX号机器余额不足
	CString m_strReminderBalance;

};

}