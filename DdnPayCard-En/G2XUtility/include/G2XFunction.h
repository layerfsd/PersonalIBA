#pragma once

namespace G2XUtility{

//----------------------------------------
// 成员函数的函数对象定义,支持2个参数
//----------------------------------------
template<class _Result,
class _Ty,
class _Arg1,
class _Arg2>
class mem_fun2_t
	: public std::binary_function<_Arg1, _Arg2, _Result>
{	// functor adapter (*p->*pfunc)(val), non-const *pfunc
public:
	explicit mem_fun2_t(_Ty * pObj, _Result (_Ty::*_Pm)(_Arg1,_Arg2))
		: _pObj(pObj), _Pmemfun(_Pm)
	{	// construct from pointer
	}

	_Result operator()(_Arg1 _Left, _Arg2 _Right) const
	{	// call function with operand
		return (_pObj->*_Pmemfun)(_Left,_Right);
	}

private:
	_Result (_Ty::*_Pmemfun)(_Arg1, _Arg2);	// the member function pointer
	_Ty* _pObj;
};

template<class _Result,
class _Ty,
class _Arg1,
class _Arg2> inline
	mem_fun2_t<_Result,_Ty,_Arg1,_Arg2> 
	mem_fun2(_Ty * pObj, _Result (_Ty::*pm)(_Arg1,_Arg2))
{
	return  mem_fun2_t<_Result,_Ty,_Arg1,_Arg2>(pObj, pm);
}




//----------------------------------------
// 成员函数的函数对象定义,支持2个参数,可为函数对象设定额外的参数(该参数将做为绑定函数的第一个参数)
//----------------------------------------
template<class _Result,
class _Ty,
class _Ex,
class _Arg1,
class _Arg2>
class mem_fun2_ex_t
	: public std::binary_function<_Arg1, _Arg2, _Result>
{	// functor adapter (*p->*pfunc)(val), non-const *pfunc
public:
	explicit mem_fun2_ex_t(_Ty * pObj, _Result (_Ty::*_Pm)(_Ex, _Arg1, _Arg2), _Ex ex)
		: _ex(ex), _pObj(pObj), _Pmemfun(_Pm)
	{	// construct from pointer
	}

	_Result operator()(_Arg1 _Left, _Arg2 _Right) const
	{	// call function with operand
		return (_pObj->*_Pmemfun)(_ex,_Left,_Right);
	}

private:
	_Result (_Ty::*_Pmemfun)(_Ex, _Arg1, _Arg2);	// the member function pointer
	_Ty* _pObj;
	_Ex _ex;
};

template<class _Result,
class _Ty,
class _Ex,
class _Arg1,
class _Arg2> inline
	mem_fun2_ex_t<_Result,_Ty,_Ex,_Arg1,_Arg2> 
	mem_fun2_ex(_Ty * pObj, _Result (_Ty::*pm)(_Ex, _Arg1, _Arg2), _Ex ex)
{
	return  mem_fun2_ex_t<_Result,_Ty,_Ex,_Arg1,_Arg2>(pObj, pm, ex);
}

}// end namespace