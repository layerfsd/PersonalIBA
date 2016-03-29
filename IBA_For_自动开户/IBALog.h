#pragma once

#include "Singleton.h"


class CIBALog : public CLog
{
	SINGLETON_DECLARE(CIBALog);

public :
	
	static CIBALog* GetInstance();

	enum ELOGTYPE
	{
		ELT_DEBUG = 0,
		ELT_COMMON = 1,
		ELT_API = 2,
		ELT_SERVLET = 4,
		ELT_HTTPREQ = 8,
		ELT_SQL = 16,
		ELT_LOCALSERVER = 32,
		ELT_PARAM = 64,
		ELT_RESPFIELD = 65536,
	};

	CString GetAppVer() const;
	CString GetAppBuildTime() const;
	CString GetFileHash() const;

private :

	void WriteLogHead();

private :
	 
	CString m_strVer;
	CString m_strComment;

	CString m_strFileMD5Hash;
};
