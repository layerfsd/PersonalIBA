#pragma once
/********************************************************************
NS_COMMON::CThreadTemplate<TClass,TResult>
	使用线程模板，可以指定类成员函数为线程的执行函数
wirter: guoxuxing
date: 2011-4-22
*********************************************************************/


#include <functional>

namespace NS_COMMON
{
	template<class TClass,typename TResult=void>
	class CThreadTemplate : public CThread
	{
	public:
		friend class TClass;
		CThreadTemplate()
		{
			m_pObj = NULL;
			m_callback = 0;
		}
		CThreadTemplate(TClass *pObj,TResult (TClass::*callback_fun)())
		{
			SetCallBack(callback_fun);
		}

		// 设置回调函数，必须为类成员函数
		void SetCallBack(TClass *pObj,TResult (TClass::*callback_fun)())
		{
			m_pObj = pObj;
			m_callback = callback_fun;
		}

	protected:
		virtual INT Run()
		{
			ASSERT(m_pObj);
			ASSERT(m_callback!=NULL);

			(m_pObj->*m_callback)();
			return 0;
		}

	private:
		TClass* m_pObj;
		TResult (TClass::*m_callback)();
	};

}// end-namespace-NS_UTITLITY