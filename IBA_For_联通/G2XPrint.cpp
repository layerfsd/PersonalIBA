#include "StdAfx.h"
#include "G2XPrint.h"
#include <..\src\mfc\afximpl.h>
#include "resource.h"

IMPLEMENT_DYNCREATE(CG2XPreviewView, CPreviewView)

CG2XPreviewView::CG2XPreviewView()
{
	m_hDialog = NULL;
}

CG2XPreviewView::~CG2XPreviewView()
{

}

BEGIN_MESSAGE_MAP(CG2XPreviewView, CPreviewView)
	//{{AFX_MSG_MAP(CG2XPreviewView)
	//}}AFX_MSG_MAP
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)
END_MESSAGE_MAP()

// CG2XPreviewView diagnostics
#ifdef _DEBUG
void CG2XPreviewView::AssertValid() const
{
	// Bug nearby Article ID: Q192853 
	//	CPreviewView::AssertValid();
}

void CG2XPreviewView::Dump(CDumpContext& dc) const
{
	CPreviewView::Dump(dc);
}

#endif //_DEBUG

void CG2XPreviewView::OnPreviewPrint()
{
	if (m_hDialog)
	{
		::PostMessage(m_hDialog, WM_COMMAND, ID_FILE_PRINT, 0);
	}
	else
	{
		// the view created for the dialog needs to set the m_pDialog pointer
		ASSERT(FALSE);
	}
	OnPreviewClose();               // force close of Preview
}

void CG2XPreviewView::SetDialogPointer(HWND hDialog)
{
	m_hDialog = hDialog;
}

void CG2XPreviewView::ClosePreview()
{
	OnPreviewClose();
}

//-------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CG2XPrintFrameWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(CG2XPrintFrameWnd,CFrameWnd)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CG2XPrintFrameWnd::OnClose()
{
	CG2XPreviewView *pMyPreview = (CG2XPreviewView*)GetActiveView();
	pMyPreview->ClosePreview();
}

//-------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CG2XPrintDummyView, CView)

CG2XPrintDummyView::CG2XPrintDummyView()
{
	m_pPrintInterface = NULL;
	m_pOldFrame = NULL;
}

CG2XPrintDummyView::~CG2XPrintDummyView()
{
	m_pPrintInterface = NULL;
	m_pOldFrame = NULL;
}

void CG2XPrintDummyView::SetPrintDialog(CPrintInterfaceBase* pInterface)
{
	m_pPrintInterface = pInterface;
}

void CG2XPrintDummyView::OnDoPrintPreview()
{
	// In derived classes, implement special window handling here
	// Be sure to Unhook Frame Window close if hooked.

	// must not create this on the frame.  Must outlive this function
	CPrintPreviewState* pState = new CPrintPreviewState;

	// DoPrintPreview's return value does not necessarily indicate that
	// Print preview succeeded or failed, but rather what actions are necessary
	// at this point.  If DoPrintPreview returns TRUE, it means that
	// OnEndPrintPreview will be (or has already been) called and the
	// pState structure will be/has been deleted.
	// If DoPrintPreview returns FALSE, it means that OnEndPrintPreview
	// WILL NOT be called and that cleanup, including deleting pState
	// must be done here.

	if (!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,
		RUNTIME_CLASS(CG2XPreviewView), pState))
	{
		// In derived classes, reverse special window handling here for
		// Preview failure case

		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}
	// the preview view should be the active view, set the pointer for it
	// we need to do this so that the ID_FILE_PRINT command will goto the dialog
	// and not any global view.
	CFrameWnd *pWnd = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	CG2XPreviewView *pMyPreview = (CG2XPreviewView*)pWnd->GetActiveView();
	pMyPreview->SetDialogPointer(m_pPrintInterface->GetOwnerHandle());
}

