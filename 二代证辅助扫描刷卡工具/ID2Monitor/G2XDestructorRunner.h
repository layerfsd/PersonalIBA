/**************************************************************************
file: G2XDestructorRunner.h
class: CG2XDestructorRunner 

     析构函数运行器：当CG2XDestructorRunner对象生命周期结束时，它的析构函数调用它在初始化
	    时被指定的回调函数
	
	某对象可以将某个成员函数委托给CG2XDestructorRunner去执行。
remark:
   
   CG2XDestructorRunner提供4种方式的类重载.
   1. 生命周期结束时，调用某个对象的成员函数(函数带一个参数)
   2. 生命周期结束时，调用某个对象的成员函数(函数无参数)
   3. 生命周期结束时，调用某个普通函数(函数带一个参数)
   4. 生命周期结束时，调用某个普通函数(函数无参数)

   例如一种使用情况:
   某个函数的返回路径有N条，并且在每个return前需要调用另一个函数,那么此时就可以用到
   CG2XDestructorRunner, CG2XDestructorRunner必须被创建为函数的局部对象。

date: 2011-5-18
writer: guoxuxing

mail: gemeryguo@126.com
***************************************************************************/

#pragma once

template<class TClass,typename TResult,typename TParam>
class CG2XDestructorRunner
{
	// 析构时，调用某个对象的函数(函数具备一个参数)
public:
	typedef TResult (TClass::*Function)(TParam);

	CG2XDestructorRunner(Function callback,TParam param,TClass *pClassObj):
	m_callback(callback),m_pClassObj(pClassObj),m_param(param)
	{

	}
	~CG2XDestructorRunner()
	{
		(m_pClassObj->*m_callback)(m_param);
	}

	Function m_callback;
	TClass *m_pClassObj;
	TParam m_param;
};
template<class TClass,typename TResult>
class CG2XDestructorRunner<TClass,TResult,void>
{
	// 析构时，调用某个对象的函数(函数无参数)
public:
	typedef TResult (TClass::*Function)(void);

	CG2XDestructorRunner(Function callback,TClass *pClassObj):
	m_callback(callback),m_pClassObj(pClassObj)
	{

	}
	~CG2XDestructorRunner()
	{
		(m_pClassObj->*m_callback)();
	}

	Function m_callback;
	TClass *m_pClassObj;
};

template<typename TResult,typename TParam>
class CG2XDestructorRunner<void,TResult,TParam>
{
	// 析构时，调用一个普通函数
public:
	typedef TResult(*Function)(TParam);
	CG2XDestructorRunner(Function callback,TParam param):
	m_callback(callback),m_param(param)
	{

	}
	~CG2XDestructorRunner()
	{
		(m_callback)(m_param);
	}

	Function m_callback;
	TParam m_param;
};

template<typename TResult>
class CG2XDestructorRunner<void,TResult,void>
{
	// 析构时，调用一个普通函数
public:
	typedef TResult(*Function)(void);
	CG2XDestructorRunner(Function callback):
	m_callback(callback)
	{

	}
	~CG2XDestructorRunner()
	{
		(m_callback)();
	}

	Function m_callback;
};