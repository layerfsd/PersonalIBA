#include "stdafx.h"
// Lastdate:2010/4/26
#include "MyString.h"
#include <vector>
#include <string>

#ifdef __GXX_REGX
//#include <regex>
#endif 
 
using namespace std;

using namespace NS_GXX;

NS_GXX::CMyStringW::CMyStringW()
{
	m_nMaxSize = 1023;
	m_pData = new wchar_t[ m_nMaxSize + 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
}

NS_GXX::CMyStringW::CMyStringW( const long nNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%d",nNum);
}

NS_GXX::CMyStringW::CMyStringW( const unsigned long nNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%u",nNum);
}

NS_GXX::CMyStringW::CMyStringW( const int nNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%d",nNum);
}

NS_GXX::CMyStringW::CMyStringW( const float fNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%f",fNum);
}

NS_GXX::CMyStringW::CMyStringW( const unsigned int nNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%u",nNum);
}

NS_GXX::CMyStringW::CMyStringW( const double dNum )
{
	m_nMaxSize = 0;
	m_pData = new wchar_t[ 1 ];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	Format(L"%lf",dNum);
}

NS_GXX::CMyStringW::CMyStringW( const CMyStringW& myStr )
{
	m_nMaxSize = myStr.m_nMaxSize;
	m_pData = new wchar_t[ m_nMaxSize + 1];
	m_iAppendAt = myStr.m_iAppendAt;
	memcpy( m_pData,myStr.m_pData,m_iAppendAt*2 );
	m_pData[m_iAppendAt] = L'\0';
}

NS_GXX::CMyStringW::CMyStringW( CString& str )
{
	m_nMaxSize = 1023;
	m_pData = new wchar_t[ m_nMaxSize + 1];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	this->Append( str.GetBuffer() );
}

NS_GXX::CMyStringW::CMyStringW( const wchar_t *lpstr )
{
	m_nMaxSize = 1023;
	m_pData = new wchar_t[ m_nMaxSize + 1];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	this->Append( lpstr );
}

NS_GXX::CMyStringW::CMyStringW( const char *lpsz )
{
	m_nMaxSize = 1023;
	m_pData = new wchar_t[ m_nMaxSize + 1];
	m_iAppendAt = 0;
	m_pData[0] = L'\0';
	this->Append( CA2W(lpsz) );
}

NS_GXX::CMyStringW::~CMyStringW()
{
	if( m_pData )
	{
		delete []m_pData;
		m_pData = NULL;
	}
}

BOOL NS_GXX::CMyStringW::ReAllocate( size_t nNewMaxSize,BOOL bCopyOld /*= FALSE*/ )
{
	if( !bCopyOld )
	{
		if( nNewMaxSize > m_nMaxSize )
		{
			delete []m_pData;
			m_pData = NULL;
			m_pData = new wchar_t[nNewMaxSize + 1];
			if( m_pData == NULL )
			{
				return FALSE;
			}
		}
		m_iAppendAt = 0;
		m_pData[ 0 ] = L'\0';
		m_nMaxSize = nNewMaxSize;
	}
	else
	{
		if( nNewMaxSize <= m_nMaxSize )
		{
			//使用原来的内存
			return TRUE;
		}
		wchar_t *pWByte = new wchar_t[ nNewMaxSize + 1];
		if( pWByte == NULL )
		{
			return FALSE;
		}
		memcpy( pWByte,m_pData,(m_iAppendAt + 1)*2 );
		delete []m_pData;
		m_pData = pWByte;
		m_nMaxSize = nNewMaxSize;
	}
	return TRUE;
}

BOOL NS_GXX::CMyStringW::SetAppendAt( long iAt )
{
	if( iAt < 0 || iAt >= m_iAppendAt )
	{
		return FALSE;
	}
	m_iAppendAt = iAt;
	m_pData[ iAt ] = L'\0';
	return TRUE;
}

BOOL NS_GXX::CMyStringW::AppendChar( WCHAR ch )
{
	if( m_iAppendAt + 1 > (int)m_nMaxSize )  //超过了总长度
	{
		WCHAR tch[2];
		tch[0] = ch;
		tch[1] = L'\0';
		return Append( tch );
	}
	m_pData[m_iAppendAt] = ch;
	m_pData[++m_iAppendAt]= L'\0';
	
	return TRUE;
}

BOOL NS_GXX::CMyStringW::AppendChar( char ch )
{
	char chs[2] = { ch,'\0'};
	return Append( CA2W(chs) );
}
BOOL NS_GXX::CMyStringW::Append( const wchar_t *lpszwStr )
{
	ASSERT(lpszwStr != NULL);
	size_t nLen = wcslen( lpszwStr );
	if( m_iAppendAt + nLen > (int)m_nMaxSize )  //超过了总长度
	{
		long nNewAdding = (nLen / 1024 +1)*1024;
		BOOL bRes = FALSE;
		bRes = ReAllocate( m_nMaxSize + nNewAdding,TRUE); // m_iAppendAt保持原位置不变
		if( bRes == FALSE)
		{
			return FALSE;
		}
	}
	memcpy( &m_pData[m_iAppendAt],lpszwStr,nLen*2 );
	m_iAppendAt += nLen;
	m_pData[ m_iAppendAt ] = L'\0';
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Append( CMyStringW& myStr )
{
	if( (&myStr) == this )
	{
		return TRUE;
	}
	return Append(myStr.GetData());
}

BOOL NS_GXX::CMyStringW::Append( CString& str )
{
	return Append(str.GetBuffer());
}

BOOL NS_GXX::CMyStringW::Append( const int num )
{
	(*this) += num;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Append( const unsigned int num )
{
	(*this) += num;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Append( const long num )
{
	(*this) += num;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Append( const unsigned long num )
{
	(*this) += num;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Append( const char * lpsz )
{
	return Append( CA2W(lpsz) );
}
BOOL NS_GXX::CMyStringW::AppendFormat( const wchar_t *lpszFormat,... )
{
	ASSERT(lpszFormat != NULL);
	va_list  args;
	va_start(args,lpszFormat);
	wchar_t _wbuf[1024];
	vswprintf(_wbuf,lpszFormat,args);
	_wbuf[1023] = L'\0';   //防止意外
	BOOL bRel = this->Append(_wbuf);
	va_end(args);
	return bRel;
}

BOOL NS_GXX::CMyStringW::AppendFormat( const char *lpszFormat,... )
{
	ASSERT(lpszFormat != NULL);
	va_list  args;
	va_start(args,lpszFormat);
	char _buf[1024];
	vsprintf(_buf,lpszFormat,args);
	_buf[1023] = '\0';   //防止意外
	BOOL bRel = this->Append(_buf);
	va_end(args);
	return bRel;
}
BOOL NS_GXX::CMyStringW::Insert( int iAt ,IN const wchar_t *lpszwStr )
{
	if( iAt < 0 || lpszwStr == NULL || lpszwStr[0] == L'\0')
	{
		return FALSE;
	}
	if( iAt >= m_iAppendAt )
	{
		return Append( lpszwStr );
	}
	size_t nLen = wcslen( lpszwStr );
	//移动末尾剩余的字符串
	int nTailLen = m_iAppendAt - iAt;
	WCHAR *pTemp = new WCHAR[ nTailLen +1];
	memcpy(pTemp,&m_pData[ iAt ],nTailLen*2);
	pTemp[nTailLen] = L'\0';
	SetAppendAt( iAt );
	Append( lpszwStr );
	Append( pTemp );
	delete []pTemp;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::Insert( int iAt ,IN const char *lpszwStr )
{
	return Insert(iAt,CA2W(lpszwStr) );
}
wchar_t * NS_GXX::CMyStringW::GetData() const
{
	return m_pData;
}


long NS_GXX::CMyStringW::GetLength() const
{
	return m_iAppendAt;
}

wchar_t * NS_GXX::CMyStringW::GetBuffer( int iAt /*= 0*/ )
{
	return &m_pData[iAt];
}

void NS_GXX::CMyStringW::Format( IN const wchar_t* lpszFormat,... )
{
	ASSERT(lpszFormat != NULL);
	va_list  args;
	va_start(args,lpszFormat);
	wchar_t _wbuf[1024];
	vswprintf(_wbuf,lpszFormat,args);
	(*this) = _wbuf;
	va_end(args);
}

void NS_GXX::CMyStringW::Format( IN const char* lpszFormat,... )
{
	ASSERT(lpszFormat != NULL);
	va_list  args;
	va_start(args,lpszFormat);
	char  _buf[1024];
	
	vsprintf(_buf,lpszFormat,args);
	(*this) = CA2W(_buf);
	va_end(args);
}
CMyStringW& NS_GXX::CMyStringW::MakeUpper()
{
	for(int i = 0 ; i < GetLength() ; i ++)
	{
		if( GetAt(i) >= L'a' && GetAt(i) <= L'z' )
		{
			SetAt(i,GetAt(i)-32);
		}
	}
	return (*this);
}

CMyStringW& NS_GXX::CMyStringW::MakeLower()
{
	for(int i = 0 ; i < GetLength() ; i ++)
	{
		if( GetAt(i) >= L'A' && GetAt(i) <= L'Z' )
		{
			SetAt(i,GetAt(i)+32);
		}
	}
	return (*this);
}

NS_GXX::CMyStringW NS_GXX::CMyStringW::Left( int nCount )
{
	CMyStringW str;
	if( nCount >= m_iAppendAt )
	{
		return *this;
	}
	wchar_t ch = m_pData[nCount];
	m_pData[nCount] = L'\0';
	str = m_pData;
	m_pData[nCount] = ch;
	return str;
}

NS_GXX::CMyStringW NS_GXX::CMyStringW::Clip( int iFrom,int iTo ) /* 截取，iFrom <= 索引 < iTo */
{
	if( iFrom < 0 || iFrom >= m_iAppendAt || iFrom >= iTo)
	{
		return L"";
	}
	if( iTo >= m_iAppendAt )
	{
		return GetBuffer(iFrom);
	}
	WCHAR ch = m_pData[iTo];
	m_pData[iTo] = L'\0';
	CMyStringW str( &m_pData[iFrom] );
	m_pData[iTo] = ch;
	return str;
}

NS_GXX::CMyStringW NS_GXX::CMyStringW::Right( int nCount )
{
	if( nCount >= m_iAppendAt)
	{
		return *this;
	}
	return GetBuffer(m_iAppendAt - nCount);
}
WCHAR NS_GXX::CMyStringW::GetAt(int iAt)
{
	return m_pData[iAt];
}
void NS_GXX::CMyStringW::SetAt( int iAt,WCHAR ch )
{
	m_pData[iAt] = ch;
}

void NS_GXX::CMyStringW::Remove( int iAt,int nLen )
{
	if( iAt < 0 || iAt >= m_iAppendAt )
	{
		return;
	}
	if( iAt + nLen >= m_iAppendAt )
	{
		SetAppendAt( iAt );
		return;
	}
	int iLastAt = iAt + nLen ;
	for(int i = iLastAt ; i < m_iAppendAt ; i ++ )
	{
		m_pData[ i - nLen ] = m_pData[ i ];
	}
	m_iAppendAt -= nLen;
	m_pData[ m_iAppendAt ] = L'\0';
}

int NS_GXX::CMyStringW::Remove( WCHAR ch )
{
	WCHAR *pTemp = new WCHAR[ m_nMaxSize + 1];
	int iCount = 0;
	int iFirst = -1;
	for(int i = 0 ; i < m_iAppendAt ; i ++)
	{
		if( m_pData[i] != ch )
		{
			pTemp[ iCount ++] = m_pData[i];
		}
	}
	memcpy(m_pData,pTemp,iCount *2);
	m_pData[iCount] = L'\0';
	int nRes = m_iAppendAt - iCount;
	m_iAppendAt = iCount;
	delete []pTemp;
	return nRes;
}

int NS_GXX::CMyStringW::Remove( IN const WCHAR* lpwsz /* != NULL */, const int iAt /*= 0*/ )
{
	if( lpwsz == NULL || iAt < 0 || iAt >= m_iAppendAt )
	{
		ASSERT(lpwsz != NULL);
		return 0;
	}
	size_t nLen = wcslen( lpwsz );
	if( nLen == 0 || iAt + nLen > m_iAppendAt )
	{
		return 0;
	}
	std::vector<int> iIndexs;
	int iFrom = FindFromLeft( lpwsz,iAt );
	while( iFrom != -1 )
	{
		iIndexs.push_back( iFrom );
		iFrom = FindFromLeft( lpwsz,iFrom + nLen );
	}
	if( iIndexs .size() == 0)
	{
		return 0;
	}
	WCHAR * chTemp = new WCHAR[ GetLength() + 1];
	int iIndexAt = 0;
	int iCur = 0;
	for(int i = 0 ;i < GetLength() ; )
	{
		if( iIndexAt < (int)iIndexs.size() )
		{
			if( i == iIndexs[iIndexAt] )
			{
				i += nLen;
				iIndexAt ++;
				continue;
			}
		}
		chTemp[iCur] = GetAt(i);
		iCur++;
		i++;
	}
	chTemp[iCur] = L'\0';
	SetAppendAt(0);
	Append(chTemp);
	delete []chTemp;
	chTemp = NULL;
	return (int)iIndexs.size();
}
int NS_GXX::CMyStringW::FindFromLeft( const int iAt,const WCHAR ch )
{
	if( iAt < 0 )
	{
		return -1;
	}
	int iResult = -1;
	for(int i = iAt ; i < m_iAppendAt ; i ++ )
	{
		if( GetAt(i) == ch )
		{
			iResult =  i;
			break;
		}
	}
	return iResult;
}

int NS_GXX::CMyStringW::FindFromLeft(const WCHAR ch )
{
	return FindFromLeft(0,ch);
}

int NS_GXX::CMyStringW::FindFromLeft( IN const WCHAR * pwsz,const int iFrom/*=0*/ )
{
	return FindString( pwsz,iFrom );
}

int NS_GXX::CMyStringW::FindFromRight( IN const WCHAR * pwsz,const int iFrom )
{
	if( pwsz == NULL || pwsz[0] == L'\0')
	{
		return -1;
	}
	size_t nLen = wcslen(pwsz );
	if( iFrom - nLen +1 < 0 )
	{
		return -1;
	}
	CMyStringW strTemp1(*this);
	CMyStringW strTemp2(pwsz);
	strTemp1.Reverse();
	strTemp2.Reverse();
	int iAt = strTemp1.FindFromLeft( strTemp2.GetData() );
	if( iAt == -1)
	{
		return -1;
	}
	iAt = strTemp1.GetLength() - iAt - nLen ;
	return iAt;
}

int NS_GXX::CMyStringW::FindFromRight( int iAt,const WCHAR ch )
{
	if( iAt >= m_iAppendAt )
	{
		return -1;
	}
	int iResult = -1;
	for(int i = iAt ; i >= 0 ; i --)
	{
		if( GetAt(i) == ch )
		{
			iResult =  i;
			break;
		}
	}
	return iResult;
}

int NS_GXX::CMyStringW::FindFromRight(const WCHAR ch )
{
	return FindFromRight( m_iAppendAt - 1,ch);
}

int NS_GXX::CMyStringW::FindString( IN const WCHAR * lpszStr,int iFrom /*= 0*/ )
{
	if( lpszStr == NULL || lpszStr[0] == L'\0')
	{
		return -1;
	}
	size_t nLen = wcslen( lpszStr );
	if( nLen > (m_iAppendAt - iFrom) ) //比源字符串长直接返回
	{
		return -1;
	}
	int iResult = -1;	
	int iEnd = m_iAppendAt - nLen;
	for(int i = iFrom ; i <= iEnd ; i ++)
	{
		if( memcmp( &m_pData[i],lpszStr,nLen*2 ) == 0 )
		{
			iResult = i;
			break;
		}

	}
	return iResult;
}

int NS_GXX::CMyStringW::Replace( const WCHAR chSrc,const WCHAR chNew )
{
	if( chNew == L'\0')
	{
		return Remove(chNew);
	}
	int nCount = 0;
	for(int i = 0 ; i < m_iAppendAt ; i ++)
	{
		if( m_pData [ i ] == chSrc )
		{
			m_pData[ i ] = chNew;
			nCount ++;
		}
	}
	return nCount;
}

int NS_GXX::CMyStringW::Replace( const WCHAR* lpwszSrc,const WCHAR* lpwszNew ,int iFr/*=0*/ )
{
	if( lpwszSrc == NULL || lpwszSrc[0] == L'\0' || lpwszNew == NULL )
	{
		return 0;
	}
	int iAt = iFr;
	if( lpwszNew[0] == L'\0')
	{
		return Remove( lpwszSrc,iAt);
	}
	int nLen = wcslen( lpwszSrc );
	if( nLen == 0 || iAt + nLen > m_iAppendAt )
	{
		return 0;
	}
	std::vector<int> iIndexs;
	int iFrom = FindFromLeft( lpwszSrc,iAt );
	while( iFrom != -1 )
	{
		iIndexs.push_back( iFrom );
		iFrom = FindFromLeft( lpwszSrc,iFrom + nLen );
	}
	if( iIndexs .size() == 0)
	{
		return 0;
	}
	int nNewLen = wcslen( lpwszNew );
	WCHAR * chTemp = new WCHAR[ GetLength() + nNewLen*(int)iIndexs.size()+1];
	int iIndexAt = 0;
	int iCur = 0;
	// 效率 1*n
	for(int i = 0 ;i < GetLength() ; )
	{
		if( iIndexAt < (int)iIndexs.size() )
		{
			if( i == iIndexs[iIndexAt] )
			{
				for(int k = 0 ; k < nNewLen ; k ++)
				{
					chTemp[iCur ++ ] = lpwszNew[k];
				}
				i += nLen;
				iIndexAt ++;
				continue;
			}
		}
		chTemp[iCur] = GetAt(i);
		iCur++;
		i++;
	}
	chTemp[iCur] = L'\0';
	SetAppendAt(0);
	Append(chTemp);
	delete []chTemp;
	chTemp = NULL;
	return (int)iIndexs.size();
}

CMyStringW& NS_GXX::CMyStringW::TrimLeft( const WCHAR ch/*=L' '*/ )
{
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

CMyStringW& NS_GXX::CMyStringW::TrimLeft( IN const WCHAR * pwsz )
{
	if( pwsz != NULL && pwsz[0] != L'\0' )
	{
		int nLen = wcslen( pwsz );
		int iAt = 0;
		int iLen = 0;
		BOOL bShould = FALSE;
		iAt = FindString(pwsz) ; //如果不为0的话直接跳过了
		while( iAt == iLen )
		{
			bShould = TRUE;
			iLen += nLen;
			iAt = FindString(pwsz,iAt + 1);
		}
		if( bShould )
		{
			Remove(0,iLen);
		}
	}
	return *this;
}
CMyStringW& NS_GXX::CMyStringW::TrimRight( const WCHAR ch/*=L' '*/ )
{
	int iAppendAt = m_iAppendAt;
	for(int i = m_iAppendAt-1 ; i>= 0;i --)
	{
		if( m_pData[i] != ch)
		{
			break;
		}
		else
		{
			iAppendAt --;
		}
	}
	SetAppendAt( iAppendAt );
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::TrimRight( IN const WCHAR * pwsz )
{
	if( pwsz != NULL && pwsz[0] != L'\0')
	{
		int nLen = wcslen( pwsz );
		if( nLen > GetLength() )
		{
			return *this;
		}
		int iAt = -1;
		int iFrom = GetLength() - nLen;;
		int iLen = 0;
		BOOL bShould = FALSE;
		iAt = FindString(pwsz,iFrom) ; //如果不为0的话直接跳过了
		while( iAt == iFrom )
		{
			SetAppendAt( iAt );
			iFrom = GetLength() - nLen;
			iAt = FindString(pwsz,iFrom);
		}
	}
	return *this;
}

BOOL NS_GXX::CMyStringW::IsEmpty()
{
	return m_iAppendAt == 0;
}

void NS_GXX::CMyStringW::Split( IN const WCHAR* lpszSplit,OUT CMyStringW*& pStrArray,OUT int &nCount )
{
	pStrArray = NULL;
	nCount = 0;
	if( lpszSplit == NULL || lpszSplit[0] == L'\0')
	{

		return ;
	}
	int nLen = wcslen( lpszSplit );
	if( nLen > m_iAppendAt ) //比源字符串长直接返回
	{
		return ;
	}
	INT *iIndexStart = new int[m_iAppendAt];
	INT *iIndexEnd = new int[m_iAppendAt];

	int iCount = 0;
	int iEnd = m_iAppendAt - nLen;
	int iFrom = 0;
	while(TRUE)
	{
		iIndexStart[ iCount ] = iFrom;
		iFrom = FindString( lpszSplit, iFrom );
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

	pStrArray = new CMyStringW[ iCount + 1];
	for( int i = 0 ; i <= iCount; i ++)
	{
		pStrArray[i] = this->Clip(iIndexStart[i],iIndexEnd[i] + 1);
	}
	nCount = iCount + 1;
	delete []iIndexStart;
	delete []iIndexEnd;
}

void NS_GXX::CMyStringW::Split( IN const WCHAR ch,OUT CMyStringW*& pStrArray,OUT int &nCount )
{
	WCHAR chh[2] = { ch,L'\0'};
	Split(chh,pStrArray,nCount);
}

CMyStringW& NS_GXX::CMyStringW::Reverse()
{
	int nSize = GetLength() / 2;
	for(int i = 0 ; i<  nSize; i ++)
	{
		WCHAR ch = GetAt(i);
		WCHAR ch2 = GetAt( GetLength()-1-i);
		SetAt( i,ch2);
		SetAt( GetLength()-1-i , ch);
	}
	return (*this);
}

CMyStringW& NS_GXX::CMyStringW::SortUp()
{
	for(int i =0; i < GetLength() -1;i ++)
	{
		for(int j=0 ; j < GetLength() - 1; j++)
		{
			if( GetAt(j) > GetAt(j+1) )
			{
				WCHAR ch = GetAt(j);
				SetAt(j,GetAt(j+1));
				SetAt(j+1,ch);
			}
		}
	}
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::SortDown()
{
	for(int i =0; i < GetLength() -1;i ++)
	{
		for(int j=0 ; j < GetLength() - 1; j++)
		{
			if( GetAt(j) < GetAt(j+1) )
			{
				WCHAR ch = GetAt(j);
				SetAt(j,GetAt(j+1));
				SetAt(j+1,ch);
			}
		}
	}
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::MakeIPv4( BYTE b1,BYTE b2,BYTE b3,BYTE b4 )
{
	Format(L"%d.%d.%d.%d",b1,b2,b3,b4);
	return *this;
}

void NS_GXX::CMyStringW::ParseIPv4( BYTE& b1,BYTE& b2,BYTE& b3,BYTE& b4 )
{
	b1 = b2 = b3 = b4 = 0;
	CMyStringW *pBytes = NULL;
	int nCount = 0;
	Split(L".",pBytes,nCount);
	if( nCount > 0 )
	{
		if( nCount != 4 )
		{
			ASSERT( nCount == 4 );
			delete []pBytes;
		}			
		b1 = (BYTE)pBytes[0].ParseInt();
		b2 = (BYTE)pBytes[1].ParseInt();
		b3 = (BYTE)pBytes[2].ParseInt();
		b4 = (BYTE)pBytes[3].ParseInt();
		delete []pBytes;
	}
}
CMyStringW& NS_GXX::CMyStringW::MakeModuleFullPath( HMODULE hMod /*= NULL*/ )
{
	WCHAR ch[1024];
	if( hMod == NULL )
	{
		hMod = GetModuleHandleW(NULL);
		if( hMod == NULL )
		{
			SetAppendAt(0);
			return *this;
		}
	}
	if( GetModuleFileNameW(hMod,ch,1024) == 0)
	{
		SetAppendAt(0);
		return *this;
	}
	(*this) = ch;
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::MakeFromTempPath()
{
	WCHAR chBuf[1024];
	::GetTempPathW(1024,chBuf);
	*this = chBuf;
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::MakeFromComputerName()
{
	WCHAR chBuf[1024];
	DWORD nLen = 1024;
	::GetComputerNameW(chBuf,&nLen);
	*this = chBuf;
	return *this;
}

BOOL NS_GXX::CMyStringW::IsBinNumString()
{
	BOOL bNum = TRUE;
	for( int i = 0 ; i< GetLength() ;i ++)
	{
		if( !( L'0'== GetAt(i) || GetAt(i) == L'1'))
		{
			bNum = FALSE;
			break;
		}
	}
	return bNum;
}

BOOL NS_GXX::CMyStringW::IsHexNumString()
{
	BOOL bNum = TRUE;
	for( int i = 0 ; i< GetLength() ;i ++)
	{
		if( !( (L'0'<= GetAt(i) && GetAt(i) <= L'9')
			||(L'a'<=GetAt(i) && GetAt(i) <= L'f')
			||(L'A'<=GetAt(i) && GetAt(i) <= L'F')
			))
		{
			bNum = FALSE;
			break;
		}
	}
	return bNum;
}

BOOL NS_GXX::CMyStringW::IsNumString()
{
	BOOL bNum = TRUE;
	for( int i = 0 ; i< GetLength() ;i ++)
	{
		if( !(L'0'<= GetAt(i) && GetAt(i) <= L'9'))
		{
			bNum = FALSE;
			break;
		}
	}
	return bNum;
}

BOOL NS_GXX::CMyStringW::ToHexStringFromNum()
{
	if( !IsNumString() )
	{
		return FALSE;
	}
	__int64 nNum= _wtoi64( GetData() );
	this->Format(L"%llx",nNum);
	return TRUE;
}

BOOL NS_GXX::CMyStringW::ToHexStringFromBinString()
{
	if( !IsBinNumString() )
	{
		return FALSE;
	}
	CMyStringW strTemp;
	strTemp.ReAllocate(m_iAppendAt/4+1);
	int iV = GetLength() % 4;
	if( iV == 1)
	{
		this->Insert(0,L"000");
	}
	else if( iV == 2)
	{
		this->Insert(0,L"00");
	}
	else if( iV == 3)
	{
		this->Insert(0,L"0");
	}
	int nCount = GetLength() / 4;
	for(int i = 0 ; i < nCount ; i ++)
	{
		CMyStringW str = Clip(i*4,i*4+4);
		if( str[0] == L'0')
		{	// 0~7
			if( str[1] == L'0')
			{	//0~3
				if( str[2] == L'0')
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'0');
					}
					else
					{
						strTemp.AppendChar(L'1');
					}
				}
				else
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'2');
					}
					else
					{
						strTemp.AppendChar(L'3');
					}
				}
			}
			else
			{	//4~7
				if( str[2] == L'0')
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'4');
					}
					else
					{
						strTemp.AppendChar(L'5');
					}
				}
				else
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'6');
					}
					else
					{
						strTemp.AppendChar(L'7');
					}
				}
			}
		}
		else
		{	//8~15
			if( str[1] == L'0')
			{	//8~11
				if( str[2] == L'0')
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'8');
					}
					else
					{
						strTemp.AppendChar(L'9');
					}
				}
				else
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'A');
					}
					else
					{
						strTemp.AppendChar(L'B');
					}
				}
			}
			else
			{	//12~15
				if( str[2] == L'0')
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'C');
					}
					else
					{
						strTemp.AppendChar(L'D');
					}
				}
				else
				{
					if( str[3] == L'0')
					{
						strTemp.AppendChar(L'E');
					}
					else
					{
						strTemp.AppendChar(L'F');
					}
				}
			}
		}
	}
	*this = strTemp;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::ToBinStringFromHexString()
{
	if( !IsHexNumString() )
	{
		return FALSE;
	}
	CMyStringW strTemp;
	strTemp.ReAllocate( GetLength() *4 );
	for(int i = 0 ; i < GetLength() ; i ++ )
	{
		switch( GetAt(i) )
		{
		case L'0':
			strTemp.Append(L"0000");
			break;
		case L'1':
			strTemp.Append(L"0001");
			break;
		case L'2':
			strTemp.Append(L"0010");
			break;
		case L'3':
			strTemp.Append(L"0011");
			break;
		case L'4':
			strTemp.Append(L"0100");
			break;
		case L'5':
			strTemp.Append(L"0101");
			break;
		case L'6':
			strTemp.Append(L"0110");
			break;
		case L'7':
			strTemp.Append(L"0111");
			break;
		case L'8':
			strTemp.Append(L"1000");
			break;
		case L'9':
			strTemp.Append(L"1001");
			break;
		case L'a':
		case L'A':
			strTemp.Append(L"1010");
			break;
		case L'b':
		case L'B':
			strTemp.Append(L"1011");
			break;
		case L'c':
		case L'C':
			strTemp.Append(L"1100");
			break;
		case L'd':
		case L'D':
			strTemp.Append(L"1101");
			break;
		case L'e':
		case L'E':
			strTemp.Append(L"1110");
			break;
		case L'f':
		case L'F':
			strTemp.Append(L"1111");
			break;
		}
	}
	*this = strTemp;
	return TRUE;
}

