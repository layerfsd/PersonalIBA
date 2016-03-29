#include "stdafx.h"
#include "gxx_base.h"

bool isBlankChar(const char ch){
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

void appendTabByDepth(std::string& strResult, int depth)
{
	if (depth == 0) return;
	char * p = new char[ depth + 1 ];
	for (int i = 0; i < depth; i++) {
		p[ i ] = '\t';
	}
	p[ depth ] = 0;
	strResult.append(p);
	delete []p;
}

void skipEscapeChar(const char* &p){
	if (*p != '\\') {
		return;
	}
	if ((*(p+1)) == '\\'){
		p += 2;
	}
}

#define GXX_INNER_CYCLE(_point) {const char* _p = _point;while(((unsigned char)*_p) & 0x80)_p++;if (_p != _point){ _point += (_p - _point); continue;}}

GxxObject::GxxObject()
:retainCount(0)
{
	
}

GxxObject::~GxxObject()
{

}

void GxxObject::retain()
{
	retainCount += 1;
}

void GxxObject::release()
{
	retainCount -= 1;
	if (retainCount == 0)
	{
		objectReleased();
		delete this;
	}
}

void GxxObject::_print( const char* x )
{
	GXX_PRINT(x);
}

void GxxObject::_println( const char* x )
{
	GXX_PRINT(x); GXX_PRINT("\n");
}

//-------------------------------------------------------------------------

bool GxxDictionary::init()
{
	keyValues = GxxValueMap::create();
	return true;
}

void GxxDictionary::objectReleased()
{
	
}

bool GxxDictionary::initWithJsonText(const char* jsonText)
{
	const char* p = jsonText;
	int nStack = 0;
	const char* pFirst = 0;
	const char* pLast = 0;
	int nQutCount = 0; // 引号个数

	while(*p)
	{
		GXX_INNER_CYCLE(p);
		if (*p == '{' && nQutCount%2 == 0){
			if ( (++nStack) == 1) pFirst = p;
		}
		else if(*p == '}' && nQutCount%2 == 0){
			--nStack;
			if (nStack < 0){
				G2X_ASSERT(0, "{} not matched !");// {}括弧不匹配
				return false;
			}
			if (nStack == 0){
				pLast = p;
				break;
			}
		}
		else if(*p == '\"'){
			nQutCount++;
		}
		p++;
	}
	if (!pFirst || !pLast){
		G2X_ASSERT(0, "{} not matched !"); // {}括弧不匹配
		return false;
	}
	if (pLast - pFirst < 2){
		return true; // 空白信息
	}

	// 已经找到花括弧的位置, 读取key引号的位置
	while(pFirst)
	{
		// 1: 找到key的首引号和尾引号的位置
		int nQuoMark = 0;
		const char* pFirstQuo = 0;
		const char* pLastQuo = 0;
		const char* pColon = 0; // 冒号的位置
		p = pFirst + 1;
		while (p != pLast)
		{
			GXX_INNER_CYCLE(p);
			if (*p == '\"'){
				++nQuoMark;
				if (nQuoMark == 1){
					pFirstQuo = p;
				}
				else
				{
					pLastQuo = p;
				}
			}
			else if(*p == ':' && nQuoMark >= 2)
			{
				pColon = p;
				break;
			}
			p++;
		}
		if (!pFirstQuo || !pLastQuo || !pColon)
		{
			G2X_ASSERT(0,"invalid json text"); // invalid json text
			return false;
		}

		CGxxKey key;
		key.append(pFirstQuo+1, pLastQuo - pFirstQuo - 1);

		// 2: 得到value
		p = pColon + 1;
		const char* pFirstValue = 0;
		while(p != pLast)
		{
			if (!isBlankChar(*p))
			{
				pFirstValue = p;
				break;
			}
			p++;
		}
		const char* pEndValue = 0;
		switch(*pFirstValue)
		{
		case '\"':
			{
				p = pFirstValue + 1;
				int nReCount = 1;
				const char* pTmpVa = NULL;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '\"')
					{
						nReCount++;
						pEndValue = p;
					}
					if (*p == ',' && nReCount % 2 == 0)
					{
						p--;break;
					}
					p++;
				}
				if (!pEndValue)
				{
					G2X_ASSERT(0, "invalid json text!");
					return false;
				}
				std::string tmp;
				tmp.append(pFirstValue+1, pEndValue - pFirstValue - 1);

				GxxValuePtr ptrV = GxxValue::create(tmp.c_str());
				keyValues->PutValue(key, ptrV);

				if (strcmp(key.c_str(), "page") == 0)
				{
					if (ptrV->intValue() == 18)
					{
						int a = 0;
						a = 10;
					}
				}
			}
			break;
		case '{':
			{
				p = pFirstValue;
				nStack = 0;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '{') ++nStack;
					else if(*p == '}')
					{
						--nStack;
						if (nStack == 0)
						{
							pEndValue = p;
							break;
						}
					}
					p++;
				}
				if (!pEndValue)
				{
					G2X_ASSERT(0, "invalid json text!");
					return false;
				}
				std::string strDic;
				strDic.append(pFirstValue, pEndValue - pFirstValue + 1);
				GxxDictionaryPtr dictionary = GxxDictionary::create();
				dictionary->initWithJsonText(strDic.c_str());
				keyValues->PutValue(key, GxxValue::create(dictionary));
			}
			break;
		case '[':
			{
				p = pFirstValue;

				
				nStack = 0;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '[')
					{
						++nStack;
					}
					else if(*p == ']')
					{
						--nStack;
						if (nStack == 0)
						{
							pEndValue = p;
							break;
						}
					}
					p++;
				}
				if (!pEndValue)
				{
					throw "invalid json text!";
					return false;
				}
				std::string strDic;
				strDic.append(pFirstValue, pEndValue - pFirstValue + 1);
				GxxArrayPtr array = GxxArray::create();
				array->initWithJsonText(strDic.c_str());
				keyValues->PutValue(key, GxxValue::create(array));
				if (array->count() > 2)
				{
					if (array->valueAtIndex(1)->getString())
					{
						if (strcmp(array->valueAtIndex(1)->stringValue(), "530112198909290320") == 0)
						{
							int a = 0;
							a = 10;
						}
					}
					
				}
			}
			break;
		default:
			{
				// 数值, 找到最后一个非数值
				p = pFirstValue;
				if (!(*p == '-' || *p == '+' || *p == '.' || ('0' <= *p && *p <= '9')))
				{
					G2X_ASSERT(0, ""); // invalid json text
					return false;
				}
				if (*p == '-' || *p == '+') p++;
				while(*p)
				{
					if (!(('0' <= *p && *p <= '9') || *p == '.'))
					{
						if (*p == 'f' || *p == 'F')
							pEndValue = p;
						else
							pEndValue = p - 1;
						break;
					}
					p++;
				}
				std::string tmp;
				tmp.append(pFirstValue, pEndValue - pFirstValue + 1);
				keyValues->PutValue(key, GxxValue::create(tmp.c_str()));
			}
			break;
		}
		// 找到 , or }
		p = pEndValue + 1;
		pFirst = 0;
		while(p != pLast)
		{
			if (*p == ',')
			{
				pFirst = p;
				break;
			}
			p++;
		}

	}
	return true;
}

