#pragma once
#include <afxtempl.h>
#include <map>
#include <vector>

//-------------------------------------------------------------------------
// class G2X_CLASSES : CG2XWnd 基于Gdi++绘图的模拟窗口类的基类
//
// CG2XWnd 虚拟化窗口, 不是真正的窗口，没有窗口句柄，没有资源
//   实现主窗口无控件。就像QQ的聊天界面一样，只有主界面一个窗口。
//
// 注: 
//   1) G2XUI界面库中，所有的CG2XWnd控件都称作为虚拟窗口, CWnd窗口称作为真实窗口
//   2) 所有从CG2XWnd类继承的子类对象必须从堆中分配，真实窗口只需要销毁顶层的CG2XWnd
//      的DestroyWnd,就可以自动销毁所有的子虚拟窗口。
//   3) 不要对CG2XWnd对象使用delete 关键字, CG2XWnd对象占用的内存自动回收。
//-------------------------------------------------------------------------
#define g_msg 
#pragma warning(disable: 4244)

G2XUI_BEGIN

class G2X_CLASSES CG2XToolTipCtrl;

class G2X_CLASSES CG2XWnd : public CG2XObject
{
	DECLARE_DYNCREATE(CG2XWnd)

	DECLARE_GDI_MESSAGE_MAP()

public:
	CG2XWnd(void);
	virtual ~CG2XWnd(void);

	void DestroyWnd();

	//---------------------------------------------------------------------
	// 创建顶层的界面CG2XWnd窗口
	//		CWnd里的第一个CG2XWnd必须调用
	// pRealWnd: 真实窗口，必须是一个有效的窗口
	// bHighspeedRendering: 是否启用高速绘图
	//---------------------------------------------------------------------
	BOOL CreateSuperWnd(
		CWnd* pRealWnd, 
		LPCRECT lpRect, 
		UINT nID=0, 
		BOOL bHighspeedRendering=TRUE);

	//---------------------------------------------------------------------
	// 创建虚拟窗口
	//---------------------------------------------------------------------
	BOOL Create(
		CG2XWnd* pParent, 
		LPCTSTR lpWndText, 
		LPCRECT lpRect, 
		UINT nID,
		G2XBorderStyle borderStyle=G2XBorderNone);

	static CG2XWnd* FromHandle(HG2XWND hG2XWnd);

	HG2XWND GetHandle() { return m_hG2XWnd; }
	HG2XWND GetSafeHandle();

	CG2XToolTipCtrl* GetToolTipCtrl();
	BOOL ReplaceToolTipCtrl(CG2XToolTipCtrl* pNewToolTip);

	BOOL RemoveChild(HG2XWND hG2XWnd);
	BOOL RemoveChild(CG2XWnd*& pWnd);

	BOOL Replace(UINT nID, CG2XWnd* pWndNew, BOOL bCopyOldInfo=FALSE);
	BOOL Replace(CG2XWnd* pWnd, CG2XWnd* pWndNew, BOOL bCopyOldInfo=FALSE);
public:
	// 绘制响应(父窗口调用此方法)
	//- pFrontBmp: 和pGraphics绑定在一起的问题对象
	g_msg virtual void OnDraw(Graphics *pGraphics, Bitmap *pFrontBmp);  

	// 处理绘图动作，绘制自身，绘制所有子窗口
	virtual void DoDraw(Graphics *pGraphics, Bitmap *pFrontBmp);

	// 背景模式指定为非透明时自动调用,绘制背景,默认用指定的背景颜色填充
	virtual void DrawBackground(Graphics *pGraphics);
	
	
	// 绘制自身
	virtual void DrawSelf(Graphics *pGraphics) { }       
	virtual void DrawBorder(Graphics *pGraphics);

public:
	void Redraw(BOOL bDrawAll = FALSE);    

	BOOL PointIdWnd(CPoint point); // 实际窗口的点是否处于本虚拟窗口
	void EnableWnd(BOOL bEnable, BOOL bRepaint=TRUE);
	void ShowWnd(BOOL bShow);
	void MoveWnd(CRect rcNew, BOOL bRepaint=TRUE);
	BOOL GetWndShown() { return m_bShown; }

