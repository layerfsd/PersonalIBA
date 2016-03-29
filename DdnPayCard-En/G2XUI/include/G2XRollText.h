#pragma once
#include "g2xcontrol.h"

G2XUI_BEGIN

class G2X_CLASSES CG2XRollText :
	public CG2XControl
{
	DECLARE_DYNCREATE(CG2XRollText)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XRollText(void);
	virtual ~CG2XRollText(void);

	static const INT DEFAULT_LINE_SPACE=5;       // 默认行间距像素
	static const INT DEFAULT_COLUMN_SPACE = 50;  // 默认列间距像素

	void Create(
		CG2XWnd* pParent,
		CRect rcInParent, 
		UINT nID,
		G2XRollTextStyle style=G2XRollTextStyleTop);

	// 设置行间距, 只对滚动风格G2XRollTextStyleTop,G2XRollTextStyleBottom有效
	void SetLineSpace(INT nLineSpace);

	// 设置列间距, 只对滚动风格G2XRollTextStyleLeft,G2XRollTextStyleRight有效
	void SetColumnSpace(INT nColumnSpace);

	// 鼠标划过时，是否停止滚动
	void SetHoverStop(BOOL bHoverStop);

	G2XRollTextStyle GetStyle();

	// 向文字集合末尾添加复合字符串，复合字符串用tchSp分割成多个字符串
	void AppendComplexString(LPCTSTR,TCHAR tchSp=_T('\n'));

	// 向文字集合末尾添加字符串
	void AppendString(LPCTSTR);

	//
	void InsertString(LPCTSTR,INT nAt=0);

	void SetStartPosition(INT nPos);

	// 
	void Update();

protected:
	g_msg void OnCreate();
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnTimer(UINT nEvent);
	g_msg void OnMouseLeave();
	g_msg void OnSize();
	virtual void DrawControl(Graphics* pGraphics,G2XControlState ctrlState);

protected:
	G2XRollTextStyle m_style;
	INT m_nLineSpance;   // 行间距
	INT m_nColumnSpance; // 列间距
	BOOL m_bHoverStop;   // 是否允许鼠标划过时停止滚动
	BOOL m_bIsStop;      // 是否为停止状态 
	INT m_nOffSet;
	INT m_nStartPos;
	Bitmap *m_pBmp;
	std::vector<CString> m_strArray;

};

G2X_END