void GxxDictionary::print()
{
	_println("{");
	for (GxxValueMap::Iterator it = keyValues->FirstValue(); !keyValues->IsLastValue(it); keyValues->NextValue(it))
	{
		GxxValueMap::Pair* p = *it;
		GxxString* pString = p->value->getString();
		_print(p->key.c_str());
		_print(": ");
		if (pString)
		{
			pString->print();
		}
		if (p->value->getDictionary())
		{
			p->value->getDictionary()->print();
		}
		if (p->value->getArray())
		{
			p->value->getArray()->print();
		}
	}
	_println("}");
}

bool GxxDictionary::isKeyExist(const char* key)
{
	return keyValues->ValueForKey(CGxxKey(key)) != NULL ;
}

GxxValue* GxxDictionary::valueForKey(const char* key)
{
	return keyValues->ValueForKey(CGxxKey(key));
}

int GxxDictionary::count()
{
	return (int)keyValues->count();
}

GxxValue* GxxDictionary::operator[](const char* key)
{
	return valueForKey(key);
}

void GxxDictionary::setValueForKey(GxxValuePtr& pValue, const char* key)
{
	keyValues->PutValue(CGxxKey(key), pValue);
}

GxxValue* GxxDictionary::valueForKeyPath(const char* keyPath)
{
	std::string segment;
	const char* first = keyPath;
	const char* p = keyPath;
	GxxValue* pCurrentValue = 0;
	while(*p)
	{
		if (*p == '.'){
			if (p - first < 1){
				++p;
				continue;
			}

			bool bIsArray = false;
			const char* temp = p;
			if (*(temp - 1) == ']'){
				while(temp != first)
				{
					if (*temp == '['){
						bIsArray = true;
						break;
					}
					--temp;
				}
			}
			if (bIsArray){
				segment.clear();
				segment.append(first, temp - first);
				if (pCurrentValue == 0) {
					pCurrentValue = this->valueForKey(segment.c_str());
				}
				else {
					if (pCurrentValue->getDictionary() == 0){
						return 0;
					}
					pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
				}

				if (pCurrentValue == 0){
					_println("valueForKeyPath error: key not exist!");
					return 0;
				}
				if (pCurrentValue->getArray() == 0){
					_println("valueForKeyPath error: key is invalid, it is not a array object");
					return 0;
				}
				std::string strTmp;
				strTmp.append(temp+1, p - temp - 2);
				int nIndex = atoi(strTmp.c_str());
				pCurrentValue = pCurrentValue->getArray()->valueAtIndex(nIndex);
				if (pCurrentValue == 0){
					_println("valueForKeyPath error: invalid index");
					return 0;
				}
			}
			else{
				segment.clear();
				segment.append(first, temp - first);
				if (pCurrentValue == 0) {
					pCurrentValue = this->valueForKey(segment.c_str());
				}
				else {
					if (pCurrentValue->getDictionary() == 0){
						return 0;
					}
					pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
				}

				if (pCurrentValue == 0){
					_println("valueForKeyPath error: key not exist!");
					return 0;
				}
			}
			if (pCurrentValue->getDictionary() == 0){
				_println("valueForKeyPath error: key is invalid, it is not a dictionary object");
				return 0;
			}
			first = p + 1;
		}
		++p;
	}
	if (p - first < 1){
		return pCurrentValue;
	}

	bool bIsArray = false;
	const char* temp = p;
	if (*(temp - 1) == ']'){
		while(temp != first)
		{
			if (*temp == '['){
				bIsArray = true;
				break;
			}
			--temp;
		}
	}
	if (bIsArray){
		segment.clear();
		segment.append(first, temp - first);
		if (pCurrentValue == 0) {
			pCurrentValue = this->valueForKey(segment.c_str());
		}
		else {
			if (pCurrentValue->getDictionary() == 0){
				return 0;
			}
			pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
		}

		if (pCurrentValue == 0){
			_println("valueForKeyPath error: key not exist!");
			return 0;
		}
		if (pCurrentValue->getArray() == 0){
			_println("valueForKeyPath error: key is invalid, it is not a array object");
			return 0;
		}
		std::string strTmp;
		strTmp.append(temp+1, p - temp - 2);
		int nIndex = atoi(strTmp.c_str());
		pCurrentValue = pCurrentValue->getArray()->valueAtIndex(nIndex);
		if (pCurrentValue == 0){
			_println("valueForKeyPath error: invalid index");
			return 0;
		}
	}
	else{
		segment.clear();
		segment.append(first, temp - first);
		if (pCurrentValue == 0) {
			pCurrentValue = this->valueForKey(segment.c_str());
		}
		else {
			if (pCurrentValue->getDictionary() == 0){
				return 0;
			}
			pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
		}

		if (pCurrentValue == 0){
			_println("valueForKeyPath error: key not exist!");
			return 0;
		}
	}

	return pCurrentValue;
}

