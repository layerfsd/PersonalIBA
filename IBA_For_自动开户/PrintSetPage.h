#pragma once

// CPrintSetPage 对话框

class CPrintSetPage : public CIBAPage
{
	DECLARE_DYNAMIC(CPrintSetPage)

public:
	CPrintSetPage();
	virtual ~CPrintSetPage();

// 对话框数据
	enum { IDD = IDD_SET_PRINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	int m_nTicketFontSize; 

	BOOL m_bPrintMemberOpen;
	BOOL m_bPrintMemberCredit;
	BOOL m_bPrintMemberBT;
	BOOL m_bPrintMemberReturn;

	BOOL m_bPrintTempOpen;
	BOOL m_bPrintTempCredit;
	BOOL m_bPrintTempBT;
	BOOL m_bPrintTempReturn;
};
