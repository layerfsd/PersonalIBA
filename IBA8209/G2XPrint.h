#pragma once

class CPrintInterfaceBase
{
public:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) = 0;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) = 0;
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) = 0;
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo) = 0;
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) = 0;
	virtual HWND GetOwnerHandle() = 0;
	virtual void GetTitle(CString& str) = 0;
};

//--------------------------------------
//- CG2XPreviewView
//--------------------------------------
class CG2XPreviewView : public CPreviewView
{
protected:
	CG2XPreviewView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CG2XPreviewView)

	// Attributes
public:

protected:
	// Operations
	HWND m_hDialog;
public:
	// Overrides
	void	SetDialogPointer(HWND hDialog);


	// Implementation
protected:
	virtual ~CG2XPreviewView();
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CG2XPreviewView)
	//}}AFX_MSG
	afx_msg void	OnPreviewPrint();

public:
	void ClosePreview();
protected:
	DECLARE_MESSAGE_MAP()
};



//--------------------------------------
// CG2XPrintFrameWnd
//--------------------------------------
class CG2XPrintFrameWnd : public CFrameWnd
{
public:
	DECLARE_DYNCREATE(CG2XPrintFrameWnd)
	DECLARE_MESSAGE_MAP()

	afx_msg void OnClose();
};


//-------------------------------------------
// CG2XPrintDummyView
//-------------------------------------------
class CG2XPrintDummyView : public CView
{
public:
	CG2XPrintDummyView();

	virtual ~CG2XPrintDummyView();
protected:
	DECLARE_DYNCREATE(CG2XPrintDummyView)
	friend class CG2XPreviewView;

	CPrintInterfaceBase *m_pPrintInterface;
	// Attributes
public:
	void		SetPrintDialog(CPrintInterfaceBase* pInterface);
	void		OnDoPrintPreview();
	void		OnDoFilePrint();
	CFrameWnd*	m_pOldFrame;

protected:
	virtual void OnDraw(CDC* pDC) {}
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo)
	{
		return m_pPrintInterface->OnPreparePrinting(pInfo);
	}
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
	{
		m_pPrintInterface->OnBeginPrinting(pDC, pInfo);
	}
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL)
	{
		m_pPrintInterface->OnPrepareDC(pDC, pInfo);
	}
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo)
	{
		m_pPrintInterface->OnPrint(pDC, pInfo);
	}
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
	{
		m_pPrintInterface->OnEndPrinting(pDC, pInfo);
	}
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);

};



class CG2XPrintHelper : public CPrintInterfaceBase
{
public:
	CG2XPrintHelper(CWnd* pWnd)
	{
		m_pTemplate = NULL;
		m_pView = NULL;
		m_pOwner = pWnd;
	}
	virtual ~CG2XPrintHelper()
	{
		m_pTemplate = NULL;
		m_pView = NULL;
	}

public:
	CG2XPrintDummyView* m_pView;
	CSingleDocTemplate*	m_pTemplate;
	CWnd* m_pOwner;

public:
	void OnDialogPrint();
	void OnDialogPrintPreview();

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) {}
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL) {}
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo) {}
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {}
	virtual HWND GetOwnerHandle();
	virtual void GetTitle(CString& str) {  }
};