void GxxDictionary::setValueForKeyPath(GxxValuePtr& pValue, const char* keyPath)
{
	std::string segment;
	const char* first = keyPath;
	const char* p = keyPath;
	GxxValue* pCurrentValue = 0;
	while(*p)
	{
		if (*p == '.'){
			if (p - first < 1){
				++p;
				continue;
			}

			bool bIsArray = false;
			const char* temp = p;
			if (*(temp - 1) == ']'){
				while(temp != first)
				{
					if (*temp == '['){
						bIsArray = true;
						break;
					}
					--temp;
				}
			}
			if (bIsArray){
				segment.clear();
				segment.append(first, temp - first);
				if (pCurrentValue == 0) {
					pCurrentValue = this->valueForKey(segment.c_str());
				}
				else {
					if (pCurrentValue->getDictionary() == 0){
						return ;
					}
					pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
				}

				if (pCurrentValue == 0){
					_println("valueForKeyPath error: key not exist!");
					return;
				}
				if (pCurrentValue->getArray() == 0){
					_println("valueForKeyPath error: key is invalid, it is not a array object");
					return;
				}


				std::string strTmp;
				strTmp.append(temp+1, p - temp - 2);
				int nIndex = atoi(strTmp.c_str());
				pCurrentValue = pCurrentValue->getArray()->valueAtIndex(nIndex);
				if (pCurrentValue == 0){
					_println("valueForKeyPath error: invalid index");
					return;
				}
			}
			else{
				segment.clear();
				segment.append(first, temp - first);
				if (pCurrentValue == 0) {
					pCurrentValue = this->valueForKey(segment.c_str());
				}
				else {
					if (pCurrentValue->getDictionary() == 0){
						return ;
					}
					pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
				}

				if (pCurrentValue == 0){
					_println("valueForKeyPath error: key not exist!");
					return ;
				}
			}
			if (pCurrentValue->getDictionary() == 0){
				_println("valueForKeyPath error: key is invalid, it is not a dictionary object");
				return ;
			}
			first = p + 1;
		}
		++p;
	}
	if (p - first < 1){
		return;
	}

	bool bIsArray = false;
	const char* temp = p;
	if (*(temp - 1) == ']'){
		while(temp != first)
		{
			if (*temp == '['){
				bIsArray = true;
				break;
			}
			--temp;
		}
	}
	if (bIsArray){
		segment.clear();
		segment.append(first, temp - first);
		if (pCurrentValue == 0) {
			pCurrentValue = this->valueForKey(segment.c_str());
		}
		else {
			if (pCurrentValue->getDictionary() == 0){
				return;
			}
			pCurrentValue = pCurrentValue->getDictionary()->valueForKey(segment.c_str());
		}

		if (pCurrentValue == 0){
			_println("valueForKeyPath error: key not exist!");
			return;
		}
		if (pCurrentValue->getArray() == 0){
			_println("valueForKeyPath error: key is invalid, it is not a array object");
			return;
		}
		std::string strTmp;
		strTmp.append(temp+1, p - temp - 2);
		int nIndex = atoi(strTmp.c_str());

		GxxArray* pArray = pCurrentValue->getArray();
		if (nIndex >= 0 && nIndex < pArray->count()){
			pArray->setValue(pValue, nIndex);
		}else{
			pArray->addValue(pValue);
		}
	}
	else{
		segment.clear();
		segment.append(first, temp - first);
		if (pCurrentValue == 0) {
			this->setValueForKey(pValue, segment.c_str());
		}
		else {
			if (pCurrentValue->getDictionary() == 0){
				return ;
			}
			pCurrentValue->getDictionary()->setValueForKey(pValue, segment.c_str());
		}
	}
}

