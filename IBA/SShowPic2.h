#pragma once

// 解决CSShowPic有些JPG图片无法显示的问题(GDI+的问题)
class CSShowPic2 : public CStatic
{
	DECLARE_DYNAMIC(CSShowPic2)

public:
	CSShowPic2();
	virtual ~CSShowPic2();
	// 按照路径显示一张图片
	void ShowPic(LPCTSTR szFullPath);
	void SetPicBk(COLORREF clr) { m_clr = clr; }		// 设置照片底色
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

private:
	CString m_strFullPath;
	COLORREF m_clr;
};
bool ShowJpgGif(CDC* pDC,CString strPath, int x, int y, int width, int height);
