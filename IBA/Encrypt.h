#pragma once

class CEncrypt
{
private :
	CEncrypt(void);
	~CEncrypt(void);

public :

	static void Buffer2Byte(UCHAR *pucBuffer, UCHAR *pucOut, size_t ulLenght);
	static void AtoH(const char *src, char *dest, int destlen);
	static UCHAR BtoH(char ch);

	static void des(UCHAR *pucSource, UCHAR *pucDest, UCHAR *pucKey, BOOL bEncrypt);
	
	static void DES(const char* pszSrc, TCHAR* pszDest, const char* pszKey, BOOL bEncrypt);
	
	static void CalcMD5(CString strIn, CString& strMD5);

	static void CalcMD5Bytes(void* pData, const long nSize, char* pOut);

	static void CalcMac(CString strIn, CString& strMAC, CString strKey, BOOL bEncrypt = TRUE);
	
	static void Decrypt(CString strSrc, CString strKey, CString& strDest, BOOL bReturnHex = TRUE);

	static CString EncryptString(LPCTSTR lpszSrc, LPCTSTR lpszKey, TCHAR chMakeup = 0); 

	static CString DecryptString(LPCTSTR lpszSrc, LPCTSTR lpszKey, TCHAR chMakeup = 0);
	
};
