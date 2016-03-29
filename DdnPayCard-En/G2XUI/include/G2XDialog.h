#pragma once
#include <afxwin.h>
#include <wtypes.h>
// CG2XDialog dialog

G2XUI_BEGIN
G2X_FRAME

class G2X_CLASSES CG2XDialog : public CDialog
{
	DECLARE_DYNCREATE(CG2XDialog)

public:
	CG2XDialog();
	CG2XDialog(LPCTSTR lpszTemplateName, LPCTSTR lpszCaption, CWnd* pParent = NULL);
	CG2XDialog(UINT nIDTemplate, LPCTSTR lpszCaption, CWnd* pParent = NULL);

	virtual ~CG2XDialog();

	void SetG2XBaseWnd(CRuntimeClass* classG2XBase);
	void SetG2XCaptionbar(CRuntimeClass* classG2XCatipnbar);
	void SetG2XClientArea(CRuntimeClass* classG2XClientArea);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnInitDialog();

	// G2XUI库绘制前有擦除背景的机会,可重载此方法
	virtual void EraseBackground(CDC* pDC);

	virtual BOOL MFCToG2X(CWnd* pWnd, LPCTSTR lpszClassName, LPCRECT lpRect);
 
	// 显示 or 隐藏 标题栏
	void ShowCaptionBar(BOOL bShow=TRUE);
	void SetCaptionBarHeight(INT nHeight = 25);
	
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

public:
	void AddPlaceCtrl(CG2XWnd* pG2XWnd, G2XPanelPlace place);

	CG2XPanel* GetPanel();

	void AdjustG2XWnd();

	CG2XCaptionBar* GetCaptionBar();
	INT GetBorderWidth() { return m_nBorderWidth; }
	
	void DrawG2XUI(Graphics* pGraphics, Bitmap* pFrontBmp);

public:
	BOOL m_bHighspeedRendering;

private:
	CG2XWnd* m_pG2XWnd;
	

protected:
	CString m_strCaption;
	CG2XCaptionBar* m_pG2XCaption; // 标题栏
	CG2XPanel* m_pG2XPanel; // 客户区面板
	BOOL m_bHasBorder;
	INT m_nBorderWidth;

	CRuntimeClass* m_pClassBase;
	CRuntimeClass* m_pClassCaption;
	CRuntimeClass* m_pClassClientArea;
	virtual void CreateClassObjects();
	
	Bitmap* m_pFrontBmp;
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	
};

G2X_END
G2X_END
