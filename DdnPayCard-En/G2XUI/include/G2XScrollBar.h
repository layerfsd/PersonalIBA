#pragma once
#include "G2XWnd.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XScrollBar :
	public CG2XControl
{
	DECLARE_DYNCREATE(CG2XScrollBar)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XScrollBar(void);
	virtual ~CG2XScrollBar(void);

	BOOL Create(CG2XWnd* pParent, BOOL bHorizontal, CRect rcInParent, UINT nID);
protected:
	//virtual void DrawSelf(Graphics* pGraphics);
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

	// 绘制滚动条左边的按钮或者顶部的按钮
	virtual void DrawLeftButton(Graphics* pGraphics,LPRECT lpRect,G2XControlState ctrlState);

	// 绘制滚动条右边的按钮或者底部的按钮
	virtual void DrawRightButton(Graphics* pGraphics,LPRECT lpRect,G2XControlState ctrlState);

	// 绘制滑块
	virtual void DrawSlider(Graphics* pGraphics,LPRECT lpRect,G2XControlState ctrlState);

	// 绘制背景边框
	virtual void DrawBlackground(Graphics* pGraphics,LPRECT lpRect,G2XControlState ctrlState);

protected:
	g_msg void OnCreate();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseHover(UINT nFlags, CPoint point);
	void OnMouseLeave();

	g_msg void OnDragging(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnDragged(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnSize();
	g_msg void OnTimer(UINT nEvent);
protected:
	RECT GetSliderRect();
	BOOL TestPointInButton(CPoint);
	virtual void OnLeftButtonClick();
	virtual void OnRightButtonClick();

	void RelayOut();

public:
	void SetRange(INT nFrom, INT nEnd, INT nPage);
	void SetPosition(INT nPos);
	INT GetPosition();

	void SetIncreasement(INT nInc);
	INT GetRangeDown();
	INT GetRangeUp();
	INT GetPageSize();

protected:
	INT GetPositonLong(INT nPos);
	INT GetLongPositionOff(INT nLong);
	void ChangePosAndNotify(INT nPos);

protected:
	INT m_nPosition;  // 滑块的当前位置
	INT m_nRangeDown; // 滚动条的范围下限
	INT m_nRangeUp;   // 滚动条的范围上限
	INT m_nPageSize;  // 页的大小
	BOOL m_bHorizontal; // 是否为水平滚动条
	BOOL m_bMouseDown;

	CPoint m_ptMouseDown;
	INT m_nMouseIn;

	BOOL m_bDraggingSlider;

	INT m_nPosDraggingFrom;

	CRect m_rcBtn1;
	CRect m_rcBtn2;
	CRect m_rcSliderArea; // 滑动区域
	CSize m_szSlider;     // 滑块的大小

	INT m_nIncMent; // 每点一下三角按钮，滑块一移动的位置大小。
};

G2X_END