	CRect VirtualToReal(CRect rect); // 将本对象的某个区域转换为实际窗口中的位置
	void VirtualToReal(CPoint& point);
	void RealToVirtual(CPoint& point);
	CRect GetWndRect();  // 获取本对象在负对象中的位置
	void SetToolTip(BOOL bHasTip, LPCTSTR lpTip=NULL);
	
	BOOL IsEnabled() { return m_bEnable; }
	BOOL IsShown() { return m_bShown; }
	CWnd* GetSuperWnd();
	CG2XWnd* GetParentWnd();
	CG2XWnd* GetOwner();
	void SetOwner(CG2XWnd* pWnd);
	CG2XWnd* GetTopmostParentWnd();
	UINT GetControlID();

	CRect GetClientRect();
	void SetTransparent(bool bTransparent);
	BOOL GetTransparent() { return m_bTransparent; }
	void SetBKColor(const CColor& clr) { m_clrBk = clr; }
	void SetBkImage(const Image& img);

	CG2XWnd* GetWndByID(UINT nID);  // 获取ID对应的虚拟窗口,检索自身和自身的所有子虚拟窗口
	void SetWindowText(LPCTSTR strTxt, BOOL bRepaint=TRUE);
	CString GetWindowText();
	void SetFocus(BOOL bFocus);
	CG2XWnd* GetFocus();
	void EnableHoverOrLeave(BOOL bMouseHover, BOOL bMouseLeave, UINT nTimeElapse = 20);
	void SetTabStop();

	CFont* GetFont();
	void SetFont(CFont* pFont, BOOL bRepaint=TRUE);

	void SetCapture();     // 设置捕获，鼠标拖拽时有用，不需要时调用ReleaseCapture
	void ReleaseCapture(); // 释放捕获

	void EnableDragging(BOOL bEnable); // 是否允许拖拽，允许的话就会产生OnDragging, 和OnDragged;

	INT GetChildWndCount();

	void SetG2XCursor(HCURSOR hCursor);
	void SetToolTipDelayTime(DWORD dwMs);

	INT GetBorderBreadth() { return m_nBorderBreadth; }
	G2XBorderStyle GetBorderStyle() { return m_styleBorder; }

public:

	//---------------------------------------------------------------------
	//- 处理SuperWnd发送的所有消息
	//---------------------------------------------------------------------
	virtual BOOL PreTranslateMessage(MSG* pMsg); 

	//---------------------------------------------------------------------
	//- 给G2XWnd自身发送消息
	//---------------------------------------------------------------------
	BOOL SendMessage(UINT nMsg, UINT nGMsgType=GDIM_MESSAGE, WPARAM wParam=0, LPARAM lParam=0);

protected:

	//---------------------------------------------------------------------
	//- 把消息转移给所有的子虚拟窗口处理
	//---------------------------------------------------------------------
	BOOL TransferMessage(MSG* pMsg);


	//---------------------------------------------------------------------
	//- 处理所有的G2XWnd消息
	//- nGMsgType:  GDIM_MESSAGE , GDIM_MOUSE , GDIM_COMMAND , GDIM_NOTIFY
	//---------------------------------------------------------------------
	virtual BOOL OnGdiWndMsg(UINT nGMsgType, UINT nID, WPARAM wParam, LPARAM lParam, LRESULT* lpResult);

	//---------------------------------------------------------------------
	//- 处理G2XWnd的命令消息
	//---------------------------------------------------------------------
	virtual BOOL OnGdiCommand(UINT nID, WPARAM wParam, LPARAM lParam);

	//---------------------------------------------------------------------
	//- 处理G2XWnd的自定义消息
	//---------------------------------------------------------------------
	virtual BOOL OnGdiMessage(UINT nID, WPARAM wParam, LPARAM lParam, LONG* lpResult);

	//---------------------------------------------------------------------
	//- 处理G2XWnd的通知消息
	//- wParam 指向NMHDR结构
	//---------------------------------------------------------------------
	virtual BOOL OnGdiNotify(UINT nID, WPARAM wParam, LPARAM lParam, LONG* lpResult);

