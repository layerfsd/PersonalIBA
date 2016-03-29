
#pragma once


class CIBADoc : public CDocument
{
protected: // create from serialization only
	CIBADoc();
	DECLARE_DYNCREATE(CIBADoc)
	DECLARE_MESSAGE_MAP()
// Attributes
public:

// Operations
public:

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);

public:
	virtual ~CIBADoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:

	

	
};

