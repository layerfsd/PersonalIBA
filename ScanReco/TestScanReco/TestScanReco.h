// TestScanReco.h : main header file for the TESTSCANRECO application
//

#if !defined(AFX_TESTSCANRECO_H__2AF5C2BF_7E9E_4553_BF73_8AE205C7A2CB__INCLUDED_)
#define AFX_TESTSCANRECO_H__2AF5C2BF_7E9E_4553_BF73_8AE205C7A2CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestScanRecoApp:
// See TestScanReco.cpp for the implementation of this class
//

class CTestScanRecoApp : public CWinApp
{
public:
	CTestScanRecoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestScanRecoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestScanRecoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTSCANRECO_H__2AF5C2BF_7E9E_4553_BF73_8AE205C7A2CB__INCLUDED_)
