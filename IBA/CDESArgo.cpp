#include "stdafx.h"
#include "CDESArgo.h"

const char* DES_KEY = "60728412";
const char* DES_IV = "08793212";

CDESArgo* CDESArgo::m_pstInstance = NULL;

CDESArgo::CDESArgo()
{
	m_hKey = NULL;
	Init((char*)DES_KEY, (char*)DES_IV);
}

CDESArgo::~CDESArgo()
{
	if (m_hKey)
		CryptDestroyKey(m_hKey);
	if (m_hHash)
		CryptDestroyHash(m_hHash);
	if (m_hProv)
		CryptReleaseContext(m_hProv, 0);
}

CDESArgo* CDESArgo::GetInstance()
{
	if (m_pstInstance == NULL)
		m_pstInstance = new CDESArgo();
	return m_pstInstance;
}

void CDESArgo::Init(char* pcKey, char* pcIv)
{
    TCHAR szDebug[256];
    DWORD dwLength;
    try {
       // 获得密钥容器
       if( (!CryptAcquireContext(&m_hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0)) &&
		   (!CryptAcquireContext(&m_hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET)) )
	   {
            //wsprintf(szDebug, "Error %x during CryptAcquireContext!/n", GetLastError());
			throw szDebug;
       }
       // 创建MD5散列对象
       if(!CryptCreateHash(m_hProv, CALG_MD5, 0, 0, &m_hHash)) {
            //sprintf(szDebug, "Error %x during CryptCreateHash!/n", GetLastError());
            throw szDebug;
       }
       // 散列密钥
       dwLength = strlen(pcKey);
       if(!CryptHashData(m_hHash, (LPBYTE)pcKey, dwLength, 0)) {
            //sprintf(szDebug, "Error %x during CryptHashData!/n", GetLastError());
            throw szDebug;
       }
       // 创建会话密钥
       if(!CryptDeriveKey(m_hProv, CALG_DES, m_hHash, 0, &m_hKey)) {
            //sprintf(szDebug, "Error %x during CryptDeriveKey!/n", GetLastError());
            throw szDebug;
       }
       // 设置初始化向量.
       if(!CryptSetKeyParam(m_hKey, KP_IV, (LPBYTE)pcIv, 0)) {
            //sprintf(szDebug, "Error %x during CryptSetKeyParam!/n", GetLastError());
            throw szDebug;
       }
    } catch(LPTSTR lpszDebug) {
       // Destroy the hash object.
       if(m_hHash != 0) CryptDestroyHash(m_hHash);
       // Release the provider handle.
       if(m_hProv != 0) CryptReleaseContext(m_hProv, 0);
       throw lpszDebug;
    }
}

BOOL CDESArgo::DESEncode(char* pcDes, char* pcSrc, DWORD srcLen, int bufLen, PINT dstLen)
{
	if ((pcSrc == NULL)||(pcDes == NULL))
		return FALSE;

	char* pcBuf = new char[srcLen*2 + 8];
	::ZeroMemory(pcBuf, srcLen*2 + 8);
	memcpy(pcBuf, pcSrc, srcLen);

	if (!CryptEncrypt(m_hKey, 0, true, 0, (BYTE*)pcBuf, &srcLen, srcLen*2 + 8))
	{
		delete pcBuf;
		return FALSE;
	}

	if (!Base64Encode((BYTE*)pcBuf, srcLen, (LPSTR)pcDes, dstLen))
	{
		delete pcBuf;
		return FALSE;
	}

	delete pcBuf;
	return TRUE;
}

BOOL CDESArgo::DESEncodeEx(char* pcSrc, DWORD srcLen, string& strDest)
{
	if ((pcSrc == NULL)||(srcLen == 0))
		return FALSE;

	int nDestLen = 2*srcLen + 8; 
	char* pcDest = new char[nDestLen];
	::ZeroMemory(pcDest, nDestLen);

	if (DESEncode(pcDest, pcSrc, srcLen, srcLen + 8, &nDestLen))
	{
		strDest = string(pcDest, nDestLen);
		delete pcDest;
		return TRUE;
	}

	delete pcDest;
	return FALSE;
}

BOOL CDESArgo::DESEncodeHex(char* pcSrc, DWORD srcLen, string& strDest)
{
	int nDestLen = 2*srcLen + 8; 
	char* pcDest = new char[nDestLen];
	::ZeroMemory(pcDest, nDestLen);
	char cHexTemp[4] = {0};
	strDest = "";
	if (DESEncode(pcDest, pcSrc, srcLen, srcLen + 8, &nDestLen))
	{
		for (int i = 0; i < nDestLen; i++)
		{
			sprintf(cHexTemp, "%02X", pcDest[i]);
			strDest.append(cHexTemp, 2);
		}
		delete pcDest;
		return TRUE;
	}
	delete pcDest;
	return FALSE;
}

BOOL CDESArgo::DESDecode(char* pcSrc, char* pcDst, PINT pnLen) {
    if (pcSrc == NULL || pcDst == NULL || pnLen == NULL) {
       SetLastErrorEx(ERROR_INVALID_PARAMETER ,SLE_ERROR);
       return FALSE;
    }
	int nLen = strlen(pcSrc);
    if (!Base64Decode(pcSrc, nLen, (BYTE*)pcDst, pnLen))
       return FALSE;
    if (!CryptDecrypt(m_hKey, 0, true, 0, (BYTE*)pcDst, (DWORD*)pnLen))
		return FALSE;
    return TRUE;
}

BOOL  CDESArgo::DESDecodeEx(char* pcSrc, int nSrcLen, string& strDest)
{
	char* pcDest = new char[nSrcLen];
	::ZeroMemory(pcDest, nSrcLen);
	if (DESDecode(pcSrc, pcDest, &nSrcLen))
	{
		strDest = string(pcDest, nSrcLen);
		delete pcDest;
		return TRUE;
	}
	delete pcDest;
	return FALSE;
}

BOOL  CDESArgo::DESDecodeHex(char* pcSrc, int nSrcLen, string& strDest)
{
	char cHexTemp[3] = {0};
	unsigned char cTemp;
	string strTemp;
	for (int i = 0; i < nSrcLen; i=i+2)
	{
		cHexTemp[0] = pcSrc[i];
		cHexTemp[1] = pcSrc[i + 1];
		cTemp = (unsigned char)strtol(cHexTemp, NULL, 16);
		strTemp.append(1, cTemp);
	}

	int nTempLen = strTemp.length();
	char* pcDest = new char[nTempLen];
	::ZeroMemory(pcDest, nTempLen);
	if (DESDecode((char*)strTemp.c_str(), pcDest, &nTempLen))
	{
		strDest = string(pcDest, nTempLen);
		delete pcDest;
		return TRUE;
	}
	
	
	delete pcDest;
	return FALSE;
}

string CDESArgo::UrlEncode(const string& src)
{
    static    char hex[] = "0123456789ABCDEF";
    std::string dst;
    
    for (size_t i = 0; i < src.size(); i++)
    {
        unsigned char ch = src[i];
        if (isalnum(ch))
        {
            dst += ch;
        }
        else
            if (src[i] == ' ')
            {
                dst += '+';
            }
            else
            {
                unsigned char c = static_cast<unsigned char>(src[i]);
                dst += '%';
                dst += hex[c / 16];
				dst += hex[c % 16];
            }
    }
    return dst;
}