BOOL NS_GXX::CMyStringW::ToNumStringFromHexString()
{
	if( !IsHexNumString() )
	{
		return FALSE;
	}
	__int64 a = 0;
	swscanf(GetData(),L"%llx",&a);
	this->Format(L"%lld",a);
	return TRUE;
}

BOOL NS_GXX::CMyStringW::ToNumStringFromBinString()
{
	BOOL bRel = ToHexStringFromBinString();
	if( !bRel )
	{
		return FALSE;
	}
	bRel = ToNumStringFromHexString();
	return bRel;
}

int NS_GXX::CMyStringW::ParseInt()
{
	return _wtoi( GetData() );
}

UINT NS_GXX::CMyStringW::ParseUINT()
{
	return (UINT)ParseInt();
}

long NS_GXX::CMyStringW::ParseLong()
{
	return _wtol( GetData() );
}

DWORD NS_GXX::CMyStringW::ParseDWORD()
{
	return (DWORD)ParseLong();
}

double NS_GXX::CMyStringW::ParseDouble()
{
	return _wtof( GetData() );
}

float NS_GXX::CMyStringW::ParseFloat()
{
	return (float)ParseDouble();
}
BOOL NS_GXX::CMyStringW::ParseASCIICode( IN const int nSize,OUT char *chStream,OUT int& nLength)
{
	if( !IsHexNumString() )
	{
		return FALSE;
	}
	CMyStringW tmpStr;
	if( GetLength() %2 != 0 )
	{	//在字符串前面加一个L'0'
		tmpStr.AppendChar(L'0');
	}
	tmpStr += *this;
	const wchar_t *pStr = tmpStr.GetData();

	int nGroup = 0;
	nGroup = tmpStr.GetLength()/2;

	nLength = 0;
	char cVar = 0;
	wchar_t wChar = 0;
	for(int i = 0 ; i < nGroup ; i ++)
	{
		wChar = pStr[i*2];
		cVar =(char) (wChar - L'0')<10 ? (wChar - L'0') : (
			(wChar - L'A')<27 ? (10+wChar - L'A') :(10+wChar - L'a'));

		cVar <<= 4;

		wChar = pStr[i*2+1];

		cVar |=(char) (wChar - L'0')<10 ? (wChar - L'0') : (
			(wChar - L'A')<27 ? (10+wChar - L'A') :(10+wChar - L'a'));

		chStream[ nLength ++] = cVar;
		
	}
	return TRUE;
}

