#pragma once

//#include 
// CSetPageOfSeat 对话框

class CSetPageOfSeat : public CIBAPage
{
	DECLARE_DYNAMIC(CSetPageOfSeat)
	DECLARE_MESSAGE_MAP()

public:
	CSetPageOfSeat();
	virtual ~CSetPageOfSeat();

// 对话框数据
	enum { IDD = IDD_DIALOG_SEATSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedButtonBrowser();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString m_strBitmapPath;

};
