#include "stdafx.h"
#include ".\encrypt.h"

//******************************************************************************
/* Data structure for MD5 (Message-Digest) computation */
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

//******************************************************************************



void CEncrypt::CalcMD5(CString strIn, CString& strMD5)
{
	MD5_CTX ctx = {0};

	CStringA strTmp = CT2A(strIn);

	MD5Init(&ctx);

	MD5Update(&ctx, strTmp.GetBuffer(), strTmp.GetLength());

	strTmp.ReleaseBuffer();

	MD5Final(&ctx);

	TCHAR szTmp[33] = {0};

	for (UINT i = 0; i < 16; i++)
	{
		wsprintf(szTmp + i * 2, _T("%02x"), ctx.digest[i]);
	}

	strMD5 = szTmp;
}

void CEncrypt::CalcMac(CString strIn, CString& strMAC, CString strKey, BOOL bEncrypt)
{
	CStringA strInA = CT2A(strIn, 936);
	CStringA strKeyA = CT2A(strKey, 936);

	TCHAR szBuf[17] = {0};

	DES(strInA, szBuf, strKeyA, bEncrypt);

	strMAC = szBuf;
}

void CEncrypt::Decrypt(CString strSrc, CString strKey, CString& strDest, BOOL bReturnHex)
{
	strDest.Empty();

	UCHAR ucOut[8] = {0}, ucSrc[8] = {0}, ucKey[8] = {0};

	CEncrypt::AtoH(CW2A(strKey), (char*)ucKey, 8);		
	CEncrypt::AtoH(CW2A(strSrc), (char*)ucSrc, 8);	
	CEncrypt::des(ucSrc, ucOut, ucKey, FALSE);

	if (bReturnHex)
	{
		for (INT i = 0; i < 8; i++)
		{
			strDest.AppendFormat(_T("%02X"), ucOut[i]);
		}
	}
	else
	{
		char szTmp[9] = {0};
		memcpy(szTmp, ucOut, 8);
		strDest = CA2W(szTmp);
	}
}

CString CEncrypt::EncryptString(LPCTSTR lpszSrc, LPCTSTR lpszKey, TCHAR chMakeup/* = 0*/)
{
	CStringA strSrc = CW2A(lpszSrc);
		
	CString strRet;

	INT nTmp = strSrc.GetLength() % 8;

	nTmp = 8 - nTmp;

	for (INT i = 0; i < nTmp; i++)
	{
		strSrc.AppendChar(chMakeup); //²¹×ã8Î»
	}

	nTmp = strSrc.GetLength() / 8;

	UCHAR ucKey[8] = {0};

	AtoH(CW2A(lpszKey), (char*)ucKey, 8);		//put asc hex to bin

	for (INT i = 0; i < nTmp; i++)
	{
		UCHAR ucOut[8] = {0}, ucSrc[8] = {0};

		CopyMemory(ucSrc, strSrc.GetBuffer() + i * 8, 8);

		strSrc.ReleaseBuffer();

		des((UCHAR*)ucSrc, (UCHAR*)ucOut, (UCHAR*)ucKey, TRUE);	//decrypt pszInPDK to ucOut using pszKey

		TCHAR szOut[17] = {0};

		wsprintf(szOut,_T("%02X%02X%02X%02X%02X%02X%02X%02X"),
			ucOut[0],ucOut[1],ucOut[2],ucOut[3],ucOut[4],ucOut[5],ucOut[6],ucOut[7]);

		strRet.Append(szOut);		
	}

	return strRet;
}

CString CEncrypt::DecryptString(LPCTSTR lpszSrc, LPCTSTR lpszKey, TCHAR chMakeup /*= 0*/)
{
	CStringA strSrc = CW2A(lpszSrc);

	CString strRet;

	UCHAR ucKey[8] = {0};

	AtoH(CW2A(lpszKey), (char*)ucKey, 8);		//put asc hex to bin

	INT nTmp = strSrc.GetLength() / 16;

	for (INT i = 0; i < nTmp; i++)
	{
		UCHAR ucOut[8] = {0}, ucSrc[8] = {0};

		AtoH(strSrc.GetBuffer() + i * 16, (char*)ucSrc, 8);		//covn asc hex to bin

		strSrc.ReleaseBuffer();

		des((UCHAR*)ucSrc, (UCHAR*)ucOut, (UCHAR*)ucKey, FALSE);	//decrypt pszInPDK to ucOut using pszKey

		char szOut[9] = {0};

		CopyMemory(szOut, ucOut, 8);

		strRet += CA2W(szOut);		
	}

	nTmp = strRet.Find(chMakeup);

	if (nTmp >= 0)
	{
		strRet = strRet.Left(nTmp);
	}

	return strRet;
}

