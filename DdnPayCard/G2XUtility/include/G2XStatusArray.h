#pragma once
#include <vector>

namespace G2XUtility{

#define G2XStatusFALSE 0
#define G2XStatusTRUE  1

template<typename ValueType>
class CG2XStatusArray
{
public:
	struct _VItem
	{
		unsigned char status;       // 状态值
		ValueType vtValue; // 变量值
	};

public:
	CG2XStatusArray(){
		m_bSingle = TRUE;
	}

	CG2XStatusArray(unsigned int nItemCount, ValueType defaultValue, bool bSingleTRUE=true){
		_VItem vm;
		vm.status = G2XStatusFALSE;
		vm.vtValue = defaultValue;
		m_items.resize(nItemCount, vm);
		SetStatusStyle(TRUE);
	}

	// 设置状态风格
	// bSingleTRUE: [TRUE] 状态数组中最多只有一项的状态值为G2XStatusTRUE
	// bSingleTRUE: [FALSE] 状态数组中可以有多个项的状态值为G2XStatusTRUE
	// 注: 如果是第一种风格，那么调用SetStatus(nAt2,TRUE)时，如果nAt1处的状态为TRUE,
	//       那么nAt1处的状态自动被设成FALSE
	void SetStatusStyle(bool bSingleTRUE = true)
	{
		m_bSingle = bSingleTRUE;
	}

	void AddValue(const ValueType& val, unsigned char nStatus)
	{
		_VItem vi;
		vi.status = G2XStatusFALSE;
		vi.vtValue = val;
		m_items.push_back(vi);
		SetStatus(GetCount()-1, nStatus);
	}

	void InsertValue(int nIndex, const ValueType& val, unsigned char nStatus)
	{
		_VItem vi;
		vi.status = G2XStatusFALSE;
		vi.vtValue = val;
		m_items.insert(m_items.begin() + nIndex, vi);
		SetStatus(nIndex, nStatus);
	}
	void SetValue(int nIndex, const ValueType& val)
	{
		m_items[nIndex].vtValue = val;
	}
	
	int GetCount(){
		return (int)m_items.size();
	}

	// 设置指定位置处的状态, 并返回设置状态前的状态
	unsigned char SetStatus(int nAt, unsigned char status)
	{
		if (nAt >= GetCount())
		{
			throw _T("array overflow!");
			return G2XStatusFALSE;
		}
		if (m_bSingle)
		{
			int nOld = GetStatusTRUE();
			if (nOld == -1)
			{
				BYTE oldS = m_items.at(nAt).status;
				m_items.at(nAt).status = status;
				return oldS;
			}
			else
			{
				m_items.at(nOld).status = G2XStatusFALSE;
				BYTE oldS = m_items.at(nAt).status;
				m_items.at(nAt).status = status;
				return oldS;
			}
		}
		else
		{
			BYTE oldS = m_items.at(nAt).status;
			m_items.at(nAt).status = status;
			return oldS;
		}
	}

	// 得到指定位置处的状态 
	unsigned char GetStatus(int nAt)
	{
		return m_items.at(nAt).status;
	}

	// 得到状态为G2XStatusTRUE的项的位置,没有返回-1
	int GetStatusTRUE()
	{
		int nAt = -1;
		for (int i=0; i < (int)m_items.size(); i++)
		{
			if (m_items[i].status == G2XStatusTRUE)
			{
				nAt = i;
				break;
			}
		}
		return nAt;
	}

	// 得到所有状态为G2XStatusTRUE的位置列表, 没有返回0，否则返回个数
	int GetStatusTRUE1(std::vector<int>* res)
	{
		if (res)
			res->clear();

		int nCount = 0;
		for(int i = 0; i < (int)m_items.size(); i++)
		{
			if (GetStatus(i) == G2XStatusTRUE)
			{
				nCount++;
				if (res)
				{
					res->push_back(i);
				}
			}
		}
		return nCount;
	}

	// 得到所有状态为G2XStatusTRUE的对象列表, 没有返回0，否则返回个数
	int GetStatusTRUE2(std::vector<ValueType>* res)
	{
		if (res)
			res->clear();

		int nCount = 0;
		for(int i = 0; i < (int)m_items.size(); i++)
		{
			if (GetStatus(i) == G2XStatusTRUE)
			{
				nCount++;
				if (res)
				{
					res->push_back(GetValue(i));
				}
			}
		}
		return nCount;
	}

	ValueType GetValue(int nIndex)
	{
		return m_items.at(nIndex).vtValue;
	}
	unsigned char operator[](int nIndex)
	{
		return GetStatus(nIndex);
	}
	void Swap(int index1, int index2)
	{
		_VItem viTmp = m_items[index1];
		m_items[index1] = m_items[index2];
		m_items[index2] = viTmp;
	}

	// 反选，SetStatusStyle设置的是单选，该方法调用无效
	void Inverse()
	{
		if (m_bSingle)
			return;

		for (int i = 0; i < GetCount(); i++)
		{
			if (GetStatus(i) == G2XStatusTRUE)
				SetStatus(i, G2XStatusFALSE);
			else
				SetStatus(i, G2XStatusTRUE);
		}
	}
	// 全选，SetStatusStyle设置的是单选，该方法调用无效
	void SelectAll()
	{
		if (m_bSingle)
			return;

		for (int i = 0; i < GetCount(); i++)
		{
			SetStatus(i, G2XStatusTRUE);
		}
	}

	// 重置
	void Reset(int index)
	{
		SetStatus(index, G2XStatusFALSE);
	}

	// 设置
	void Set(int index)
	{
		SetStatus(index, G2XStatusTRUE);
	}

	// 重置所有
	void ResetAll()
	{
		for (int i = 0; i < GetCount(); i++)
		{
			m_items[i].status = G2XStatusFALSE;
		}
	}

	// 移除某项
	void Remove(int index)
	{
		m_items.erase(m_items.begin() + index);
	}

	// 移除所有值为value的项
	void Remove2(const ValueType& val)
	{
		int nIndex = 0;
		std::vector<_VItem>::iterator it = m_items.begin();
		while(it != m_items.end())
		{
			if (it->vtValue == val)
			{
				m_items.erase(it);
			}
			else
			{
				nIndex ++;	
			}
			it = m_items.begin() + nIndex;
		}
	}

	void Clear()
	{
		m_items.clear();
	}

protected:
	std::vector<_VItem> m_items;
	bool m_bSingle;
};

}// end namespace