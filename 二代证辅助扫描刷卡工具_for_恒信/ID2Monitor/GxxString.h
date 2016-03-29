#pragma once
#include <stdarg.h>
/**************************************************************************
FILE: CGxxString.h
class: CGxxString<template> 
writer: guoxuxing
date: 2011-5-10
***************************************************************************/

namespace G2XUtility{

#ifndef ASSERT
  #define ASSERT(s) void(0);
#endif

#define GXX_CHAR        1
#define GXX_WCHAR       2
#define GXX_UNKNOWNCHAR 255
#define ZERO_CHAR   CharType(0)
#define INVALIDE_INDEX  (int(0-1))

template<typename type>
struct gxx_char_type
{
	enum {
		char_type = GXX_UNKNOWNCHAR,
		char_size = 0
	};
};

template<>
struct gxx_char_type<char>
{
	enum {
		char_type = GXX_CHAR,
		char_size = sizeof(char)
	};
};

template<>
struct gxx_char_type<wchar_t>
{
	enum {
		char_type = GXX_WCHAR,
		char_size = sizeof(wchar_t)
	};
};


template<typename CharType>
class CGxxString : public gxx_char_type<CharType>
{
public:
	bool IsWideCharCode() { 
		return gxx_char_type<CharType>::char_type == GXX_WCHAR ; }

	typedef const CharType const_char;
	typedef CharType* char_pointer; 
	typedef const CharType* const_char_pointer;
	typedef unsigned int Diff;
	typedef int Index;
	typedef CGxxString& reference;
	typedef const CGxxString& const_reference;

	static const size_t nDefaultSize = 1024;

	CGxxString(){
		_init( nDefaultSize );
	}
	CGxxString( const long nNum ){
		_init( 20 );
		operator=(nNum);
	}
	CGxxString( const unsigned long nNum ){
		_init( 20 );
		operator=(nNum);
	}
	CGxxString( const int nNum ){
		_init( 20 );
		operator=(nNum);
	}
	CGxxString( const unsigned int nNum ){
		_init( 20 );
		operator=(nNum);
	}
	CGxxString( const_char_pointer pStr){
		_init(1);
		Append(pStr);
	}
	CGxxString( const_reference myStr ){
		_init(myStr.GetLength()+1);
		Append(myStr);
	}

	virtual ~CGxxString(){
		if( m_pData )
		{
			delete []m_pData;
			m_pData = NULL;
		}
	}

public:
	bool ReAllocate( size_t nNewMaxSize,bool bCopyOld = false)
	{
		if( !bCopyOld )
		{
			if( m_pData )
			{
				delete []m_pData;
				m_pData = NULL;
			}	
			_init( nNewMaxSize + 1);
		}
		else
		{
			if( nNewMaxSize <= m_nMaxSize )
			{
				return true; // using old memory
			}
			char_pointer pTmp = new CharType[ nNewMaxSize + 1 ];
			if( !pTmp )
				return false;
			memcpy( pTmp,m_pData,(m_iAppendAt+1)*char_size );
			delete []m_pData;
			m_pData = pTmp;
			m_nMaxSize = nNewMaxSize;
		}
		return true;
	}

	int GetLength()const
	{
		return m_iAppendAt;
	}
	int GetLength(const_char_pointer pStr)
	{
		int nSize = 0;
		for( ;*pStr; pStr++)
			nSize++;
		return nSize;
	}
	const_char_pointer GetData()const
	{
		return m_pData;
	}

	void SetAppendAt(Index nOff)
	{
		m_iAppendAt = nOff;
		m_pData[nOff] = ZERO_CHAR;
	}

	void AppendChar(CharType ch)
	{
		CharType chs[2] = {ch,ZERO_CHAR};
		Append(chs);
	}
	void Append(const_char_pointer pStr)
	{
		Append(pStr,-1);
	}
	void Append(const_char_pointer pStr, const int nCount)
	{
		int nLen = nCount;
		if (nCount < 0)
		{
			nLen = GetLength(pStr);
		}
		int nNewAdding = m_iAppendAt + nLen;
		if( nNewAdding > (int)m_nMaxSize )
		{
			if( !ReAllocate(nNewAdding,true) )
				return;
		}
		memcpy(&m_pData[m_iAppendAt],pStr,nLen*char_size);
		m_iAppendAt += nLen;
		m_pData[m_iAppendAt] = ZERO_CHAR;
	}
	void Append(const_reference str)
	{
		Append(str.GetData() );
	}

