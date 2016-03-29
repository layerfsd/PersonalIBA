#pragma once

namespace G2XUtility{

//-------------------------------------------------------------------------
// CG2XCommonTool
//    一般通用的方法集合
//-------------------------------------------------------------------------
class G2X_CLASSES CG2XCommonTool
{
public:
	CG2XCommonTool(void);
	~CG2XCommonTool(void);

	// 解析参数,例如: userId=200454&name=小张&age=25
	static void ParseStrParams(
		const wchar_t* lpwStrParams,
		std::map<std::wstring,std::wstring>& mapParams,
		const wchar_t chAssign=L'=',
		const wchar_t chSplit=L'&');

	// 解析参数,例如: userId=200454&name=小张&age=25
	static void ParseStrParams(
		const char* lpStrParams,
		std::map<std::string,std::string>& mapParams,
		const char chAssign='=',
		const char chSplit='&');

	static int ParseInt(const wchar_t* lpwStr);
	static int ParseInt(const char* lpStr);
	static unsigned int ParseUInt(const wchar_t* lpwStr);
	static unsigned int ParseUInt(const char* lpStr);

	// 将整数转换为字符串, nLen表示字符串的长度,实际长度不足时用wchFill填充,bFront决定在字符串前面填充还是末尾填充
	// 如果实际长度超过nLen那么实际的字符换会被截断。
	// 如果nLen指定为-1,那么函数返回转换后的实际字符串，wchFill和bFront参数无效
	static std::wstring ParseString(
		const unsigned int nValue, 
		int nLen, 
		wchar_t wchFill=L'0', 
		bool bFront=true);

	// 将整数转换为字符串, nLen表示字符串的长度,实际长度不足时用chFill填充,bFront决定在字符串前面填充还是末尾填充
	// 如果实际长度超过nLen那么实际的字符换会被截断。
	// 如果nLen指定为-1,那么函数返回转换后的实际字符串，chFill和bFront参数无效
	static std::string ParseString(
		const unsigned int nValue, 
		int nLen, 
		char chFill='0', 
		bool bFront=true);

	static std::wstring ParseString(
		const wchar_t* lpwValue,
		int nLen,
		wchar_t wchFill=L' ',
		bool bFront=true);

	static std::string ParseString(
		const char* lpValue,
		int nLen,
		char chFill=' ',
		bool bFront=true);

};

//-------------------------------------------------------------------------
// CG2XParamsParser  
//     解析参数，并将参数保存起来。
//     [关键字 赋值符号 值],...
//-------------------------------------------------------------------------
template<class CharType>
class CG2XParamsParser
{
	typedef std::basic_string<CharType, 
		std::char_traits<CharType>, std::allocator<CharType> > str_type;

	typedef std::map<str_type,str_type > str_map ;

public:
	CG2XParamsParser(
		const CharType* lpParams,
		const CharType cAssign=(const CharType)'=',
		const CharType cSplit=(const CharType)'&')
	{
		CG2XCommonTool::ParseStrParams(lpParams,m_strMap,cAssign,cSplit);
	}

	const CharType* GetValueString(const CharType* lpKey)
	{
		if (!GetKeyExist(lpKey))
			return NULL;

		return m_strMap[lpKey].c_str();
	}

	const int GetValueInt(const CharType* lpKey)
	{
		if (!GetKeyExist(lpKey))
			return 0;

		return CG2XCommonTool::ParseInt(m_strMap[lpKey]);
	}

	const int GetKeyCount()const
	{
		return (int)m_strMap.size();
	}

	bool GetKeyExist(const CharType* lpKey)
	{
		return m_strMap.find(lpKey) != m_strMap.end();
	}

private:
	str_map m_strMap;
};

//-------------------------------------------------------------------------
// CG2XParamsInput
//     字符串格式保存的参数
//-------------------------------------------------------------------------
enum G2XParamType
{
	G2XParamTypeNumber,
	G2XParamTypeString
};

template<class CharType>
class CG2XParam
{
public:
	typedef std::basic_string<CharType, 
		std::char_traits<CharType>, std::allocator<CharType> > str_type;

	CG2XParam()
		:m_paramType(G2XParamTypeNumber),m_nCharCount(0)
	{
	}

	CG2XParam(G2XParamType paramType, int nCharCount)
	{	
		SetParam(paramType, nCharCount);
	}

	CG2XParam(const CG2XParam& oneParam)
	{
		m_strParam = oneParam.m_strParam;
		m_nCharCount = oneParam.m_nCharCount;
		m_paramType = oneParam.m_paramType;
	}

