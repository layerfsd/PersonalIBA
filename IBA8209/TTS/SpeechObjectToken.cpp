
#include "stdafx.h"
#include ".\SpeechObjectToken.h"

CSpeechObjectToken::CSpeechObjectToken(LPDISPATCH pDispatch) 
: COleDispatchDriver(pDispatch)
{
}

CString CSpeechObjectToken::GetId()
{
	CString result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

LPDISPATCH CSpeechObjectToken::GetDataKey()
{
	LPDISPATCH result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

LPDISPATCH CSpeechObjectToken::GetCategory()
{
	LPDISPATCH result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
}

CString CSpeechObjectToken::GetDescription(LONG Locale /*= 0*/)
{
	CString result;
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, Locale);
	return result;
}

void CSpeechObjectToken::SetId(LPCTSTR Id, LPCTSTR CategoryID, BOOL CreateIfNotExist)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BOOL ;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Id, CategoryID, CreateIfNotExist);
}

CString CSpeechObjectToken::GetAttribute(LPCTSTR AttributeName)
{
	CString result;
	static BYTE parms[] = VTS_BSTR ;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, AttributeName);
	return result;
}

LPUNKNOWN CSpeechObjectToken::CreateInstance(LPUNKNOWN pUnkOuter, LONG ClsContext)
{
	LPUNKNOWN result;
	static BYTE parms[] = VTS_UNKNOWN VTS_I4 ;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_UNKNOWN, (void*)&result, parms, pUnkOuter, ClsContext);
	return result;
}

void CSpeechObjectToken::Remove(LPCTSTR ObjectStorageCLSID)
{
	static BYTE parms[] = VTS_BSTR ;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ObjectStorageCLSID);
}

CString CSpeechObjectToken::GetStorageFileName(LPCTSTR ObjectStorageCLSID, LPCTSTR KeyName, LPCTSTR FileName, LONG Folder)
{
	CString result;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 ;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, ObjectStorageCLSID, KeyName, FileName, Folder);
	return result;
}

void CSpeechObjectToken::RemoveStorageFileName(LPCTSTR ObjectStorageCLSID, LPCTSTR KeyName, BOOL DeleteFile)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BOOL ;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ObjectStorageCLSID, KeyName, DeleteFile);
}

BOOL CSpeechObjectToken::IsUISupported(LPCTSTR TypeOfUI, VARIANT * ExtraData, LPUNKNOWN Object)
{
	BOOL result;
	static BYTE parms[] = VTS_BSTR VTS_PVARIANT VTS_UNKNOWN ;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, TypeOfUI, ExtraData, Object);
	return result;
}

void CSpeechObjectToken::DisplayUI(LONG hWnd, LPCTSTR Title, LPCTSTR TypeOfUI, VARIANT * ExtraData, LPUNKNOWN Object)
{
	static BYTE parms[] = VTS_I4 VTS_BSTR VTS_BSTR VTS_PVARIANT VTS_UNKNOWN ;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_EMPTY, NULL, parms, hWnd, Title, TypeOfUI, ExtraData, Object);
}

BOOL CSpeechObjectToken::MatchesAttributes(LPCTSTR Attributes)
{
	BOOL result;
	static BYTE parms[] = VTS_BSTR ;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Attributes);
	return result;
}

//******************************************************************************

CSpeechObjectTokens::CSpeechObjectTokens(LPDISPATCH pDispatch) 
: COleDispatchDriver(pDispatch)
{
}

LONG CSpeechObjectTokens::GetCount()
{
	LONG result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

LPDISPATCH CSpeechObjectTokens::Item(LONG Index)
{
	LPDISPATCH result;
	static BYTE parms[] = VTS_I4 ;
	InvokeHelper(0x0, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, parms, Index);
	return result;
}

LPUNKNOWN CSpeechObjectTokens::GetNewEnum()
{
	LPUNKNOWN result;
	InvokeHelper(0xfffffffc, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}