int GxxDictionary::intValueForKey( const char* key, int defaultValue /*= 0*/ )
{
	GxxValue* pValue = valueForKey(key);
	if (pValue)
	{
		return pValue->intValue();
	}
	return defaultValue;
}


bool GxxDictionary::boolValueForKey( const char* key, bool defaultValue /*= false*/ )
{
	GxxValue* pValue = valueForKey(key);
	if (pValue)
	{
		return pValue->intValue() > 0;
	}
	return defaultValue;
}


std::string GxxDictionary::describe()
{
	return describe_d (0, true);
}

std::string GxxDictionary::describe_d( int depth, bool bFormat)
{
	std::string strResult;
	if (bFormat) {
		appendTabByDepth(strResult, depth);
	}
	strResult.append("{");
	if (bFormat)
		strResult.append("\n");
	
	int i = 0;
	int nCount = count();
	GxxValueMap::Iterator pos = this->keyValues->FirstValue();
	for (; !keyValues->IsLastValue(pos); keyValues->NextValue(pos),i++ )
	{
		if (bFormat) {
			appendTabByDepth(strResult, depth+1);
		}
		GxxValueMap::Pair*p = *pos;
		strResult.append("\"");
		strResult.append(p->key);
		strResult.append("\":");
		if (p->value->getString()) {
			strResult.append("\"");
			strResult.append(p->value->stringValue());
			strResult.append("\"");
			if (i != nCount - 1)
				strResult.append(","); // 添加逗号
			if (bFormat)
				strResult.append("\n");
		}
		else if(p->value->getDictionary()) {
			if (bFormat)
				strResult.append("\n");
			strResult.append( p->value->getDictionary()->describe_d(depth + 1, bFormat));
			if (i != nCount-1) {
				if (strResult[strResult.length()-1] == '\n'){
					strResult[strResult.length()-1] = ',';
				}else {
					strResult.append(",");
				}
				if (bFormat) {
					strResult.append("\n");
				}
			}
		}
		else if(p->value->getArray()) {
			if (bFormat)
				strResult.append("\n");
			strResult.append( p->value->getArray()->describe_d(depth + 1, bFormat));
			if (i != nCount-1) {
				if (strResult[strResult.length()-1] == '\n'){
					strResult[strResult.length()-1] = ',';
				}else {
					strResult.append(",");
				}
				if (bFormat) {
					strResult.append("\n");
				}
			}
		}
	}
	if (bFormat) {
		appendTabByDepth(strResult, depth);
	}
	strResult.append("}");
	if (bFormat) {
		strResult.append("\n");
	}

	return strResult;
}



