#include "stdafx.h"
#include ".\identitytoolbarbutton.h"

IMPLEMENT_SERIAL(CIdentityToolbarButton,CBCGPToolbarCtrlCommon,1)

CIdentityToolbarButton::CIdentityToolbarButton(void)
{
}

CIdentityToolbarButton::CIdentityToolbarButton(UINT nID,CBCGPToolBar *pToolbar)
	:CBCGPToolbarCtrlCommon( nID, 
		CImageHash::GetImageOfCommand (nID, FALSE),
		emBCGP_static,pToolbar,SS_OWNERDRAW,DEFAULT_WIDTH)
{

}

CIdentityToolbarButton::~CIdentityToolbarButton(void)
{
}

void CIdentityToolbarButton::CopyFrom(const CBCGPToolbarButton& src)
{
	CBCGPToolbarCtrlCommon::CopyFrom(src);
}

BOOL CIdentityToolbarButton::CreateCtrlWnd()
{
	BOOL bRes = CBCGPToolbarCtrlCommon::CreateCtrlWnd();
	if( bRes )
	{
		CVCenterStatic* pStatic = (CVCenterStatic*)GetCtrlWnd();
	
		VERIFY(m_hFont.CreateFont(
			28,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("ËÎÌו")));                 // lpszFacename

		pStatic->SetFont(&m_hFont);
		pStatic->m_clrTxt = RGB(0,0,0);
	}

	return bRes;
}


