// DlgCapPhoto.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgCapPhoto.h"
#include ".\dlgcapphoto.h"
#include "CaptureVideo.h"
#include "MainFrm.h"
#include "CapImageFunc.h"
#include <gdiplus.h>
#include "IBAHelpper.h"
using namespace Gdiplus;
// CDlgCapPhoto 对话框
extern CEvent g_eCapEvent;
#define UM_VERIFYRET WM_USER + 1
IMPLEMENT_DYNAMIC(CDlgCapPhoto, CDialog)
CDlgCapPhoto::CDlgCapPhoto(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCapPhoto::IDD, pParent)
	, m_strName(_T(""))
	, m_strID(_T(""))
{
	m_pCap = new CCaptureVideo;
}

CDlgCapPhoto::~CDlgCapPhoto()
{
	if(m_pCap)
	{
		delete m_pCap ;
	}
}

void CDlgCapPhoto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PIC, m_static_Pic);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_ID, m_strID);
	DDX_Control(pDX, IDC_COMBO_CAMERADEVICES, m_comboDevices);
}


BEGIN_MESSAGE_MAP(CDlgCapPhoto, CDialog)
	ON_BN_CLICKED(IDC_BTNCAPPHOTO, OnBnClickedBtncapphoto)
	ON_MESSAGE(UM_VERIFYRET, OnVerifyRet)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgCapPhoto 消息处理程序

BOOL CDlgCapPhoto::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitFilePath();
	InitCamera();
	CJuBao* pJubao = ((CMainFrame*)AfxGetMainWnd)->GetJubaoInf();
	if(pJubao)
	{	
		pJubao->SetJubaoDelegate(this);
	}
	else
	{
		IBA_LOG0(_T("获取聚宝指针为空 Init"));
		ASSERT(FALSE);
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgCapPhoto::InitFilePath()
{
	CString strPath;
	strPath.Format(_T("%s\\%s"), theApp.GetWorkPath(), _T("JubaoImages"));
	CIBAHelpper::MakeSurePathExists(strPath);	// 确保路径存在
	m_strCapBmpPath		= strPath + _T("\\jubao.bmp");
	m_strCapBmpPathTmp	= strPath + _T("\\jubaotmp.bmp");
	m_strConvertJpgPath = strPath + _T("\\jubao.jpg");
	m_strJubaoRetPath	= strPath + _T("\\jubaoret.jpg");
}

void CDlgCapPhoto::OnBnClickedBtncapphoto()
{
	// TODO: 在此添加控件通知处理程序代码
	VerifyUI();
	UpdateData();
	m_threadVerify.Create(this, false);
}

void CDlgCapPhoto::VerifyUI()
{
	GetDlgItem(IDC_BTN_CAPPHOTO)->SetWindowText(_T("聚宝验证中"));
	GetDlgItem(IDC_BTN_CAPPHOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CHOOSECAMERA)->EnableWindow(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT_NAME))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_ID))->SetReadOnly(TRUE);
	GetDlgItem(IDC_COMBO_CAMERADEVICES)->EnableWindow(FALSE);
}

void CDlgCapPhoto::NormalUI()
{
	GetDlgItem(IDC_BTN_CAPPHOTO)->SetWindowText(_T("拍照并验证"));
	GetDlgItem(IDC_BTN_CAPPHOTO)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CHOOSECAMERA)->EnableWindow(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_NAME))->SetReadOnly(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT_ID))->SetReadOnly(FALSE);
	((CComboBox*)GetDlgItem(IDC_COMBO_CAMERADEVICES))->EnableWindow(TRUE);
}

LRESULT CDlgCapPhoto::OnVerifyRet(WPARAM wParam, LPARAM lParam)
{
	if(FALSE == lParam)
	{
		AfxMessageBox(_T("验证失败！"));
		NormalUI();
	}
	else
	{
		OnOK();
	}
	return 0;
}
void CDlgCapPhoto::CapAPhoto()
{
	m_pCap->GrabOneFrame(TRUE);
}

BOOL CDlgCapPhoto::GetBase64CapPhotoBuf(std::string& stbuf)
{
	BYTE *pBuffer;
	int nSize;
	if(!GetCapPhotoBuf(pBuffer, &nSize))
	{
		return FALSE;
	}
	stbuf= CJuBao::Encode((char*)pBuffer, nSize);
	return TRUE;
}

BOOL CDlgCapPhoto::GetCapPhotoBuf(BYTE *&pBuffer, int *bufLen)
{
	g_eCapEvent.ResetEvent();
	CapAPhoto();
	if( WaitForSingleObject( g_eCapEvent, 5 * 1000 ) != WAIT_OBJECT_0 )
	{
		IBA_LOG0(_T("设备超时"));
		return FALSE;
	}
	

	BOOL bFlag = FALSE;
	try
	{
		GdiplusStartupInput gdiplusStartupInput; 
		ULONG_PTR gdiplusToken = NULL;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 

		BMPScale( m_strCapBmpPathTmp, m_strCapBmpPath, 320, 240 );

		BMP2JPG( m_strCapBmpPathTmp, m_strConvertJpgPath );

		GdiplusShutdown(gdiplusToken);

		Sleep( 50 );
		CFile f;
		CFileStatus fStatus;
		if( f.Open( m_strConvertJpgPath, CFile::modeRead ) )
		{
			f.GetStatus( fStatus );
			pBuffer = new BYTE[(size_t)fStatus.m_size]; 
			f.Read( pBuffer, (UINT)fStatus.m_size );
			*bufLen = (int)fStatus.m_size;
			f.Close();
			bFlag = TRUE;
		}


	}
	catch (...)
	{
		bFlag = FALSE;
		IBA_LOG0(_T("照片转换成jpg出现错误！"));
	}
	return bFlag;
}

