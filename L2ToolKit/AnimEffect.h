// Written by SiGMan / iO UpG  13-04-2000
// 
//

#pragma once


enum AnimType 
{
	AnimInit, AnimDraw, AnimErase, AnimTerm
};

struct AnimData
{
	BOOL	bOpen;
	RECT	rcScreen;
	RECT	rcWnd;
	int		iParameter;
	int		iAfterimages;
	int		iTotalSteps;
	int		iStep;
	POINT	ptRelRightTop;
	POINT	ptCenter;
	HWND	hWnd;
	HDC		hDC;
	enum	AnimType animType;
	BYTE	bBuffer[4000];
};


class L2_EXT_CLASS CAnimEffect  
{
public:
	enum EffectType {
		Random=-1,
		Spin=0,
		Vortex,
		ScatterGather,
		Spike,
		Fireworks
	};

	typedef BOOL (*EFFUNC)( AnimData * );

	CAnimEffect();
	virtual ~CAnimEffect();

	void Setup( int iSteps, int iAfterimages, int iDelay = 10);
	void Effect( EffectType effect );
	void Open( const CRect &rcWnd );
	void Close( const CRect &rcWnd );

private:
	void Defaults();
	void ChooseFunc();
	void Animate( AnimData &animData );

private:
	RECT	m_rcScreen;
	int		m_iParameter;
	int		m_iAfterimages;
	int		m_iTotalSteps;
	int		m_iDelay;
	EffectType	m_Effect;
	EFFUNC	m_Func;

	RECT	m_rcWnd;
	POINT	m_ptRelRightTop;
	POINT	m_ptCenter;

	HWND	m_hWnd;
	HDC		m_hDC;
	HPEN	m_hPen;
};