//-------------------------------------------------------------------------

void GxxValue::objectReleased()
{
	
}

void GxxValue::_init()
{
}

const char* GxxValue::stringValue()
{
	if (ptrStringData){
		return ptrStringData->getString();
	}
	return 0;
}

int GxxValue::intValue()
{
	if (ptrStringData){
		return atoi(stringValue());
	}
	return 0;
}

float GxxValue::floatValue()
{
	if (ptrStringData){
		return (float)atof(stringValue());
	}
	return 0;
}

double GxxValue::doubleValue()
{
	if (ptrStringData){
		return atof(stringValue());
	}
	return 0;
}

GxxValuePtr GxxValue::create(GxxDictionaryPtr& pValue)
{
	GxxValuePtr ptrObj = create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->ptrDictionaryData = pValue;
	return ptrObj;
}

GxxValuePtr GxxValue::create(GxxArrayPtr& pValue)
{
	GxxValuePtr ptrObj = create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->ptrArrayData = pValue;
	return ptrObj;
}

GxxValuePtr GxxValue::create(GxxStringPtr& pValue)
{
	GxxValuePtr ptrObj = create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->ptrStringData = pValue;
	return ptrObj;
}

GxxValuePtr GxxValue::create(const char* pValue)
{
	GxxValuePtr ptrObj = create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->ptrStringData = GxxString::create(pValue);
	return ptrObj;
}

GxxValuePtr GxxValue::create(GxxObjectPtr& ptrOtherValue)
{
	GxxValuePtr ptrObj = create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->ptrOtherData = ptrOtherValue;
	return ptrObj;
}

GxxValuePtr GxxValue::create( long intValue )
{
	char tmp[20];
	sprintf(tmp, "%ld", intValue);
	return GxxValue::create((const char*)tmp);
}

GxxValuePtr GxxValue::create( __int64 int64Value )
{
	char tmp[20];
	sprintf(tmp, "%lld", int64Value);
	return GxxValue::create((const char*)tmp);
}

GxxValuePtr GxxValue::create( float floatValue)
{
	char tmp[20];
	sprintf(tmp, "%f", floatValue);
	return GxxValue::create((const char*)tmp);
}

void GxxValue::print()
{
	if		 (ptrStringData)			ptrStringData->print();
	else if (ptrDictionaryData)			ptrDictionaryData->print();
	else if (ptrArrayData) 				ptrArrayData->print();
	else if (ptrOtherData)				ptrOtherData->print();
}


//-------------------------------------------------------------------------

