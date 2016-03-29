#pragma  once

namespace NS_GXX
{
    
typedef DWORD  DATA_PTR;     //指针的数据形式
//com initialize and uninitialize
class CComBase 
{
protected:
	CComBase(){
		::CoInitialize(NULL);
	}
public:
	virtual ~CComBase(){
		::CoUninitialize();
	}
};

/*
 * My Clock
*/
class CMyClock
{
public:
	CMyClock();
	~CMyClock(){}
	//
	//开始计时
	void Start();
	//
	//结束时钟，返回逝去的毫秒数
	DWORD End();    
private:
	DWORD m_dwTickCount;
};

#ifdef USING_MYCLOCK
#define START_MYCLOCK(_idSymbol)\
	NS_GXX::CMyClock classMyClock##_idSymbol;\
	classMyClock##_idSymbol.Start();
#define END_MYCLOCK(_idSymbol)\
	(classMyClock##_idSymbol.End())
#else
#define START_MYCLOCK(_idSymbol)
#define END_MYCLOCK(_idSymbol)
#endif
//----------------------------------------------------------

}