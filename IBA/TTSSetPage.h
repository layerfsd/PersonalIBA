#pragma once

#include "tts\SpeechVoice.h"
// CSetPageOfTTS 对话框

class CSetPageOfTTS : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfTTS)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfTTS();
	virtual ~CSetPageOfTTS();

// 对话框数据
	enum { IDD = IDD_SET_TTS };

private:

	BOOL m_bEnableTTS;
	
	CString m_strText;

	CBCGPComboBox m_cboTTS;
	
	CBCGPSliderCtrl m_sldVol;

	CSpeechVoice m_SpeechVoice;

	BOOL m_bAutoReturn; // 自动退款报语音

protected:

	BOOL IsTTSInstalled();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonTts();
	afx_msg void OnBnClickedCheckTts();
	
public:
	BOOL m_bReminderBalance;
};
