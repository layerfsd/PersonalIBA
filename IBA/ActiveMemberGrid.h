#pragma once

#include "ColorGridCtrl.h"
#include "ActiveMemberList.h"

class CActiveMemberGrid : public CIBAGrid
{
	DECLARE_MESSAGE_MAP()

public:
	CActiveMemberGrid(void);
	~CActiveMemberGrid(void);

	/**
	* 刷新显示
	* @param bRedrawAll TRUE:重画所有信息，FALSE:只重画更新的信息
	*/
	void Refresh(BOOL bRedrawAll);

	CActiveMemberList* const GetActiveMemberList() const { return m_lpActiveMemberList; }

	// 2011/06/08-gxx: 改成了公有，并增加是否插入到列表首参数
	void AddNewRow(CActiveMember &ActiveMember, BOOL bAtFront=FALSE);
protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual int GetExtraHierarchyOffset()const{ return 3;}

private:

	void InitColumnHead();
	void UpdateRowData(CBCGPGridRow* pRow, CActiveMember& ActiveMember);
	void RedrawAllActiveMembers();
	void UpdateAllComputersUseTime();
	void RedrawUpdateActiveMembers();

protected:
	// 2011/06/08-gxx: 
	virtual int CompareItems(const CBCGPGridItem* pItem1, const CBCGPGridItem* pItem2)const;

private:

	CActiveMemberList* const m_lpActiveMemberList;
	
	static const UINT TimeIdOfUpdateTime = 1000;
	static const UINT IntervalOfUpdateTime = 60 * 1000;
	static const UINT TimeIdOfRefresh = 1001;
	static const UINT IntervalOfRefresh = 10 * 1000;

	static const UINT TimerIdOfBundleTimeTry = 1002;
	static const UINT IntervalOfBundleTimeTry = 10 * 1000;  // 10秒

	CFont m_TextFont;
	
	CImageList m_Images;

	// 2011/06/08-gxx: 保存上一次排序的列
	int m_nSortColumn;

};