UCHAR CEncrypt::BtoH(char ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');			// Handle numerals
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);	// Handle capitol hex digits
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);	// Handle small hex digits
	return (255); 
}

void CEncrypt::AtoH(const char *src, char *dest, int destlen)
{
	const char *srcptr = src;
	while (destlen--)
	{
		*dest = BtoH(*srcptr++) << 4;	// Put 1st ascii byte in upper nibble.
		*dest++ += BtoH(*srcptr++);		// Add 2nd ascii byte to above.     
	}
}

void CEncrypt::Buffer2Byte(UCHAR *pucBuffer, UCHAR *pucOut, size_t ulLenght)
{
	UCHAR ucOut[8] = {0};

	for (size_t i = 0; i < ulLenght; i++)
	{
		ucOut[i % 8] ^= *pucBuffer;
		pucBuffer++;
	}

	memcpy(pucOut, ucOut, 8);
}

void CEncrypt::DES(const char* pszSrc, TCHAR* pszDest, const char* pszKey, BOOL bEncrypt)
{
	UCHAR ucOut[8] = {0}, ucMac[8] = {0}, ucKey[8] = {0};

	AtoH(pszKey, (char*)ucKey, 8);

	Buffer2Byte((UCHAR*)pszSrc, ucOut, strlen(pszSrc));

	des(ucOut, ucMac, ucKey, bEncrypt);

	TCHAR szRet[17] = {0};

	wsprintf(szRet, _T("%02X%02X%02X%02X%02X%02X%02X%02X"),
		ucMac[0], ucMac[1], ucMac[2], ucMac[3], 
		ucMac[4], ucMac[5], ucMac[6], ucMac[7]);

	lstrcpy(pszDest, szRet);
}