	void AppendFormat(const_char_pointer pFmt,...)
	{
		ASSERT(pFmt != NULL);
		va_list args;
		va_start(args,pFmt);
		CharType _buf[10240];
		_Format(_buf,pFmt,args);	
		Append(_buf);
		va_end(args);
	}
	void Format(const_char_pointer pFmt,...)
	{
		ASSERT(pFmt != NULL);
		va_list args;
		va_start(args,pFmt);
		CharType _buf[10240];
		_Format(_buf,pFmt,args);	
		SetAppendAt(0);
		Append(_buf);
		va_end(args);
	}
	CGxxString Right(Diff nCount)
	{
		int nLen = GetLength();
		if( nLen <= (int)nCount )
			return *this;
		return CGxxString(&m_pData[nLen-nCount]);
	}
	CGxxString Left(Diff nCount)
	{
		if( nCount >= (Diff)GetLength() )
			return *this;
		CharType ch = m_pData[nCount];
		m_pData[nCount] = ZERO_CHAR;
		CGxxString tmp(m_pData);
		m_pData[nCount] = ch;
		return tmp;
	}
	void Insert(Index iAt,const_char_pointer pStr)
	{
		Insert(iAt,pStr,-1);		
	}
	void Insert(Index iAt,const_char_pointer pStr, const int nCount)
	{	
		int nLen = nCount;
		if (nCount < 0)
		{
			nLen = GetLength(pStr);
		}
		if (iAt < 0)
		{
			iAt = 0;
			ASSERT(0);
		}
		if (iAt >= m_iAppendAt)
		{
			Append(pStr,nCount);
		}
		else
		{
			int nNewAdding = m_iAppendAt + nLen;
			if( nNewAdding > (int)m_nMaxSize )
			{
				if( !ReAllocate(nNewAdding,true) )
					return;
			}
			memmove(&m_pData[iAt+nLen],&m_pData[iAt],(m_iAppendAt-iAt+1)*char_size);
			memcpy(&m_pData[iAt],pStr,nLen*char_size);
			m_iAppendAt += nLen;
			m_pData[m_iAppendAt] = ZERO_CHAR;
		}
	}
	CGxxString Mid(Index iAt,Diff nDiff)
	{
		if( Index(iAt + nDiff) > (Index)GetLength())
			return &m_pData[iAt];
		CharType ct = m_pData[iAt+nDiff];
		m_pData[iAt+nDiff] = ZERO_CHAR;
		CGxxString tmp(&m_pData[iAt]);
		m_pData[iAt+nDiff] = ct;
		return tmp;
	}
	CGxxString Mid(Index iAt)
	{
		ASSERT(iAt<(Index)GetLength());
		return &m_pData[iAt];
	}

