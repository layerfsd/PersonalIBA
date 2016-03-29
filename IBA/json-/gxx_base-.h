#ifndef __GXX_JSON__H__
#define __GXX_JSON__H__

#include <map>
#include <vector>
#include <string>

#ifndef G2X_CLASSES
#define G2X_CLASSES 
#endif

#ifndef G2X_ASSERT
#define G2X_ASSERT(ex,msg) ASSERT(ex)
#endif

#define GXX_PRINT(x) {do{TRACE(x);} while (0);}

template<class _Ty>
class GxxAutoPtr
{
public:
	typedef _Ty element_type;

	/*explicit*/ GxxAutoPtr(_Ty* _Ptr = 0)
		: _Myptr(_Ptr)
	{
		if (_Ptr) _Myptr->retain();
	}
	GxxAutoPtr(const GxxAutoPtr<_Ty>& _Right)
	{
		if (_Myptr == _Right.get()){
			if (_Myptr) _Myptr->retain();
		}else{
			_Myptr = _Right.get();
			if (_Myptr) _Myptr->retain();
		}
	}
	template<class _Other>
		GxxAutoPtr(GxxAutoPtr<_Other>& _Right)
	{
		if (_Myptr == _Right.get()){
			if (_Myptr) _Myptr->retain();
		}else{
			_Myptr = _Right.get();
			if (_Myptr) _Myptr->retain();
		}
	}
	virtual ~GxxAutoPtr()
	{
		if (_Myptr) _Myptr->release();
	}
	_Ty& operator*() const
	{	// return designated value
		return (*_Myptr);
	}

	_Ty *operator->() const
	{	// return pointer to class object
		return (&**this);
	}

	_Ty* get() const
	{
		return _Myptr;
	}

	template<class _Other>
		operator GxxAutoPtr<_Other>()
	{
		return (GxxAutoPtr<_Other>(*this));
	}
	operator _Ty* () const
	{
		return _Myptr;
	}

	GxxAutoPtr<_Ty>& operator = (_Ty* _Ptr)
	{
		if (_Myptr == _Ptr)
			return *this;
		if (_Myptr) _Myptr->release();
		_Myptr = _Ptr;
		if (_Myptr) _Myptr->retain();

		return *this;
	}

	GxxAutoPtr<_Ty>& operator = (GxxAutoPtr<_Ty>& _Right)
	{
		if (_Myptr == _Right.get())
			return *this;
		if (_Myptr) _Myptr->release();
		_Myptr = _Right.get();
		if (_Myptr) _Myptr->retain();

		return *this;
	}

private:
	_Ty* _Myptr;
};

typedef std::string CGxxKey;
typedef std::string CGxxValueString;

#define GXX_CREATE_FUNC(classTy) \
protected:\
	classTy(){}\
public:\
	static GxxAutoPtr<classTy> create()\
	{\
		classTy* object = (new classTy);\
		if (!object->init())\
		{\
			object->release();\
			return (GxxAutoPtr<classTy>((classTy*)0));\
		}\
		return (GxxAutoPtr<classTy>(object));\
	}

class G2X_CLASSES GxxObject;
class G2X_CLASSES GxxDictionary;
class G2X_CLASSES GxxArray;
class G2X_CLASSES GxxString;
class G2X_CLASSES GxxValue;
class G2X_CLASSES GxxValueMap;

typedef GxxAutoPtr<GxxObject> GxxObjectPtr;
typedef GxxAutoPtr<GxxDictionary> GxxDictionaryPtr;
typedef GxxAutoPtr<GxxArray> GxxArrayPtr;
typedef GxxAutoPtr<GxxString> GxxStringPtr;
typedef GxxAutoPtr<GxxValue> GxxValuePtr;
typedef GxxAutoPtr<GxxValueMap> GxxValueMapPtr;

class G2X_CLASSES GxxObject
{
protected:
	GxxObject();

public:
	virtual ~GxxObject();

	virtual void print() {};
	
