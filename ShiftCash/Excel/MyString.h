//
// Author:guoxuxing
// CreateTime:2009/9/1
// Lastdate:2010/4/26
#pragma once
//
//使用正则表达式的方法，必须定义 __GXX_REGX
//
//#define __GXX_REGX

#include <vector>
#include <string>

#include <atlstr.h>
#ifndef ASSERT
#define ASSERT(x)
#endif
namespace NS_GXX
{

//
//
//---------------------函数说明---------------------
/*
构造函数--
	CMyStringW()                  默认构造函数，字符串为空
	CMyStringW(const long nNum)   将整数转换为字符串（十进制的）
	CMyStringW(const unsigned long nNum) 将整数转换为字符串（十进制的）
	CMyStringW(const int nNum)
	CMyStringW(const unsigned int nNum)
	CMyStringW(const float fNum)
	CMyStringW(const double dNum)
	CMyStringW(CMyStringW& myStr)
	CMyStringW(CString& str)
	CMyStringW(const wchar_t *lpstr)
	CMyStringW( const char *lpsz )

基本方法--
	ReAllocate( size_t nNewMaxSize,BOOL bCopyOld = FALSE)  重新分配缓冲区大小
	SetAppendAt(long iAt)             将iAt以后(包括iAt)的字符串擦除
	AppendChar(WCHAR ch)              累加一个字符
	Append(const wchar_t *lpszwStr)	  累加一个‘\0’字符结尾的字符串
	Append(CMyStringW& myStr)   累加字符串对象
	Append(CString& str)              累加一个CString对象
	AppendFormat(const wchar_t *lpszFormat,...)  累加一个格式化字符串
	Insert(int iAt ,IN const wchar_t *lpszwStr)  在指定位置插入字符串
	GetData()                         得到整个字符串
	GetBuffer(int iAt = 0)			  得到在指定位置处后面的字符串
	GetLength()                       得到字符串长度
	Format(IN const wchar_t* lpszFormat,...)   格式化，并覆盖原有的字符串
	MakeUpper()						  转换为大写
	MakeLower()                       转换为小写

辅助方法--
	Left(int nCount)                  从左边截取nCount长度的字符串
	Right(int nCount)                 从右边截取nCount长度的字符串
	Clip(int iFrom,int iTo)           截取指定区间的字符串,从iFrom开始，到iTo结束，不包含iTo
	GetAt(int i)					  得到指定位置处的字符
	SetAt(int iAt,WCHAR ch)           设置指定位置处的字符
	Remove(int iAt,int nLen)          移除一段字符
	Remove(WCHAR ch)                  移除所有的字符ch
	FindFromLeft(const int iAt,const WCHAR ch)     从左边某个位置开始查找出现的第一个ch的所在位置
	FindFromLeft(WCHAR ch)            从最左边开始查找第一个出现的ch所在的位置
	FindFromLeft(IN const WCHAR * pwsz,const int iFrom=0)    从左边指定位置处查找第一个出现 pwsz字符串的所在位置
	FindFromRight(IN const WCHAR * pwsz,const int iFrom )    从右边指定位置处查找第一个出现 pwsz字符串的所在位置
	FindString(IN const WCHAR * lpszStr,int iFrom = 0)       从右边某个位置开始查找出现的第一个ch的所在位置
	FindFromRight(WCHAR ch)			  从最右边开始查找第一个出现的ch所在的位置
	FindString(IN const WCHAR * lpszStr,int iFrom = 0)   等同于FindFromLeft(IN const WCHAR * pwsz,const int iFrom=0)
	Replace(const WCHAR chSrc,const WCHAR chNew)         替换 所有chSrc
	TrimLeft(const WCHAR ch=L' ')                        清除所有在最左边的ch
	TrimRight(const WCHAR ch=L' ')                       清除多有在最右边的ch
	TrimLeft(IN const WCHAR * pwsz)                      清除最左边的与pwsz匹配的字符串
	TrimRight(IN const WCHAR * pwsz)					 清除最右边的与pwsz匹配的字符串
	IsEmpty()                                            是否为空串
	Split(IN const WCHAR* lpszSplit,OUT CMyStringW*& pStrArray,OUT int &nCount )
														 用lpszSplit字串来分隔字符串，生成数组，需要在外边释放内存
	Split(IN const WCHAR ch,OUT CMyStringW*& pStrArray,OUT int &nCount )													 
														 通过一个字符来分隔字符串
    Reverse()                  将字符串翻转
	SortUp()                   将字符串升序排列
	SortDown()                 将字符串降序排列

特殊操作--
	MakeIPv4(BYTE b1,BYTE b2,BYTE b3,BYTE b4)           创建为IP地址的字符串
	ParseIPv4(BYTE& b1,BYTE& b2,BYTE& b3,BYTE& b4)      转换为IP地址
	IsBinNumString()									判断是否为 二进制字符串
	IsHexNumString()                                    是否为 16进制字符串

*/
class CMyStringA;
//
// CMyStringW 对WCHAR型字符串的所有操作
// 
class CMyStringW
{
	friend class CMyStringW;
public:
#pragma region //--------------构造函数-----------------------------
	