bool GxxArray::initWithJsonText(const char* jsonText)
{
	const char* p = jsonText;
	int nStack = 0;
	const char* pFirst = 0;
	const char* pLast = 0;
	int nQutCount = 0; // 引号个数

	while(*p)
	{
		GXX_INNER_CYCLE(p);
		if (*p == '[' && nQutCount%2 == 0){
			if ( (++nStack) == 1) pFirst = p;
		}
		else if(*p == ']' && nQutCount%2 == 0){
			--nStack;
			if (nStack < 0){
				G2X_ASSERT(0, "[]括弧不匹配");
				return false;
			}
			if (nStack == 0){
				pLast = p;
				break;
			}
		}
		else if(*p == '\"'){
			nQutCount++;
		}
		p++;
	}

	if (!pFirst || !pLast){
		G2X_ASSERT(0, "invalid json text!");
		return false;
	}

	// 已经找到花括弧的位置, 读取key引号的位置
	if (pLast - pFirst < 2){
		return false; // 无信息
	}

	while(pFirst)
	{
		const char* pFirstValue= 0;
		p = pFirst + 1;
		while(p != pLast)
		{
			GXX_INNER_CYCLE(p);
			if (!isBlankChar(*p))
			{
				pFirstValue = p;
				break;
			}
			p++;
		}
		const char* pEndValue = 0;
		switch(*pFirstValue)
		{
		case '\"':
			{
				p = pFirstValue + 1;
				int nReCount = 1;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '\"')
					{
						nReCount ++;
						pEndValue = p;
					}
					else if(*p == ',' && nReCount >= 2)
					{
						p--;
						break;
					}
					p++;
				}
				if (!pEndValue)
				{
					G2X_ASSERT(0, "invalid json text!");
					return false;
				}
				std::string tmp;
				tmp.append(pFirstValue+1, pEndValue - pFirstValue - 1);
				arrayValues.push_back(GxxValue::create(tmp.c_str()));
			}
			break;
		case '{':
			{
				p = pFirstValue;
				nStack = 0;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '{') 
					{
						++nStack;
					}
					else if(*p == '}')
					{
						--nStack;
						if (nStack == 0)
						{
							pEndValue = p;
							break;
						}
					}
					
					p++;
				}
				if (!pEndValue)
				{
					G2X_ASSERT(0, "invalid json text!");
					return false;
				}
				std::string strDic;
				strDic.append(pFirstValue, pEndValue - pFirstValue + 1);
				GxxDictionaryPtr dictionary = GxxDictionary::create();
				dictionary->initWithJsonText(strDic.c_str());
				arrayValues.push_back(GxxValue::create(dictionary));
			}
			break;
		case '[':
			{
				p = pFirstValue;
				nStack = 0;
				while(p != pLast)
				{
					GXX_INNER_CYCLE(p);
					if (*p == '[') ++nStack;
					else if(*p == ']')
					{
						--nStack;
						if (nStack == 0)
						{
							pEndValue = p;
							break;
						}
					}
					p++;
				}
				if (!pEndValue)
				{
					G2X_ASSERT(0, "invalid json text!");
					return false;
				}
				std::string strDic;
				strDic.append(pFirstValue, pEndValue - pFirstValue + 1);
				GxxArrayPtr array = GxxArray::create();
				array->initWithJsonText(strDic.c_str());
				arrayValues.push_back(GxxValue::create(array));
			}
			break;
		default:
			{
				// 数值, 找到最后一个非数值
				p = pFirstValue;
				if (!(*p == '-' || *p == '+' || *p == '.' || ('0' <= *p && *p <= '9')))
				{
					G2X_ASSERT(0, ""); // invalid json text
					return false;
				}
				if (*p == '-' || *p == '+') p++;
				while(*p)
				{
					if (!(('0' <= *p && *p <= '9') || *p == '.'))
					{
						if (*p == 'f' || *p == 'F')
							pEndValue = p;
						else
							pEndValue = p - 1;
						break;
					}
					p++;
				}
				std::string tmp;
				tmp.append(pFirstValue, pEndValue - pFirstValue + 1);
				arrayValues.push_back(GxxValue::create(tmp.c_str()));
			}
			break;
		}
		// 找到 , or }
		p = pEndValue + 1;
		pFirst = 0;
		while(p != pLast)
		{
			if (*p == ',')
			{
				pFirst = p;
				break;
			}
			p++;
		}
	}
	return true;
}

void GxxArray::print()
{
	_println("[");
	for (ArrayValue::iterator it = arrayValues.begin(); it != arrayValues.end(); it++)
	{
		GxxString* pString = (*it)->getString();
		if (pString)
		{
			pString->print();
		}
		if ((*it)->getDictionary())
		{
			(*it)->getDictionary()->print();
		}
		if ((*it)->getArray())
		{
			(*it)->getArray()->print();
		}
	}
	_println("]");
}



void GxxArray::objectReleased()
{
	
}

int GxxArray::count()
{
	return (int)arrayValues.size();
}

GxxValue* GxxArray::valueAtIndex(int iIndex)
{
	if (iIndex < 0 || iIndex >= count())
		return 0;
	return arrayValues[iIndex];
}

