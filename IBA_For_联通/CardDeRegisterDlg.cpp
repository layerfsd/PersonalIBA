// CardDeRegisterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "RealName\IBARealName.h"
#include "ServletHellper.h"
#include "NDReturnDlg.h"
#include ".\cardderegisterdlg.h"

// CRealNameDeRegDlg 对话框

IMPLEMENT_DYNAMIC(CRealNameDeRegDlg, CIBADialog)
CRealNameDeRegDlg::CRealNameDeRegDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CRealNameDeRegDlg::IDD, pParent)
{
}

CRealNameDeRegDlg::~CRealNameDeRegDlg()
{
}

void CRealNameDeRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NETID, m_strNetId);
	DDX_Control(pDX, IDC_EDIT_NETID, m_edtNetId);
}

BEGIN_MESSAGE_MAP(CRealNameDeRegDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CRealNameDeRegDlg 消息处理程序

void CRealNameDeRegDlg::OnBnClickedOk()
{
	UpdateData();
	
	if (m_strNetId.IsEmpty())
	{
		SetToolTipPos(IDOK);
		ShowToolTip(IDS_INPUTNETID);
		GetDlgItem(IDC_EDIT_NETID)->SetFocus();
		return;
	}
	else
	{	
		if (CServletHellper::GetMemberId(m_strNetId) > 0)
		{
			//只有已经退款的用户才能注销实名
			CIBAString strText;
			strText.LoadString(IDS_CARDDEREGISTER);
			MsgBox(strText);

			CNDReturnDlg dlg;
			dlg.SetNetId(m_strNetId);
			dlg.DoModal();

			return;
		}
		
		if (theApp.GetRealName()->InvokeRealName(100, (LPVOID)m_strNetId.GetString()))
		{
			SetToolTipPos(IDOK);
			ShowToolTip(IDS_OPERATEOK);

			m_strNetId.Empty();
			UpdateData(FALSE);
		}

		GetDlgItem(IDC_EDIT_NETID)->SetFocus();
	}

	//CBCGPDialog::OnOK();
}

void CRealNameDeRegDlg::OnOK()
{
	CBCGPDialog::OnOK();
}

void CRealNameDeRegDlg::OnCancel()
{
	CBCGPDialog::OnCancel();
}

BOOL CRealNameDeRegDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	m_edtNetId.SetFont(&m_Font);
	GetDlgItem(IDC_EDIT_NETID)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
