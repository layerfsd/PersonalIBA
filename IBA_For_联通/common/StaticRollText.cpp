#include "StdAfx.h"
#include "StaticRollText.h"
#include ".\staticrolltext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace NS_COMMON;
// CStaticRollText

IMPLEMENT_DYNAMIC(CStaticRollText, CStatic)
CStaticRollText::CStaticRollText() : 
	m_nGroupSize(1),
	m_eStyle(eRoll_BottomToTop),
	m_nBorderWidth(1),
	m_clrBorder(RGB(255,255,255)/*RGB(214,226,255)*/),
	m_clrTxt(RGB(80,80,80)),
	m_dwStayTime(5000),
	m_dwEffectTime(200)
{
	m_nTransitionPercentage = 0;
	m_bTransitionEffect = FALSE;
	m_pFont = NULL;
}

CStaticRollText::~CStaticRollText()
{
	
}


BEGIN_MESSAGE_MAP(CStaticRollText, CStatic)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CStaticRollText::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CStaticRollText::OnPaint()
{
	CPaintDC dc(this); 

	OnDraw(&dc);
}

void CStaticRollText::OnDraw(CDC*pDC)
{
	CRect rc;
	GetClientRect(rc);

	if (m_nBorderWidth!=0)
	{
		DrawBorder(pDC,rc);
		rc.DeflateRect(m_nBorderWidth,m_nBorderWidth,m_nBorderWidth,m_nBorderWidth);
	}

	DrawBackgroud(pDC,rc);


	CString str = GetGroupText(m_vecCurrent); 
	if (str.IsEmpty())
	{
		return;
	}

	int nMarginL = GetMarginLeftRight();
	int nMarginT = GetMarginTopBottom();

	rc.DeflateRect(nMarginL,nMarginT,nMarginL,nMarginT);

	DrawText(pDC,rc,str);

	//KillTimer(EVENT_TRANSITION);
}

void CStaticRollText::EnableMouseStop(BOOL bEnable)
{
	m_bMouseStop = bEnable;
}

void CStaticRollText::AddString(CString str,BOOL bFront/*=FALSE*/)
{
	if (bFront)
	{
		m_queueTexts.push_front(str);
	}
	else
	{
		m_queueTexts.push_back(str);
	}
	
	UpdateUI();
}

void CStaticRollText::ClearAll()
{
	m_queueTexts.clear();
	m_vecCurrent.clear();
	m_bTransitionEffect = FALSE;
	m_nTransitionPercentage = 0;

	UpdateUI();
}

void CStaticRollText::UpdateUI()
{
	Invalidate();
}
// CStaticRollText 消息处理程序


void NS_COMMON::CStaticRollText::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case EVENT_ROLL:
		{
			MoveNextShow();
		}
		break;
	case EVENT_TRANSITION:
		{
			UpdateUI();
		}
		break;
	}
	CStatic::OnTimer(nIDEvent);
}

void CStaticRollText::MoveNextShow()
{
	if (m_queueTexts.size() == 0)
	{
		return;
	}

	for (vector<CString>::iterator it=m_vecCurrent.begin(); it != m_vecCurrent.end() ;
		++it )
	{
		m_queueTexts.push_back(*it);
	}

	m_strPre = GetGroupText(m_vecCurrent);

	m_vecCurrent.clear();

	for (int i = 0; i < m_nGroupSize && m_queueTexts.size() != 0; i++)
	{
		m_vecCurrent.push_back( m_queueTexts.front() );
		m_queueTexts.pop_front();
	}

	if (!m_strPre.IsEmpty())
	{
		m_bTransitionEffect = TRUE;
		SetTimer(EVENT_TRANSITION,200,0);
	}
	

	UpdateUI();
	
}