	// cut away the tail of this string by the "chBy"
	CGxxString& CutTail(const CharType chBy)
	{
		Index iAt = ReverseFind(chBy);
		if( iAt == INVALIDE_INDEX )
			return *this;
		SetAppendAt(iAt);
		return *this;	
	}
	// cut away the tail of this string by the length
	CGxxString& CutTail(const Diff nDiff)
	{
		if( nDiff >= (Diff)GetLength() )
			SetAppendAt(0);
		else
			SetAppendAt(m_iAppendAt-nDiff);
		return *this;
	}
	CharType GetAt(Index iAt)const
	{
		return m_pData[iAt];
	}
	void SetAt(Index iAt,const CharType ch)
	{
		ASSERT(ch != ZERO_CHAR);
		m_pData[iAt] = ch;
	}
	Index ReverseFind(const CharType chBy, Index nBeforeAt = -1)const
	{
		Index iF = m_iAppendAt;
		if (nBeforeAt >= 0 )
		{
			iF = nBeforeAt;
		}
		for( ; iF >= 0 ; iF --)
		{
			if( GetAt(iF) == chBy )
				break;
		}
		return iF;
	}
	CGxxString& Reverse()
	{
		int nSize = GetLength() / 2;
		for(int i = 0 ; i<  nSize; i ++)
		{
			CharType ch = GetAt(i);
			CharType ch2 = GetAt( GetLength()-1-i);
			SetAt( i,ch2);
			SetAt( GetLength()-1-i , ch);
		}
		return (*this);
	}
	CGxxString& ToUpper(){
		char_pointer pCh = m_pData;
		while( *pCh )
		{
			if( CharType('a')<=*pCh && *pCh<=CharType('z'))
				*pCh -= 32;
			*pCh++;
		}
		return *this;
	}
	CGxxString& ToLower(){
		char_pointer pCh = m_pData;
		while( *pCh )
		{
			if( CharType('A')<=*pCh && *pCh<=CharType('Z'))
				*pCh += 32;
			*pCh++;
		}
		return *this;
	}
	bool IsEmpty()const{
		return GetLength() == 0;
	}
	CGxxString& Remove( Index iFrom,Diff nLen ){
		if( iFrom < 0 || iFrom >= m_iAppendAt )
			return *this;
		if( long(iFrom + nLen) >= m_iAppendAt ){
			SetAppendAt( iFrom );
			return *this;
		}
		Index iLastAt = iFrom + nLen;
		for(Index i = iLastAt ; i < m_iAppendAt ; i ++ )
		{
			m_pData[ i - nLen ] = m_pData[i];
		}
		m_iAppendAt -= nLen;
		m_pData[ m_iAppendAt ] = ZERO_CHAR;
		return *this;
	}
	CGxxString& Replace( const_char ch, const_char newCh)
	{
		if (newCh == ZERO_CHAR)
		{   //移除所有的字符ch
			char_pointer pFront = m_pData;
			char_pointer pIt = m_pData;
			while(*pIt != ZERO_CHAR)
			{
				if (*pIt != ch){
					*pFront = *pIt;
					pFront++;
				}
				else{
					m_iAppendAt--;
				}
				pIt++;
			}
			m_pData[m_iAppendAt] = ZERO_CHAR;
		}
		else
		{
			char_pointer pIt = m_pData;
			while(*pIt != ZERO_CHAR)
			{
				if (*pIt == ch){
					*pIt = newCh;
				}
				pIt++;
			}
		}
		return *this;
	}
	CGxxString& Replace( const_char_pointer oldStr, const_char_pointer newStr)
	{
		if (oldStr == (void*)0)
		{
			ASSERT(0);
			return *this;
		}
		if (GetLength(oldStr) == 0)
		{
			return *this;
		}
		if (newStr == (void*)0)
		{
			ASSERT(0);
			return *this;
		}
		
		CGxxString strTmp = *this;
		int nFrom = 0;
		int nFound = strTmp.FindString(oldStr, nFrom);
		if (nFound >= 0)
		{
			SetAppendAt(0);
		}
		int nLen = GetLength(oldStr);
		while( nFound >= 0 )
		{
			Append( strTmp.Mid(nFrom, nFound- nFrom) );
			Append( newStr );
			nFrom = nFound + nLen;
			nFound = strTmp.FindString(oldStr, nFrom);
		}
		if (nFrom > 0)
		{
			Append(strTmp.Mid(nFrom));
		}
		return *this;
	}
	CGxxString& Trim(const_char ch = (CharType)' '){
		TrimRight(ch);
		TrimLeft(ch);
		return *this;
	}
	CGxxString& TrimLeft(const_char ch = (CharType)' '){
		int iLen = 0;
		for(int i = 0 ; i< m_iAppendAt ;i ++)
		{
			if( m_pData[i] != ch)
			{
				break;
			}
			else
			{
				iLen ++;
			}
		}
		if( iLen > 0 )
		{
			Remove(0,iLen);
		}
		return *this;
	}
	CGxxString& TrimLeft(const_char_pointer pCh){
		if (!pCh)
		{
			return *this;
		}
		int nLen = GetLength(pCh);
		int nEnd = -1;	
		int nRes = FindString(pCh,0);
		if (nRes != 0)
		{
			return *this;
		}
		while( nRes >= 0)
		{
			if (nRes % nLen == 0)
			{
				nEnd = nRes + nLen;
				nRes = FindString(pCh, nEnd);
				if (nEnd != nRes)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		if ( nEnd!=-1 )
		{
			Remove(0, nEnd);
		}

		return *this;
	}
	CGxxString& TrimRight(const_char ch = (CharType)' '){
		Diff nDiff = 0;
		for(int i = m_iAppendAt-1 ;i >=0 ; i --)
		{
			if( m_pData[i] != ch )
				break;
			nDiff ++;
		}
		if( nDiff != 0 )
			CutTail(nDiff);
		return *this;	
	}
	CGxxString& TrimRight(const_char_pointer pStr)
	{
		if (!pStr)
		{
			return *this;
		}
		int nLen = GetLength(pStr);
		if (nLen > GetLength() || nLen == 0)
		{
			return *this;
		}
		while ( nLen <= GetLength() && Mid(GetLength()-nLen) == pStr )
		{
			CutTail((Diff)nLen);
		}
		return *this;
	}
	int CompareNoCase(reference str)const
	{
		int nLen1 = GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 == 0 && nLen2 == 0)
			return 0;
		int nMin = nLen2 > nLen1?nLen1:nLen2;
		for(int i = 0 ; i < nMin ; i ++ )
		{
			CharType ch1 = GetAt(i);
			CharType ch2 = str.GetAt(i);
			if( CharType(65)<=ch1 && ch1<=CharType(90) )
				ch1 += 32;
			if( CharType(65)<=ch2 && ch2<=CharType(90) )
				ch2 += 32;
			if( ch1 < ch2 )
				return -1;
			if( ch1 > ch2 )
				return 1;
		}
		if( nLen2 == nLen1 )
			return 0;
		if( nLen1 < nLen2 )
			return -1;
		return 1;
	}

	int Compare(const_reference str)const
	{
		int nLen1 = GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 == 0 && nLen2 == 0)
			return 0;
		int nMin = nLen2 > nLen1?nLen1:nLen2;
		for(int i = 0 ; i < nMin ; i ++ )
		{
			CharType ch1 = GetAt(i);
			CharType ch2 = str.GetAt(i);
			if( ch1 < ch2 )
				return -1;
			if( ch1 > ch2 )
				return 1;
		}
		if( nLen2 == nLen1 )
			return 0;
		if( nLen1 < nLen2 )
			return -1;
		return 1;
	}
	int FindString(const_char_pointer pStr,Index from)
	{
		if( pStr == NULL || pStr[0] == ZERO_CHAR)
		{
			return -1;
		}
		int nLen = GetLength(pStr);
		if( nLen > (m_iAppendAt - from) ) //比源字符串长直接返回
		{
			return -1;
		}
		int iResult = -1;	
		int iEnd = m_iAppendAt - nLen;
		for(int i = from ; i <= iEnd ; i ++)
		{
			if( memcmp( &m_pData[i],pStr,nLen*char_size ) == 0 )
			{
				iResult = i;
				break;
			}

		}
		return iResult;
	}