void GxxArray::addValue(GxxValuePtr& pValue)
{
	arrayValues.push_back(pValue);
}

void GxxArray::setValue(GxxValuePtr& pValue, int iIndex)
{
	if (iIndex < 0 || iIndex >= count())
		return;

	arrayValues[iIndex] = pValue;
}

void GxxArray::removeValue( int iIndex )
{
	arrayValues.erase(arrayValues.begin() + iIndex);
}

void GxxArray::removeAllValues()
{
	arrayValues.clear();
}

std::string GxxArray::describe()
{
	return describe_d(0, true);
}

std::string GxxArray::describe_d( int depth, bool bFormat )
{
	std::string strResult;
	if (bFormat) {
		appendTabByDepth(strResult, depth);
	}
	strResult.append("[");
	if (bFormat)
		strResult.append("\n");

	for (int i = 0; i <  this->count(); i++) 
	{
		GxxValue* pValue = this->valueAtIndex(i);
		
		if (pValue->getString()) {
			if (bFormat) {
				appendTabByDepth(strResult, depth + 1);
			}
			strResult.append("\"");
			strResult.append(pValue->stringValue());
			strResult.append("\"");
			if (i != this->count()-1)
				strResult.append(","); // 添加逗号
			if (bFormat)
				strResult.append("\n"); // 添加换行
		}
		else if(pValue->getDictionary()) {
			strResult.append( pValue->getDictionary()->describe_d(depth + 1, bFormat));
			if (i != this->count()-1) {
				if (strResult[strResult.length()-1] == '\n'){
					strResult[strResult.length()-1] = ',';
				}else {
					strResult.append(",");
				}
				if (bFormat) {
					strResult.append("\n");
				}
			}
		}
		else if(pValue->getArray()) {
			strResult.append( pValue->getArray()->describe_d(depth + 1, bFormat));
			if (i != this->count()-1) {
				if (strResult[strResult.length()-1] == '\n'){
					strResult[strResult.length()-1] = ',';
				}else {
					strResult.append(",");
				}
				if (bFormat) {
					strResult.append("\n");
				}
			}
		}
	}
	if (bFormat) {
		appendTabByDepth(strResult, depth);
	}
	strResult.append("]");
	if (bFormat) {
		strResult.append("\n");
	}

	return strResult;
}

//-------------------------------------------------------------------------

void GxxString::print()
{
	_println(_string.c_str());
}

const char* GxxString::getString()
{
	return _string.c_str();
}

GxxStringPtr GxxString::create(const char* str)
{
	GxxStringPtr ptrObj = GxxString::create();
	if (!ptrObj){
		return ptrObj;
	}
	ptrObj->_string = str;
	return ptrObj;
}
//--------------------------------------------------------------------------
bool GxxValueMap::init()
{
	__pMap = new TyMap;
	return true;
}

void GxxValueMap::objectReleased()
{
	for (TyMap::iterator it = FirstValue(); !IsLastValue(it); NextValue(it))
	{
		Pair* pair = (*it);
		if (pair)
		{
			delete pair;
		}
	}
	__pMap->clear();
	delete __pMap;
}

void GxxValueMap::PutValue( CGxxKey& _key, GxxValuePtr& _value )
{
	for (TyMap::iterator it = FirstValue(); !IsLastValue(it); NextValue(it))
	{
		Pair *p = *it;
		if (p->key.compare(_key) == 0)
		{
			p->value = _value;
			return;
		}
	}
	Pair *p = new Pair;
	p->key = _key;
	p->value = _value;
	__pMap->push_back(p);
}

GxxValuePtr GxxValueMap::ValueForKey( CGxxKey& _key )
{
	for (TyMap::iterator it = FirstValue(); !IsLastValue(it); NextValue(it))
	{
		Pair *p = *it;
		if (p->key.compare(_key) == 0)
			return p->value;
	}
	return GxxValuePtr();
}

GxxValueMap::Iterator GxxValueMap::FirstValue()
{
	return __pMap->begin();
}

void GxxValueMap::NextValue( Iterator& pos )
{
	if (pos != __pMap->end())
	{
		++pos;
	}
}

int GxxValueMap::count()
{
	return (int)__pMap->size();
}

bool GxxValueMap::IsLastValue( Iterator& it )
{
	return it == __pMap->end();
}
