#pragma once

G2XUI_BEGIN
G2X_FRAME

// 系统按钮信息结构
struct G2XSysBtnInfo
{
	INT nBtnWidth;							// 按钮宽度
	UINT nID;								// 按钮ID
	BOOL bEnabled;							// 按钮的启用状态
	CString strTip;							// 提示文本
	INT iImageArray[G2XControlStateCount];	// 控件所有状态下的图像索引
	CColor clrArray1[G2XControlStateCount];	// 控件所有状态下的按钮上边缘的颜色
	CColor clrArray2[G2XControlStateCount];	// 控件所有状态下的按钮下边缘的颜色
};

#define ZEOR_G2XSYSBTNINFO(info) \
	info.nBtnWidth=0;\
	info.nID = 0;\
	info.bEnabled = TRUE;\
	info.strTip = _T("");\
	for (int i=0; i < 4; i++) info.iImageArray[i] = -1;\
	info.clrArray1[G2XControlStateNormal]  = CColor(RGB(80,158,218));\
	info.clrArray1[G2XControlStateHover]   = CColor(RGB(158,202,234));\
	info.clrArray1[G2XControlStateSinkage] = CColor(RGB(158,202,234));\
	info.clrArray1[G2XControlStateDisable] = CColor(RGB(210,210,210));\
	info.clrArray2[G2XControlStateNormal]  = CColor(RGB(80,158,218),0);\
	info.clrArray2[G2XControlStateHover]   = CColor(RGB(158,202,234),0);\
	info.clrArray2[G2XControlStateSinkage] = CColor(RGB(158,202,234),0);\
	info.clrArray2[G2XControlStateDisable] = CColor(RGB(210,210,210),0);

class G2X_CLASSES CG2XSysBtnBar : public CG2XControl
{
	DECLARE_DYNCREATE(CG2XSysBtnBar)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XSysBtnBar(void);
	virtual ~CG2XSysBtnBar(void);

	BOOL Create(CG2XWnd* pParent, LPCRECT lpRect, G2XSysBtnInfo* pBtnInfos, INT nCount);

	void SetStyle(G2XSysBtnBarStyle style/*=G2XSysBtnBarStyleQQ2012*/, BOOL bRedraw=TRUE);

protected:
	virtual void DrawControl(Graphics *pGraphics,G2XControlState ctrlState);

	virtual void DrawSysBtn(Graphics *pGraphics, LPCRECT lpRect, G2XControlState btnState, UINT nID);

	virtual void DrawCustomBK(Graphics *pGraphics, LPCRECT lpRect, G2XControlState btnState);
	virtual void DrawCustomButton(Graphics *pGraphics, LPCRECT lpRect, const G2XSysBtnInfo& btnInfo, G2XControlState btnState);

	g_msg void OnCreate();
	g_msg void OnLButtonUp(UINT nFlags, CPoint point);
	g_msg void OnMouseHover(UINT nFlags, CPoint point);
	g_msg void OnMouseLeave();

public:
	// 系统按钮条中某个ID是否存在
	BOOL ExistID(UINT nID);

	// 擦除按钮
	BOOL EraseID(UINT nID);

	// 按钮使能或不能
	BOOL EnableID(UINT nID, BOOL bEnable);

	void InsertBefore(UINT nID, const G2XSysBtnInfo& btnInfo);
	void InsertAfter(UINT nNewID, const G2XSysBtnInfo& btnInfo);

	G2XSysBtnInfo* GetButtonInfo(UINT nID);

	BOOL SetImageList(const CG2XImageList& imgList);

public:
	std::vector<G2XSysBtnInfo> m_infoBtns;
	CG2XImageList m_imgList;

	BOOL m_bMaxiszed;
	UINT m_nHoverID;
	CColor m_clrSplitLine;  // 分割线的颜色

	G2XSysBtnBarStyle m_style; // 按钮的风格
};

G2X_END
G2X_END
