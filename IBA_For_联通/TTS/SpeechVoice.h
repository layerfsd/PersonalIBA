
#pragma once

#include "SpeechObjectToken.h"

namespace NS_TTS
{

enum SPEAKFLAGS
{	
	SPF_DEFAULT	= 0,
	SPF_ASYNC	= 1L << 0,
	SPF_PURGEBEFORESPEAK	= 1L << 1,
	SPF_IS_FILENAME	= 1L << 2,
	SPF_IS_XML	= 1L << 3,
	SPF_IS_NOT_XML	= 1L << 4,
	SPF_PERSIST_XML	= 1L << 5,
	SPF_NLP_SPEAK_PUNC	= 1L << 6,
	SPF_NLP_MASK	= SPF_NLP_SPEAK_PUNC,
	SPF_VOICE_MASK	= SPF_ASYNC | SPF_PURGEBEFORESPEAK | SPF_IS_FILENAME | SPF_IS_XML | SPF_IS_NOT_XML | SPF_NLP_MASK | SPF_PERSIST_XML,
	SPF_UNUSED_FLAGS	= ~SPF_VOICE_MASK
};


class CSpeechVoice : private COleDispatchDriver
{
public:
	
	CSpeechVoice(BOOL bAutoCreate = TRUE); // 调用 COleDispatchDriver 默认构造函数
	
	BOOL Create();

public: //属性

	/**
	*从零开始
	*/
	void SetVoice(LONG Index);

	void SetVoice(LPDISPATCH newValue);
	LPDISPATCH GetVoice();
	
	/**
	*速度范围为：-10 - +10
	*/
	void SetRate(LONG newValue);
	LONG GetRate();

	/**
	*音量范围为: 0 - 100
	*/
	void SetVolume(LONG newValue);
	LONG GetVolume();

	LPDISPATCH GetAudioOutput();
	void SetAudioOutput(LPDISPATCH newValue);

	LPDISPATCH GetAudioOutputStream();
	void SetAudioOutputStream(LPDISPATCH newValue);

	void SetAllowAudioOutputFormatChangesOnNextSet(BOOL newValue);
	BOOL GetAllowAudioOutputFormatChangesOnNextSet();

	LONG GetEventInterests();
	void SetEventInterests(LONG newValue);

	void SetPriority(LONG newValue);
	LONG GetPriority();

	void SetAlertBoundary(LONG newValue);
	LONG GetAlertBoundary();

	void SetSynchronousSpeakTimeout(LONG newValue);
	LONG GetSynchronousSpeakTimeout();

	LPDISPATCH GetStatus();

public: //方法

	LONG Speak(LPCTSTR Text, SPEAKFLAGS Flags = SPF_DEFAULT);

	LONG SpeakStream(LPDISPATCH Stream, SPEAKFLAGS Flags = SPF_DEFAULT);

	void Pause();

	void Resume();

	LONG Skip(LPCTSTR Type, LONG NumItems);

	LPDISPATCH GetVoices(LPCTSTR RequiredAttributes = _T(""), LPCTSTR OptionalAttributes = _T(""));
	
	LPDISPATCH GetAudioOutputs(LPCTSTR RequiredAttributes = _T(""), LPCTSTR OptionalAttributes = _T(""));
	
	BOOL WaitUntilDone(LONG msTimeout);
	
	LONG SpeakCompleteEvent();
	
	BOOL IsUISupported(LPCTSTR TypeOfUI, VARIANT * ExtraData);
	
	void DisplayUI(LONG hWndParent, LPCTSTR Title, LPCTSTR TypeOfUI, VARIANT * ExtraData);

};

}