	CMyStringW();
	CMyStringW(const long nNum);
	CMyStringW(const unsigned long nNum);
	CMyStringW(const int nNum);
	CMyStringW(const unsigned int nNum);
	CMyStringW(const float fNum);
	CMyStringW(const double dNum);
	CMyStringW(const CMyStringW& myStr);
	CMyStringW(CString& str);
	CMyStringW(const wchar_t *lpsz);
	CMyStringW(const char *lpsz);
	~CMyStringW();


#pragma endregion
public:
#pragma region //--------------------------------基本操作------------
	//--------------------------------------------------
	// Method:  ReAllocate   [ public ]
	// Description: 重新分配大小,分配一个较大的空间，可有效的提高字符串大量累加的效率
	// Returns [BOOL]: 
	// Parameters: 
	//    [ size_t nNewMaxSize ]:  新的最大长度
	//    [ BOOL bCopyOld ]:       是否拷贝前一次分配区里的字符串
	// Author:  Guoxuxing   
	// Modify-Date:2009/10/22
	//************************************
	BOOL ReAllocate( size_t nNewMaxSize,BOOL bCopyOld = FALSE);
	//
	// 设置从指定位置累计字符串,iAt后面的所有字符将被截断
	BOOL SetAppendAt(long iAt);
	//
	//累加字符
	BOOL AppendChar(WCHAR ch);
	BOOL AppendChar(char ch);
	BOOL Append(const char * lpsz);
	BOOL Append(const wchar_t *lpszwStr);
	BOOL Append(CMyStringW& myStr);
	BOOL Append(CString& str);
	BOOL Append(const int num);
	BOOL Append(const unsigned int num);
	BOOL Append(const long num);
	BOOL Append(const unsigned long num);
	
	/// 将字符串格式化后累加到原字符串末尾
	BOOL AppendFormat(const wchar_t *lpszFormat,...);
	/// 将字符串格式化后累加到原字符串末尾
	BOOL AppendFormat(const char *lpszFormat,...);

	/// 在指定位置插入字符串
	BOOL Insert(int iAt ,IN const wchar_t *lpszwStr);
	/// 在指定位置插入字符串
	BOOL Insert(int iAt ,IN const char *lpszwStr);
	//
	// 得到整个字符串
	wchar_t *GetData() const;
	//
	// 获取字符串长度,(针对WCHAR)
	long GetLength() const;
	//
	/*获取指定位置处的字符串*/
	wchar_t *GetBuffer(int iAt = 0);
	//格式化
	void Format(IN const wchar_t* lpszFormat,...);
	void Format(IN const char* lpszFormat,...);

	/// 转换为大写 
	CMyStringW& MakeUpper();	
	/// 转换为小写 
	CMyStringW& MakeLower();

	///拷贝数据到数组，返回有效字符长度
	int CopyTo(IN char * lpChArray,IN int nCount);
	//
	// 拷贝数据到数组，返回有效字符长度
	int CopyTo(IN WCHAR * lpWChArray,IN int nCount);

#pragma endregion 
#pragma region //------------------------辅助操作------------------------

