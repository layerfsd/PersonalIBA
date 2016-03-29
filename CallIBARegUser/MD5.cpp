
#include "StdAfx.h"
#include ".\md5.h"

#include "WinMD5.h"
#pragma comment(lib, "WinMD5.lib")
#include "HelpFuc.h"


CString CMD5::MD5Buffer(LPCVOID lpBuf, size_t Len)
{
	MD5_CTX ctx = {0};

	MD5Init(&ctx);
	MD5Update(&ctx, lpBuf, (UINT)Len);
	MD5Final(&ctx);

	CString strMD5;

	for (UINT i = 0; i < 16; i++)
	{
		strMD5.AppendFormat(_T("%02x"), ctx.digest[i]);
	}

	return strMD5;
}


CString CMD5::MD5String(LPCTSTR lpszString)
{
	CStringA strTmp = CDllHelper::TC2AC(lpszString);
	return MD5Buffer(strTmp.GetBuffer(), strTmp.GetLength());
}


CString CMD5::MD5File(LPCTSTR lpszFileName)
{
	CString strMD5;

	CFile MyFile;

	if (MyFile.Open(lpszFileName, CFile::modeRead))
	{
		const UINT LEN_BUFFER = 65536;

		UCHAR Buffer[LEN_BUFFER] = {0};

		MD5_CTX ctx = {0};

		MD5Init(&ctx);
		
		while (TRUE)
		{
			UINT nRead = MyFile.Read(Buffer, LEN_BUFFER);

			MD5Update(&ctx, Buffer, nRead);

			if (nRead < LEN_BUFFER)
			{
				break;
			}
		}

		MyFile.Close();

		MD5Final(&ctx);

		for (UINT i = 0; i < 16; i++)
		{
			strMD5.AppendFormat(_T("%02x"), ctx.digest[i]);
		}
	}

	return strMD5;
}