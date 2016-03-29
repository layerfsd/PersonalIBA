
#include "IBADialog.h"
#include "NetBarUser.h"

class CUploadImageDlg : public CIBADialog
{
	DECLARE_DYNAMIC(CUploadImageDlg)
	DECLARE_MESSAGE_MAP()

public:
	CUploadImageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUploadImageDlg();

// 对话框数据
	enum { IDD = IDD_RN_REGIMAGEDLG };
	
	static const UINT UI_UPLOAD_NOTCLOSE = 512;
	static const UINT UI_UPLOAD_PERSONIMAGE = 1024;
	static const UINT UI_UPLOAD_IDIMAGE = 2048;

	BOOL ShowModal(CNetBarUser& NetBarUser, UINT nOption = UI_UPLOAD_PERSONIMAGE | UI_UPLOAD_IDIMAGE);

private:
	
	CL2TKPictureBox m_IDPictureBox;
	CL2TKDirectShowCamera m_Camera;

	CNetBarUser m_NetBarUser;

	UINT m_nOption;

	UINT m_nCounter;
	
protected:

	using CIBADialog::DoModal;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual void OnCancel();

	BOOL SendToProxy(CByteArray &buffer);
	BOOL SendImage(UINT nType);

};
