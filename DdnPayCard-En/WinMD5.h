
#pragma once

#pragma comment(lib, "WinMD5.lib")

struct MD5_CTX
{
	ULONG num[2];			/* number of _bits_ handled mod 2^64 */
	ULONG buf[4];			/* scratch buffer */
	BYTE  input[64];		/* input buffer */
	BYTE  digest[16];		/* actual digest after MD5Final call */
};

void WINAPI MD5Init(MD5_CTX* lpCtx);
void WINAPI MD5Update(MD5_CTX* lpCtx, const void * lpBuf, unsigned int nLen);
void WINAPI MD5Final(MD5_CTX* lpCtx);