void CStaticRollText::StartRolling(DWORD dwStayTime,int nDefaultGroup/*=0*/,eRollStyle RollStyle)
{
	m_dwStayTime = dwStayTime;
	ASSERT(m_dwStayTime>1000); // 时间至少要大于1S,否则滚动太快
	
	if (m_dwStayTime == 0)
	{
		m_dwStayTime = 5*1000; // 5s
	}

	if (nDefaultGroup<0)
	{
		nDefaultGroup = 0;
	}

	int nGroups = (int)m_queueTexts.size();
	ASSERT(nGroups!=0);
	ASSERT(nDefaultGroup<nGroups);

	m_eStyle = RollStyle;

	MoveNextShow();

	SetTimer(EVENT_ROLL, m_dwStayTime,0);
}


void CStaticRollText::StopRolling()
{
	m_bTransitionEffect = FALSE;
	m_nTransitionPercentage = 0;
	UpdateUI();
	KillTimer(EVENT_ROLL);
}

void CStaticRollText::SetText(CString str,const TCHAR ch/*=_T('\n')*/)
{
	CString strSub;
	int iFrom = 0;
	int iRes = str.Find(ch, iFrom);

	while( iRes!=-1 )
	{
		strSub = str.Mid(iFrom,iRes-iFrom);
		
		iFrom = iRes+1;
		iRes = str.Find(ch, iFrom);

		m_queueTexts.push_back(strSub);
	}
}

CString CStaticRollText::GetGroupText(vector<CString>& vecStrings)
{
	CString str;
	for ( vector<CString>::iterator it = m_vecCurrent.begin();
		it != m_vecCurrent.end() ; ++it )
	{
		str += *it;
		str += _T("     ");
	}
	str.TrimRight();

	return str;
}


void CStaticRollText::DrawBackgroud(CDC* pDC,LPRECT rc)
{
	pDC->FillSolidRect(rc,RGB(255,255,255));
}

