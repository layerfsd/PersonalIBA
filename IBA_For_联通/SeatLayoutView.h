#pragma once


#include "SeatList.h"
#include "Seat.h"

// CSeatLayoutView 视图
const size_t DefaultBitmapSize = 2000;
const INT_PTR DefaultRadius = 12;

enum SEAT_COMMAND
{
	M_SEAT_LOCK,
	M_SEAT_ADD,
	M_SEAT_SAVE,
	M_SEAT_LOAD,
	M_SEAT_COMPUTER_MENU    // 2011/10/21-8208-gxx: 
};

class CSeatLayoutView : public CScrollView
{
	DECLARE_DYNCREATE(CSeatLayoutView)
	DECLARE_MESSAGE_MAP()

protected:
	CSeatLayoutView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSeatLayoutView();

public:
	virtual void OnInitialUpdate();

protected:

	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMenuScrollzoom();
	afx_msg void OnUpdateMenuScrollzoom(CCmdUI *pCmdUI);
	afx_msg void OnMenuFullzoom();
	afx_msg void OnUpdateMenuFullzoom(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSeatLockseat();
	afx_msg void OnUpdateSeatLockseat(CCmdUI *pCmdUI);
	afx_msg void OnSeatAddseat();
	afx_msg void OnUpdateSeatAddseat(CCmdUI *pCmdUI);
	afx_msg void OnSeatLoadseat();
	afx_msg void OnUpdateSeatLoadseat(CCmdUI *pCmdUI);
	afx_msg void OnSeatDel();
	afx_msg void OnSeatRefresh();
	afx_msg void OnUpdateSeatDel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSeatRefresh(CCmdUI *pCmdUI);
	afx_msg void OnClockwise();
	afx_msg void OnUpdateClockwise(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg void OnSeatComputerMenu();

private:
	//提示非模态窗体
	CPPToolTip m_ToolTip;
	//保存移动元素的标号
	INT m_nIndex;
	//是否是移动座位
	BOOL m_bMoveRect;
	//命令模式
	SEAT_COMMAND m_CommandMode;
	//座位图
	CBitmap * m_pSeatBitmap;
	//座位信息列表
	CSeatList * m_pSeatList;
	// 是否整体显示
	BOOL m_bTotalView;
	//滚动范围
	CSize m_sizeTotal;
	//鼠标按下的坐标
	CPoint m_ptPrePoint;

private:
	// 载入座位图，用成员变量接收对象
	void LoadSeatBitmap(CBitmap * pSeatBitmap);
	// 显示区域设置
	void SetViewSize(BOOL bViewType, BOOL bDefault = FALSE);
	// 绘画座位正方形,参数里包含X.Y方向的放缩系数
	void DrawSeat(CDC * pDC = NULL,  double XRatio = 1.0, double YRatio = 1.0);
	// 把整体显示模式下的坐标转换成滚动模式下的坐标
	BOOL ToScrollRect(CRect& srcFullRect);
	// 转换成正常模式下的点坐标
	BOOL ToScrollPoint(CPoint& srcFullPoint);
	// 把正常滚动显示模式下的坐标转换成整体模式下的坐标
	BOOL ToTotalRect(CRect& srcTotalRect);
	// 转换成整体模式下的点坐标
	BOOL ToTotalPoint(CPoint& srcTotalPoint);
	// 往数据链添加座位数据
	void AddSeat(CPoint ptCurpoint, CString strTerminalID, CString strTerminalIP, INT_PTR nDirection = 0);

};


