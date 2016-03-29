// ShiftCashDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShiftCash.h"
#include "ShiftCashDlg.h"
#include ".\shiftcashdlg.h"
#include <shlwapi.h>
#include ".\Excel\ExportOutputListThread.h"
#include <afxdb.h>
#include <vector>
#include <atlbase.h>

#include "ExportRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const ExcelColCount = 15;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CShiftCashDlg 对话框



CShiftCashDlg::CShiftCashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShiftCashDlg::IDD, pParent)
	, m_strFilePath(_T("C:\\交班记录表.xls"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShiftCashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFilePath);
}

BEGIN_MESSAGE_MAP(CShiftCashDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EXPORT_EXCEL, OnBnClickedBtnExportExcel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_EXPORT_ONKEY_COST, OnBnClickedBtnExportOnkeyCost)
	ON_BN_CLICKED(IDC_BTN_EXPORT_CARDOPEN, OnBnClickedBtnExportCardopen)
	ON_BN_CLICKED(IDC_BTN_EXPORT_CASH_PAY, OnBnClickedBtnExportCashPay)
END_MESSAGE_MAP()


// CShiftCashDlg 消息处理程序

BOOL CShiftCashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRegKey RegKey;
	if (ERROR_SUCCESS == RegKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ShiftCash")))
	{
		ULONG ulLen = 256;
		RegKey.QueryStringValue(_T("ExcelPath"), m_strFilePath.GetBuffer(256), &ulLen);
		m_strFilePath.ReleaseBuffer();	
	}
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CShiftCashDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShiftCashDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CShiftCashDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

struct ShiftCashInfo
{
	CString ID;
	CString StartDate;
	CString StandbyAmount;
	CString Deposit;
	CString CreditAmount;
	CString PresentAmount; 
	CString ReturnAmount;
	CString CardSoldAmount;
	CString CashSoldAmount; 
	CString CardCreditedAmount;
	CString TotalAmount;
	CString CostExpense; 
	CString Operator; 
	CString NextOperator;
	CString Description; 
	CString EndDate;
	CString Payout;
	CString remainAmount; 
	CString CountAmount;
	CString CashRegisterID;

	CString Combine()
	{
		CString strCombine;
		/*strCombine.AppendFormat(_T("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\%s\t%s\t%s\t%s\t%s"), 
							 ID,
							 StartDate,
							 StandbyAmount,
							 Deposit,
							 CreditAmount,
							 PresentAmount,
							 ReturnAmount,
							 CardSoldAmount,
							 CashSoldAmount,
							 CardCreditedAmount,
							 TotalAmount,
							 CostExpense, 
							 Operator,
							 NextOperator,
							 Description,
							 EndDate,
							 Payout,
							 remainAmount,
							 CountAmount,
							 CashRegisterID);*/

		strCombine.AppendFormat(_T("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t"),
							 StartDate,			// 班次开始时间
							 EndDate,			// 班次结束时间
							 CreditAmount,		// 充值总额
							 ReturnAmount,		// 退款总额				
							 Deposit,			// 上一班预留		 
							 CardSoldAmount,		// 充值卡金额
							 CashSoldAmount,		// 现金购物
							 CardCreditedAmount,	// 点卡充值总额
							 TotalAmount,			// 上缴金额 
							 CostExpense,			// 工本费
							 Operator,				// 操作员
							 NextOperator,			// 下一个班次名称
							 Payout,				// 其他支出
							 remainAmount,			// 预留金额
							 Description			// 班次描述
							 /*CountAmount,mount,
							 StandbyAmount,	*/ 
							 );
		// 班次开始时间\t班次结束时间\t充值总额\t退款总额\t上一班预留\t充值卡金额\t现金购物\t点卡充值总额\t上缴金额\t工本费\t操作员\t下一个班次名称\t其他支出\t预留金额\t班次描述
		return strCombine;
	}
};

BOOL GetShiftCash(std::vector<ShiftCashInfo> &vecSci)
{
	CString strSql;
	strSql.Format(_T("SELECT * FROM SHIFTCASH"));

	CDatabase database;

	CString strDb;

	try
	{
		//strDb.Format( _T("ODBC;Database=%s;Provider=SQLOLEDB;Driver={MySQL ODBC 3.51 Driver};Server=%s;Uid=%s;Pwd=%s"), 
		//	_T("netbar"), _T("127.0.0.1"), _T("root"), _T("szsunlikedataserver"));
		//strDb.Format( _T("DSN=%s"), _T("netbar") );

		strDb = _T("DSN=netbar;Uid=root;Pwd=szsunlikedataserver");

		database.OpenEx( strDb, CDatabase::openReadOnly | 
			CDatabase::noOdbcDialog );

		if( !database.IsOpen() )
		{
			CString strError(_T(""));
			strError.Format(_T("打开数据库失败！%d"), GetLastError());
			AfxMessageBox(strError);
			return FALSE;
		}
		
		CRecordset ds(&database);
		if(!ds.Open(AFX_DB_USE_DEFAULT_TYPE, strSql))
		{
			AfxMessageBox(_T("打开执行失败！"));
			return FALSE;
		}

		ShiftCashInfo shiftCashInfo;
		while(!ds.IsEOF())
		{
			{
				ds.GetFieldValue(_T("ID"), shiftCashInfo.ID);
				ds.GetFieldValue(_T("StartDate"), shiftCashInfo.StartDate);
				ds.GetFieldValue(_T("StandbyAmount"), shiftCashInfo.StandbyAmount);
				ds.GetFieldValue(_T("Deposit"), shiftCashInfo.Deposit);
				ds.GetFieldValue(_T("CreditAmount"), shiftCashInfo.CreditAmount);
				ds.GetFieldValue(_T("PresentAmount"), shiftCashInfo.PresentAmount);
				ds.GetFieldValue(_T("ReturnAmount"), shiftCashInfo.ReturnAmount);
				ds.GetFieldValue(_T("CardSoldAmount"), shiftCashInfo.CardSoldAmount);
				ds.GetFieldValue(_T("CashSoldAmount"), shiftCashInfo.CashSoldAmount);
				ds.GetFieldValue(_T("CardCreditedAmount"), shiftCashInfo.CardCreditedAmount);
				ds.GetFieldValue(_T("TotalAmount"), shiftCashInfo.TotalAmount);
				ds.GetFieldValue(_T("CostExpense"), shiftCashInfo.CostExpense);
				ds.GetFieldValue(_T("Operator"), shiftCashInfo.Operator);
				ds.GetFieldValue(_T("NextOperator"), shiftCashInfo.NextOperator);
				ds.GetFieldValue(_T("Description"), shiftCashInfo.Description);
				ds.GetFieldValue(_T("EndDate"), shiftCashInfo.EndDate);
				ds.GetFieldValue(_T("Payout"), shiftCashInfo.Payout);
				ds.GetFieldValue(_T("remainAmount"), shiftCashInfo.remainAmount);
				ds.GetFieldValue(_T("CountAmount"), shiftCashInfo.CountAmount);
				ds.GetFieldValue(_T("CashRegisterID"), shiftCashInfo.CashRegisterID);
				vecSci.push_back(shiftCashInfo);
			}
			ds.MoveNext();
		}
		//CString str;
		//ds.GetFieldValue((short)0, str);
		return TRUE;
		//return (0 == nCount);
	}
	catch(CDBException* e )
	{
		TCHAR szMeg[256] = {0};
		e->GetErrorMessage(szMeg, 256);
		
		CString strMsg;
		strMsg.Format(_T("操作数据库异常！%s"), szMeg);
		AfxMessageBox(strMsg);
	}
	return FALSE;
}

void CShiftCashDlg::OnBnClickedBtnExportExcel()
{
	GetDlgItemText(IDC_EDIT1, m_strFilePath);

	CRegKey RegKey;
	if (ERROR_SUCCESS == RegKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ShiftCash")))
	{
		RegKey.SetStringValue(_T("ExcelPath"), m_strFilePath);
	}
	
	// TODO: 在此添加控件通知处理程序代码
	// 获取行数和列数
	/*int nRowCount = m_listCheckOut.GetItemCount();
	CBCGPHeaderCtrl& Header = m_listCheckOut.GetHeaderCtrl();
	int nColCount = Header.GetItemCount();*/
	int nRowCount = 0;
	int nColCount = ExcelColCount;
	std::vector<ShiftCashInfo> vecSci;
	GetShiftCash(vecSci);
	nRowCount = vecSci.size();


	CString strAppend("");
	CString strTmp("");

	//for(int nRow = 0; nRow < nRowCount; nRow++)
	//{
	//	for(int nCol = 0; nCol < nColCount; nCol++)
	//	{
	//		strTmp.Empty();
	//		//strTmp = m_listCheckOut.GetItemText(nRow, nCol);
	//		// "\t" 表示跳到下一个单元格
	//		strAppend += strTmp + _T("\t");
	//	}
	//	// "\n" 表示跳到下一行
	//	strAppend += _T("\n");	
	//}
	//strAppend += _T("\n");	

	CString strFullPath;
	/*GetTempPath(MAX_PATH, strFullPath.GetBuffer(MAX_PATH));
	strFullPath.ReleaseBuffer();*/
	strFullPath = m_strFilePath;

	//strFullPath += _T("交班记录表.xls");


	if(PathFileExists(strFullPath))
	{
		while(!DeleteFile(strFullPath))
		{
			static UINT nCount = 0;
			CString strReplace("");
			strReplace.Format(_T("tmp%d.xls"), nCount++);
			strFullPath.Replace(_T(".xls"), strReplace);		// 如果文件无法删除，就修改文件名

			if(nCount > 5)		// 超过5个就不要再尝试了
				break;
		}
	}

	for(std::vector<ShiftCashInfo>::iterator iter = vecSci.begin(); iter != vecSci.end(); ++iter)
	{
		strAppend += iter->Combine();
		strAppend += _T("\n");
	}
	//strAppend = 
	// nRowCount+1 数据行数加上一行标题行
	static CExportOutputListThread expThread;
	expThread.SetParam(strAppend, strFullPath, nRowCount, nColCount);
	expThread.Start();
}

void CShiftCashDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog    dlgFile(FALSE, m_strFilePath, m_strFilePath, OFN_HIDEREADONLY, _T("Excel (*.xls)|*.xls|All Files (*.*)|*.*||"), NULL);


	if (IDOK == dlgFile.DoModal())
	{
		m_strFilePath = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}

void CShiftCashDlg::OnBnClickedBtnExportOnkeyCost()
{
	// 上机消费记录
	GetDlgItemText(IDC_EDIT1, m_strFilePath);

	static CExportRecordOnkeyCost onkeyCost;
	if (onkeyCost.Init(m_strFilePath.GetBuffer()))
		onkeyCost.Work();
}

void CShiftCashDlg::OnBnClickedBtnExportCardopen()
{
	// 开户记录
	GetDlgItemText(IDC_EDIT1, m_strFilePath);

	static CExportRecordCardOpen cardOpen;
	if (cardOpen.Init(m_strFilePath.GetBuffer()))
		cardOpen.Work();
}

void CShiftCashDlg::OnBnClickedBtnExportCashPay()
{
	// 现金充值记录
	GetDlgItemText(IDC_EDIT1, m_strFilePath);

	static CExportRecordCashPay cashPay;
	if (cashPay.Init(m_strFilePath.GetBuffer()))
		cashPay.Work();
}
