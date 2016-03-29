
#include "stdafx.h"
#include "IBA.h"
#include "IBADoc.h"
#include ".\ibadoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CIBADoc

IMPLEMENT_DYNCREATE(CIBADoc, CDocument)

BEGIN_MESSAGE_MAP(CIBADoc, CDocument)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIBADoc construction/destruction

CIBADoc::CIBADoc()
{
	// TODO: add one-time construction code here

}

CIBADoc::~CIBADoc()
{
}

BOOL CIBADoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIBADoc serialization

void CIBADoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CString strTmp;
		
		strTmp = _T("FFFFFFFFFFFF");

		//char c1 = 0xFF, c2 = 0xFE;

		//ar << c1 << c2 << strTmp;
		
		ar.WriteString(strTmp);

	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CIBADoc diagnostics

#ifdef _DEBUG
void CIBADoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIBADoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIBADoc commands

void CIBADoc::SetTitle(LPCTSTR lpszTitle)
{
	CString strTmp(_T("Íø°ÉÃû³Æ.txt"));

	CDocument::SetTitle(strTmp);
}