	CMyStringW Left(int nCount);
	CMyStringW Right(int nCount);
	CMyStringW Clip(int iFrom,int iTo);      // 截取，iFrom <= 索引 < iTo;
	WCHAR GetAt(int iAt);
	void SetAt(int iAt,WCHAR ch);
	//
	// 移除所有iAt之后（包括iAt）的所有字符ch
	void Remove(int iAt,int nLen);
	//
	// 移除所有的字符ch,返回移除了的个数
	int Remove(WCHAR ch);
	//
	// 移除iAt指定位置之后的所有 字符串lpwsz
	int Remove(IN const WCHAR* lpwsz /* != NULL */, const int iAt = 0);
	//
	// 从左边查找字符，返回字符位置,-1无效 
	int FindFromLeft(const int iAt,const WCHAR ch);
		/* 从左边查找字符，返回字符位置,-1无效 */
	int FindFromLeft(const WCHAR ch);
		/* 从左边查找第一个字符串的位置,-1无效*/
	int FindFromLeft(IN const WCHAR * pwsz,const int iFrom=0);
	int FindFromRight(IN const WCHAR * pwsz,const int iFrom );
		/* 从右边查找，-1无效 */
	int FindFromRight(int iAt,const WCHAR ch);
	int FindFromRight(const WCHAR ch);
		/* 查找子串的位置 */
	int FindString(IN const WCHAR * lpszStr,int iFrom = 0);
	//
	// 字符替换,如果chNew=L'\0',那么就删除所有的字符chSrc,返回替换的个数
	int Replace(const WCHAR chSrc,const WCHAR chNew);
	//
	// 子串替换,如果lpwsznew = L"",那么就删除所有的子串，返回替换的个数
	int Replace(const WCHAR* lpwszSrc,const WCHAR* lpwszNew,int iFrom=0);

	//
	// 用lpwszNew字符串去覆盖掉 iAt位置处长度nLength的字符串
	// 如果lpwszNew超过nLength指定的长度，就丢弃lpwszNew的nLength后面的字符
	// 如果lpwszNew长度小于 nLength,相当于替换
	// 如果lpwszNew == L"",不发生任何改变
	// bReplace 决定 当lpwszNew的长度小于nLength,是否替换nLength长度区域的字符串
	CMyStringW& Over(int iAt,int nLength,const WCHAR* lpwszNew,BOOL bReplace=FALSE);
	//
	// 清除头部的连续空格(或其他字符)
	CMyStringW& TrimLeft(const WCHAR ch=L' ');
	//
	// 清除头部的连续空格(或其他字符) 
	CMyStringW& TrimRight(const WCHAR ch=L' ');
	//
	// 清除头部的连续子串 例如："abab我佩服你",调用TrimLeft(L"ab") ,结果为:"我佩服你"
	CMyStringW& TrimLeft(IN const WCHAR * pwsz);
	//
	// 清除尾部的连续子串 例如："我佩服你abab",调用TrimRight(L"ab") ,结果为:"我佩服你"
	CMyStringW& TrimRight(IN const WCHAR * pwsz);
	//
	// 剪掉尾巴,iAt为尾巴所在的位置
	CMyStringW& CutAway(int iAt);
	//
	// 剪掉尾巴,从尾部找到第一字符ch，剪掉ch开始处(包括ch)后面的所有字符
	CMyStringW& CutAway(const WCHAR ch);
	//
	// 是否为空串
	BOOL IsEmpty();
	//
	// Method:  Split   [ public ]
	// Description: 把lpszSplit作为分界字符串，得到分隔的字符串数组
	// Returns [void]: 
	// Parameters: 
	//    [ IN const WCHAR * lpszSplit ]: 
	//    [ OUT CMyStringW * & pStrArray ]: 创建字符串数组,调用程序需要删除内存
	//    [ OUT int & nCount ]:     字符串数组的长度
	//************************************
	void Split(IN const WCHAR* lpszSplit,OUT CMyStringW*& pStrArray,OUT int &nCount );
	//
	// 参考"Split(IN const WCHAR* lpszSplit..."的注释
	void Split(IN const WCHAR ch,OUT CMyStringW*& pStrArray,OUT int &nCount );
	//
	// 字符串反转
	CMyStringW& Reverse();
	//
	// 字符串升序排列
	CMyStringW& SortUp();
	//
	// 字符串降序排列
	CMyStringW& SortDown();
	//************************************
	// Method:    RemoveByWChars,按照lpwszChars中指定的字符序列来移除或者保留字符
	//             例如: CMyStringW str=L"abbbccccdddd"; str.RemoveByWChars(L"bd");  结果为: str=L"acccc".就是删除了所有的 'b','d'
	//             例如: CMyStringW str=L"abbbccccdddd----"; str.RemoveByWChars(L"bd",FALSE);  结果为: str=L"bbbdddd".就是保留了所有的 'b','d'
	// FullName:  NS_GXX::CMyStringW::RemoveByWChars
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// [Parameter: IN const WCHAR * lpwszChars ] 字符序列
	// [Parameter: IN const BOOL bRemove ] 指定是 移除还是保留
	// [Parameter: IN const int iFrom ] 从指定位置处开始处理
	//************************************
	int RemoveByWChars(IN const WCHAR * lpwszChars,
		IN const BOOL bRemove = TRUE ,
		IN const int iFrom = 0);


