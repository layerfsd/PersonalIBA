#include "StdAfx.h"
#include ".\capturehengxin.h"

const char* CCaptureHengXin::STR_CLASS_NAME = "WindowsForms10.Window.8.app.0.33c0d9d";
const char* CCaptureHengXin::STR_WINDOW_CAPTION = "上机";

CCaptureHengXin::CCaptureHengXin(void)
{
	AddNetidID(ID_NETID);
	AddIdNumberID(ID_IDNUMBER);
	AddNameID(ID_NAME);
}

CCaptureHengXin::~CCaptureHengXin(void)
{
}

BOOL CCaptureHengXin::Create(BOOL bDialog /*= FALSE*/, LPCTSTR lpszCaption /*= STR_WINDOW_CAPTION*/, LPCTSTR lpszClassName /*= STR_CLASS_NAME*/)
{
	return CCaptureBase::Create(bDialog, lpszCaption, lpszClassName);
}

//
////----------------------------
//const char* CCaptureHengXin2::STR_CLASS_NAME = "WindowsForms10.Window.8.app.0.33c0d9d";
//const char* CCaptureHengXin2::STR_WINDOW_CAPTION = "上机";
//
//CCaptureHengXin2::CCaptureHengXin2(void)
//{
//	AddNetidID(ID_NETID);
//	AddIdNumberID(ID_IDNUMBER);
//	AddNameID(ID_NAME);
//}
//
//CCaptureHengXin2::~CCaptureHengXin2(void)
//{
//}
//
//BOOL CCaptureHengXin2::Create(BOOL bDialog /*= FALSE*/, LPCTSTR lpszCaption /*= STR_WINDOW_CAPTION*/, LPCTSTR lpszClassName /*= STR_CLASS_NAME*/)
//{
//	return CCaptureBase::Create(bDialog, lpszCaption, lpszClassName);
//}