void CStaticRollText::DrawBorder(CDC* pDC,LPRECT rc)
{
	CBrush br;
	br.CreateSolidBrush(m_clrBorder);
	CRect rect(rc);

	for(int i = 0 ; i < m_nBorderWidth ; i++)
	{
		pDC->FrameRect(rect,&br);
		rect.DeflateRect(1,1,1,1);
	}
}
void CStaticRollText::DrawText(CDC* pDC,LPRECT lpRect,CString str)
{
	pDC->SelectObject(m_pFont);

	CRect rc(lpRect);
	CRect rcTxtSize(0,0,0,0);

	pDC->DrawText(str,rcTxtSize,DT_CALCRECT);	
	int nPanding = (rc.Height()-rcTxtSize.Height())/2;
	rc.top = rc.top + nPanding;
	rc.bottom = rc.bottom - nPanding;

	if (!m_bTransitionEffect || m_strPre.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrTxt);
		pDC->DrawText(str,rc,DT_LEFT);
	}
	else
	{
		CString strNext = GetGroupText(m_vecCurrent);
		DrawTransitionEffects(pDC,rc,m_strPre,strNext,m_nTransitionPercentage);
	}

}
void CStaticRollText::DrawTransitionEffects(CDC* pDC,LPRECT lpRect,CString strPre,CString strNext,int& nPercentage)
{
	CRect rc(lpRect);

	CRect rcTextSize(0,0,0,0);
	CRect rcPreSize(0,0,0,0);

	CString strTxt;

#define TB_GAP _T("\n")
#define LR_GAP _T("    ")

	// 合并字符串,便于产生过渡效果
	switch(m_eStyle)
	{
	case eRoll_BottomToTop:
		strTxt = strPre + TB_GAP +strNext;
		break;
	case eRoll_TopToBottom:
		strTxt = strNext + TB_GAP +strPre;
		break;
	case eRoll_LeftToRight:
		strTxt = strNext + LR_GAP +strPre;
		break;
	case eRoll_RightToLeft:
		strTxt = strPre + LR_GAP +strNext;
		break;
	default:
		ASSERT(0);
		break;
	}

	

	pDC->DrawText(strTxt, rcTextSize, DT_CALCRECT);
	pDC->DrawText(strPre, rcPreSize, DT_CALCRECT);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;

	bmp.CreateCompatibleBitmap(pDC,rcTextSize.Width(),rcTextSize.Height());
	memDC.SelectObject(&bmp);

	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(m_clrTxt);
	memDC.SelectObject(m_pFont);
	memDC.FillSolidRect(rcTextSize,COLOR_MASK);
	memDC.DrawText(strTxt,rcTextSize,DT_LEFT);

	if (m_eStyle == eRoll_TopToBottom)
	{
		int nYGap = 0;
		if (nPercentage == 100)
		{
			nYGap = 0;
		}
		else
		{
			nYGap = int(((100-nPercentage)/100.0)*double(rcTextSize.Height()/2));
		}

		CRect rcAt = rc;
		rcAt.top = rc.top + (rc.Height()-rcPreSize.Height())/2;
		rcAt.bottom = rc.bottom - (rc.Height()-rcPreSize.Height())/2;
		pDC->TransparentBlt(rc.left,rc.top,
			rcTextSize.Width(),rcPreSize.Height(),&memDC,0,nYGap,
			rcTextSize.Width(),rcPreSize.Height(),
			COLOR_MASK);
	}
	else if (m_eStyle == eRoll_BottomToTop)
	{
		int nYGap = 0;
		if (nPercentage == 100)
		{
			nYGap = rcTextSize.Height()/2;
		}
		else
		{
			nYGap = int((nPercentage/100.0)*double(rcTextSize.Height()/2));
		}

		CRect rcAt = rc;
		rcAt.top = rc.top + (rc.Height()-rcPreSize.Height())/2;
		rcAt.bottom = rc.bottom - (rc.Height()-rcPreSize.Height())/2;
		pDC->TransparentBlt(rc.left,rc.top,
			rcTextSize.Width(),rcPreSize.Height(),&memDC,0,nYGap,
			rcTextSize.Width(),rcPreSize.Height(),
			COLOR_MASK);
	}
	else if (m_eStyle == eRoll_RightToLeft)
	{
		CRect rcSpace(0,0,0,0);
		pDC->DrawText(LR_GAP, rcSpace, DT_CALCRECT);

		int nXGap = 0;
		if (nPercentage == 100)
		{
			nXGap = 0;
		}
		else
		{
			nXGap = int((nPercentage/100.0)*double(rcPreSize.Width()+rcSpace.Width()));
		}

		pDC->TransparentBlt(rc.left,rc.top,
			rcTextSize.Width()-nXGap,rcPreSize.Height(),&memDC,nXGap,0,
			rcTextSize.Width()-nXGap,rcPreSize.Height(),
			COLOR_MASK);
	}
	else if (m_eStyle == eRoll_LeftToRight)
	{
		CRect rcSpace(0,0,0,0);
		pDC->DrawText(LR_GAP, rcSpace, DT_CALCRECT);

		int nXGap = 0;
		if (nPercentage == 100)
		{
			nXGap = rcTextSize.Width()-rcPreSize.Width();
			//pDC->DrawText(strNext,rc, DT_LEFT);
		}
		else
		{
			nXGap = int((nPercentage/100.0)*double(rcTextSize.Width()-rcPreSize.Width()));
		}
		int nNextWidth = rcTextSize.Width()-rcPreSize.Width();
		int nDestWidth = rcPreSize.Width()+nXGap;
		if (nDestWidth > nNextWidth)
		{
			nDestWidth = nNextWidth;
		}
		pDC->TransparentBlt(rc.left,rc.top,
			nDestWidth,rcPreSize.Height(),&memDC,nNextWidth-nXGap,0,
			nDestWidth,rcPreSize.Height(),
			COLOR_MASK);
		
	}


	if (nPercentage>=100)
	{
		KillTimer(EVENT_TRANSITION);
		m_bTransitionEffect = FALSE;
		m_nTransitionPercentage = 0;
		nPercentage = 0;
	}
	nPercentage += 20;
	memDC.DeleteDC();

}


void CStaticRollText::SetBorderWidth(const int nWeight)
{
	if (nWeight<=0)
	{
		m_nBorderWidth = 0;
	}
	else
	{
		m_nBorderWidth = nWeight;
	}
}

int CStaticRollText::GetStringCount()
{
	int iCount = (int)m_vecCurrent.size();
	iCount += (int)m_queueTexts.size();
	return iCount;
}


