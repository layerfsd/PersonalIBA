#pragma once

class  CWxMiddlewareDelete 
{
public:
	virtual void getNetbarIdAndDomainId(UINT * netbarId, UINT * domainId) = 0;
	virtual BOOL getMiddlewareCenterIPPort(TCHAR* ip, UINT * port) = 0;
	virtual BOOL checkin(LPCTSTR lpszNetId, LPCTSTR lpszTermId, LPCTSTR lpszPassword) = 0;
	virtual BOOL checkout(LPCTSTR lpszNetId, LPCTSTR lpszTermId) = 0;
	virtual void showmsg(LPCTSTR lpszTermId, LPCTSTR lpszUserID, LPCTSTR lpszPayAmount, LPCTSTR lpszGiveAmount, LPCTSTR lpszTime) = 0;
};