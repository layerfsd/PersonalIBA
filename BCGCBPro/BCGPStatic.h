#if !defined(AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_)
#define AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPStatic.h : header file
//

#include "BCGCBPro.h"

/////////////////////////////////////////////////////////////////////////////
// CBCGPStatic window

class BCGCBPRODLLEXPORT CBCGPStatic : public CStatic
{
// Construction
public:
	CBCGPStatic();

// Attributes
public:
	BOOL m_bOnGlass;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCGPStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBCGPStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBCGPStatic)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnBCGSetControlAero (WPARAM, LPARAM);
	afx_msg LRESULT OnSetText (WPARAM, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPSTATIC_H__A34CC2CA_E7ED_4D03_93F4_E2513841C22C__INCLUDED_)
