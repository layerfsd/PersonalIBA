#pragma once

#include <stdlib.h>
#include <atlenc.h>
#include <wincrypt.h>
#include <string>
using namespace std;

//#pragma comment(lib, "Crypt32.lib")
class CDESArgo
{
private:
	CDESArgo();
public:
	~CDESArgo();
	static CDESArgo* GetInstance();

	//BOOL DESDecode(char* pcSrc, char* pcDst, PINT pnLen);
	BOOL DESEncode(char* pcDes, char* pcSrc, DWORD srcLen, int bufLen, PINT dstLen);
	BOOL DESEncodeEx(char* pcSrc, DWORD srcLen, string& strDest);
	BOOL DESEncodeHex(char* pcSrc, DWORD srcLen, string& strDest);

	BOOL DESDecode(char* pcSrc, char* pcDst, PINT pnLen);
	BOOL DESDecodeEx(char* pcSrc, int nSrcLen, string& strDest);
	BOOL DESDecodeHex(char* pcSrc, int nSrcLen, string& strDest);
	string UrlEncode(const string& src);
private:
	static CDESArgo* m_pstInstance;
	HCRYPTKEY m_hKey;
	HCRYPTPROV m_hProv;
	HCRYPTHASH m_hHash;
	void Init(char* pcKey, char* pcIv);
};