BOOL NS_GXX::CMyStringW::ParseUnicodeCode( OUT CMyStringW& strOut )
{
	if( !IsHexNumString() )
	{
		return FALSE;
	}
	CMyStringW tmpStr;
	if( GetLength() %4 != 0 )
	{	//在字符串前面加一个L'0'
		if( GetLength() %4 == 1 )
		{
			tmpStr.AppendChar(L'0');
		}
		else if( GetLength() %4 == 2 )
		{
			tmpStr.Append(L"00");
		}
		else if( GetLength() %4 == 3 )
		{
			tmpStr.Append(L"000");
		}
	}
	tmpStr += *this;
	int nGroup = 0;
	nGroup = tmpStr.GetLength()/4;
	CMyStringW testStr;
	CMyStringW strValue;
	for(int i = 0 ; i < nGroup ; i ++)
	{
		testStr = tmpStr.Clip(i*4,i*4+4);
		testStr.ToNumStringFromHexString();
		WCHAR ch = (WCHAR)testStr.ParseInt();
		strValue.AppendChar(ch);
	}
	strOut = strValue;
	return TRUE;
}


WCHAR& NS_GXX::CMyStringW::operator[]( int iAt )
{
	return m_pData[iAt];
}

void NS_GXX::CMyStringW::operator+=( const WCHAR ch )
{
	AppendChar(ch);
}