	void SetParam(G2XParamType paramType, int nCharCount)
	{
		m_nCharCount = nCharCount;
		m_paramType = paramType;

		if (paramType == G2XParamTypeNumber)
			PutInt(0);
		else
			PutString(NULL);
	}

	int ParseInt()
	{
		return CG2XCommonTool::ParseInt(m_strParam.c_str());
	}
	unsigned int ParseUInt()
	{
		return CG2XCommonTool::ParseUInt(m_strParam.c_str());
	}

	str_type ParseString(bool bEraseLeftSpace=true)
	{
		str_type str = m_strParam;
		
		if (bEraseLeftSpace)
		{
			str_type::iterator it = std::find_if(str.begin(),str.end(),
				std::bind1st(not_equal_to<CharType>(), CharType(' ')));
			
			if (it != str.end())
			{
				str.erase(str.begin(), it);
			}
		}
		return str;
	}

	void PutUInt(unsigned int nValue)
	{
		if (m_paramType != G2XParamTypeNumber)
		{
			ASSERT(0);
			return;
		}
		m_strParam = CG2XCommonTool::ParseString(nValue,m_nCharCount,CharType('0'));
	}
	void PutInt(int nValue)
	{
		PutUInt(nValue);
	}
	void PutString(const CharType* lpzStr, CharType chFill=CharType(' '))
	{
		if (m_paramType != G2XParamTypeString)
		{
			ASSERT(0);
			return;
		}
		m_strParam = CG2XCommonTool::ParseString(lpzStr,m_nCharCount,chFill);
	}

	int GetLength()
	{
		return m_nCharCount;
	}
	str_type GetString()
	{
		return m_strParam;
	}
	void SetString(const CharType* lpzStr)
	{
		m_strParam = lpzStr;
		
		m_strParam.empty();
		m_strParam.append(lpzStr, m_nCharCount);
		if (m_strParam.size() != m_nCharCount)
		{
			ASSERT(0); // 长度不相符
		}
	}
protected:
	str_type m_strParam;
	G2XParamType m_paramType;
	int m_nCharCount;
	
};
template<class CharType>
class CG2XParamsInput
{
public:
	typedef std::basic_string<CharType, 
		std::char_traits<CharType>, std::allocator<CharType> > str_type;

	typedef CG2XParam<CharType> CG2XParamT;
	typedef std::map<str_type,CG2XParamT*> param_map ;
	typedef std::vector<CG2XParamT*> param_list ;

	CG2XParamsInput()
	{}
	~CG2XParamsInput()
	{
		param_list::iterator it = m_paramList.begin();
		for(; it != m_paramList.end(); ++it)
		{
			delete (*it);
		}
	}

	void AddParam(const CharType* name, G2XParamType paramType, int nLength)
	{
		CG2XParamT *pParam = new CG2XParamT(paramType, nLength);
		m_paramMap[str_type(name)] = pParam;

		m_paramList.push_back(pParam);
	}

	void PutValue(const CharType* name, unsigned int nValue)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			it->second->PutUInt(nValue);
		}
	}
	void PutValue(const CharType* name, int nValue)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			it->second->PutInt(nValue);
		}
	}
	
	void PutValue(const CharType* name, const CharType* lptValue)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			it->second->PutString(lptValue);
		}
	}

	str_type GetAll()
	{
		str_type strTmp;
		param_list::iterator it = m_paramList.begin();
		for(; it != m_paramList.end(); ++it)
		{
			strTmp += (*it)->GetString();
		}
		
		return strTmp;
	}


protected:
	param_map m_paramMap;
	param_list m_paramList;
};

template<class CharType>
class CG2XParamsReceive : protected CG2XParamsInput<CharType>
{
public:
	CG2XParamsReceive()
	{}

	void AddParam(const CharType* name, G2XParamType paramType, int nLength)
	{
		CG2XParamsInput<CharType>::AddParam(name, paramType, nLength);
	}

	void ReceiveData(const CharType* lptData)
	{
		param_list::iterator it = m_paramList.begin();
		for(; it != m_paramList.begin(); ++it)
		{
			CG2XParamT* pParam = *it;
			pParam->SetString(lptData);
			lptData += pParam->GetLength();
		}
	}

	int ParseInt(const CharType* name)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			return it->second->ParseInt();
		}
		return 0;
	}
	unsigned int ParseUInt(const CharType* name)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			return it->second->ParseUInt();
		}
		return 0;
	}
	str_type ParseString(const CharType* name)
	{
		param_map::iterator it = m_paramMap.find(str_type(name));
		if (it != m_paramMap.end())
		{
			return it->second->ParseString();
		}
		return str_type();
	}
};


}// end namespace
