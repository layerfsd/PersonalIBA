#pragma once

#include "ComputerInfo.h"

class CComputerList
{
public:

	class CStatInfo
	{
	public:

		friend class CComputerList;

		CStatInfo()
		{
			m_nOnline = 0;
			m_nLock = 0;
			m_nUnLock = 0;
			m_ManCount = 0;
			m_WomanCount = 0;
			m_nAllCount = 0;
			m_ThiefCount = 0;
			//会员用户数目
			m_nMemberUserCount = 0;
			//普通用户数目
			m_nCommonUserCount = 0;
			//包时用户
			m_nBundTimePayCount = 0;
			//普通计费用户
			m_nCommonPayCount = 0;
		}

		UINT GetOnline() const { return m_nOnline; }
		UINT GetLock() const { return m_nLock; }
		UINT GetUnLock() const { return m_nUnLock; }
		UINT GetManCount() const { return m_ManCount; }
		UINT GetWomanCount() const { return m_WomanCount; }
		UINT GetThiefCount() const { return m_ThiefCount; }
		UINT GetAllCount() const { return m_nAllCount; }
		//普通用户数目
		UINT GetCommonUserCount() const { return m_nCommonUserCount; }
		//会员用户数目
		UINT GetMemberUserCount() const { return m_nMemberUserCount; }
		//包时用户数目
		UINT GetBundTimePayCount() const { return m_nBundTimePayCount; }
		//普通计费用户
		UINT GetCommonPayCount() const { return m_nCommonPayCount; }

	private:

		UINT m_nAllCount;

		UINT m_nOnline;

		UINT m_nLock;

		UINT m_nUnLock;

		UINT m_ManCount;

		UINT m_WomanCount;

		UINT m_ThiefCount;
		//会员用户数目
		UINT m_nMemberUserCount;
		//普通用户数目
		UINT m_nCommonUserCount;
		//包时用户
		UINT m_nBundTimePayCount;
		//普通计费用户
		UINT m_nCommonPayCount;
	};

public:
	CComputerList(void);
	~CComputerList(void);

private :

	//!<存储全部终端电脑数据
	CArray<CComputerInfo, CComputerInfo&> m_ComputerArray;
	
	//!<存储更新的电脑索引，用于快速刷新界面显示
	CArray<INT_PTR, INT_PTR&> m_UpdateIndexArray;
	
	//!<存储关键字，用于准确的快速查找，程序内部使用
	std::map<CString, INT_PTR> m_ComputerKeyMap;

	//!<存储根据终端号排序的电脑索引，用于模糊查找
	CArray<INT_PTR, INT_PTR&> m_SortIndexArray;

public :

	BOOL AddComputer(CComputerInfo& computer);
	
	BOOL UpdateComputer(INT_PTR nIndex, CComputerInfo& computer);

	void Clear();

	INT_PTR GetCount();

	//!<精确查找
	INT_PTR Find(CString strTermID, CString strComputerIP);
	INT_PTR Find(UINT nMemberID);

	/**
	* 模糊查找
	* @param nKeyType 关键字类型
	* @param lpszKeyValue 关键字
	*/
	INT_PTR LookFor(UINT nKeyType, LPCTSTR lpszKeyValue);

	void MakeComputerKeyMap();

	/**
	* 获取电脑信息。
	* @param 数组索引
	*/
	CComputerInfo& GetComputer(INT_PTR nIndex);

	// 2011/06/20-gxx: 获取电脑信息
	BOOL GetComputerInfo(UINT nMemberID, CComputerInfo&);

	BOOL GetComputerInfo(CString strTermID, CComputerInfo&);

	/**
	* 获取最近更新的电脑信息，本函数只要是为了提高效率。
	* @return 小于0则表示失败
	*/
	INT_PTR GetUpdateComputer();
	
	/**
	* 判断是不是无效的索引。
	*/
	BOOL IsInvaildIndex(INT_PTR nIndex);

	/**
	* 获取统计信息
	*/
	BOOL GetStatInfo(CStatInfo & StatInfo);
	
	/**
	* 更新所有的电脑状态
	*/
	void UpdateAllComputerStatus();

	/**
	* 清除用户信息
	*/
	void ClearComputerUserInfo(UINT nMemberId);
};