	// 分割字符换
	int Split( const_char_pointer pSplit,std::vector<CGxxString>&strArray)
	{
		strArray.clear();
		if( pSplit == NULL || pSplit[0] == ZERO_CHAR )
			return 0;

		int nLen = GetLength(pSplit);
		if( nLen > GetLength() )
			return 0;

		INT *iIndexStart = new int[m_iAppendAt];
		INT *iIndexEnd = new int[m_iAppendAt];

		int iCount = 0;
		int iFrom = 0;
		while(TRUE)
		{
			iIndexStart[ iCount ] = iFrom;
			iFrom = FindString( pSplit, iFrom );
			if( iFrom == -1)
			{
				iIndexEnd[ iCount ] = m_iAppendAt -1;
				break;
			}
			iIndexEnd[ iCount ] = iFrom -1;
			++iCount;
			iIndexStart[ iCount ] = iFrom + nLen;
			iFrom += nLen;
		} 
		iIndexEnd[ iCount ] = m_iAppendAt - 1;

		for( int i = 0 ; i <= iCount; i ++)
		{
			if( iIndexStart[i] == m_iAppendAt)
				break;
			strArray.push_back( Mid(iIndexStart[i],iIndexEnd[i]-iIndexStart[i]+1));
		}
		delete []iIndexStart;
		delete []iIndexEnd;
		return (int)strArray.size();
	}

