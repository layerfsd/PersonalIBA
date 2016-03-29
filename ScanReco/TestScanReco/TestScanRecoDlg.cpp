// TestScanRecoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestScanReco.h"
#include "TestScanRecoDlg.h"
#include "..\\ScanReco.h"
#include ".\testscanrecodlg.h"

          
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#pragma comment(lib, "..\\debug\\ScanReco.lib")
#else
#pragma comment(lib, "..\\release\\ScanReco.lib")
#endif



void WINAPI MyFun(int idType)
{
	CString str;
	char cBuf[] = "Test.jpg";
	int i = ScanImage(idType, cBuf);
	if(i==1)
	{
		IDENTITY_INFO stIDENTITY_INFO = {0};
		char cBuf[] = "Head.jpg";
		i = RecogniseImage(idType, stIDENTITY_INFO, cBuf);
		if(i==1)
		{
			str.Format("Recognise: %u\r\n\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", i,
				stIDENTITY_INFO.name,// stIDCARD_ALL.name,// 20);     ,//姓名
				stIDENTITY_INFO.sex,// stIDCARD_ALL.sex,// 2);        ,//性别
				stIDENTITY_INFO.number,// stIDCARD_ALL.number,// 40);   ,//证件编码
				stIDENTITY_INFO.nation,// stIDCARD_ALL.people,// 20);    ,//民族
				stIDENTITY_INFO.birthday,// stIDCARD_ALL.birthday,// 10);  ,//出生日期
				stIDENTITY_INFO.address,// stIDCARD_ALL.address,// 200);  ,//地址
				stIDENTITY_INFO.signdate,// stIDCARD_ALL.signdate,// 10);  ,//签发日期
				stIDENTITY_INFO.validterm,// stIDCARD_ALL.validterm,// 30);  ,//有效期
				stIDENTITY_INFO.department// "",// 40); ,//发证机关
				);

		}
		else
		{
			str.Format("Recognise: %u ret: %u", idType, i);

		}
	}
	else
	{
		str.Format("Scan: %u ret: %u", idType, i);
	}

	AfxMessageBox(str);

}


/////////////////////////////////////////////////////////////////////////////
// CTestScanRecoDlg dialog

CTestScanRecoDlg::CTestScanRecoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestScanRecoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestScanRecoDlg)
	m_iType = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestScanRecoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestScanRecoDlg)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_objTypeID);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_iType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestScanRecoDlg, CDialog)
	//{{AFX_MSG_MAP(CTestScanRecoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_RECOGNISE, OnButtonRecognise)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_RECO_ALL, OnButtonScanRecoAll)
	ON_BN_CLICKED(IDC_BUTTON_READER, OnButtonReader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestScanRecoDlg message handlers

BOOL CTestScanRecoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_objTypeID.SetCurSel(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestScanRecoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestScanRecoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestScanRecoDlg::OnOK() 
{
	// TODO: Add extra validation here
	//char cTemp[128] = "\0";
	//m_objTypeID.GetWindowText(cTemp, sizeof(cTemp));
	//int iType = atol(cTemp);
	//CString str;
	//char cBuf[] = "Test.jpg";
	//int i = ScanImage(iType, cBuf);
	//str.Format("Scan: %u ret: %u", iType, i);

	//OnButtonRecognise();
	//MessageBox(str);
	//CDialog::OnOK();
}

void CTestScanRecoDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	OnButtonClose();
	CDialog::OnCancel();
}

void CTestScanRecoDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	CString str;
	int i = OpenDevice();
	str.Format("OpenDevice: %u", i);
	MessageBox(str);
	if(i == 1)
	{
		i = SetCallBack((CallBack)MyFun);
		str.Format("SetCallBack: 0x%X ret:%u", MyFun, i);
		MessageBox(str);
	}

}

void CTestScanRecoDlg::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	char cTemp[128] = "\0";
	m_objTypeID.GetWindowText(cTemp, sizeof(cTemp));
	int iType = atol(cTemp);
	
	CString str;
	char cBuf[] = "Test.jpg";
	int i = ScanImage(iType, cBuf);
	str.Format("Scan: %u ret: %u", iType, i);
	MessageBox(str);
	
}
//stIDENTITY_INFO.signdate,// stIDCARD_ALL.signdate,// 10);  ,//签发日期
void CTestScanRecoDlg::OnButtonRecognise() 
{
	// TODO: Add your control notification handler code here
	char cTemp[128] = "\0";
	m_objTypeID.GetWindowText(cTemp, sizeof(cTemp));
	int iType = atol(cTemp);
	IDENTITY_INFO stIDENTITY_INFO = {0};
	memset(&stIDENTITY_INFO,0,sizeof(stIDENTITY_INFO));
	char cBuf[] = "Head.jpg";
	int i = RecogniseImage(iType, stIDENTITY_INFO, cBuf);
	if(i == 0) {MessageBox("读数据错误"); return ;}
	CString str;
	str.Format("Scan: %u\r\n\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", i,
		stIDENTITY_INFO.name,// stIDCARD_ALL.name,// 20);     ,//姓名
		stIDENTITY_INFO.sex,// stIDCARD_ALL.sex,// 2);        ,//性别
		stIDENTITY_INFO.number,// stIDCARD_ALL.number,// 40);   ,//证件编码
		stIDENTITY_INFO.nation,// stIDCARD_ALL.people,// 20);    ,//民族
		stIDENTITY_INFO.birthday,// stIDCARD_ALL.birthday,// 10);  ,//出生日期
		stIDENTITY_INFO.address,// stIDCARD_ALL.address,// 200);  ,//地址
		stIDENTITY_INFO.signdate,
		stIDENTITY_INFO.validterm,
		stIDENTITY_INFO.department// "",// 40); ,//发证机关
		);
	MessageBox(str);
	
}

