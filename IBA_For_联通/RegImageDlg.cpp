// RegImageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "NetBarUser.h"
#include "RealName\Proxy.h"
#include "Socket\LocalServer.h"
#include ".\RegImageDlg.h"

using namespace NS_REALNAME;

// CRegImageDlg 对话框

IMPLEMENT_DYNAMIC(CUploadImageDlg, CIBADialog)
CUploadImageDlg::CUploadImageDlg(CWnd* pParent /*=NULL*/)
: CIBADialog(CUploadImageDlg::IDD, pParent)
, m_nCounter(5), m_nOption(0)
{
}

CUploadImageDlg::~CUploadImageDlg()
{
}

void CUploadImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CIBADialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_IDIMAGE, m_IDPictureBox);
	DDX_Control(pDX, IDC_STATIC_PERSONGIMAGE, m_Camera);
}

BEGIN_MESSAGE_MAP(CUploadImageDlg, CIBADialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CRegImageDlg 消息处理程序

BOOL CUploadImageDlg::OnInitDialog()
{
	CIBADialog::OnInitDialog();

	if (m_nOption & UI_UPLOAD_NOTCLOSE) //不允许关闭
	{
		GetSystemMenu(FALSE)->RemoveMenu(1, MF_BYPOSITION);
	}

	if (m_nOption & UI_UPLOAD_IDIMAGE) //上传证件照片
	{
		m_IDPictureBox.LoadFile(theApp.GetWorkPath() + _T("\\IBATemp\\ID.jpg"));
	}

	if (m_nOption & UI_UPLOAD_PERSONIMAGE) //上传人脸照片
	{
		m_Camera.Start(CIBAConfig::GetInstance()->GetPersonImageDrvId());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUploadImageDlg::OnBnClickedOk()
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	BOOL bRet = FALSE;

	if (m_nOption & UI_UPLOAD_IDIMAGE)
	{
		bRet = SendImage(1);	
	}
	else
	{
		bRet = TRUE;
	}

	if (m_nOption & UI_UPLOAD_PERSONIMAGE)
	{
		if (bRet)
		{
			bRet = SendImage(2);
		}
	}

	if (bRet)
	{
		m_Camera.Stop();

		CBCGPDialog::OnOK();
	}
	else
	{
		m_nCounter--;

		CString strTmp;
		strTmp.LoadString(IDS_CONNETPROYERROR);

		strTmp.AppendFormat(_T(" -  [%d]"), m_nCounter);

		MsgBox(strTmp, MB_ICONINFORMATION);

		if (m_nCounter == 0)
		{
			m_Camera.Stop();
			CBCGPDialog::OnCancel();
		}
	}

	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

BOOL CUploadImageDlg::ShowModal(CNetBarUser& NetBarUser, UINT nOption)
{
	m_nOption = nOption;
	m_NetBarUser = NetBarUser;

	return (DoModal() == IDOK);
}

void CUploadImageDlg::OnCancel()
{
	if (m_nOption & UI_UPLOAD_NOTCLOSE) //不允许关闭
	{
		
	}
	else
	{
		CIBADialog::OnCancel();
	}
}

BOOL CUploadImageDlg::SendToProxy(CByteArray &buffer)
{
	BOOL bRet = FALSE;

	CSocket ProxySocket;

	if (ProxySocket.Create())
	{
		if (ProxySocket.Connect(CNetBarConfig::GetInstance()->GetProxyIP(), 7834))
		{
			if (ProxySocket.Send(buffer.GetData(), buffer.GetSize()) > 0)
			{
				bRet = TRUE;
			}
		}
		else
		{
			CIBALog::GetInstance()->WriteFormat(_T("Can't connect to %s(7834)!"), CNetBarConfig::GetInstance()->GetProxyIP());
		}

		ProxySocket.Close();
	}

	return bRet;
}

BOOL CUploadImageDlg::SendImage(UINT nType)
{
	NET_HEAD head = {0};
	head.Version = COM_VER_1;
	head.Length = sizeof(ID_IMAGE);

	CString strFile(theApp.GetWorkPath() + _T("\\IBATemp\\"));
	
	if (nType == 1)
	{
		head.Cmd = C_IDIMG;
		strFile.Append(_T("ID.jpg"));
	}
	else
	{
		head.Cmd = C_CAPIMG;

		strFile.Append(_T("Person.jpg"));
		m_Camera.TakePicture(strFile);
	}
	
	ID_IMAGE IDImage = {0};

	IDImage.idtype = m_NetBarUser.GetCertificateType();
	lstrcpynA(IDImage.IdNumber, CT2A(m_NetBarUser.GetCertificateID()), LEN_ID);
	lstrcpynA(IDImage.name, CT2A(m_NetBarUser.GetUserName()), LEN_NAME);

	CFile ImageFile;

	if (ImageFile.Open(strFile, CFile::modeRead)) 
	{
		if (ImageFile.GetLength() < LEN_IMAGE) 
		{
			ImageFile.Read(IDImage.image, (UINT)ImageFile.GetLength());
			IDImage.imageSize = (INT)ImageFile.GetLength();		
		}
		else
		{
			//做什么呢？文件太大了
			CIBALog::GetInstance()->Write(_T("The image file is too large!"));
		}

		ImageFile.Close();
	}
	else
	{
		CIBALog::GetInstance()->Write(_T("Failed to open file!"));
	}

	CByteArray buffer;
	buffer.SetSize(sizeof NET_HEAD + head.Length);

	memcpy(buffer.GetData(), &head, sizeof NET_HEAD);
	memcpy(buffer.GetData() + sizeof NET_HEAD, &IDImage, head.Length);

	return SendToProxy(buffer);
}
