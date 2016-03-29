
#pragma once

#include "ComputerList.h"

class CIBAGrid : public CBCGPGridCtrl
{
	DECLARE_MESSAGE_MAP()

public:

	CIBAGrid();
	virtual ~CIBAGrid();

protected:

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	//2011-3-29-gxx-比较金额字符串
	virtual int CompareMoneyString(CString left, CString right)const;

	virtual int CompareItems(const CBCGPGridItem* pItem1, const CBCGPGridItem* pItem2)const;
};

class CComputerGrid : public CIBAGrid
{
	DECLARE_MESSAGE_MAP()

public:

	CComputerGrid(CComputerList* lpComputerList);
	virtual ~CComputerGrid();

private : 

	static const UINT TimeIdOfUpdateTime = 1000;
	static const UINT IntervalOfUpdateTime = 60 * 1000;

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual int GetExtraHierarchyOffset()const{ return 3;}

public :
	
	/**
	* 刷新显示
	* @param bRedrawAll TRUE:重画所有信息，FALSE:只重画更新的信息
	*/
	void Refresh(BOOL bRedrawAll);

	CComputerList* const GetComputerList() const { return m_lpComputerList; }

private :
	/**
	* 自动按照当前的排序规则进行排序
	*/
	void AutoSort();

	/**
	* 初始化栏
	*/
	void InitColumnHead();

	/**
	* 检查是否符合显示条件，所有的显示条件判断都在这里
	*/
	BOOL CheckShowCondition(CComputerInfo &computer);
	
	/**
	* 检查是否符合区域显示条件
	*/
	BOOL CheckShowZone(CComputerInfo &computer);

	/**
	* 更新终端数据信息到行或者添加到新的行
	*/
	void UpdateComputerInfoToRow(DWORD_PTR dwData, CComputerInfo& computer);
	
	/**
	* 将终端数据信息写入到行
	*/
	void UpdateRowData(CBCGPGridRow* pRow, CComputerInfo& computer);

	/**
	* 更新用户级别颜色
	*/
	void UpdateRowUserType(CBCGPGridRow* pRow, CComputerInfo& computer);
	
	/**
	* 更新上机时间
	*/
	void UpdateAllComputersUseTime();
	
	/**
	* 根据终端状态获取文字显示颜色
	*/
	COLORREF GetTextColor(CComputerInfo& computer);
	
	/**
	* 更新所有终端信息到表格
	*/
	void RedrawAllComputers();

	/**
	* 只更新变化的终端信息到表格
	*/
	void RedrawUpdateComputers();

	/**
	* 保存用户信息到文件
	*/
	void SaveToFile();

private : 

	CFont m_TextFont;

	CComputerList* const m_lpComputerList;  //即指向CLocalServer::ComputerList

};

