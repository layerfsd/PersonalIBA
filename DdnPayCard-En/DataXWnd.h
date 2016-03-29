#pragma once

#include "DataX.h"

// CDataXWnd
namespace NS_DataX
{

class CDataXWnd : public CWnd
{
	DECLARE_DYNAMIC(CDataXWnd)
	DECLARE_MESSAGE_MAP()

public:
	CDataXWnd();
	virtual ~CDataXWnd();

	static const UINT WM_DATAX = WM_USER + 1433;

	BOOL CreateRevWnd(IDataXReceive* lpReceiver);

protected:
	
	IDataXReceive* m_lpReceiver;

	afx_msg LRESULT OnDataX(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

}
