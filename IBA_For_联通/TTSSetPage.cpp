// TTSSetPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "TTS\IBATTS.h"
#include ".\TTSSetPage.h"


// CSetPageOfTTS 对话框

IMPLEMENT_DYNAMIC(CSetPageOfTTS, CIBAPage)
CSetPageOfTTS::CSetPageOfTTS()
: CIBAPage(CSetPageOfTTS::IDD)
, m_bEnableTTS(TRUE)
, m_bReminderBalance(false)
, m_bAutoReturn(FALSE)
{
}

CSetPageOfTTS::~CSetPageOfTTS()
{
}

void CSetPageOfTTS::DoDataExchange(CDataExchange* pDX)
{
	CIBAPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_TTS, m_cboTTS);
	DDX_Control(pDX, IDC_SLIDER_SOUND, m_sldVol);
	DDX_Text(pDX, IDC_EDIT_MEMO, m_strText);
	DDX_Check(pDX, IDC_CHECK_TTS, m_bEnableTTS);
	DDX_Check(pDX, IDC_CHECK_REMINDER, m_bReminderBalance);
	DDX_Check(pDX, IDC_CHECK_AUTO_RETURN, m_bAutoReturn);
}


BEGIN_MESSAGE_MAP(CSetPageOfTTS, CIBAPage)
	ON_BN_CLICKED(IDC_BUTTON_TTS, OnBnClickedButtonTts)
	ON_BN_CLICKED(IDC_CHECK_TTS, OnBnClickedCheckTts)
END_MESSAGE_MAP()


// CSetPageOfTTS 消息处理程序

BOOL CSetPageOfTTS::OnInitDialog()
{
	m_bEnableTTS = CIBAConfig::GetInstance()->GetEnableTTS();
	
	m_strText = LOAD_STRING(IDS_TTSTEST);

	CIBAPage::OnInitDialog();

	CSpeechObjectTokens SpeechObjectTokens(m_SpeechVoice.GetVoices());

	for (INT i = 0; i < SpeechObjectTokens.GetCount(); i++)
	{
		CSpeechObjectToken SpeechObjectToken(SpeechObjectTokens.Item(i));

		m_cboTTS.AddString(SpeechObjectToken.GetDescription());
	}
	
	m_cboTTS.SetCurSel(CIBAConfig::GetInstance()->GetTTSVoice());

	m_sldVol.SetRangeMin(0);
	m_sldVol.SetRangeMax(100);
	m_sldVol.SetPos(CIBAConfig::GetInstance()->GetTTSVol());
	
	m_bReminderBalance = CNetBarConfig::GetInstance()->GetReminderBalance();
	m_bAutoReturn = CNetBarConfig::GetInstance()->GetReminderAutoReturn(); 
	UpdateData(FALSE);

	OnBnClickedCheckTts();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetPageOfTTS::OnOK()
{
	UpdateData();

	CIBAConfig::GetInstance()->SetEnableTTS(m_bEnableTTS);
	
	CIBAConfig::GetInstance()->SetTTSVol(m_sldVol.GetPos());

	CIBAConfig::GetInstance()->SetTTSVoice(m_cboTTS.GetCurSel());
		
	if (CIBAConfig::GetInstance()->GetEnableTTS())
	{
		CIBATTS::GetInstance()->CreateSpeechVoiceObject();
	}

	CNetBarConfig::GetInstance()->SetReminderBalance(m_bReminderBalance);

	// 2011/10/18-8206-gxx: 
	CNetBarConfig::GetInstance()->SetReminderAutoReturn(m_bAutoReturn);

	CIBAPage::OnOK();
}

void CSetPageOfTTS::OnBnClickedButtonTts()
{
	UpdateData();

	CSpeechVoice SpeechVoice;

	m_SpeechVoice.SetVoice(m_cboTTS.GetCurSel());
	m_SpeechVoice.SetVolume(m_sldVol.GetPos());
	m_SpeechVoice.Speak(m_strText, SPF_ASYNC);
}

void CSetPageOfTTS::OnBnClickedCheckTts()
{
	UpdateData();

	GetDlgItem(IDC_BUTTON_TTS)->EnableWindow(m_bEnableTTS);
	GetDlgItem(IDC_EDIT_MEMO)->EnableWindow(m_bEnableTTS);
	m_cboTTS.EnableWindow(m_bEnableTTS);
	m_sldVol.EnableWindow(m_bEnableTTS);
	GetDlgItem(IDC_CHECK_REMINDER)->EnableWindow(m_bEnableTTS);
	GetDlgItem(IDC_CHECK_AUTO_RETURN)->EnableWindow(m_bEnableTTS);
}

BOOL CSetPageOfTTS::IsTTSInstalled()
{
	TCHAR szTmp[MAX_PATH] = {0};
	ULONG Len = MAX_PATH; 

	CRegKey RegKey;

	if (ERROR_SUCCESS == RegKey.Open(HKEY_CLASSES_ROOT, _T("SAPI.SpVoice\\CLSID")))
	{
		RegKey.QueryStringValue(NULL, szTmp, &Len);

		RegKey.Close();

		CString strTmp;
		strTmp.Format(_T("CLSID\\%s\\InprocServer32"), szTmp);

		ZeroMemory(szTmp, MAX_PATH);
		Len = MAX_PATH;

		if (ERROR_SUCCESS == RegKey.Open(HKEY_CLASSES_ROOT, strTmp))
		{
			RegKey.QueryStringValue(NULL, szTmp, &Len);
			RegKey.Close();

			if (PathFileExists(szTmp))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}