void NS_GXX::CMyStringW::operator+=( const wchar_t* lpszwStr )
{
	Append(lpszwStr);
}

void NS_GXX::CMyStringW::operator+=( CString& str )
{
	Append(str.GetBuffer());
}

void NS_GXX::CMyStringW::operator+=( const long nNum )
{
	CMyStringW str;
	str.Format(L"%d",nNum);
	Append( str.GetData() );
}

void NS_GXX::CMyStringW::operator+=( const unsigned long nNum )
{
	CMyStringW str;
	str.Format(L"%d",nNum);
	Append( str.GetData() );
}

void NS_GXX::CMyStringW::operator+=( const unsigned int nNum )
{
	CMyStringW str;
	str.Format(L"%d",nNum);
	Append( str.GetData() );
}

void NS_GXX::CMyStringW::operator+=( const int nNum )
{
	CMyStringW str;
	str.Format(L"%d",nNum);
	Append( str.GetData() );
}

void NS_GXX::CMyStringW::operator+=( CMyStringW& myStr )
{
	Append( myStr.GetData() );
}
void NS_GXX::CMyStringW::operator=( const WCHAR ch )
{
	SetAppendAt(0);
	AppendChar(ch);
}

void NS_GXX::CMyStringW::operator=( const unsigned int nNum )
{
	Format(L"%d",nNum);
}

