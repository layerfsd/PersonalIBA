#pragma once

G2XUI_BEGIN

//========================================
// CG2XProgressCtrl
//    进度控件, 进度条的进度范围0~100
//    可为进度条指定总步骤数
// 注：
//    
//    所有从CG2XWnd类继承的子类对象必须从堆中分配，
//    不要对CG2XWnd对象使用delete 关键字，
//    CG2XWnd对象的销毁需要调用 DestroyWnd方法
//========================================
class G2X_CLASSES CG2XProgressCtrl : public CG2XWnd
{
	DECLARE_DYNCREATE(CG2XProgressCtrl)
	DECLARE_GDI_MESSAGE_MAP()
public:
	CG2XProgressCtrl();
	virtual ~CG2XProgressCtrl();

	BOOL Create(CG2XWnd* pParent,G2XProgressStyle style,LPCRECT lpRect, UINT nID);

protected:
	virtual void DrawSelf(Gdiplus::Graphics *pGraphics);
	virtual void DrawBallProgress(
		Gdiplus::Graphics *pGraphics, 
		LPRECT lpWhere, 
		BYTE nBallCount,
		BYTE nBallPassed,
		BYTE nBallPassing);

	void DrawBall(Gdiplus::Graphics *pGraphics, CPoint ptCenter, INT nDistance, INT nRadius, float fAngle, Color& clr1, Color& clr2,INT nStatus);

	g_msg void OnCreate();
	g_msg void OnTimer(UINT nEvent);

public:
	// 设置总步数
	void SetStepCount(INT nCount);

	// 获取总步数
	INT GetStepCount();

	// 得到当前的进度半分比
	BYTE GetCurrentProgress();

	// 进度前进一步
	void StepIt();

	// 获取当前步数
	INT GetStep();

	// 设置当前步数
	void SetStep(INT nStep);

	void SetShowPercentage(BOOL bShow, BOOL bRepaint=TRUE);

protected:
	G2XProgressStyle m_style;
	BYTE m_nProgress; // 当前进度, 初始为0
	INT m_nStepCount; // 总步数
	INT m_nCurrentStep;   // 当前步骤
	UINT m_nTimerID;
	BYTE m_nBallCount; // 球的个数（G2XProgressStyleBalls风格）
	INT m_nHoverBall;
	BOOL m_bShowPercentage; // 是否显示百分比
};

G2X_END