	//用 lpwszStr替换掉,匹配pStrArray数组中任何一个字符串,
	// 如果数组中有一个空串，调用将会失败
	//
	int ReplaceByWStrings(IN CMyStringW * pStrArray,IN const int nArrayCount ,IN const WCHAR *lpwszStr,IN const int iFrom = 0);
	int CountByWChar(IN const WCHAR ch,IN const int iFrom=0);
	int CountByWString(IN const WCHAR *lpwsz,IN const int iFrom=0);
	//
	//将Excel的列转换为对应的字符串
	//1<=nCol<= 256
	CMyStringW& MakeExcelColumn(int nCol);
#pragma endregion 
#pragma region //----------------------特殊操作-------------
	//
	// 制造一个IPv4字符串
	CMyStringW& MakeIPv4(BYTE b1,BYTE b2,BYTE b3,BYTE b4);

	//
	// 制造字符串从模块的全路径
	// hMod为NULL表示当前模块
	CMyStringW& MakeModuleFullPath(HMODULE hMod = NULL);
	//
	// 制造从系统临时目录获得的字符串
	CMyStringW& MakeFromTempPath();
	//
	// 制造从计算机名获得的字符串
	CMyStringW& MakeFromComputerName();
	// 
	// 制造一个字节流的16进制字符串,覆盖this.可以使用ParseASCIICode进行逆转换
	CMyStringW& MakeHexByteStream(IN const BYTE * byteStream,IN const int nLen);

	//
	// 判断是否为二进制字符串 
	BOOL IsBinNumString();
	//
	// 判断是否为完整的16进制字符串
	BOOL IsHexNumString();
	// 
	// 判断是否为完整的十进制数字符串*/
	BOOL IsNumString();
	//
	// 将十进制数字字符串转换为16进制的数字字符串，如果本字符串为非数字字符串，返回FALSE
	// 先把字符串转换为数字，再转换为16进制形式
	//
	BOOL ToHexStringFromNum();
	//
	// 把2进制串转换为16进制串 */
	BOOL ToHexStringFromBinString();
	BOOL ToBinStringFromHexString();
	//
	// 从十六进制字符串转换为十进制字符串
	//
	BOOL ToNumStringFromHexString();
	//
	// 从2进制转换为十进制的字符串
	//
	BOOL ToNumStringFromBinString();
	//
	// 把16进制串，按单字节转换为对应的ASCII码字符串,得到新的（char）字符串，
	BOOL ParseASCIICode(IN const int nSize,OUT char *chStream,OUT int& nLength);
	//
	// 把16进制串，按双字节转换为对应的Unicode码字符串,得到新的（WCHAR）字符串
	BOOL ParseUnicodeCode(OUT CMyStringW& strOut);
	//
	// 如果字符串为"x.x.x.x",就可以得到有效的IP分量
	void ParseIPv4(BYTE& b1,BYTE& b2,BYTE& b3,BYTE& b4);
	//
	// 转换成为整数,如果字符串过长，可能出现越界的问题
	int ParseInt();
	//
	// 转换成为整数,如果字符串过长，可能出现越界的问题
	UINT ParseUINT();
	//
	// 转换成为整数,如果字符串过长，可能出现越界的问题
	long ParseLong();
	//
	// 转换成为整数,如果字符串过长，可能出现越界的问题
	DWORD ParseDWORD();
	//
	// 转换成为双精度,如果字符串过长，可能出现越界的问题
	double ParseDouble();
	//
	// 转换成为单精度,如果字符串过长，可能出现越界的问题
	float ParseFloat();


#pragma endregion 
#pragma region //------------------------操作符重载----------------------


	WCHAR& operator [](int iAt);
	void operator +=(const WCHAR ch);
	void operator +=(const wchar_t* lpszwStr);
	void operator +=(CString& str);
	void operator +=(CMyStringW& myStr);
	void operator +=(const int nNum);
	void operator +=(const unsigned int nNum);
	void operator +=(const long nNum);
	void operator +=(const unsigned long nNum);
	void operator =(CString& str);
	void operator =(const WCHAR ch);
	void operator =(const wchar_t* lpszwStr);
	void operator =(const int nNum);
	void operator =(const unsigned int nNum);
	void operator =(const long nNum);
	void operator =(const unsigned long nNum);	
	CMyStringW& operator =(const CMyStringW& myStr);
	operator wchar_t* ();