	virtual BOOL OnGdiKeyboard(UINT nID, WPARAM wParam, LPARAM lParam, LONG* lpResult);

	g_msg void OnCreate();
	g_msg void OnMouseMove(UINT nFlags, CPoint point);
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnLButtonUp(UINT nFlags, CPoint point);
	g_msg void OnRButtonDown(UINT nFlags, CPoint point);
	g_msg void OnRButtonUp(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	g_msg void OnLButtonDoubleClick(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave(); // 鼠标离开本对象
	g_msg void OnTimer(UINT nEvent);
	g_msg void OnDragging(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnDragged(UINT nFlag, CPoint ptFrom, CPoint point);
	g_msg void OnSize();
	g_msg void OnShowWnd();
	g_msg void OnDestroy();
	g_msg BOOL OnKeyDown(UINT,UINT,UINT);
protected:
	void SetTimer(UINT nEvent, UINT nTimeElapse=10);
	void KillTimer(UINT nEvent);
	
	// 给SuperWnd发送命令通知
	LRESULT SendNotify(UINT nCode);
	
	BOOL NextTabStop();              // 按Tab键时，是否能处理移动到下一个焦点

	UINT GetHoverTimeID();

	BOOL GetParentEnable();

	CG2XDrawImpl* GetDrawImpl();
	void CG2XWnd::CalcTextRect( LPCTSTR lpText,LPRECT lpRect,Gdiplus::Graphics *pGraphics );
protected:
	BOOL m_bEnable;         // 是否启用本控件
	BOOL m_bShown;          // 是否显示本控件
	BOOL m_bHasToolTip;     // 是否有文本提示
	BYTE  m_nBorderBreadth;  // 窗口边框的宽度,宽度为0表示没有边框
	G2XBorderStyle m_styleBorder; // 边框框的风格

	bool m_bTipShown;       
	CString m_strTip;       // 文本提示的内容
	UINT m_nTipDelayTime;   // 文本提示前的延迟时间   

	bool m_bTransparent;            // 本虚拟窗口的背景是否为透明, TRUE:背景色或者背景图像全部被忽略
	CColor m_clrBk;					// 本虚拟窗口的背景色,如果背景图像为非空，背景色被忽略
	Image* m_pBkImg;                // 背景图像
	BOOL m_bEnableHover;            // 是否启用鼠标划过事件
	BOOL m_bEnableLeave;            // 是否启用鼠标离开事件
	UINT m_nHoverTime;              // 鼠标划过的周期
	CPoint m_mousePoint;            // 鼠标移动在虚拟窗口上方的鼠标位置（相对于实际窗口）
	UINT m_nMouseFlags;             // ctrl,shift,alt等键按下的标志,参考WM_MOUSEMOVE
	BYTE m_nMouseInAndOut;
	
	BOOL m_bFocus;                  // 本虚拟窗口是否为焦点状态
	BOOL m_bTabStop;                // 是否启用TabStop
	CFont* m_pFont;
	BOOL m_bEnableDragging;  
	CPoint m_ptDraggingFrom; 
	
	BOOL m_bHighspeedDraw;			// 是否启用高度绘图

public:
	Bitmap* m_pFrontBk;				// 窗口的前景位图,窗口重绘时绘制到这个位图上,顶层窗口把它绘制到设备环境
	

	// 顶层虚拟窗口专用
private:
	typedef std::map<UINT, std::vector<HG2XWND> > MapTimerWnd;
	typedef std::vector<UINT> IDArray;

	MapTimerWnd m_mapTimerWnd;
	CG2XWnd* m_pFocus;
	CG2XWnd* m_pWndMouseIn;
	IDArray m_tabStops;
	CG2XWnd* m_pWndCaptured; // 当前设置了鼠标捕获的窗口

	CG2XToolTipCtrl* m_pToolTipCtrl;
	
	UINT m_nTabLastID;
	HCURSOR m_hCursor;	

public:
	static const UINT TIMERREDRAW = 0xFFFD;
	static const UINT TIMERHOVER = 0xFFFE;
	INT m_nRedrawRequestCount;
	typedef std::vector<HG2XWND> G2XWndArray;
	G2XWndArray m_arrayOnlyRedraw;
};

G2X_END
