#pragma once


#include "IBADialog.h"
#include "SeatList.h"
// CDlgAddTerm 对话框

class CDlgAddTerm : public CIBADialog
{
	DECLARE_DYNAMIC(CDlgAddTerm)
	DECLARE_MESSAGE_MAP()

public:
	CDlgAddTerm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddTerm();


// 对话框数据
	enum { IDD = IDD_SEAT_ADDTERMINAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	afx_msg void OnCbnSelchangeComboTerminalid();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();

public:
	virtual BOOL OnInitDialog();

	void SetExistSeatList(CSeatList * newValue) { m_SeatList = newValue; }

	CString GetTerminalID(void) const { return  m_strTerminalID; }
	void SetTerminalID(CString newValue) { m_strTerminalID = newValue; }

	CString GetTerminalIP(void) const { return  m_strTerminalIP; }
	void SetTerminalIP(CString newValue) { m_strTerminalIP = newValue; }

private:
	// 终端IP
	CBCGPComboBox m_cboTerminalIP;
	// 终端ID
	CBCGPComboBox m_cboTerminalID;
	// 捆绑的终端ID
	CString m_strTerminalID;
	// 捆绑的终端IP
	CString m_strTerminalIP;
	//保存数据链指针
	CSeatList * m_SeatList;

private:
	// 通过本地数据类载入所有的终端信息
	void LoadComputeInfo(void);


};