	// 分割出来的子串转换为整数
	int Split( const_char_pointer pSplit,std::vector<int>& intArray)
	{
		std::vector<CGxxString> strArray;
		int nCount = Split(pSplit,strArray);
		for(size_t i=0; i < strArray.size(); i++)
		{
			intArray.push_back(strArray[i].ParseInt());
		}
		return nCount;
	} 
	CGxxString& CutDown(const_char ch) // 根据ch来缩减字符串，将连续重复的字符ch缩减成为1个ch，例如: "abbbbccddbbb123" 指定b去缩减，结果="abccddb123"
	{
		int nCount = 0;
		char_pointer pTmp = m_pData;
		char_pointer pAt = m_pData;
		pTmp++;
		while(*pTmp)
		{
			if (*pTmp != ch || (*pTmp == ch && *pAt != ch))
			{
				pAt++;
				*pAt = *pTmp;
			}
			else
			{
				nCount++;			
			}
			pTmp++;
		}
		m_iAppendAt -= nCount;
		m_pData[m_iAppendAt] = ZERO_CHAR;
		return *this;
	}

	CGxxString& FillCharFromLeft(unsigned long nLimit, const_char ch ) // 如果字符串的长度不足nLimit,那么就在字符串的左边插入 nLimit - GetLenght() 个ch
	{
		if (GetLength() >= (int)nLimit)
		{
			return *this;
		}
		CGxxString str(*this);
		ReAllocate(nLimit);
		SetAppendAt(0);
		for (unsigned long i=0; i < nLimit - str.GetLength(); i++)
		{
			AppendChar(ch);
		}
		Append(str);
		return *this;
	}
	
	CGxxString& FillCharFromRight(unsigned long nLimit, const_char ch) // 如果字符串的长度不足nLimit,那么就在字符串的右边插入 nLimit - GetLenght() 个ch
	{
		if (GetLength() >= (int)nLimit)
		{
			return *this;
		}
		unsigned long nLen = GetLength();
		ReAllocate(nLimit,true);
		for (unsigned long i=0; i < nLimit-nLen; i++)
		{
			AppendChar(ch);
		}
		return *this;
	}
	
