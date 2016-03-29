
#pragma once

namespace NS_TTS
{

class CSpeechObjectToken : private COleDispatchDriver
{
public:

	CSpeechObjectToken(LPDISPATCH pDispatch);
	
	void SetId(LPCTSTR Id, LPCTSTR CategoryID, BOOL CreateIfNotExist);

	CString GetId();
	
	LPDISPATCH GetDataKey();
	
	LPDISPATCH GetCategory();
	
	CString GetDescription(LONG Locale = 0);
	
	CString GetAttribute(LPCTSTR AttributeName);
	
	LPUNKNOWN CreateInstance(LPUNKNOWN pUnkOuter, LONG ClsContext);
	
	void Remove(LPCTSTR ObjectStorageCLSID);
	
	CString GetStorageFileName(LPCTSTR ObjectStorageCLSID, LPCTSTR KeyName, LPCTSTR FileName, LONG Folder);
	
	void RemoveStorageFileName(LPCTSTR ObjectStorageCLSID, LPCTSTR KeyName, BOOL DeleteFile);
	
	BOOL IsUISupported(LPCTSTR TypeOfUI, VARIANT * ExtraData, LPUNKNOWN Object);
	
	void DisplayUI(LONG hWnd, LPCTSTR Title, LPCTSTR TypeOfUI, VARIANT * ExtraData, LPUNKNOWN Object);
	
	BOOL MatchesAttributes(LPCTSTR Attributes);

};

class CSpeechObjectTokens : private COleDispatchDriver
{
public:

	CSpeechObjectTokens(LPDISPATCH pDispatch);

	LONG GetCount();

	LPDISPATCH Item(LONG Index);

	LPUNKNOWN GetNewEnum();

};

}

using namespace NS_TTS;