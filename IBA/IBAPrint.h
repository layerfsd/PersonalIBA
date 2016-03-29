#pragma once
#include "G2XPrint.h"

class CIBAPrint
{
private :

	CIBAPrint(void);
	~CIBAPrint(void);

public:

	static void Print(CString& strContent);

	static void GetRegisterTicket(CString& strContent);

	static void GetCreditTicket(CString& strContent);

	static void GetBundleTimeTicket(CString& strContent);

	static void ReplaceHeaderInfo(CString &strContent);

	static void GetRegisterAndBundleTimeTicket(CString& strContent);

	static void GetShiftCashTicket(CString& strContent);

	static void GetCreditAndBundleTimeTicket(CString& strContent);

	static void GetRefundTicket(CString& strContent, BOOL bIsMember);

	static void GetShoppingTicket(CString& strContent, BOOL bIsCredit);
	
};



class CIBAPrintA4 : public CG2XPrintHelper
{
public:
	static CIBAPrintA4* GetInstance()
	{
		static CIBAPrintA4 ibaPrinter;
		return &ibaPrinter;
	}
	CIBAPrintA4():CG2XPrintHelper(NULL)
	{

	}

	void SetOwner(CWnd* pWnd);

	void DoPrint()
	{
		CG2XPrintHelper::OnDialogPrint();
	}

	void DoPrintPriview()
	{
		CG2XPrintHelper::OnDialogPrintPreview();
	}

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

public:
	CString m_strName;
	CString m_strNumber;
	CString m_strID;
	CString m_strAddress;
	CString m_strContent[5];
};