
#pragma once

//---------------------------------------------------------------------
//- 点卡平台 V1.0,  2011-10-1 以前
//---------------------------------------------------------------------

void WINAPI CreatePayCardPlatformA(LPCSTR lpszParams);

void WINAPI CreatePayCardPlatformW(LPCWSTR lpszParams);


#ifdef UNICODE
#define CreatePayCardPlatform  CreatePayCardPlatformW
#else
#define CreatePayCardPlatform  CreatePayCardPlatformA
#endif // !UNICODE

void WINAPI DestroyPayCardPlatform();


//---------------------------------------------------------------------
//- 点卡平台 V1.1,  2011-11-18
//---------------------------------------------------------------------

/*初始化点卡平台*/
BOOL WINAPI DdnInitializePcpA(LPCSTR lpszParams);
BOOL WINAPI DdnInitializePcpW(LPCWSTR lpszParams);


/*显示点卡销售页面*/
BOOL WINAPI DdnShowPcp();


/*关闭点卡销售页面*/
BOOL WINAPI DdnHidePcp();


/*终止点卡平台*/
BOOL WINAPI DdnTerminatePcp();


#ifdef UNICODE
#define DdnInitializePcp DdnInitializePcpW
#else
#define DdnInitializePcp DdnInitializePcpA
#endif