	void retain();
	void release();

protected:
	virtual bool init() { return true; }
	virtual void objectReleased() {}
	
public:
	virtual std::string describe() { return std::string(""); }
	void _print(const char* x);
	void _println(const char* x);

private:
	int retainCount;
};



class G2X_CLASSES GxxDictionary : public GxxObject
{
	GXX_CREATE_FUNC(GxxDictionary);
protected:
	virtual bool init();
	virtual void objectReleased();
public:
	/* 用json格式的字符串初始化字典 */
	bool initWithJsonText(const char* jsonText);

	virtual std::string describe();
	std::string describe_d(int depth, bool bFormat);

	int count();
	virtual void print();
	bool isKeyExist(const char* key);
	GxxValue* valueForKey(const char* key);
	GxxValue* operator[](const char* key);
	void setValueForKey(GxxValuePtr& pValue, const char* key);
	GxxValue* valueForKeyPath(const char* keyPath);
	int intValueForKey(const char* key, int defaultValue = 0);
	void setValueForKeyPath(GxxValuePtr& pValue, const char* keyPath);
bool boolValueForKey(const char* key, bool defaultValue = false);
private:
	

private:
	GxxValueMapPtr keyValues;
};

class G2X_CLASSES GxxArray : public GxxObject
{
	GXX_CREATE_FUNC(GxxArray);
protected:
	virtual void objectReleased();

public:
	bool initWithJsonText(const char* jsonText);

	virtual std::string describe();
	std::string describe_d(int depth, bool bFormat);

	virtual void print();
	void addValue(GxxValuePtr& pValue);
	void setValue(GxxValuePtr& pValue, int iIndex);
	void removeValue(int iIndex);
	void removeAllValues();
	GxxValue* valueAtIndex(int iIndex);
	int count();

private:
	typedef std::vector<GxxValuePtr> ArrayValue;
	ArrayValue arrayValues;
};

class G2X_CLASSES GxxString : public GxxObject
{
	GXX_CREATE_FUNC(GxxString);
public:
	static GxxStringPtr create(const char* str);
	virtual void print();

	const char* getString();

private:
	std::string _string;
};



class G2X_CLASSES GxxValue : public GxxObject
{
	GXX_CREATE_FUNC(GxxValue);
public:
	static GxxValuePtr create(GxxDictionaryPtr& ptrValue);
	static GxxValuePtr create(GxxArrayPtr& ptrValue);
	static GxxValuePtr create(GxxStringPtr& ptrValue);
	static GxxValuePtr create(const char* pValue);
	static GxxValuePtr create(GxxObjectPtr& ptrOtherValue);

	virtual bool init(){
		_init();
		return true;
	}
	virtual void print();

	GxxDictionary* getDictionary(){return ptrDictionaryData;}
	GxxArray* getArray(){return ptrArrayData;}
	GxxString* getString(){return ptrStringData;}
	GxxObject* getOther(){return ptrOtherData;}
	const char* stringValue();
	int intValue();
	float floatValue();
	double doubleValue();

protected:
	virtual void objectReleased();

private:
	void _init();
private:
	 
	GxxDictionaryPtr ptrDictionaryData;
	GxxArrayPtr ptrArrayData;
	GxxStringPtr ptrStringData;
	GxxObjectPtr ptrOtherData;
};

class G2X_CLASSES GxxValueMap : public GxxObject
{
	GXX_CREATE_FUNC(GxxValueMap);
public:
	struct Pair
	{
		CGxxKey key;
		GxxValuePtr value;
	};
	typedef std::vector<Pair*> TyMap;
	typedef TyMap::iterator Iterator;

	int count();
	void PutValue(CGxxKey& _key, GxxValuePtr& _value);
	GxxValuePtr ValueForKey(CGxxKey& _key);

	Iterator FirstValue();
	bool IsLastValue(Iterator& it);
	void NextValue(Iterator& pos);

protected:
	virtual bool init();
	virtual void objectReleased();

private:
	TyMap* __pMap;
};



#endif