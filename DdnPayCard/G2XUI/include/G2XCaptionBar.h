#pragma once

G2XUI_BEGIN
G2X_FRAME

class G2X_CLASSES CG2XCaptionBar :	public CG2XWnd
{
	DECLARE_DYNCREATE(CG2XCaptionBar)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XCaptionBar(void);
	virtual ~CG2XCaptionBar(void);

	BOOL Create(CG2XWnd* pParent, LPCTSTR lpCaption);

	void SetIcon(Image* pImage, COLORREF clrTransparent=RGB(255,255,255));

	void SetIcon(HICON hIcon);

	// 替换现有的系统按钮条
	void ReplaceSysBtnBar(CG2XSysBtnBar* pNewSysBar);

protected:
	g_msg void OnSize();
	g_msg void OnLButtonDown(UINT nFlags, CPoint point);
	g_msg void OnLButtonDClick(UINT nFlags, CPoint point);
	
	g_msg void OnMinimize();
	g_msg void OnMaximize();
	g_msg void OnRestore();
	g_msg void OnClose();

	virtual void DrawSelf(Gdiplus::Graphics *pGraphics);

	// 判断鼠标点对应的位置是否为活动, 如果为活动的,鼠标不允许拖动窗口
	virtual BOOL TestMouseActive(CPoint pt);

	virtual void DrawBackground(Graphics *pGraphics);

public:
	
	CG2XSysBtnBar* GetSysBtnBar();
	void SetTitleColor(const Color& clr);

	void SetCaptionHeight(INT nHeight);

	// 控制条的高度，如果为-1,那么它的高度和标题栏的高度一致
	void SetSysBtnBarHeight(INT nHeight=-1);

protected:
	CG2XSysBtnBar* m_pSysBtnBar; // 按钮控制条,默认有最小化，最大化，关闭
	INT m_nSysBtnBarHeight;      
	Image* m_pImage;        // 标题栏图标图像
	INT m_nCaptionHeight;   // 标题栏的高度
	INT m_nSysBtnCount;     
	CColor m_clrTitle;      // 标题文本的颜色

	G2XTextHAlign m_hTitleAlign;
	G2XTextVAlign m_vTitleAlign;
};

G2X_END
G2X_END
