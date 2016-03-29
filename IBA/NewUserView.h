#pragma once


#include "IBAFormView.h"
// CNewUserView 窗体视图

class CNewUserView : public CIBAFormView
{
	DECLARE_DYNCREATE(CNewUserView)

public:
	CNewUserView();           // 动态创建所使用的受保护的构造函数
	virtual ~CNewUserView();

	enum {IDD = IDD_FORMVIEW_NEWUSER};

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

};


