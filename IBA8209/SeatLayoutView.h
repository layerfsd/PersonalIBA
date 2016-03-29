#pragma once


#include "SeatList.h"
#include "Seat.h"
#include "Seat2.h"

class CGdiPlusBitmap;
class CSeatList2;
// CSeatLayoutView 视图
const size_t DefaultBitmapSize = 2000;
//const INT_PTR DefaultRadius = 12;
const INT_PTR DefaultRadius = 24;
const INT_PTR DefaultRadius2 = 50;

enum SEAT_COMMAND
{
	M_SEAT_LOCK,			// 锁定座位菜单已经和机器菜单合并
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
	
	static inline void SetMaxRight(int nVal)		{ m_nMaxRight = nVal; }
	static inline void SetMaxBottom(int nVal)		{ m_nMaxBottom = nVal; }
	static void SetMaxRightCmp(int nVal);
	static void SetMaxBottomCmp(int nVal);
	static inline int GetMaxRight()				{ return m_nMaxRight; }
	static inline int GetMaxBottom()				{ return m_nMaxBottom; }
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
	afx_msg void OnUpdateSeatAddAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSeatDelAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSeatComputerMenu(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	afx_msg void OnSeatComputerMenu();
	afx_msg void OnSeatAddAll();
	afx_msg void OnSeatDelAll();
private:
	//提示非模态窗体
	CPPToolTip m_ToolTip;
	//保存移动元素的标号
	INT m_nIndex;
	//是否是移动座位
	BOOL m_bMoveRect;
	INT m_nIndex2;
	BOOL m_bMoveRect2;
	//命令模式
	SEAT_COMMAND m_CommandMode;
	//座位图
	//CBitmap * m_pSeatBitmap;
	CGdiPlusBitmap *m_pSeatBitmap;
	//座位状态提示图
	CBitmap * m_pSeatTipsBitmap;
	//座位信息列表
	CSeatList * m_pSeatList;

	// 主副收银台，WC
	CSeatList2 * m_pSeatList2;

	// 是否整体显示
	BOOL m_bTotalView;
	//滚动范围
	CSize m_sizeTotal;
	//鼠标按下的坐标
	CPoint m_ptPrePoint;
	CPoint m_ptPrePoint2;

	static int m_nMaxRight;				// 终端的最右边的坐标
	static int m_nMaxBottom;			// 终端的最低边的坐标
private:
	// 载入座位图，用成员变量接收对象
	void LoadSeatBitmap(CGdiPlusBitmap * pSeatBitmap);
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
	void AddSeat(CPoint ptCurpoint, CString strTerminalID, CString strTerminalIP, INT_PTR nDirection = 0, BOOL bRedraw = TRUE, bool bAddScrollPos = TRUE);
	
	struct TermInfo{
		CString strTermID;
		CString strIP;
		bool operator<(const TermInfo& rhs) const  
		{   
			CStringA strIPA(strIP), strRhsA(rhs.strIP);
			ULONG ulIPL = ntohl(inet_addr((PCSTR)strIPA.GetString()));
			ULONG ulIPR = ntohl(inet_addr((PCSTR)strRhsA));
			if (ulIPL != ulIPR)   
				return (ulIPL < ulIPR);   
			return ulIPL < ulIPR;   
		}   
	};
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT OnUpdateSeat(WPARAM wParam, LPARAM lParam);
	void Loadseat();
	void SaveSeat();
	afx_msg void OnAddCashier();
	afx_msg void OnAddToilet();
	afx_msg void OnAddRefrigraetor();
	afx_msg void OnAddEntrance();
	void AddSeat2(Seat2Type type);
	afx_msg void OnMyrefresh();
	afx_msg void OnUpdateMyRefresh(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewuser(CCmdUI *pCmdUI);
	afx_msg void OnPaint();

	void AutoAddAllSeatAfterSync();
	void UpdateSeatAfterSync();
};