void NS_GXX::CMyStringW::operator=( const int nNum )
{
	Format(L"%d",nNum);
}

void NS_GXX::CMyStringW::operator=( const unsigned long nNum )
{
	Format(L"%d",nNum);
}

CMyStringW& NS_GXX::CMyStringW::operator=( const CMyStringW& myStr )
{
	this->SetAppendAt(0);
	Append( myStr.GetData() );
	return (*this);
	//(*this) = (WCHAR*)myStr.GetData();
}

void NS_GXX::CMyStringW::operator=( const long nNum )
{
	Format(L"%d",nNum);
}

void NS_GXX::CMyStringW::operator=( const wchar_t* lpszwStr )
{
	if( lpszwStr == GetData() )
	{
		//自己对自己赋值没有意思
		return;
	}
	SetAppendAt(0);
	Append( lpszwStr );
}

void NS_GXX::CMyStringW::operator=( CString& str )
{
	SetAppendAt(0);
	Append( str.GetBuffer() );
}
NS_GXX::CMyStringW::operator wchar_t*()
{
	return m_pData;
}

int NS_GXX::CMyStringW::CopyTo( IN char * lpChArray,IN int nCount )
{
	CW2A ar( GetData() );
	int nLen = strlen(ar.m_psz);
	int nRel = 0;
	if( nLen < nCount )
	{
		memcpy(lpChArray,ar.m_psz,nLen);
		lpChArray[nLen] = '\0';
		nRel = nLen;
	}
	else
	{
		memcpy(lpChArray,ar.m_psz,nCount-1);
		lpChArray[nCount] = '\0';
		nRel = nCount - 1;
	}
	return nRel;
}

