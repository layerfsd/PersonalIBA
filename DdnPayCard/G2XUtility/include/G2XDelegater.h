#pragma once

namespace G2XUtility{

template<
class TClass, 
typename TResult, 
typename TParam1,
typename TParam2,
typename TParam3>
class CG2XDelegater
{
public:
	typedef TResult (TClass::*Function)(TParam1,TParam2,TParam3);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	TResult ExecuteTask(TParam1 p1,TParam2 p2,TParam3 p3)
	{
		return (m_pClassObj->*m_delegaterFun)(p1,p2,p3);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass>
class CG2XDelegater<TClass,void,void,void,void>
{
public:
	typedef void (TClass::*Function)(void);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	void ExecuteTask(void)
	{
		(m_pClassObj->*m_delegaterFun)();
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TParam>
class CG2XDelegater<TClass,void,TParam,void,void>
{
public:
	typedef void (TClass::*Function)(TParam);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	void ExecuteTask(TParam p)
	{
		(m_pClassObj->*m_delegaterFun)(p);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TParam1,typename TParam2>
class CG2XDelegater<TClass,void,TParam1,TParam2,void>
{
public:
	typedef void (TClass::*Function)(TParam1,TParam2);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	void ExecuteTask(TParam1 p1,TParam2 p2)
	{
		(m_pClassObj->*m_delegaterFun)(p1,p2);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TParam1,typename TParam2,typename TParam3>
class CG2XDelegater<TClass,void,TParam1,TParam2,TParam3>
{
public:
	typedef void (TClass::*Function)(TParam1,TParam2,TParam3);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	void ExecuteTask(TParam1 p1,TParam2 p2,TParam3 p3)
	{
		(m_pClassObj->*m_delegaterFun)(p1,p2,p3);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TResult>
class CG2XDelegater<TClass,TResult,void,void,void>
{
public:
	typedef TResult (TClass::*Function)(void);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	TResult ExecuteTask(void)
	{
		return (m_pClassObj->*m_delegaterFun)();
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TResult,typename TParam>
class CG2XDelegater<TClass,TResult,TParam,void,void>
{
public:
	typedef TResult (TClass::*Function)(TParam);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	TResult ExecuteTask(TParam p)
	{
		return (m_pClassObj->*m_delegaterFun)(p);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TResult,typename TParam1,typename TParam2>
class CG2XDelegater<TClass,TResult,TParam1,TParam2,void>
{
public:
	typedef TResult (TClass::*Function)(TParam1,TParam2);

	CG2XDelegater(TClass* pClassObj,Function delegaterFun)
		:m_pClassObj(pClassObj),m_delegaterFun(delegaterFun)
	{
	}

	TResult ExecuteTask(TParam1 p1,TParam2 p2)
	{
		return (m_pClassObj->*m_delegaterFun)(p1,p2);
	}

	TClass *m_pClassObj;
	Function m_delegaterFun;
};

template<class TClass,typename TResult>
class CG2XDelegater0 : public CG2XDelegater<TClass,TResult,void,void,void>
{
public:
	CG2XDelegater0(TClass* pClassObj,Function delegaterFun):
	  CG2XDelegater<TClass,TResult,void,void,void>(pClassObj,delegaterFun){}
};
template<class TClass,typename TResult,typename TParam1>
class CG2XDelegater1 : public CG2XDelegater<TClass,TResult,TParam1,void,void>
{
public:
	CG2XDelegater1(TClass* pClassObj,Function delegaterFun):
	  CG2XDelegater<TClass,TResult,TParam1,void,void>(pClassObj,delegaterFun){}
};
template<class TClass,typename TResult,typename TParam1,typename TParam2>
class CG2XDelegater2 : public CG2XDelegater<TClass,TResult,TParam1,TParam2,void>
{
public:
	CG2XDelegater2(TClass* pClassObj,Function delegaterFun):
	  CG2XDelegater<TClass,TResult,TParam1,TParam2,void>(pClassObj,delegaterFun){}
};
template<class TClass,typename TResult,typename TParam1,typename TParam2,typename TParam3>
class CG2XDelegater3 : public CG2XDelegater<TClass,TResult,TParam1,TParam2,TParam3>
{
public:
	CG2XDelegater3(TClass* pClassObj,Function delegaterFun):
	  CG2XDelegater<TClass,TResult,TParam1,TParam2,TParam3>(pClassObj,delegaterFun){}
};


}