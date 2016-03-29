// HWConnect.h : main header file for the HWCONNECT application
//

#if !defined(AFX_HWCONNECT_H__0CB09400_1C21_44C9_9423_E6A55A1659FA__INCLUDED_)
#define AFX_HWCONNECT_H__0CB09400_1C21_44C9_9423_E6A55A1659FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "IniFileManager.h"	// Added by ClassView


enum IDENTITY_TYPE
{
	IDENTITY_ID1   = 10, //一代身份证
		IDENTITY_ID2   = 11, //二代身份证
		IDENTITY_STUDENT   = 15, //学生证
		IDENTITY_OFFICER   = 90, //军官证
		IDENTITY_POLICE   = 91, //警官证
		IDENTITY_SOLDIER   = 92, //士兵证
		IDENTITY_HUKOUBU   = 93, //户口簿
		IDENTITY_PASSPORT   = 94, //护照
		IDENTITY_TAIBAO   = 95, //台胞证
		IDENTITY_OTHER   = 99, //其它证件
};	

/////////////////////////////////////////////////////////////////////////////
// CHWConnectApp:
// See HWConnect.cpp for the implementation of this class
//

class CHWConnectApp : public CWinApp
{
public:
	CIniFileManager m_objCIniFileManager;
	CHWConnectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHWConnectApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHWConnectApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_hRunOnce;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HWCONNECT_H__0CB09400_1C21_44C9_9423_E6A55A1659FA__INCLUDED_)