	// make buf to hex string
	CGxxString& MakeBuf16(const void* buf,const unsigned long nBufSize) // 转换成16进制的字符串,char(18) = 0x12
	{
		ASSERT(sizeof(unsigned char)==1);

		CharType hexBuf[16] = {CharType('0'),CharType('1'),CharType('2'),CharType('3'),CharType('4'),CharType('5'),CharType('6'),
			CharType('7'),CharType('8'),CharType('9'),CharType('A'),CharType('B'),CharType('C'),CharType('D'),CharType('E'),CharType('F')};

		const unsigned char *pBuf = (const unsigned char *)buf;
		ReAllocate(size_t(nBufSize*2+1));
		for(unsigned long i = 0 ; i < nBufSize ; ++i,++pBuf)
		{
			SetAt( m_iAppendAt++, hexBuf[(*pBuf)/16] );
			SetAt( m_iAppendAt++, hexBuf[(*pBuf)%16] );
		}
		SetAt( m_iAppendAt, ZERO_CHAR );

		return *this;
	}
	unsigned long Parse16ToBuf(void* buf,unsigned long nSize) //将16进制字符串转字节流, 返回实际读取的字节数
	{
		ASSERT(GetLength()%2 == 0);
		const int nLen = GetLength();
		CGxxString str(*this);
		str.ToUpper();
		unsigned long j = 0;
		for (int i = 0; i < nLen && j < nSize; i+=2,++j )
		{
			CharType chH = str.GetAt(i);
			CharType chL = str.GetAt(i+1);

			unsigned char chVal = 0;

			if (CharType('0')<=chH&&chH<=CharType('9'))
			{
				chVal = (chH-CharType('0'));
			}
			else if (CharType('A')<=chH&&chH<=CharType('F'))
			{
				chVal = 10+(chH-CharType('A'));
			}
			else
			{
				chVal = 0;
			}
			chVal<<=4;
			if (CharType('0')<=chL&&chL<=CharType('9'))
			{
				chVal |= (chL-CharType('0'));
			}
			else if (CharType('A')<=chL&&chL<=CharType('F'))
			{
				chVal |= 10+(chL-CharType('A'));
			}
			else
			{
				chVal |= 0;
			}
			memcpy(((char*)buf)+j,&chVal,1);
		}

		return j;
	}


	CGxxString& operator= ( const_reference str)
	{
		SetAppendAt(0);
		Append(str);
		return *this;
	}
	CGxxString& operator= ( const CharType ch)
	{
		SetAppendAt(0);
		Append(ch);
		return *this;
	}
	CGxxString& operator= ( const_char_pointer pStr)
	{
		SetAppendAt(0);
		Append(pStr);
		return *this;
	}
	CGxxString& operator= ( const long nNum)
	{
		if( IsWideCharCode() )
			_Format2(L"%ld",nNum);
		else
			_Format2("%ld",nNum);
		return *this;
	}
	CGxxString& operator= ( const unsigned long nNum)
	{
		if( IsWideCharCode() )
			_Format2(L"%lu",nNum);
		else
			_Format2("%lu",nNum);
		return *this;
	}
	CGxxString& operator= ( const int nNum)
	{
		return operator=( (const long) nNum);
	}
	CGxxString& operator= ( const unsigned int nNum)
	{
		return operator=( (const unsigned long) nNum);
	}

	CGxxString& operator+= ( const_reference str)
	{
		Append(str);
		return *this;
	}
	CGxxString& operator+= ( const CharType ch)
	{
		AppendChar(ch);
		return *this;
	}
	CGxxString& operator+= ( const_char_pointer pStr)
	{
		Append(pStr);
		return *this;
	}
	CGxxString& operator+= ( const long nNum)
	{
		Append(CGxxString(nNum));
		return *this;
	}
	CGxxString& operator+= ( const unsigned long nNum)
	{
		Append(CGxxString(nNum));
		return *this;
	}
	CGxxString& operator+= ( const int nNum)
	{
		Append(CGxxString(nNum));
		return *this;
	}
	CGxxString& operator+= ( const unsigned int nNum)
	{
		Append(CGxxString(nNum));
	}
	
	friend CGxxString operator+ (const_reference left,const_reference right)
	{
		CGxxString tmp(left);
		tmp.Append(right);
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const_char_pointer right)
	{
		CGxxString tmp(left);
		tmp.Append(right);
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const CharType right)
	{
		CGxxString tmp(left);
		tmp.AppendChar(right);
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const long right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const unsigned long right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const int right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const_reference left,const unsigned int right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}

	friend CGxxString operator+ (const_char_pointer left, const_reference right)
	{
		CGxxString tmp(left);
		tmp.Append(right);
		return tmp;
	}
	friend CGxxString operator+ (const CharType  left,const_reference right)
	{
		CGxxString tmp(left);
		tmp.AppendChar(right);
		return tmp;
	}
	friend CGxxString operator+ (const long  left,const_reference right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const unsigned long left, const_reference right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const int  left,const_reference right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}
	friend CGxxString operator+ (const unsigned int  left,const_reference right)
	{
		CGxxString tmp(left);
		tmp += right;
		return tmp;
	}


