
#include "stdafx.h"
#include "..\resource.h"
#include "..\IBA.h"
#include ".\ibatts.h"

SINGLETON_IMPLEMENT(CIBATTS)

CIBATTS::CIBATTS(void)
: m_SpeechVoice(FALSE)
{
	if (CIBAConfig::GetInstance()->GetEnableTTS())
	{
		CreateSpeechVoiceObject();
	}
}

CIBATTS::~CIBATTS(void)
{
	SaveValue(_T("TTSINFO"), _T("Balance"), m_strBalance);
	SaveValue(_T("TTSINFO"), _T("BundlePatch"), m_strBundleTimePatchMoney);
	SaveValue(_T("TTSINFO"), _T("Activation2"), m_strActivation2);
	SaveValue(_T("TTSINFO"), _T("ReminderBalance"), m_strReminderBalance);
}

void CIBATTS::LoadString()
{
	m_strIniFile = theApp.GetWorkPath() + _T("\\IBAConfig\\IBATTS.ini");

	ReadValue(_T("TTSINFO"), _T("CreditResult"), m_strCreditResult);
	m_strCreditResult.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("CreditResultWithPresent"), m_strCreditResultWithPresent);
	m_strCreditResultWithPresent.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("RegisterResult"), m_strRegisterResult);
	m_strRegisterResult.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("RegisterResultWithPresent"), m_strRegisterResultWithPresent);
	m_strRegisterResultWithPresent.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("BundleTimeRegisterResult"), m_strBundleTimeRegisterResult);
	m_strBundleTimeRegisterResult.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("ReturnResult"), m_strReturnResult);
	m_strReturnResult.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("BundleTimeResult"), m_strBundleTimeResult);
	m_strBundleTimeResult.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("Activation"), m_strActivation);
	m_strActivation.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("CreditResultWithBundTime"), m_strCreditResultWithBundTime);
	m_strCreditResultWithBundTime.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("Balance"), m_strBalance, _T("余额%.1f元。"));
	m_strBalance.Replace(_T("%.2f"), _T("%.1f"));

	ReadValue(_T("TTSINFO"), _T("ReturnResult2"), m_strReturnResult2, _T("退余额%.1f元。"));
	m_strReturnResult2.Replace(_T("%.2f"), _T("%.1f"));

	// 2011/06/17-gxx: 
	ReadValue(_T("TTSINFO"), _T("Activation2"), m_strActivation2, _T("祝您生日快乐,激活成功,余额%.2f元。"));
	m_strActivation2.Replace(_T("%.2f"), _T("%.1f"));

	// 2011/07/05-gxx: 
	ReadValue(_T("TTSINFO"), _T("BundlePatch"), m_strBundleTimePatchMoney, _T("转包时成功，补收金额%.2f元。"));
	m_strBundleTimePatchMoney.Replace(_T("%.2f"), _T("%.1f"));

	// 2011/09/07-8201-gxx: 
	ReadValue(_T("TTSINFO"), _T("ReminderBalance"), m_strReminderBalance, _T("%s 号机器余额不足。"));
}

BOOL CIBATTS::CreateSpeechVoiceObject()
{
	BOOL bRet = FALSE;

	LoadString();

	if (bRet = m_SpeechVoice.Create())
	{
		m_SpeechVoice.SetVolume(CIBAConfig::GetInstance()->GetTTSVol());
		m_SpeechVoice.SetVoice(CIBAConfig::GetInstance()->GetTTSVoice());
	}

	return bRet;
}

void CIBATTS::SpeakText(LPCTSTR lpszText)
{
	if (CIBAConfig::GetInstance()->GetEnableTTS())
	{
		CString strTmp(lpszText);
		
		strTmp.Replace(_T(".0"), _T(""));

		CIBALog::GetInstance()->WriteFormat(_T("SpeakText:%s"), strTmp);

		m_SpeechVoice.Speak(strTmp, SPF_ASYNC);
	}
}

void CIBATTS::SpeakRegisterResult(INT nCredit, INT nPresent)
{
	CString strTmp;

	if (nPresent > 0)
	{
		strTmp.Format(m_strRegisterResultWithPresent, nCredit / 100.0, nPresent / 100.0);
	}
	else
	{
		strTmp.Format(m_strRegisterResult, nCredit / 100.0, _T(""));
	}

	SpeakText(strTmp);
}

void CIBATTS::SpeakCreditResult(INT nCredit, INT nPresent, INT nBalance)
{
	CString strTmp;

	if (nPresent > 0)
	{
		strTmp.Format(m_strCreditResultWithPresent, nCredit / 100.0, nPresent / 100.0, nBalance / 100.0);
	}
	else
	{
		strTmp.Format(m_strCreditResult, nCredit / 100.0, nBalance / 100.0);
	}

	SpeakText(strTmp);
}

void CIBATTS::SpeakReturnResult(INT nReturned)
{
	CString strTmp;

	strTmp.Format(m_strReturnResult, nReturned / 100.0);

	SpeakText(strTmp);
}

void CIBATTS::SpeakReturn2Result(INT nReturned)
{
	CString strTmp;

	strTmp.Format(m_strReturnResult2, nReturned / 100.0);

	SpeakText(strTmp);
}

void CIBATTS::SpeakBundleTimeResult(INT nBundleTimeMoney)
{
	CString strTmp;

	strTmp.Format(m_strBundleTimeResult, nBundleTimeMoney / 100.0);

	SpeakText(strTmp);
}

void CIBATTS::SpeakActivation(INT nBalance, BOOL bHaveBirthday)
{
	CString strTmp;

	if (!bHaveBirthday)
	{
		strTmp.Format(m_strActivation, nBalance / 100.0);
	}
	else
	{
		strTmp.Format(m_strActivation2, nBalance / 100.0);
	}
	
	SpeakText(strTmp);
}


void CIBATTS::SpeakBundleTimeRegisterResult(INT nBundleTime, INT nBlance)
{
	CString strTmp;

	strTmp.Format(m_strBundleTimeRegisterResult, nBundleTime / 100.0, nBlance / 100.0);

	SpeakText(strTmp);
}

//充值包时发声
void CIBATTS::SpeakCreditBundTimeResult(INT nCredit, INT nPresent, INT nBalance, INT nBundleTimeMoney)
{
	CString strTmp;

	strTmp.Format(m_strCreditResultWithBundTime, nCredit / 100.0, 
		nPresent / 100.0, nBundleTimeMoney / 100.0, nBalance / 100.0);

	SpeakText(strTmp);

}

void CIBATTS::SpeakBalance(INT nBalance)
{
	CString strTmp;

	strTmp.Format(m_strBalance, nBalance / 100.0);

	SpeakText(strTmp);
}

void CIBATTS::SpeakUnlock()
{
	CString strSoundFile(theApp.GetWorkPath() + _T("\\IBAConfig\\Unlock.wav"));

	if (PathFileExists(strSoundFile))
	{
		sndPlaySound(strSoundFile, SND_ASYNC);
	}	
}

void NS_TTS::CIBATTS::SpeakBundleTimePatch(INT nMoney)
{
	CString strTmp;
	strTmp.Format(m_strBundleTimePatchMoney, nMoney / 100.0);

	SpeakText(strTmp);
}

void NS_TTS::CIBATTS::SpeakReminderBalance(CString strTermID)
{
	if (strTermID.IsEmpty())
	{
		IBA_ASSERT2(0,"终端号不能为空");
		return;
	}
	CString strTmp;
	strTmp.Format(m_strReminderBalance, strTermID.GetString());

	SpeakText(strTmp);
}