void CTestScanRecoDlg::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	int i = CloseDevice();
	CString str;
	str.Format("Close: %u", i);
	MessageBox(str);
}

void CTestScanRecoDlg::OnButtonScanRecoAll() 
{
	// TODO: Add your control notification handler code here
	const int iType[] = {
	IDENTITY_ID1,// 10, //一代身份证
		IDENTITY_ID2,// 11, //二代身份证
		0
	};


	for(int iT = 0; iType[iT] > 0; iT++ )
	{
		int i = ScanImage(iType[iT], "test.jpg\0");
		CString str;
		str.Format("Scan: %u ret:%u", iType[iT], i);
		MessageBox(str);
		
		IDENTITY_INFO stIDENTITY_INFO = {0};
		char cBuf[MAX_PATH] = "Test.jpg\0";
		i = RecogniseImage(iType[iT], stIDENTITY_INFO, cBuf);
		if(i == 0) {MessageBox("读数据错误"); return ;}
		str.Format("Recognise: %u\r\n\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", i,
			stIDENTITY_INFO.name,// stIDCARD_ALL.name,// 20);     ,//姓名
			stIDENTITY_INFO.sex,// stIDCARD_ALL.sex,// 2);        ,//性别
			stIDENTITY_INFO.number,// stIDCARD_ALL.number,// 40);   ,//证件编码
			stIDENTITY_INFO.nation,// stIDCARD_ALL.people,// 20);    ,//民族
			stIDENTITY_INFO.birthday,// stIDCARD_ALL.birthday,// 10);  ,//出生日期
			stIDENTITY_INFO.address,// stIDCARD_ALL.address,// 200);  ,//地址
			stIDENTITY_INFO.signdate,// stIDCARD_ALL.signdate,// 10);  ,//签发日期
			stIDENTITY_INFO.validterm,// stIDCARD_ALL.validterm,// 30);  ,//有效期
			stIDENTITY_INFO.department// "",// 40); ,//发证机关
			);
		MessageBox(str);
	}	
}

void CTestScanRecoDlg::Fun(int idType)
{

}

void CTestScanRecoDlg::OnButtonReader() 
{
	// TODO: Add your control notification handler code here
	CString str;
	char cBuf[] = "Reader.bmp";

	IDENTITY_INFO stIDENTITY_INFO = {0};
	int i = ReadCard(stIDENTITY_INFO, cBuf);
	if( i== 0){MessageBox("读数据错误"); return ;}
	str.Format("Reader: %u\r\n\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s", i,
		stIDENTITY_INFO.name,// stIDCARD_ALL.name,// 20);     ,//姓名
		stIDENTITY_INFO.sex,// stIDCARD_ALL.sex,// 2);        ,//性别
		stIDENTITY_INFO.number,// stIDCARD_ALL.number,// 40);   ,//证件编码
		stIDENTITY_INFO.nation,// stIDCARD_ALL.people,// 20);    ,//民族
		stIDENTITY_INFO.birthday,// stIDCARD_ALL.birthday,// 10);  ,//出生日期
		stIDENTITY_INFO.address,// stIDCARD_ALL.address,// 200);  ,//地址
		stIDENTITY_INFO.signdate,// stIDCARD_ALL.signdate,// 10);  ,//签发日期
		stIDENTITY_INFO.validterm,// stIDCARD_ALL.validterm,// 30);  ,//有效期
		stIDENTITY_INFO.department// "",// 40); ,//发证机关
		);
	MessageBox(str);
}

void CTestScanRecoDlg::OnBnClickedOk()
{
	char cTemp[128] = "\0";
	m_objTypeID.GetWindowText(cTemp, sizeof(cTemp));
	int iType = atol(cTemp);

	CString str;
	char cBuf[] = "Test.jpg";
	int i = ScanImage(iType, cBuf);
	str.Format("Scan: %u ret: %u", iType, i);
	MessageBox(str);
	OnOK();
}


