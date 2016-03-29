#pragma once
/// 
/// queue.h
/// Author:guoxuxing
/// CreateTime:2010-2-2
///
#include <vector>
using namespace std;

namespace NS_GXX
{
/*
* In some cases,when dequeues the element,we will to do something.
*/
template<class Ty>
struct CQueue_DequeFunction
{
	virtual Ty operator ()(const Ty& elem)
	{
		return elem;
	}
};

/*
* Class: CQueue<Ty>
* For: First-In-First-Out
*/


template<class Ty>
class CQueue
{
public:
	typedef typename vector<Ty>::iterator queue_iterator;
	typedef typename vector<Ty>::reverse_iterator queue_reverse_iterator;
	typedef typename vector<Ty>::size_type size_type;
	//进队
	virtual void enqueue(const Ty& elem)
	{
		m_queue.push_back(elem);
	}
	//出队
	virtual Ty dequeue(CQueue_DequeFunction<Ty>* pFn = NULL)
	{
		queue_iterator it = m_queue.begin();
		Ty rsElem = *it;
		m_queue.erase(it);
		if( pFn )
		{
			CQueue_DequeFunction<Ty>& fn = *pFn;
			return fn(rsElem);
		}
		return rsElem;
	}
	//插队
	void insert(size_type pos,const Ty& elem)
	{
		queue_iterator it = m_queue.begin();
		it += pos;
		m_queue.insert(it,elem);
	}
	// insert an element
	void insert(const queue_iterator itPos,const Ty& elem)
	{
		m_queue.insert(itPos,elem);
	}
	// clear all elements
	void clear()
	{
		m_queue.clear();
	}
	// get the size of the queue
	size_type size()
	{
		return m_queue.size();
	}
	// get the begin--iterator of the queue
	queue_iterator begin()
	{
		return m_queue.begin();
	}
	// get the end--iterator of the queue
	queue_iterator end()
	{
		return m_queue.end();
	}
	// get the reverse--iterator of the queue
	queue_reverse_iterator rbegin()
	{
		return m_queue.rbegin();
	}

	queue_reverse_iterator rend()
	{
		return m_queue.rend();
	}

	Ty at(size_type pos )
	{
		return m_queue.at(pos);
	}

	Ty& operator[](size_type i)
	{
		return m_queue[i];
	}

private:
	vector<Ty> m_queue;
};

/*
* Class:CFixedQueue<Ty,_QueueSize>
* For:First-In-First-Out. But the size of Queue can't be greater than _QueueSize.
*     While enqueues one element to the queue,if the size is greater than _QueueSize,
*     then the first element will be dequeued.
*     
*/

template<
typename Ty,
size_t _QueueSize
>
class CFixedQueue:public CQueue<Ty>
{
public:
	virtual void enqueue(const Ty& elem,CQueue_DequeFunction<Ty>&fn = CQueue_DequeFunction<Ty>())
	{
		if( size() == _QueueSize)
		{
			dequeue(&fn);
		}
		CQueue<Ty>::enqueue(elem);
	}
};

}