// NetBarOtherInfoPage.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "IBAHelpper.h"
#include ".\NetBarOtherInfoPage.h"


// CConfigOfOther 对话框

IMPLEMENT_DYNAMIC(CConfigOfOther, CIBADialog)
CConfigOfOther::CConfigOfOther()
: CIBADialog(CConfigOfOther::IDD)
{
}

CConfigOfOther::~CConfigOfOther()
{
}

void CConfigOfOther::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CONFIG, m_ListConfig);
	DDX_Text(pDX, IDC_EDIT_CONFIG1, m_strConfig1);
	DDX_Text(pDX, IDC_EDIT_CONFIG2, m_strConfig2);
	DDX_Text(pDX, IDC_EDIT_CONFIG3, m_strConfig3);
	DDX_Text(pDX, IDC_EDIT_MEMO, m_strDescription);
}


BEGIN_MESSAGE_MAP(CConfigOfOther, CIBADialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONFIG, OnNMClickListConfig)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CConfigOfOther 消息处理程序

BOOL CConfigOfOther::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	SetEditFont();

	CIBAString strHeader;
	//strHeader = _T("配置名称|数值1|数值2|字符串|说明");
	strHeader.LoadString(IDS_CONFIGLISTHEADER);

	CStringArray strArray;
	CIBAHelpper::SplitLine(strHeader, strArray);

	const UINT Width[] = {150, 50, 50, 180, 150};

	for (INT i = 0; i < strArray.GetCount(); i++)
	{
		m_ListConfig.InsertColumn(i, strArray.GetAt(i), LVCFMT_LEFT, Width[i]);
	}

	m_ListConfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect myRect;
	GetClientRect(myRect);

	//m_ListConfig.MoveWindow(0, 100, myRect.Width() + 10, myRect.Height() - 107);

	m_ListConfig.MoveWindow(5, 120, myRect.Width() - 10, myRect.Height() - 125);
	
	LoadConfig();
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(!m_strKeyString.IsEmpty());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CConfigOfOther::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CIBADialog::OnOK();
}

void CConfigOfOther::OnNMClickListConfig(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT nIdx = m_ListConfig.GetSelectionMark();
	
	if (nIdx >= 0)
	{
		CConfigInfo& ConfigInfo = m_ConfigArray.GetAt(nIdx);
		m_strConfig1.Format(_T("%d"), ConfigInfo.GetKeyValue1());
		m_strConfig2.Format(_T("%d"), ConfigInfo.GetKeyValue2());
		m_strConfig3 = ConfigInfo.GetKeyValue3();
		m_strKeyString = ConfigInfo.GetKeyString();
		m_strDescription = ConfigInfo.GetDescription();

		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(!m_strKeyString.IsEmpty());

		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CConfigOfOther::OnBnClickedButtonSave()
{
	if (!m_strKeyString.IsEmpty())
	{
		UpdateData();

		CConfigInfo ConfigInfo ;
		ConfigInfo.SetKeyString(m_strKeyString);
		ConfigInfo.SetKeyValue1(_ttoi(m_strConfig1));
		ConfigInfo.SetKeyValue2(_ttoi(m_strConfig2));
		ConfigInfo.SetKeyValue3(m_strConfig3);

		if (CIBADAL::GetInstance()->UpdateSystemConfig(ConfigInfo, 4))
		{
			LoadConfig();
			AfxMessageBox(IDS_OPERATEOK);
		}
	}
}

void CConfigOfOther::LoadConfig()
{
	m_ListConfig.DeleteAllItems();
	m_ConfigArray.RemoveAll();

	CIBADAL::GetInstance()->GetAllSystemConfig(m_ConfigArray);

	for (INT i = 0; i < m_ConfigArray.GetCount(); i++)
	{	
		CConfigInfo& ConfigInfo = m_ConfigArray.GetAt(i);
		m_ListConfig.InsertItem(i, ConfigInfo.GetKeyName());

		CString strTmp;
		strTmp.Format(_T("%d"), ConfigInfo.GetKeyValue1());
		m_ListConfig.SetItemText(i, 1, strTmp);

		strTmp.Format(_T("%d"), ConfigInfo.GetKeyValue2());
		m_ListConfig.SetItemText(i, 2, strTmp);

		m_ListConfig.SetItemText(i, 3, ConfigInfo.GetKeyValue3());
		m_ListConfig.SetItemText(i, 4, ConfigInfo.GetDescription());

		m_ListConfig.SetItemData(i, i);
	}
}

void CConfigOfOther::OnSize(UINT nType, int cx, int cy)
{
	CIBADialog::OnSize(nType, cx, cy);
}
