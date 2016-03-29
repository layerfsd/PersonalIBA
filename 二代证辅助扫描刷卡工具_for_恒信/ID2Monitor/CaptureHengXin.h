#pragma once
#include "capturebase.h"

// 恒信大客户版
class CCaptureHengXin : public CCaptureBase
{
public:
	CCaptureHengXin(void);
	~CCaptureHengXin(void);

	static const UINT ID_NETID = 0xE40334;
	static const UINT ID_IDNUMBER = 0x18302BC;
	static const UINT ID_NAME = 0x21902D4;

	static const char* STR_CLASS_NAME;
	static const char* STR_WINDOW_CAPTION;

	BOOL Create(
		BOOL bDialog = FALSE, 
		LPCTSTR lpszCaption = CCaptureHengXin::STR_WINDOW_CAPTION, 
		LPCTSTR lpszClassName = CCaptureHengXin::STR_CLASS_NAME);
};
//
//// 恒信普通版
//class CCaptureHengXin2: public CCaptureBase
//{
//public:
//	CCaptureHengXin2(void);
//	~CCaptureHengXin2(void);
//
//	static const UINT ID_NETID = 0xE40334;
//	static const UINT ID_IDNUMBER = 0x18302BC;
//	static const UINT ID_NAME = 0x21902D4;
//
//	static const char* STR_CLASS_NAME;
//	static const char* STR_WINDOW_CAPTION;
//
//	BOOL Create(
//		BOOL bDialog = FALSE, 
//		LPCTSTR lpszCaption = CCaptureHengXin2::STR_WINDOW_CAPTION, 
//		LPCTSTR lpszClassName = CCaptureHengXin2::STR_CLASS_NAME);
//};