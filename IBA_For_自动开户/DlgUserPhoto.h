#pragma once
#include "afxwin.h"

#include "SShowPic.h"
// CDlgUserPhoto 对话框

class CDlgUserPhoto : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgUserPhoto)

public:
	CDlgUserPhoto(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgUserPhoto();

// 对话框数据
	enum { IDD = IDD_DLG_USER_PHOTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	CSShowPic m_staticPhoto;
};
