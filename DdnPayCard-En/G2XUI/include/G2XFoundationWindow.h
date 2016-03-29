#pragma once

G2XUI_BEGIN
G2X_FRAME
// CG2XFoundationWindow

class G2X_CLASSES CG2XFoundationWindow : public CWnd
{
	DECLARE_DYNAMIC(CG2XFoundationWindow)

public:
	CG2XFoundationWindow();
	virtual ~CG2XFoundationWindow();

	BOOL Create(const RECT& rect,  LPCTSTR lpTitle, CWnd* pParentWnd);

protected:
	DECLARE_MESSAGE_MAP()

public:
	void SetG2XBaseWnd(CRuntimeClass* classG2XBase);
	void SetG2XCaptionbar(CRuntimeClass* classG2XCatipnbar);
	void SetG2XClientArea(CRuntimeClass* classG2XClientArea);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 显示 or 隐藏 标题栏
	void ShowCaptionBar(BOOL bShow=TRUE);
	void SetCaptionBarHeight(INT nHeight = 25);

	void DrawWindow(Graphics* pGraphics, Bitmap* pFrontBmp);

	// 设置窗体的整体不透明度, 255完全不透明,0 表示完全透明
	void SetAlpha(BYTE nAlpha=255, BOOL bRepaint=TRUE);

protected:
	// 重载在函数可以重绘背景,重载后如果要使效果得到显示，需要把m_pG2XPanel背景设为透明
	virtual void EraseBackground(Graphics* pGraphics, LPCRECT lpRect){}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg LRESULT OnIMEChar( WPARAM wParam, LPARAM lParam );
	afx_msg void OnChar(UINT nChar ,UINT nRepCnt, UINT nFlags);

	virtual void OnInitialUpdate();

	virtual BOOL DoIMEChar(LPCTSTR lpcStr);

public:
	void AddPlaceCtrl(CG2XWnd* pG2XWnd, G2XPanelPlace place);
	void AdjustG2XWnd();

	CG2XWnd* GetTopG2XWnd() const { return m_pG2XWnd; }
	CG2XPanel* GetPanel() const { return m_pG2XPanel; }
	CG2XCaptionBar* GetCaptionBar() const { return m_pG2XCaption; }

	INT GetBorderWidth() { return m_nBorderWidth; }

protected:
	bool DrawLayerWindow();
	virtual void CreateG2XUIObjects();
	void AdjustChildWindow();
	BOOL TranslateG2XAccelator(MSG* pMsg);
protected:
	CG2XWnd* m_pG2XWnd;
	CString m_strCaption;
	CG2XCaptionBar* m_pG2XCaption; // 标题栏
	CG2XPanel* m_pG2XPanel; // 客户区面板
	BOOL m_bHasBorder;
	INT m_nBorderWidth;
	BOOL m_bRoundBorder;    // TRUE: 圆角边框, FALSE: 矩形边框
	SIZE m_szCorner;
	BYTE m_nAlpha;  // 窗口的整地不透明度
	CRuntimeClass* m_pClassBase;
	CRuntimeClass* m_pClassCaption;
	CRuntimeClass* m_pClassClientArea;
	Bitmap* m_pFrontBmp;
public:
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMove(int x, int y);
};

G2X_END
G2X_END