	friend bool operator== (const_reference left,const_reference right)
	{
		const_reference _left = left;
		return _left.Compare(right) == 0;
	}
	friend bool operator== (const_char_pointer left,const_reference right)
	{
		return CGxxString(left) == right;
	}
	friend bool operator== (const_reference left,const_char_pointer right)
	{
		return CGxxString(right) == left;
	}
	friend bool operator!= (const_reference left,const_reference right)
	{
		return !(operator == (left, right));
	}
	friend bool operator!= (const_char_pointer left,const_reference right)
	{
		return !(operator == (left, right));
	}
	friend bool operator!= (const_reference left,const_char_pointer right)
	{
		return !(operator == (left, right));
	}
	friend bool operator< (const_reference left, const_reference right)
	{
		return left.Compare(right) < 0 ;
	}
	friend bool operator< (const_char_pointer left, const_reference right)
	{
		return operator< (CGxxString(left), right);
	}
	friend bool operator< ( const_reference left, const_char_pointer right)
	{
		return operator< (left, CGxxString(right));
	}
	friend bool operator> (const_reference left, const_reference right)
	{
		return left.Compare(right) > 0 ;
	}
	friend bool operator> (const_char_pointer left, const_reference right)
	{
		return operator> (CGxxString(left), right);
	}
	friend bool operator> ( const_reference left, const_char_pointer right)
	{
		return operator> (left, CGxxString(right));
	}

	CGxxString& operator <<(const_char right)
	{
		AppendChar(right);
		return *this;
	}
	CGxxString& operator <<(const_char_pointer right)
	{
		Append(right);
		return *this;
	}
	CGxxString& operator <<(const unsigned long right)
	{
		return operator+=(right);
	}
	CGxxString& operator <<(const long right)
	{
		return operator+=(right);
	}
	CGxxString& operator <<(const unsigned int right)
	{
		return operator+=(right);
	}
	CGxxString& operator <<(const int right)
	{
		return operator+=(right);
	}

	CharType& operator[] (Index iAt)
	{
		return m_pData[iAt];
	}

	operator const_char_pointer (){
		return m_pData;
	}

	void Empty(){
		SetAppendAt(0);
	}

	/* convert to number */
public:
	int ParseInt(){ 
		if( IsWideCharCode() )
			return _wtoi( (const wchar_t*)GetData() ); 
		else
			return atoi( (const char*)GetData() );
	}

	unsigned int ParseUInt(){ 
		return (unsigned int)ParseInt();
	}

	long ParseLong(){
		if( IsWideCharCode() )
			return _wtol( (const wchar_t*)GetData() ); 
		else
			return atol( (const char*)GetData() );
	}

	unsigned long ParseULong()	{
		return (unsigned long)ParseLong();
	}

	double ParseDouble(){
		if( IsWideCharCode() )
			return _wtof( (const wchar_t*)GetData() ); 
		else
			return atof( (const char*)GetData() );
	}

	float ParseFloat(){
		return (float)ParseDouble();
	}

protected:
	void _init(const size_t nSize){
		m_nMaxSize = nSize - 1;
		m_pData = new CharType[ nSize ];
		m_iAppendAt = 0;
		m_pData[0] = 0;
	}
	void _Format(void *_buf,const void *pFmt,va_list args)
	{
		if( IsWideCharCode() )
		{
			vswprintf((wchar_t*)_buf,(const wchar_t*)pFmt,args);
		}
		else
		{
			vsprintf((char*)_buf,(const char*)pFmt,args);
		}
	}
	void _Format2(const void *pFmt,...)
	{
		ASSERT( pFmt!=NULL );
		va_list args;
		va_start(args,pFmt);
		CharType _buf[10240] = {0};
		_Format(_buf,pFmt,args);
		SetAppendAt(0);
		Append(_buf);
		va_end(args);
	}


private:
	char_pointer m_pData;
	size_t m_nMaxSize;   
	long m_iAppendAt;
};

typedef CGxxString<char> CGxxStringA;
typedef CGxxString<wchar_t> CGxxStringW;

}// end namespace