void CG2XPrintDummyView::OnDoFilePrint()
{
	// get default print info
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (LOWORD(GetCurrentMessage()->wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo* pCmdInfo = AfxGetApp()->m_pCmdInfo;

		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
				printInfo.m_pPD->m_pd.hDC = ::CreateDC(pCmdInfo->m_strDriverName,
					pCmdInfo->m_strPrinterName, pCmdInfo->m_strPortName, NULL);
				if (printInfo.m_pPD->m_pd.hDC == NULL)
				{
					AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
					return;
				}
			}
		}

		printInfo.m_bDirect = TRUE;
	}

	if (OnPreparePrinting(&printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);

		// gather file to print to if print-to-file selected
		CString strOutput;
		if (printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE && !printInfo.m_bDocObject)
		{
			// construct CFileDialog for browsing
			CString strDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT));
			CString strPrintDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT));
			CString strFilter(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER));
			CString strCaption(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION));
			CFileDialog dlg(FALSE, strDef, strPrintDef,
				OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
			dlg.m_ofn.lpstrTitle = strCaption;

			if (dlg.DoModal() != IDOK)
				return;

			// set output device to resulting path name
			strOutput = dlg.GetPathName();
		}

		// set up document info and start the document printing process
		CString strTitle;
		m_pPrintInterface->GetTitle(strTitle);
		if (strTitle.GetLength() > 31)
			strTitle.ReleaseBuffer(31);
		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;
		CString strPortName;
		int nFormatID;
		if (strOutput.IsEmpty())
		{
			docInfo.lpszOutput = NULL;
			strPortName = printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}
		else
		{
			docInfo.lpszOutput = strOutput;
			AfxGetFileTitle(strOutput,
				strPortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			nFormatID = AFX_IDS_PRINTTOFILE;
		}

		// setup the printing DC
		CDC dcPrint;
		if (!printInfo.m_bDocObject)
		{
			dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc
			dcPrint.m_bPrinting = TRUE;
		}
		OnBeginPrinting(&dcPrint, &printInfo);

		//if (!printInfo.m_bDocObject)
		//	dcPrint.SetAbortProc(AbortProc);

		// disable main window while printing & init printing status dialog
		AfxGetMainWnd()->EnableWindow(FALSE);
		//CPrintingDialog dlgPrintStatus(this);

		//CString strTemp;
		//dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
		//dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME,
		//	printInfo.m_pPD->GetDeviceName());
		//AfxFormatString1(strTemp, nFormatID, strPortName);
		//dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strTemp);
		//dlgPrintStatus.ShowWindow(SW_SHOW);
		//dlgPrintStatus.UpdateWindow();

		// start document printing process
		if (!printInfo.m_bDocObject && dcPrint.StartDoc(&docInfo) == SP_ERROR)
		{
			// enable main window before proceeding
			AfxGetMainWnd()->EnableWindow(TRUE);

			// cleanup and show error message
			OnEndPrinting(&dcPrint, &printInfo);
			//dlgPrintStatus.DestroyWindow();
			dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
			AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
			return;
		}

		// Guarantee values are in the valid range
		UINT nEndPage = printInfo.GetToPage();
		UINT nStartPage = printInfo.GetFromPage();

		if (nEndPage < printInfo.GetMinPage())
			nEndPage = printInfo.GetMinPage();
		if (nEndPage > printInfo.GetMaxPage())
			nEndPage = printInfo.GetMaxPage();

		if (nStartPage < printInfo.GetMinPage())
			nStartPage = printInfo.GetMinPage();
		if (nStartPage > printInfo.GetMaxPage())
			nStartPage = printInfo.GetMaxPage();

		int nStep = (nEndPage >= nStartPage) ? 1 : -1;
		nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

		//VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));

		// If it's a doc object, we don't loop page-by-page
		// because doc objects don't support that kind of levity.

		BOOL bError = FALSE;
		if (printInfo.m_bDocObject)
		{
			OnPrepareDC(&dcPrint, &printInfo);
			OnPrint(&dcPrint, &printInfo);
		}
		else
		{
			// begin page printing loop
			for (printInfo.m_nCurPage = nStartPage;
				printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep)
			{
				OnPrepareDC(&dcPrint, &printInfo);

				// check for end of print
				if (!printInfo.m_bContinuePrinting)
					break;

				// write current page
				//TCHAR szBuf[80];
				//wsprintf(szBuf, strTemp, printInfo.m_nCurPage);
				//dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

				// set up drawing rect to entire page (in logical coordinates)
				printInfo.m_rectDraw.SetRect(0, 0,
					dcPrint.GetDeviceCaps(HORZRES),
					dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&printInfo.m_rectDraw);

				// attempt to start the current page
				if (dcPrint.StartPage() < 0)
				{
					bError = TRUE;
					break;
				}

				// must call OnPrepareDC on newer versions of Windows because
				// StartPage now resets the device attributes.
				//if (afxData.bMarked4)
				if (afxData.bWin95)//bMarked4)
					OnPrepareDC(&dcPrint, &printInfo);

				ASSERT(printInfo.m_bContinuePrinting);

				// page successfully started, so now render the page
				OnPrint(&dcPrint, &printInfo);
				if (dcPrint.EndPage() < 0)// || !AbortProc(dcPrint.m_hDC, 0))
				{
					bError = TRUE;
					break;
				}
			}
		}

		// cleanup document printing process
		if (!printInfo.m_bDocObject)
		{
			if (!bError)
				dcPrint.EndDoc();
			else
				dcPrint.AbortDoc();
		}

		AfxGetMainWnd()->EnableWindow();    // enable main window

		OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
		//dlgPrintStatus.DestroyWindow();

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
	}
}

