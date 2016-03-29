
#include "stdafx.h"
#include <math.h>
#include "URLEncode.h"

#pragma warning(disable: 4244)

using namespace NS_SERVLET;

CURLEncode::CURLEncode(LPCTSTR lpszURL)
{
	CStringA strA_URL = CT2A(lpszURL);

	strA_URL = URLEncode(strA_URL);

	m_strURL = CA2T(strA_URL);
}

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
CStringA CURLEncode::Convert(char val) 
{
	CStringA csRet;
	csRet += "%";
	csRet += DecToHex(val, 16);	
	return  csRet;
}

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
CStringA CURLEncode::DecToHex(char num, int radix)
{	
	int temp = 0;	
	CStringA csTmp;
	int num_char = (int)num;
	
	static const char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


	// ISO-8859-1 
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
		num_char = 256 + num_char;

	while (num_char >= radix)
    {
		temp = num_char % radix;
		num_char = floor((float)(num_char / radix));
		csTmp = hexVals[temp];
    }
	
	csTmp += hexVals[num_char];

	if (csTmp.GetLength() < 2)
	{
		csTmp += '0';
	}

	CStringA strdecToHex(csTmp);
	// Reverse the String
	strdecToHex.MakeReverse(); 
	
	return strdecToHex;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
BOOL CURLEncode::IsUnsafe(char compareChar)
{
	BOOL bcharfound = FALSE;
	
	char tmpsafeChar = 0;
	
	static const CStringA csUnsafeString = "\"<>%\\^[]`+$,@:;/!#?=&";
	
	if (csUnsafeString.Find(compareChar) >= 0)
	{
		bcharfound = TRUE;
	}

	int char_ascii_value = 0;
	//char_ascii_value = __toascii(compareChar);
	char_ascii_value = (int)compareChar;

	if (!bcharfound && char_ascii_value >= 32 && char_ascii_value < 123)
	{
		return FALSE;
	}
	// found no unsafe chars, return false		
	else
	{
		return TRUE;
	}
	
	return TRUE;
}
// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
// TO URL ENCODE FORM.
CStringA CURLEncode::URLEncode(CStringA pcsEncode)
{	
	CStringA csEncode;
	CStringA csEncoded;	
	int m_length = 0;
	int ascii_value = 0;

	csEncode = pcsEncode;
	m_length = csEncode.GetLength();
	
	for (int ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
	{
		char ch = csEncode.GetAt(ichar_pos);
		
		//if (ch < 32) 
		//{
		//	ch = ch;
		//}

		if (!IsUnsafe(ch))
		{
			// Safe Character				
			csEncoded += CStringA(ch);
		}
		else
		{
			// get Hex Value of the Character
			csEncoded += Convert(ch);
		}
	}

	return csEncoded;
}