int NS_GXX::CMyStringW::CopyTo( IN WCHAR * lpWChArray,IN int nCount )
{
	int nLen = GetLength();
	int nRel = 0;
	if( nLen < nCount )
	{
		memcpy(lpWChArray,GetData(),nLen*2);
		lpWChArray[nLen] = '\0';
		nRel = nLen;
	}
	else
	{
		memcpy(lpWChArray,GetData(),(nCount-1)*2);
		lpWChArray[nCount] = '\0';
		nRel = nCount - 1;
	}
	return nRel;
}


int NS_GXX::CMyStringW::RemoveByWChars( IN const WCHAR * lpwszChars,IN const BOOL bRemove /*= TRUE */,IN const int iFrom /*= 0*/ )
{
	if( lpwszChars == NULL || lpwszChars[0] == L'\0')
	{
		return 0;
	}
	int nLen = wcslen( lpwszChars );
	int nCount = 0;
	int nIndex = 0;
	
	WCHAR *pWcc = new WCHAR[ GetLength()+1 ];

	if( bRemove == TRUE )
	{
		for(int i = iFrom ; i< GetLength() ; i ++ )
		{
			BOOL bRR = FALSE;
			for(int j = 0 ; j < nLen ; j ++ )
			{
				if( GetAt(i) == lpwszChars[j] )
				{
					bRR = TRUE;
					break;
				}
			}
			if( !bRR )
			{
				pWcc[nIndex++] = GetAt(i);
			}
			else
			{
				nCount ++;
			}
		}
	}
	else
	{
		for(int i = iFrom ; i< GetLength() ; i ++ )
		{
			BOOL bRR = FALSE;
			for(int j = 0 ; j < nLen ; j ++ )
			{
				if( GetAt(i) == lpwszChars[j] )
				{
					bRR = TRUE;
					break;
				}
			}
			if( !bRR )
			{
				
			}
			else
			{
				pWcc[nIndex++] = GetAt(i);
				nCount ++;
			}
		}
	}
	pWcc[nIndex] = L'\0';
	SetAppendAt(0);
	Append(pWcc);
	delete []pWcc;
	return nCount;
}