void CG2XPrintDummyView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
	CView::OnEndPrintPreview(pDC, pInfo, point, pView);
	// show the original frame
	m_pOldFrame->ShowWindow(SW_SHOW);
	// restore main frame pointer
	AfxGetApp()->m_pMainWnd = m_pOldFrame;
	//m_pDialog->EndPreviewMode();
	::SetFocus(m_pPrintInterface->GetOwnerHandle());
	GetParentFrame()->DestroyWindow();
}

void CG2XPrintHelper::OnDialogPrintPreview()
{
	CFrameWnd* pOldFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	pOldFrame->ShowWindow(SW_HIDE);
	if (!m_pTemplate)
	{
		UINT nResID = 0;
#ifdef IDR_MAINFRAME
		nResID = IDR_MAINFRAME;
#endif
		m_pTemplate = new CSingleDocTemplate(
			nResID,					// so load fram is happy. This matches the IDR_MAINFRAME value from your application!
			NULL,
			RUNTIME_CLASS(CG2XPrintFrameWnd),
			RUNTIME_CLASS(CG2XPrintDummyView));
		//AfxGetApp()->AddDocTemplate(m_pTemplate);
	}
	CFrameWnd* pFrameWnd = m_pTemplate->CreateNewFrame(NULL, NULL);

	// In derived classes, implement special window handling here
	// Be sure to Unhook Frame Window close if hooked.
	// must not create this on the frame.  Must outlive this function
	//CPrintPreviewState* pState = new CPrintPreviewState;
	m_pTemplate->InitialUpdateFrame(pFrameWnd, NULL, FALSE);
	m_pView = (CG2XPrintDummyView*)pFrameWnd->GetActiveView();
	AfxGetApp()->m_pMainWnd = pFrameWnd;
	pFrameWnd->SetWindowText(_T("打印预览"));
	pFrameWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pView->SetPrintDialog(this);
	m_pView->m_pOldFrame = pOldFrame;
	m_pView->OnDoPrintPreview();
}

void CG2XPrintHelper::OnDialogPrint()
{
	CG2XPrintDummyView	view;

	view.SetPrintDialog(this);
	view.OnDoFilePrint();
}

HWND CG2XPrintHelper::GetOwnerHandle()
{
	if (m_pOwner == NULL || !::IsWindow(m_pOwner->m_hWnd))
	{
		MessageBox(NULL,_T("打印预览，必须指定有效的所有者窗口"),NULL,MB_OK);
		return 0;
	}
	return m_pOwner->m_hWnd;
}

BOOL CG2XPrintHelper::OnPreparePrinting(CPrintInfo* pInfo)
{
	return m_pView->DoPreparePrinting(pInfo);
}