#pragma once
/**************************************************************************
class: CStaticRollText : public CStatic
for: 翻滚文字的文本显示控件, 可为控件设定要翻滚的字符串集合
using:
    设置或者添加要滚动的文字;(调用SetText 或者循环调用AddString)
    调用CStaticRollText::StartRolling开始滚动文字;

writer: guoxuxing
date: 2011-5-4
***************************************************************************/
// CStaticRollText
#include <queue>
#include <vector>
using namespace std;

namespace NS_COMMON{

class CStaticRollText : public CStatic
{
	DECLARE_DYNAMIC(CStaticRollText)

public:
	enum eRollStyle
	{
		eRoll_TopToBottom,  // 文字从上往下翻滚
		eRoll_BottomToTop,  // 文字从下往上翻滚
		eRoll_LeftToRight,  // 文字从左往右翻滚
		eRoll_RightToLeft   // 文字从右往左翻滚
	};

	static const UINT EVENT_ROLL = 1; 
	static const UINT EVENT_TRANSITION = 2;
	static const UINT COLOR_MASK = RGB(255,255,255);
	static const int MARGIN_LEFTRIGHT = 4;
	static const int MARGIN_TOPBOTTOM = 0;

public:
	CStaticRollText();
	virtual ~CStaticRollText();

public:
	void StartRolling(DWORD dwStayTime,
		int nDefaultGroup=0,
		eRollStyle RollStyle=eRoll_BottomToTop);

	void StopRolling();

	void EnableMouseStop(BOOL bEnable);					// 当鼠标移动到控件上时,是否让文字停止滚动

	void SetText(CString str,const TCHAR ch=_T('\n')); // 设置文字，各字符串默认用‘\n’分隔

	void AddString(CString str,BOOL bFront=FALSE);		// 向文字队列中添加文字,bFront决定是向前插，还是向后插

	void ClearAll();									// 清空所有文字

	int GetStringCount();
	
	void SetGroupSize(int nGroupSize) // 设置每一组文字显示多少个字符串
	{ 
		m_nGroupSize = nGroupSize; 
	}

	void SetBorderColor(COLORREF clr) { m_clrBorder = clr; }

	void SetBorderWidth(const int nWeight);

	void SetFont(CFont* pFont) { m_pFont = pFont; }

	void SetTxtColor(COLORREF clr) { m_clrTxt = clr; }

	
	void SetStayTime(DWORD dwTime) // 设置每一轮文字的保留时间 (需要在StartRolling函数之前调用)
	{ 
		m_dwStayTime = dwTime; 
	} 

	void SetEffectTime(DWORD dwTime) // 文字过渡效果的每帧间隔时间
	{ 
		m_dwEffectTime = dwTime; 
	}

protected:
	virtual int GetMarginLeftRight() { return MARGIN_LEFTRIGHT; }
	virtual int GetMarginTopBottom() { return MARGIN_TOPBOTTOM; }

protected:
	virtual void OnDraw(CDC*pDC);
	virtual void DrawBorder(CDC* pDC,LPRECT rclpRect);
	virtual void DrawBackgroud(CDC* pDC,LPRECT lpRect);
	virtual void DrawText(CDC* pDC,LPRECT lpRect,CString strTxt);
	virtual void DrawTransitionEffects(CDC* pDC,LPRECT lpRect,CString strPre,CString strNext,int& nPercentage);

	void UpdateUI();

	void MoveNextShow(); // 移动到下一组文字的显示

	CString GetGroupText(vector<CString>& vecStrings);

protected:
	eRollStyle m_eStyle;			// 文字滚动风格
	BOOL m_bMouseStop;				// 当鼠标移动到控件上时,是否让文字停止滚动

	deque<CString> m_queueTexts;	// 文字队列

	int m_nGroupSize;				// 每一组文字，字符串的个数,默认为1
	vector<CString> m_vecCurrent;	// 当前组文字的字符串集合

	DWORD m_dwStayTime;				// 每组文字的逗留时间,ms
	DWORD m_dwEffectTime;           // 文字过度时的每帧间隔时间,ms

	int m_nBorderWidth;				// 边框宽度
	COLORREF m_clrBorder;			// 边框颜色
	COLORREF m_clrTxt;              // 文字颜色

	int m_nTransitionPercentage;	// 文字过度百分比
	BOOL m_bTransitionEffect;		// 是否处于过渡状态
	CString m_strPre;				// 过渡前的文本

	CFont* m_pFont;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


}// end namespace