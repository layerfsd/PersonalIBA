
#include "stdafx.h"
#include "..\IBA.h"
#include ".\SpeechVoice.h"

CSpeechVoice::CSpeechVoice(BOOL bAutoCreate /*= TRUE*/)
{
	if (bAutoCreate)
	{
		Create();
	}
}

BOOL CSpeechVoice::Create()
{
	if (m_lpDispatch != NULL)
	{
		return TRUE;
	}

	return CreateDispatch(_T("Sapi.SpVoice"));
}

LPDISPATCH CSpeechVoice::GetStatus()
{
	LPDISPATCH result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH CSpeechVoice::GetVoice()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetVoice(LPDISPATCH newValue)
{
	static BYTE parms[] = VTS_DISPATCH ;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
}

void CSpeechVoice::SetVoice(LONG Index)
{
	CSpeechObjectTokens SpeechObjectTokens(GetVoices());

	if (Index < 0 || Index >= SpeechObjectTokens.GetCount())
	{
		ASSERT(FALSE);

		return;
	}

	SetVoice(SpeechObjectTokens.Item(Index));
}

LPDISPATCH CSpeechVoice::GetAudioOutput()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetAudioOutput(LPDISPATCH newValue)
{
	static BYTE parms[] = VTS_DISPATCH ;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
}

LPDISPATCH CSpeechVoice::GetAudioOutputStream()
{
	LPDISPATCH result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetAudioOutputStream(LPDISPATCH newValue)
{
	static BYTE parms[] = VTS_DISPATCH ;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUTREF, VT_EMPTY, NULL, parms, newValue);
}

LONG CSpeechVoice::GetRate()
{
	LONG result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetRate(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

LONG CSpeechVoice::GetVolume()
{
	LONG result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetVolume(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

void CSpeechVoice::SetAllowAudioOutputFormatChangesOnNextSet(BOOL newValue)
{
	static BYTE parms[] = VTS_BOOL ;
	InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

BOOL CSpeechVoice::GetAllowAudioOutputFormatChangesOnNextSet()
{
	BOOL result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

LONG CSpeechVoice::GetEventInterests()
{
	LONG result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetEventInterests(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

void CSpeechVoice::SetPriority(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

LONG CSpeechVoice::GetPriority()
{
	LONG result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetAlertBoundary(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

LONG CSpeechVoice::GetAlertBoundary()
{
	LONG result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void CSpeechVoice::SetSynchronousSpeakTimeout(LONG newValue)
{
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
}

LONG CSpeechVoice::GetSynchronousSpeakTimeout()
{
	LONG result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LONG CSpeechVoice::Speak(LPCTSTR Text, SPEAKFLAGS Flags)
{
	try
	{
		LONG result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Text, Flags);
		return result;
	}
	catch (...)
	{
		AfxMessageBox(IDS_TTSERROR);
	}

	return -1;
}

LONG CSpeechVoice::SpeakStream(LPDISPATCH Stream, SPEAKFLAGS Flags/* = SPF_DEFAULT*/)
{
	LONG result;
	static BYTE parms[] = VTS_DISPATCH VTS_I4 ;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Stream, Flags);
	return result;
}

void CSpeechVoice::Pause()
{
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LONG CSpeechVoice::Skip(LPCTSTR Type, LONG NumItems)
{
	LONG result;
	static BYTE parms[] = VTS_BSTR VTS_I4 ;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Type, NumItems);
	return result;
}

void CSpeechVoice::Resume()
{
	InvokeHelper(0xf, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

LPDISPATCH CSpeechVoice::GetVoices(LPCTSTR RequiredAttributes, LPCTSTR OptionalAttributes)
{
	LPDISPATCH result;
	static BYTE parms[] = VTS_BSTR VTS_BSTR ;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, RequiredAttributes, OptionalAttributes);
	return result;
}

LPDISPATCH CSpeechVoice::GetAudioOutputs(LPCTSTR RequiredAttributes, LPCTSTR OptionalAttributes)
{
	LPDISPATCH result;
	static BYTE parms[] = VTS_BSTR VTS_BSTR ;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, RequiredAttributes, OptionalAttributes);
	return result;
}

BOOL CSpeechVoice::WaitUntilDone(LONG msTimeout)
{
	BOOL result;
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, msTimeout);
	return result;
}

LONG CSpeechVoice::SpeakCompleteEvent()
{
	LONG result;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

BOOL CSpeechVoice::IsUISupported(LPCTSTR TypeOfUI, VARIANT * ExtraData)
{
	BOOL result;
	static BYTE parms[] = VTS_BSTR VTS_PVARIANT ;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, TypeOfUI, ExtraData);
	return result;
}

void CSpeechVoice::DisplayUI(LONG hWndParent, LPCTSTR Title, LPCTSTR TypeOfUI, VARIANT * ExtraData)
{
	static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_PVARIANT ;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_EMPTY, NULL, parms, hWndParent, Title, TypeOfUI, ExtraData);
}