	friend CMyStringW operator+( CMyStringW& myStr,CString& str )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%s",myStr.GetData(),str.GetBuffer() );
		return strTemp;
	}
	friend CMyStringW operator+( CMyStringW& myStr,const wchar_t* lpszTxt )
	{
		CMyStringW strTemp;;
		strTemp.Append(myStr);
		strTemp.Append(lpszTxt);
		return strTemp;
	}

	friend CMyStringW operator+( CMyStringW& myStr1,CMyStringW& myStr2 )
	{
		CMyStringW strTemp;
		strTemp.Append(myStr1.GetData());
		strTemp.Append(myStr2.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( CString& str ,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%s",str.GetBuffer() ,myStr.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( const wchar_t* lpszTxt,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Append(lpszTxt);
		strTemp.Append(myStr);
		return strTemp;
	}

	friend CMyStringW operator+( const unsigned int num,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%d%s",num,myStr.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( const unsigned long num,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%d%s",num,myStr.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( CMyStringW& myStr,const int num )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%d",myStr.GetData(),num);
		return strTemp;
	}

	friend CMyStringW operator+( const long num,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%d%s",num,myStr.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( CMyStringW& myStr,const long num )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%d",myStr.GetData(),num);
		return strTemp;
	}

	friend CMyStringW operator+( const int num,CMyStringW& myStr )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%d%s",num,myStr.GetData());
		return strTemp;
	}

