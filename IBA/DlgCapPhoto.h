#pragma once
#include "afxwin.h"
#include "RealName\JuBao.h"
#include "G2XThreadTemplate.h"

// CDlgCapPhoto 对话框
class CCaptureVideo;
class CDlgCapPhoto : public CDialog
	, public CJubaoDelegate
	, public CG2XThreadDelegate
{
	DECLARE_DYNAMIC(CDlgCapPhoto)

public:
	CDlgCapPhoto(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCapPhoto();

// 对话框数据
	enum { IDD = IDD_DLG_CAPPHOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void DoCheckEnd(char* username,char* idcardno ,DWORD checkresult,char * transNumber,char * bitmap);
	virtual void didRun(CG2XThread* pThread);
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnBnClickedBtncapphoto();
	virtual BOOL OnInitDialog();
	BOOL InitCamera();
	
	CCaptureVideo* m_pCap;
	CString m_strName;
	CString m_strID;
	CString m_strCapBmpPath;
	CString m_strCapBmpPathTmp;
	CString m_strConvertJpgPath;
	CString m_strJubaoRetPath;
public:
	void GetNameID(CString& strName, CString& strID);
	void CapAPhoto();
	BOOL GetBase64CapPhotoBuf(std::string& stbuf);
	BOOL GetCapPhotoBuf(BYTE *&pBuffer, int *bufLen);
	void InitFilePath();
	void NormalUI();
	void VerifyUI();
	LRESULT OnVerifyRet(WPARAM wParam, LPARAM lParam);
	CStatic m_static_Pic;
	CComboBox m_comboDevices;
	CG2XThread m_threadVerify;
	afx_msg void OnClose();
protected:
	virtual void OnOK();
};