void CEncrypt::des(UCHAR *pucSource, UCHAR *pucDest, UCHAR *pucKey, BOOL bEncrypt)
{
	unsigned char TABLE1[64] = {0}, TABLE2[56] = {0}, TABLE3[48] = {0}, 
		TABLE4[64] = {0}, TABLE5[48] = {0}, key[64] = {0},
		nbrofshift = 0, temp1 = 0, temp2 = 0;

	int valindex = 0;
	int i = 0, j = 0, k = 0, iter = 0;

	/* INITIALIZE THE TABLES */
	/* Table - s1 */
	static unsigned char    s1[4][16] = {
		14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
			0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
			4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
			15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13                    };

	/* Table - s2 */
	static unsigned char    s2[4][16] = {
		15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
			3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
			0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
			13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9                    };

	/* Table - s3 */
	static unsigned char    s3[4][16] = {
		10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
			13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
			13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
			1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12                    };

	/* Table - s4 */
	static unsigned char    s4[4][16] = {
		7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
			13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
			10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
			3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14                    };

	/* Table - s5 */
	static unsigned char    s5[4][16] = {
		2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
			14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
			4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
			11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3                    };

	/* Table - s6 */
	static unsigned char    s6[4][16] = {
		12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
			10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
			9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
			4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13                    };

	/* Table - s7 */
	static unsigned char    s7[4][16] = {
		4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
			13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
			1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
			6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12                    };

	/* Table - s8 */
	static unsigned char    s8[4][16] = {
		13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
			1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
			7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
			2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11                    };


	/* Table - Shift */
	static unsigned char    shift[16] = {
		1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1                  };


	/* Table - Binary */
	static unsigned char    binary[64] = {
		0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1,
			0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1,
			1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
			1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1                  };

	/* MAIN PROCESS */
	/* Convert from 64-bit key into 64-byte key */
	for (i = 0; i < 8; i++) 
	{
		key[8*i] = ((j = *(pucKey + i)) / 128) % 2;
		key[8*i+1] = (j / 64) % 2;
		key[8*i+2] = (j / 32) % 2;
		key[8*i+3] = (j / 16) % 2;
		key[8*i+4] = (j / 8) % 2;
		key[8*i+5] = (j / 4) % 2;
		key[8*i+6] = (j / 2) % 2;
		key[8*i+7] = j % 2;
	}

	/* Convert from 64-bit data into 64-byte data */
	for (i = 0; i < 8; i++) 
	{
		TABLE4[8*i] = ((j = *(pucSource + i)) / 128) % 2;
		TABLE4[8*i+1] = (j / 64) % 2;
		TABLE4[8*i+2] = (j / 32) % 2;
		TABLE4[8*i+3] = (j / 16) % 2;
		TABLE4[8*i+4] = (j / 8) % 2;
		TABLE4[8*i+5] = (j / 4) % 2;
		TABLE4[8*i+6] = (j / 2) % 2;
		TABLE4[8*i+7] = j % 2;
	}

	/* Initial Permutation of Data */
	TABLE1[ 0] = TABLE4[57];
	TABLE1[ 1] = TABLE4[49];
	TABLE1[ 2] = TABLE4[41];
	TABLE1[ 3] = TABLE4[33];
	TABLE1[ 4] = TABLE4[25];
	TABLE1[ 5] = TABLE4[17];
	TABLE1[ 6] = TABLE4[ 9];
	TABLE1[ 7] = TABLE4[ 1];
	TABLE1[ 8] = TABLE4[59];
	TABLE1[ 9] = TABLE4[51];
	TABLE1[10] = TABLE4[43];
	TABLE1[11] = TABLE4[35];
	TABLE1[12] = TABLE4[27];
	TABLE1[13] = TABLE4[19];
	TABLE1[14] = TABLE4[11];
	TABLE1[15] = TABLE4[ 3];
	TABLE1[16] = TABLE4[61];
	TABLE1[17] = TABLE4[53];
	TABLE1[18] = TABLE4[45];
	TABLE1[19] = TABLE4[37];
	TABLE1[20] = TABLE4[29];
	TABLE1[21] = TABLE4[21];
	TABLE1[22] = TABLE4[13];
	TABLE1[23] = TABLE4[ 5];
	TABLE1[24] = TABLE4[63];
	TABLE1[25] = TABLE4[55];
	TABLE1[26] = TABLE4[47];
	TABLE1[27] = TABLE4[39];
	TABLE1[28] = TABLE4[31];
	TABLE1[29] = TABLE4[23];
	TABLE1[30] = TABLE4[15];
	TABLE1[31] = TABLE4[ 7];
	TABLE1[32] = TABLE4[56];
	TABLE1[33] = TABLE4[48];
	TABLE1[34] = TABLE4[40];
	TABLE1[35] = TABLE4[32];
	TABLE1[36] = TABLE4[24];
	TABLE1[37] = TABLE4[16];
	TABLE1[38] = TABLE4[ 8];
	TABLE1[39] = TABLE4[ 0];
	TABLE1[40] = TABLE4[58];
	TABLE1[41] = TABLE4[50];
	TABLE1[42] = TABLE4[42];
	TABLE1[43] = TABLE4[34];
	TABLE1[44] = TABLE4[26];
	TABLE1[45] = TABLE4[18];
	TABLE1[46] = TABLE4[10];
	TABLE1[47] = TABLE4[ 2];
	TABLE1[48] = TABLE4[60];
	TABLE1[49] = TABLE4[52];
	TABLE1[50] = TABLE4[44];
	TABLE1[51] = TABLE4[36];
	TABLE1[52] = TABLE4[28];
	TABLE1[53] = TABLE4[20];
	TABLE1[54] = TABLE4[12];
	TABLE1[55] = TABLE4[ 4];
	TABLE1[56] = TABLE4[62];
	TABLE1[57] = TABLE4[54];
	TABLE1[58] = TABLE4[46];
	TABLE1[59] = TABLE4[38];
	TABLE1[60] = TABLE4[30];
	TABLE1[61] = TABLE4[22];
	TABLE1[62] = TABLE4[14];
	TABLE1[63] = TABLE4[ 6];

	/* Initial Permutation of Key */
	TABLE2[ 0] = key[56];
	TABLE2[ 1] = key[48];
	TABLE2[ 2] = key[40];
	TABLE2[ 3] = key[32];
	TABLE2[ 4] = key[24];
	TABLE2[ 5] = key[16];
	TABLE2[ 6] = key[ 8];
	TABLE2[ 7] = key[ 0];
	TABLE2[ 8] = key[57];
	TABLE2[ 9] = key[49];
	TABLE2[10] = key[41];
	TABLE2[11] = key[33];
	TABLE2[12] = key[25];
	TABLE2[13] = key[17];
	TABLE2[14] = key[ 9];
	TABLE2[15] = key[ 1];
	TABLE2[16] = key[58];
	TABLE2[17] = key[50];
	TABLE2[18] = key[42];
	TABLE2[19] = key[34];
	TABLE2[20] = key[26];
	TABLE2[21] = key[18];
	TABLE2[22] = key[10];
	TABLE2[23] = key[ 2];
	TABLE2[24] = key[59];
	TABLE2[25] = key[51];
	TABLE2[26] = key[43];
	TABLE2[27] = key[35];
	TABLE2[28] = key[62];
	TABLE2[29] = key[54];
	TABLE2[30] = key[46];
	TABLE2[31] = key[38];
	TABLE2[32] = key[30];
	TABLE2[33] = key[22];
	TABLE2[34] = key[14];
	TABLE2[35] = key[ 6];
	TABLE2[36] = key[61];
	TABLE2[37] = key[53];
	TABLE2[38] = key[45];
	TABLE2[39] = key[37];
	TABLE2[40] = key[29];
	TABLE2[41] = key[21];
	TABLE2[42] = key[13];
	TABLE2[43] = key[ 5];
	TABLE2[44] = key[60];
	TABLE2[45] = key[52];
	TABLE2[46] = key[44];
	TABLE2[47] = key[36];
	TABLE2[48] = key[28];
	TABLE2[49] = key[20];
	TABLE2[50] = key[12];
	TABLE2[51] = key[ 4];
	TABLE2[52] = key[27];
	TABLE2[53] = key[19];
	TABLE2[54] = key[11];
	TABLE2[55] = key[ 3];

	/* 16 Iterations */
	for (iter = 1; iter < 17; iter++) 
	{
		for (i = 0; i < 32; i++)
			TABLE4[i] = TABLE1[32+i];

		/* Calculation of F(R, K) */
		/* Permute - E */
		TABLE3[ 0] = TABLE4[31];
		TABLE3[ 1] = TABLE4[ 0];
		TABLE3[ 2] = TABLE4[ 1];
		TABLE3[ 3] = TABLE4[ 2];
		TABLE3[ 4] = TABLE4[ 3];
		TABLE3[ 5] = TABLE4[ 4];
		TABLE3[ 6] = TABLE4[ 3];
		TABLE3[ 7] = TABLE4[ 4];
		TABLE3[ 8] = TABLE4[ 5];
		TABLE3[ 9] = TABLE4[ 6];
		TABLE3[10] = TABLE4[ 7];
		TABLE3[11] = TABLE4[ 8];
		TABLE3[12] = TABLE4[ 7];
		TABLE3[13] = TABLE4[ 8];
		TABLE3[14] = TABLE4[ 9];
		TABLE3[15] = TABLE4[10];
		TABLE3[16] = TABLE4[11];
		TABLE3[17] = TABLE4[12];
		TABLE3[18] = TABLE4[11];
		TABLE3[19] = TABLE4[12];
		TABLE3[20] = TABLE4[13];
		TABLE3[21] = TABLE4[14];
		TABLE3[22] = TABLE4[15];
		TABLE3[23] = TABLE4[16];
		TABLE3[24] = TABLE4[15];
		TABLE3[25] = TABLE4[16];
		TABLE3[26] = TABLE4[17];
		TABLE3[27] = TABLE4[18];
		TABLE3[28] = TABLE4[19];
		TABLE3[29] = TABLE4[20];
		TABLE3[30] = TABLE4[19];
		TABLE3[31] = TABLE4[20];
		TABLE3[32] = TABLE4[21];
		TABLE3[33] = TABLE4[22];
		TABLE3[34] = TABLE4[23];
		TABLE3[35] = TABLE4[24];
		TABLE3[36] = TABLE4[23];
		TABLE3[37] = TABLE4[24];
		TABLE3[38] = TABLE4[25];
		TABLE3[39] = TABLE4[26];
		TABLE3[40] = TABLE4[27];
		TABLE3[41] = TABLE4[28];
		TABLE3[42] = TABLE4[27];
		TABLE3[43] = TABLE4[28];
		TABLE3[44] = TABLE4[29];
		TABLE3[45] = TABLE4[30];
		TABLE3[46] = TABLE4[31];
		TABLE3[47] = TABLE4[ 0];

		/* KS Function Begin */
		if (bEncrypt) {
			nbrofshift = shift[iter-1];
			for (i = 0; i < (int) nbrofshift; i++) {
				temp1 = TABLE2[0];
				temp2 = TABLE2[28];
				for (j = 0; j < 27; j++) {
					TABLE2[j] = TABLE2[j+1];
					TABLE2[j+28] = TABLE2[j+29];
				}
				TABLE2[27] = temp1;
				TABLE2[55] = temp2;
			}
		} else if (iter > 1) {
			nbrofshift = shift[17-iter];
			for (i = 0; i < (int) nbrofshift; i++) {
				temp1 = TABLE2[27];
				temp2 = TABLE2[55];
				for (j = 27; j > 0; j--) {
					TABLE2[j] = TABLE2[j-1];
					TABLE2[j+28] = TABLE2[j+27];
				}
				TABLE2[0] = temp1;
				TABLE2[28] = temp2;
			}
		}

		/* Permute TABLE2 - PC2 */
		TABLE5[ 0] = TABLE2[13];
		TABLE5[ 1] = TABLE2[16];
		TABLE5[ 2] = TABLE2[10];
		TABLE5[ 3] = TABLE2[23];
		TABLE5[ 4] = TABLE2[ 0];
		TABLE5[ 5] = TABLE2[ 4];
		TABLE5[ 6] = TABLE2[ 2];
		TABLE5[ 7] = TABLE2[27];
		TABLE5[ 8] = TABLE2[14];
		TABLE5[ 9] = TABLE2[ 5];
		TABLE5[10] = TABLE2[20];
		TABLE5[11] = TABLE2[ 9];
		TABLE5[12] = TABLE2[22];
		TABLE5[13] = TABLE2[18];
		TABLE5[14] = TABLE2[11];
		TABLE5[15] = TABLE2[ 3];
		TABLE5[16] = TABLE2[25];
		TABLE5[17] = TABLE2[ 7];
		TABLE5[18] = TABLE2[15];
		TABLE5[19] = TABLE2[ 6];
		TABLE5[20] = TABLE2[26];
		TABLE5[21] = TABLE2[19];
		TABLE5[22] = TABLE2[12];
		TABLE5[23] = TABLE2[ 1];
		TABLE5[24] = TABLE2[40];
		TABLE5[25] = TABLE2[51];
		TABLE5[26] = TABLE2[30];
		TABLE5[27] = TABLE2[36];
		TABLE5[28] = TABLE2[46];
		TABLE5[29] = TABLE2[54];
		TABLE5[30] = TABLE2[29];
		TABLE5[31] = TABLE2[39];
		TABLE5[32] = TABLE2[50];
		TABLE5[33] = TABLE2[44];
		TABLE5[34] = TABLE2[32];
		TABLE5[35] = TABLE2[47];
		TABLE5[36] = TABLE2[43];
		TABLE5[37] = TABLE2[48];
		TABLE5[38] = TABLE2[38];
		TABLE5[39] = TABLE2[55];
		TABLE5[40] = TABLE2[33];
		TABLE5[41] = TABLE2[52];
		TABLE5[42] = TABLE2[45];
		TABLE5[43] = TABLE2[41];
		TABLE5[44] = TABLE2[49];
		TABLE5[45] = TABLE2[35];
		TABLE5[46] = TABLE2[28];
		TABLE5[47] = TABLE2[31];
		/* KS Function End */

		/* TABLE3 XOR TABLE5 */
		for (i = 0; i < 48; i++)
			TABLE3[i] = TABLE3[i] ^ TABLE5[i];

		/* 8 s-functions */
		valindex = s1[2*TABLE3[ 0]+TABLE3[ 5]]
		[2*(2*(2*TABLE3[ 1]+TABLE3[ 2])+
			TABLE3[ 3])+TABLE3[ 4]];
		valindex = valindex * 4;
		TABLE5[ 0] = binary[0+valindex];
		TABLE5[ 1] = binary[1+valindex];
		TABLE5[ 2] = binary[2+valindex];
		TABLE5[ 3] = binary[3+valindex];
		valindex = s2[2*TABLE3[ 6]+TABLE3[11]]
		[2*(2*(2*TABLE3[ 7]+TABLE3[ 8])+
			TABLE3[ 9])+TABLE3[10]];
		valindex = valindex * 4;
		TABLE5[ 4] = binary[0+valindex];
		TABLE5[ 5] = binary[1+valindex];
		TABLE5[ 6] = binary[2+valindex];
		TABLE5[ 7] = binary[3+valindex];
		valindex = s3[2*TABLE3[12]+TABLE3[17]]
		[2*(2*(2*TABLE3[13]+TABLE3[14])+
			TABLE3[15])+TABLE3[16]];
		valindex = valindex * 4;
		TABLE5[ 8] = binary[0+valindex];
		TABLE5[ 9] = binary[1+valindex];
		TABLE5[10] = binary[2+valindex];
		TABLE5[11] = binary[3+valindex];
		valindex = s4[2*TABLE3[18]+TABLE3[23]]
		[2*(2*(2*TABLE3[19]+TABLE3[20])+
			TABLE3[21])+TABLE3[22]];
		valindex = valindex * 4;
		TABLE5[12] = binary[0+valindex];
		TABLE5[13] = binary[1+valindex];
		TABLE5[14] = binary[2+valindex];
		TABLE5[15] = binary[3+valindex];
		valindex = s5[2*TABLE3[24]+TABLE3[29]]
		[2*(2*(2*TABLE3[25]+TABLE3[26])+
			TABLE3[27])+TABLE3[28]];
		valindex = valindex * 4;
		TABLE5[16] = binary[0+valindex];
		TABLE5[17] = binary[1+valindex];
		TABLE5[18] = binary[2+valindex];
		TABLE5[19] = binary[3+valindex];
		valindex = s6[2*TABLE3[30]+TABLE3[35]]
		[2*(2*(2*TABLE3[31]+TABLE3[32])+
			TABLE3[33])+TABLE3[34]];
		valindex = valindex * 4;
		TABLE5[20] = binary[0+valindex];
		TABLE5[21] = binary[1+valindex];
		TABLE5[22] = binary[2+valindex];
		TABLE5[23] = binary[3+valindex];
		valindex = s7[2*TABLE3[36]+TABLE3[41]]
		[2*(2*(2*TABLE3[37]+TABLE3[38])+
			TABLE3[39])+TABLE3[40]];
		valindex = valindex * 4;
		TABLE5[24] = binary[0+valindex];
		TABLE5[25] = binary[1+valindex];
		TABLE5[26] = binary[2+valindex];
		TABLE5[27] = binary[3+valindex];
		valindex = s8[2*TABLE3[42]+TABLE3[47]]
		[2*(2*(2*TABLE3[43]+TABLE3[44])+
			TABLE3[45])+TABLE3[46]];
		valindex = valindex * 4;
		TABLE5[28] = binary[0+valindex];
		TABLE5[29] = binary[1+valindex];
		TABLE5[30] = binary[2+valindex];
		TABLE5[31] = binary[3+valindex];

		/* Permute - P */
		TABLE3[ 0] = TABLE5[15];
		TABLE3[ 1] = TABLE5[ 6];
		TABLE3[ 2] = TABLE5[19];
		TABLE3[ 3] = TABLE5[20];
		TABLE3[ 4] = TABLE5[28];
		TABLE3[ 5] = TABLE5[11];
		TABLE3[ 6] = TABLE5[27];
		TABLE3[ 7] = TABLE5[16];
		TABLE3[ 8] = TABLE5[ 0];
		TABLE3[ 9] = TABLE5[14];
		TABLE3[10] = TABLE5[22];
		TABLE3[11] = TABLE5[25];
		TABLE3[12] = TABLE5[ 4];
		TABLE3[13] = TABLE5[17];
		TABLE3[14] = TABLE5[30];
		TABLE3[15] = TABLE5[ 9];
		TABLE3[16] = TABLE5[ 1];
		TABLE3[17] = TABLE5[ 7];
		TABLE3[18] = TABLE5[23];
		TABLE3[19] = TABLE5[13];
		TABLE3[20] = TABLE5[31];
		TABLE3[21] = TABLE5[26];
		TABLE3[22] = TABLE5[ 2];
		TABLE3[23] = TABLE5[ 8];
		TABLE3[24] = TABLE5[18];
		TABLE3[25] = TABLE5[12];
		TABLE3[26] = TABLE5[29];
		TABLE3[27] = TABLE5[ 5];
		TABLE3[28] = TABLE5[21];
		TABLE3[29] = TABLE5[10];
		TABLE3[30] = TABLE5[ 3];
		TABLE3[31] = TABLE5[24];

		/* TABLE1 XOR TABLE3 */
		for (i = 0; i < 32; i++) 
		{
			TABLE1[i+32] = TABLE1[i] ^ TABLE3[i];
			TABLE1[i] = TABLE4[i];
		}
	} /* End of Iter */

	/* Prepare Output */
	for (i = 0; i < 32; i++) 
	{
		j = TABLE1[i];
		TABLE1[i] = TABLE1[32+i];
		TABLE1[32+i] = j;
	}

	/* Inverse Initial Permutation */
	TABLE4[ 0] = TABLE1[39];
	TABLE4[ 1] = TABLE1[ 7];
	TABLE4[ 2] = TABLE1[47];
	TABLE4[ 3] = TABLE1[15];
	TABLE4[ 4] = TABLE1[55];
	TABLE4[ 5] = TABLE1[23];
	TABLE4[ 6] = TABLE1[63];
	TABLE4[ 7] = TABLE1[31];
	TABLE4[ 8] = TABLE1[38];
	TABLE4[ 9] = TABLE1[ 6];
	TABLE4[10] = TABLE1[46];
	TABLE4[11] = TABLE1[14];
	TABLE4[12] = TABLE1[54];
	TABLE4[13] = TABLE1[22];
	TABLE4[14] = TABLE1[62];
	TABLE4[15] = TABLE1[30];
	TABLE4[16] = TABLE1[37];
	TABLE4[17] = TABLE1[ 5];
	TABLE4[18] = TABLE1[45];
	TABLE4[19] = TABLE1[13];
	TABLE4[20] = TABLE1[53];
	TABLE4[21] = TABLE1[21];
	TABLE4[22] = TABLE1[61];
	TABLE4[23] = TABLE1[29];
	TABLE4[24] = TABLE1[36];
	TABLE4[25] = TABLE1[ 4];
	TABLE4[26] = TABLE1[44];
	TABLE4[27] = TABLE1[12];
	TABLE4[28] = TABLE1[52];
	TABLE4[29] = TABLE1[20];
	TABLE4[30] = TABLE1[60];
	TABLE4[31] = TABLE1[28];
	TABLE4[32] = TABLE1[35];
	TABLE4[33] = TABLE1[ 3];
	TABLE4[34] = TABLE1[43];
	TABLE4[35] = TABLE1[11];
	TABLE4[36] = TABLE1[51];
	TABLE4[37] = TABLE1[19];
	TABLE4[38] = TABLE1[59];
	TABLE4[39] = TABLE1[27];
	TABLE4[40] = TABLE1[34];
	TABLE4[41] = TABLE1[ 2];
	TABLE4[42] = TABLE1[42];
	TABLE4[43] = TABLE1[10];
	TABLE4[44] = TABLE1[50];
	TABLE4[45] = TABLE1[18];
	TABLE4[46] = TABLE1[58];
	TABLE4[47] = TABLE1[26];
	TABLE4[48] = TABLE1[33];
	TABLE4[49] = TABLE1[ 1];
	TABLE4[50] = TABLE1[41];
	TABLE4[51] = TABLE1[ 9];
	TABLE4[52] = TABLE1[49];
	TABLE4[53] = TABLE1[17];
	TABLE4[54] = TABLE1[57];
	TABLE4[55] = TABLE1[25];
	TABLE4[56] = TABLE1[32];
	TABLE4[57] = TABLE1[ 0];
	TABLE4[58] = TABLE1[40];
	TABLE4[59] = TABLE1[ 8];
	TABLE4[60] = TABLE1[48];
	TABLE4[61] = TABLE1[16];
	TABLE4[62] = TABLE1[56];
	TABLE4[63] = TABLE1[24];

	j = 0;
	for (i = 0; i < 8; i++) 
	{
		*(pucDest + i) = 0x00;
		for (k = 0; k < 7; k++)
			*(pucDest + i) = ((*(pucDest + i)) + TABLE4[j+k]) * 2;
		*(pucDest + i) = *(pucDest + i) + TABLE4[j+7];
		j += 8;
	}
}

void CEncrypt::CalcMD5Bytes(void* pData, const long nSize, char* pOut)
{
	MD5_CTX ctx = {0};

	MD5Init(&ctx);

	MD5Update(&ctx, pData, nSize);

	MD5Final(&ctx);

	TCHAR szTmp[33] = {0};

	for (UINT i = 0; i < 16; i++)
	{
		sprintf(pOut + i * 2, "%02x", ctx.digest[i]);
	}
}