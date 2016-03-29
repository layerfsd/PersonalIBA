// HWConnect.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HWConnect.h"
#include "HWConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHWConnectApp

BEGIN_MESSAGE_MAP(CHWConnectApp, CWinApp)
	//{{AFX_MSG_MAP(CHWConnectApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHWConnectApp construction

CHWConnectApp::CHWConnectApp()
{
	m_hRunOnce = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHWConnectApp object

CHWConnectApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHWConnectApp initialization

BOOL CHWConnectApp::InitInstance()
{
	m_hRunOnce = ::CreateMutex(NULL,FALSE,"RunOnce_CHWConnectApp_FLAG");
	if(m_hRunOnce != NULL)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(m_hRunOnce);
			m_hRunOnce = NULL;

			PostQuitMessage(0);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	
	AfxEnableControlContainer();

	// Standard initialization
	m_objCIniFileManager.SetIniFilePath();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CHWConnectDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CHWConnectApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_hRunOnce != NULL)
	{
		CloseHandle(m_hRunOnce);
		m_hRunOnce = NULL;
	}
	
	return CWinApp::ExitInstance();
}