	friend CMyStringW operator+( CMyStringW& myStr,const unsigned int num )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%d",myStr.GetData(),num);
		return strTemp;
	}

	friend CMyStringW operator+( CMyStringW& myStr,const unsigned long num )
	{
		CMyStringW strTemp;
		strTemp.Format(L"%s%d",myStr.GetData(),num);
		return strTemp;
	}

	friend BOOL operator==( CMyStringW& myStr,const unsigned long num )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,const long num )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,const unsigned int num )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,const int num )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,CString& str )
	{
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,const WCHAR* str )
	{
		size_t nLen1 = myStr.GetLength();
		size_t nLen2 = wcslen(str);
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CMyStringW& myStr,CMyStringW& str )
	{
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( const unsigned long num,CMyStringW& myStr )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( const long num,CMyStringW& myStr )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( const unsigned int num,CMyStringW& myStr )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( const int num,CMyStringW& myStr )
	{
		CMyStringW str = num;
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( CString& str,CMyStringW& myStr )
	{
		int nLen1 = myStr.GetLength();
		int nLen2 = str.GetLength();
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	friend BOOL operator==( const WCHAR* str,CMyStringW& myStr )
	{
		size_t nLen1 = myStr.GetLength();
		size_t nLen2 = wcslen(str);
		if( nLen1 != nLen2 )
		{
			return FALSE;
		}
		for(int i = 0 ; i < nLen1 ; i ++ )
		{
			if( myStr[i] != str[i] )
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	
	friend BOOL operator >( const WCHAR* str,CMyStringW& myStr )
	{
		if( str == 0 )
		{
			return FALSE;
		}
		if( str[0] == L'\0')
		{
			return FALSE;
		}
		size_t nLen1 = wcslen(str);
		size_t nLen2 = wcslen(myStr);
		int nMin = nLen1 > nLen2 ?nLen2:nLen1;
		int i = 0;
		for(; i < nMin ; i ++ )
		{
			if( str[i] < myStr[i] )
			{
				return FALSE;  //有一个字符小于就返回FALSE
			}
			else if( str[i] > myStr[i] )
			{
				return TRUE;
			}
		}
		if( nLen1 > nLen2 )// <= nMin的字符都相等，就判断长度是不是大于nLen2
		{
			return TRUE;
		}
		return FALSE;  
	}
	friend BOOL operator <( const WCHAR* str,CMyStringW& myStr )
	{
		if( str == 0 )
		{
			return FALSE;
		}
		if( str[0] == L'\0')
		{
			return FALSE;
		}
		size_t nLen1 = wcslen(str);
		size_t nLen2 = wcslen(myStr);
		int nMin = nLen1 > nLen2 ?nLen2:nLen1;
		int i = 0;
		for(; i < nMin ; i ++ )
		{
			if( str[i] > myStr[i] )
			{
				return FALSE;  //有一个字符大于就返回FALSE
			}
			else if( str[i] < myStr[i] )
			{
				return TRUE;
			}
		}
		if( nLen1 < nLen2 )  // <= nMin的字符都相等，就判断长度是不是小于nLen2
		{
			return TRUE;
		}
		return FALSE;
	}
	friend BOOL operator >( CMyStringW& myStr,const WCHAR* str )
	{
		return str < myStr;
	}
	friend BOOL operator <( CMyStringW& myStr,const WCHAR* str )
	{
		return str > myStr;
	}
	friend BOOL operator >( CMyStringW& myStr1,CMyStringW& myStr2)
	{
		return (const WCHAR*)myStr1.GetData() > myStr2;
	}
	friend BOOL operator <( CMyStringW& myStr1,CMyStringW& myStr2)
	{
		return myStr2 > myStr1;
	}
	friend BOOL operator >(CMyStringW& myStr1,CString& str)
	{
		return (const WCHAR*)str.GetBuffer() < myStr1;
	}
	friend BOOL operator >(CString& str,CMyStringW& myStr1)
	{
		return (const WCHAR*)str.GetBuffer() > myStr1;
	}
	friend BOOL operator <(CMyStringW& myStr1,CString& str)
	{
		return (const WCHAR*)str.GetBuffer() > myStr1;
	}
	friend BOOL operator <(CString& str,CMyStringW& myStr1)
	{
		return (const WCHAR*)str.GetBuffer() < myStr1;
	}
#pragma endregion 
#pragma region //------------------------正则表达式特殊处理------------------------
	//使用std::tr1的正则表达式

#ifdef __GXX_REGX

	//
	//lpszReg 为表达式字符串
	//如果完全匹配的话，返回TRUE ，or FALSE
	//
	BOOL RegexMatch(const WCHAR* lpszReg);	

	// 搜索出所有符合表达式的子串
	// return: 子串的个数
	int  RegexSearchAll(const wchar_t* lpszReg,std::vector<CMyStringW>& strResults);
	int  RegexSearchAll(const wchar_t* lpszReg,std::vector<std::wstring>& strResults);

	// 搜索第一个符合正则表达式的子串，-1没有找到， >=0子串的所在位置
	int  RegexSearch(const wchar_t* lpszReg,CMyStringW& strResult);
	// 从iFrom指定的位置出开始搜索第一个符合正则表达式的子串
	// -1没有找到， >=0子串的所在位置
	int  RegexSearch(int iFrom,const wchar_t* lpszReg,CMyStringW& strResult);

	// 分割字符串
	int  RegexSplit(const wchar_t* lpszRegx,std::vector<CMyStringW>& vecResults);
	int  RegexSplit(const wchar_t* lpszRegx,std::vector<std::wstring>& vecResults);

	// 用lpszFmt替换lpszRegx的子串,返回一个新的字符串,可用以下的语法对符合表达式的内容进行再次引用
	//*  字符串格式语法：
	//	*  $1: What matches the first capture group 
	//	*  $2: What matches the second capture group 
	//	*  $&: What matches the whole regular expression 
	//	*  $`: What appears before the whole regex 
	//	*  $': What appears after the whole regex 
	//	*  $$: $ 
	// eg:
	//   替换所有的"ing" 为 "ed" ,RegexReplace(L"ing",L"ed");
	//   将所有的a,e,i,o,,u开头的单词前面的修饰符"a" 替换成 "an", RegexReplace(L"([\\s]a[\\s])([aeiou])",L" an $2")
	CMyStringW  RegexReplace(const wchar_t* lpszRegx,const wchar_t* lpszFmt);

	// 用lpszNewStr在符合表达式lpszRegx子串处 插入
	CMyStringW  RegexInsert(const wchar_t* lpszRegx,const wchar_t* lpszNewStr,bool bBefore=true);

	// 
	bool Regex_IsDouble(bool bSigned=true);

	//
	bool Regex_IsInt(bool bSigned = true);

#endif
#pragma endregion 
private:
	wchar_t *m_pData;
	size_t m_nMaxSize;
	long  m_iAppendAt;
};
//============================================================================
//
// CMyStringA没有提供向CMyStringW那么多的方法,但可以转嫁给CMyStringW
class CMyStringA
{
public:
	CMyStringA();
	CMyStringA( const char* lpsz );
	CMyStringA( CMyStringW& str );
	~CMyStringA();
	char* SetData(const char* lpsz );
	char* SetData( CMyStringW& str);
	operator char*();
	char* GetData();
	int GetLength();
	int CopyTo( IN char * lpChArray,IN int nCount );
private:
	char *m_pData ;
	int m_nMaxSize;
	int m_iAppendAt;
};


}
