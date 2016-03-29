#pragma once

#include <atlimage.h>

class L2_EXT_CLASS CL2TKPictureBox : public CStatic
{
	DECLARE_DYNAMIC(CL2TKPictureBox)
	DECLARE_MESSAGE_MAP()

public:

	CL2TKPictureBox();
	virtual ~CL2TKPictureBox();

	BOOL LoadFile(LPCTSTR lpszFileName, BOOL IsPNG = FALSE);
	void LoadFromResource(HINSTANCE hInstance, UINT nIDResource, BOOL IsPNG = FALSE);

	BOOL GetStretch() const;
	void SetStretch(BOOL newVal = TRUE);

protected:

	BOOL m_bStretch;
	CImage m_Image;

	void DoForPNG();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();


};