void CDlgCapPhoto::GetNameID( CString& strName, CString& strID )
{
	strName = m_strName; 
	strID = m_strID;
}

BOOL CDlgCapPhoto::InitCamera()
{
	CArray<CString, CString> arCapList;
	m_pCap->EnumDevices (arCapList);
	if(0 == arCapList.GetCount())
	{
		AfxMessageBox(_T("摄像头初始化失败！"));
		return FALSE;
	}
	else
	{
		for(int i = 0; i < arCapList.GetCount(); i++)
		{
			m_comboDevices.AddString(arCapList.GetAt(i));
		}
	}
	UINT nCameraIndex = CIBAConfig::GetInstance()->GetCameraIndex();
	if(nCameraIndex > m_comboDevices.GetCount())
	{
		nCameraIndex = 0;
		CIBAConfig::GetInstance()->SetCameraIndex(nCameraIndex);
	}
	m_comboDevices.SetCurSel (nCameraIndex); 

	HWND hWnd	= m_static_Pic.GetSafeHwnd() ; 
	HRESULT hr	= m_pCap->Init(nCameraIndex,hWnd);

	m_pCap->SetSaveFilePath(m_strCapBmpPath);
	return TRUE;
}

void CDlgCapPhoto::DoCheckEnd( char* username,char* idcardno ,DWORD checkresult,char * transNumber,char * bitmap )
{
	/*参数username：表示验证接口传过来的用户姓名 
	参数idcardno：表示验证接口传过来的身份证号； 
	参数checkresult：表示验证的结果 
	参数transNo：表示流水号 */
	// 获取其他信息 

	CString strName(username);
	CString strID(idcardno);
	CString strTransNum(transNumber);

	IBA_LOG0(strName); 
	IBA_LOG0(strID); 
	IBA_LOG0(strTransNum); 

	CString strRet("检查结果:");
	switch(checkresult) 
	{ 
		// 0是验证成功（证件有效） 
		// 1不一致 
		// 2无效证件 
		// 3出现其它错误 
		// 4网络请求超时 
		// 5拍照照片不存在 
		// 6还有个核查线程未终止 
		case 0: 
			strRet += _T("一致"); 
			break; 
		case 1: 
			strRet += _T("不一致"); 
			break; 
		case 2: 
			strRet += _T("无效证件"); 
			break; 
		case 3: 
			strRet += _T("出现其它错误"); 
			break; 
		case 4: 
			strRet += _T("网络请求超时"); 
			break; 
		case 5: 
			strRet += _T("拍照照片不存在"); 
			break; 
		case 6: 
			strRet += _T("还有个核查线程未终止"); 
			break;
		default:
			strRet += _T("未知");
	} 
	IBA_LOG0(strRet);

	// 获取图片 
	int OutByte=0; 
	std::string strTmpResult = CJuBao::Decode(bitmap,strlen(bitmap),OutByte); 
	int i,len = strTmpResult.length(); 

	BYTE *buf = new BYTE[len]; 

	for (i=0;i<len;++i) 
	{ 
		buf[i] = strTmpResult[i]; 
	} 

	FILE * pFile; 
	pFile = fopen ( CT2A(m_strJubaoRetPath) , "wb" ); 
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);} 
	fwrite(buf,1,len,pFile);

	PostMessage(UM_VERIFYRET, 0, (checkresult == 0) ? TRUE : FALSE);
}

void CDlgCapPhoto::didRun( CG2XThread* pThread )
{
	if(pThread)
	{
		CMainFrame* pMainWnd = ((CMainFrame*)theApp.GetMainWnd());
		CJuBao* pJubao = NULL;
		if(pMainWnd)
		{	
			pJubao = pMainWnd->GetJubaoInf();	
		}

		int nRet;
		if(pJubao)
		{
			char *pName = CT2A(m_strName);
			char *pID = CT2A(m_strID);
			std::string stPhoto;
			if(!GetBase64CapPhotoBuf(stPhoto))
			{
				IBA_LOG0(_T("获取Base64照片失败"));
				nRet = FALSE;
			}

			if(0 != (nRet =  pJubao->VerifyUserInfo(pName, pID, stPhoto)))
			{
				IBA_LOG(_T("聚宝验证接口返回失败:%d"), nRet);
				nRet = FALSE;
			}
			else
			{
				nRet = TRUE;
			}
		}
		else
		{
			IBA_LOG0(_T("获取聚宝指针为空"));
			ASSERT(FALSE);
			nRet = FALSE;
		}
	}	
}



void CDlgCapPhoto::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pCap->Stop();
	__super::OnClose();
}

void CDlgCapPhoto::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pCap->Stop();
	__super::OnOK();
}