CMyStringW& NS_GXX::CMyStringW::CutAway( int iAt )
{
	if( iAt < 0 || iAt >= m_iAppendAt )
	{
		return *this;
	}
	Remove(iAt,GetLength() - iAt );
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::CutAway( const WCHAR ch )
{
	return CutAway( FindFromRight(ch) );
}
#ifdef __GXX_REGX

BOOL NS_GXX::CMyStringW::RegexMatch( const WCHAR* lpszReg )
{
	if( lpszReg == NULL || lpszReg[0] == L'\0')
	{
		return FALSE;
	}
	std::tr1::wregex rx( lpszReg ); 
	return regex_match(GetData(),rx);
}
int NS_GXX::CMyStringW::RegexSearch( const wchar_t* lpszReg,CMyStringW& strResult )
{
	return RegexSearch(0,lpszReg,strResult);
}
int NS_GXX::CMyStringW::RegexSearch( int iFrom,const wchar_t* lpszReg,CMyStringW& strResult )
{
	ASSERT( iFrom < GetLength() );
	const std::tr1::wregex exp(lpszReg);
	std::wstring str = GetBuffer(iFrom);
	std::tr1::wsmatch result;
	bool bRes = std::tr1::regex_search(str,result,exp);
	if( bRes )
	{
		strResult = result.str().c_str();
		return (int)result.position() + iFrom;
	}
	else
	{
		strResult = L"";
		return -1;
	}
}

int NS_GXX::CMyStringW::RegexSearchAll( const wchar_t* lpszReg,std::vector<CMyStringW>& strResults )
{
	strResults.clear();
	const std::tr1::wregex exp(lpszReg);
	std::wstring wstrSrc = this->GetData();
	std::tr1::wsregex_token_iterator itI(wstrSrc.begin(),wstrSrc.end(),exp);
	std::tr1::wsregex_token_iterator end;

	std::wstring strRes;
	for( ;itI != end ; itI ++)
	{
		strRes = *itI;
		strResults.push_back( CMyStringW(strRes.c_str() ));
	}
	return int( strResults.size() );
}
int NS_GXX::CMyStringW::RegexSearchAll( const wchar_t* lpszReg,std::vector<std::wstring>& strResults )
{
	strResults.clear();
	const std::tr1::wregex exp(lpszReg);
	std::wstring wstrSrc = this->GetData();
	std::tr1::wsregex_token_iterator itI(wstrSrc.begin(),wstrSrc.end(),exp);
	std::tr1::wsregex_token_iterator end;

	std::wstring strRes;
	for( ;itI != end ; itI ++)
	{
		strRes = *itI;
		strResults.push_back( strRes );
	}
	return int( strResults.size() );
}

int NS_GXX::CMyStringW::RegexSplit( const wchar_t* lpszReg,std::vector<std::wstring>& vecResults )
{
	vecResults.clear();
	if( lpszReg == NULL || lpszReg[0] == L'\0')
	{
		return 0;
	}
	const std::tr1::wregex reg(lpszReg);
	std::tr1::wsmatch result;
	std::wstring strSrc = GetBuffer();
	std::wstring::iterator itBegin = strSrc.begin();
	std::wstring::iterator itIt = itBegin;
	std::wstring::iterator itEnd = strSrc.end();

	std::wstring strTmp;
	while(TRUE)
	{
		if( itIt !=itEnd )
			strTmp = &(*itIt);
		else 
			strTmp = L"";
		bool bSearch = std::tr1::regex_search(strTmp,result,reg,std::tr1::regex_constants::match_default);
		if( !bSearch )
		{   //not found
			std::wstring strPre;
			strPre.assign(itIt,itEnd);		
			vecResults.push_back( strPre );
			break;
		}
		else
		{   //found
			int nResLen = result.prefix().length();
			std::wstring strPre;
			itIt += nResLen;
			strPre.assign(itBegin,itIt);  
			vecResults.push_back( strPre );  //保存前一次分割的值
			itIt += result.length();   //移动到下一次搜索的位置
			itBegin = itIt;
		}

	}

	return (int)vecResults.size();
}

int NS_GXX::CMyStringW::RegexSplit( const wchar_t* lpszRegx,std::vector<CMyStringW>& vecResults )
{
	vecResults.clear();
	std::vector<std::wstring> strV;
	if( RegexSplit(lpszRegx,strV) > 0)
	{
		for(unsigned int i = 0 ; i < strV.size() ; i ++)
			vecResults.push_back( CMyStringW(strV[i].c_str()) );
	}
	return (int)strV.size();
}
CMyStringW NS_GXX::CMyStringW::RegexReplace( const wchar_t* lpszRegx,const wchar_t* lpszFmt )
{
	std::wstring strSrc = GetData();
	std::tr1::wregex  strRegx(lpszRegx);
	std::wstring strFmt = lpszFmt;
	strSrc = std::tr1::regex_replace(strSrc,strRegx,strFmt);
	CMyStringW strRes = strSrc.c_str();
	return strRes;
}
NS_GXX::CMyStringW NS_GXX::CMyStringW::RegexInsert( const wchar_t* lpszRegx,const wchar_t* lpszFmt,bool bBefore/*=true*/ )
{
	if( bBefore )
	{
		std::wstring strFmt = lpszFmt;
		strFmt += L"$&";	
		return RegexReplace(lpszRegx,strFmt.c_str());
	}
	else
	{
		std::wstring strFmt = L"$&";
		strFmt += lpszFmt;
		return RegexReplace(lpszRegx,strFmt.c_str());
	}
}

bool NS_GXX::CMyStringW::Regex_IsDouble( bool bSigned/*=true*/ )
{
	if( bSigned )
	{
		return RegexMatch(L"[+-]?(\\d+|\\d+\\.\\d*|\\d*\\.\\d+)") == TRUE;
	}
	else
	{
		return RegexMatch(L"[+]?(\\d+|\\d+\\.\\d*|\\d*\\.\\d+)") == TRUE;
	}
}

bool NS_GXX::CMyStringW::Regex_IsInt( bool bSigned /*= true*/ )
{
	if( bSigned )
	{
		return RegexMatch(L"[+-]?\\d+") == TRUE;
	}
	else
	{
		return RegexMatch(L"[+]?\\d+") == TRUE;
	}
}

#endif  ///----end __GXX_REGX


int NS_GXX::CMyStringW::ReplaceByWStrings( IN CMyStringW * pStrArray,IN const int nArrayCount ,IN const WCHAR *lpwszStr,IN const int iFromT /*= 0*/ )
{
	if( pStrArray == NULL || nArrayCount <=  0)
	{
		return 0;
	}
	for(int i = 0 ; i < nArrayCount ; i ++)
	{
		if( pStrArray[0].GetLength() == 0)
		{
			return 0;
		}
	}
	//将字符串按长度降序排序
	std::vector<CMyStringW*> vecStrs;
	for(int i = 0 ; i < nArrayCount ; i ++ )
	{
		vecStrs.push_back( &pStrArray[i] );
	}
	for(int i = 0 ; i < nArrayCount-1 ; i ++)
	{
		for(int j = 0 ; j < nArrayCount-1; j++)
		{
			if( vecStrs[j]->GetLength() < vecStrs[j+1]->GetLength() )
			{
				CMyStringW* strP = vecStrs[j];
				vecStrs[j] = vecStrs[j+1];
				vecStrs[j+1] = strP;
			}
		}
	}
	//
	int iFrom = iFromT;
	int iAt = 0;
	std::vector<int> vecStart;
	std::vector<int> vecLen;
	while( TRUE )
	{
		BOOL bFind = FALSE;
		for(int i = 0 ; i < nArrayCount ; i ++ )
		{
			iAt = FindFromLeft(vecStrs[i]->GetData(),iFrom);
			if( iAt != -1)
			{
				bFind = TRUE;
				iFrom = iAt + vecStrs[i]->GetLength();
				vecStart.push_back(iAt);
				vecLen.push_back(vecStrs[i]->GetLength());
				break;
			}
		}
		if( bFind == FALSE )
		{
			break;  //没有找到一个匹配的就返回
		}
	}
	if( vecStart.size() == 0  )
	{
		return 0;
	}
	CMyStringW strTemp;
	int nLen = 0;
	if( lpwszStr != NULL)
	{
		nLen = wcslen(lpwszStr);
	}
	strTemp.ReAllocate( GetLength() + (int)vecStart.size() * nLen,FALSE);
	int iFromAt = 0;
	int iFromLen = 0;
	int iIndex = 0;
	for(int i = 0 ; i < (int)vecStart.size() ; i ++)
	{
		iFromLen = vecStart[i] - iFromAt;
		for(int k = 0 ; k < iFromLen ; k ++ )
		{
			strTemp[iIndex++] = GetAt( iFromAt + k );
		}
		for(int m = 0 ; m < nLen ; m++)
		{
			strTemp[iIndex++] = lpwszStr[m];  //拷贝进去
		}
		iFromAt = vecStart[i] + vecLen[i];
	}
	for(int i = iFromAt ; i < GetLength() ; i ++ )
	{
		strTemp[iIndex++] = GetAt(i);
	}
	strTemp[iIndex] = L'\0';
	strTemp.m_iAppendAt = iIndex;
	(*this) = strTemp;
	return (int)vecStart.size();
}

int NS_GXX::CMyStringW::CountByWChar( IN const WCHAR ch,IN const int iFrom/*=0*/ )
{
	int nCount = 0;
	for(int i = iFrom ; i < GetLength() ; i ++)
	{
		if( GetAt(i) == ch )
		{
			nCount ++;
		}
	}
	return nCount;
}

int NS_GXX::CMyStringW::CountByWString( IN const WCHAR *lpwsz,IN const int iFrom/*=0*/ )
{
	ASSERT(iFrom >= 0 );
	if( lpwsz == NULL )
	{
		return 0 ;
	}
	int nCount = 0;
	if( lpwsz[0] == L'\0')
	{
		return nCount;
	}
	int iAt = 0;
	iAt = FindFromLeft(lpwsz,iFrom);
	int nLen = wcslen(lpwsz);
	while(iAt >= 0)
	{
		iAt = FindFromLeft(lpwsz,iAt + nLen);
		nCount ++;
	}
	return nCount;
}

CMyStringW& NS_GXX::CMyStringW::MakeExcelColumn( int nCol )
{
	ASSERT( nCol > 0 && nCol < 257);
	int iLease = (nCol-1)/26;
	int iLast = (nCol-1)%26;
	WCHAR chs[3];
	if( iLease == 0)
	{
		chs[0] = L'A'+ iLast;
		chs[1] = L'\0';
	}
	else
	{
		chs[0] = L'A' + iLease - 1;
		chs[1] = L'A'+ iLast;
		chs[2] = L'\0';
	}
	(*this) = chs;
	return (*this);
}

CMyStringW& NS_GXX::CMyStringW::MakeHexByteStream( IN const BYTE * byteStream,IN const int nLen )
{
	ASSERT(byteStream!= NULL);
	if( byteStream == NULL )
	{
		return *this;
	}
	if( nLen <= 0 )
	{
		SetAppendAt(0);
		return *this;
	}
	BOOL bRes = this->ReAllocate(nLen*2);
	ASSERT(bRes == TRUE);
	if( bRes == FALSE )
	{
		return *this;
	}
	for(int i = 0 ; i < nLen ; i ++)
	{
		WCHAR ch1,ch2;
		int iH = (int)(BYTE)byteStream[i]/16;
#pragma region //读取字节个高位对应字符
		switch( iH )
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			ch1 = WCHAR(L'0'+ iH);
			break;
		case 10:
			ch1 = L'A';
			break;
		case 11:
			ch1 = L'B';
			break;
		case 12:
			ch1 = L'C';
			break;
		case 13:
			ch1 = L'D';
			break;
		case 14:
			ch1 = L'E';
			break;
		case 15:
			ch1 = L'F';
			break;
		default:
			ch1 = L'0';
			break;
		}
#pragma endregion
		int iL = (int)(BYTE)byteStream[i]%16;
#pragma region //读取字节个低位对应字符
		switch( iL )
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			ch2 = WCHAR(L'0'+ iL);
			break;
		case 10:
			ch2 = L'A';
			break;
		case 11:
			ch2 = L'B';
			break;
		case 12:
			ch2 = L'C';
			break;
		case 13:
			ch2 = L'D';
			break;
		case 14:
			ch2 = L'E';
			break;
		case 15:
			ch2 = L'F';
			break;
		default:
			ch2 = L'0';
			break;
		}
#pragma endregion
		m_pData[i*2] = ch1;
		m_pData[i*2+1] = ch2;
	}
	m_iAppendAt = nLen*2;
	m_pData[m_iAppendAt] = L'\0';
	return *this;
}

CMyStringW& NS_GXX::CMyStringW::Over( int iAt,int nLength,const WCHAR* lpwszNew,BOOL bReplace)
{
	ASSERT(iAt >= 0);
	ASSERT(lpwszNew != NULL );
	if( nLength <= 0 || lpwszNew[0] == L'\0')
	{
		return *this;
	}
	if( iAt >= GetLength() )
	{
		return *this;
	}
	if( (iAt + nLength) > GetLength() )
	{
		nLength = GetLength() - iAt;
	}
	int nNewLen = (int)wcslen( lpwszNew );
	if( nNewLen >= nLength )
	{
		for(int i = iAt ; i < iAt + nLength ; i ++ )
		{
			SetAt( i, lpwszNew[i-iAt] );
		}
	}
	else
	{
		for(int i = iAt ; i < iAt + nNewLen ; i ++ )
		{
			SetAt( i, lpwszNew[i-iAt] );
		}
		if( bReplace == TRUE )
		{
			int nGapLen = nLength - nNewLen;
			//替换就需要把后面的字符向前移动 nGapLen个位置
			int i = 0;
			for(i = iAt + nNewLen ; i < GetLength() - nGapLen ; i ++ )
			{
				SetAt( i, GetAt( i + nGapLen) );
			}
			SetAt(i,L'\0');
			SetAppendAt(i);
		}
	}
	return *this;
}








//=====================================================================


NS_GXX::CMyStringA::CMyStringA()
{
	m_nMaxSize = 0;
	m_pData = new char[m_nMaxSize+1];
	m_pData[0] = '\0';
	m_iAppendAt = 0;
}

NS_GXX::CMyStringA::CMyStringA( const char* lpsz )
{
	m_pData = NULL;
	SetData( lpsz );
}

NS_GXX::CMyStringA::CMyStringA( CMyStringW& str )
{
	m_pData = NULL;
	SetData( CW2A(str.GetData()));
}
NS_GXX::CMyStringA::~CMyStringA()
{
	if( m_pData )
	{
		delete []m_pData;
	}
}

NS_GXX::CMyStringA::operator char*()
{
	return m_pData;
}

char* NS_GXX::CMyStringA::GetData()
{
	return m_pData;
}

char* NS_GXX::CMyStringA::SetData( const char* lpsz )
{
	if( m_pData )
	{
		delete []m_pData;
		m_pData = NULL;
	}
	int nLen = strlen(lpsz);
	m_nMaxSize = nLen;
	m_pData = new char[m_nMaxSize+1];
	memcpy(m_pData,lpsz,nLen);
	m_pData[nLen] = L'\0';
	m_iAppendAt = m_nMaxSize;
	return m_pData;
}

char* NS_GXX::CMyStringA::SetData( CMyStringW& str )
{
	return SetData( CW2A(str.GetData()));
}

int NS_GXX::CMyStringA::GetLength()
{
	return m_nMaxSize;
}

int NS_GXX::CMyStringA::CopyTo( IN char * lpChArray,IN int nCount )
{
	int nLen =  m_nMaxSize;
	int nRel = 0;
	if( nLen < nCount )
	{
		memcpy(lpChArray,GetData(),nLen);
		lpChArray[nLen] = '\0';
		nRel = nLen;
	}
	else
	{
		memcpy(lpChArray,GetData(),nCount-1);
		lpChArray[nCount] = '\0';
		nRel = nCount - 1;
	}
	return nRel;
}