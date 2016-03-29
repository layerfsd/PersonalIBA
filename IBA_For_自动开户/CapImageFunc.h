#pragma once

// 将图片对象保存到bmp文件
BOOL	SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName );
// 获取图片编码格式对应的clsid
int		GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
// 将bmp格式图片文件转换成jpg格式图片文件
void	BMP2JPG(CString strBMPFile, CString strJPGFile);
// 按照指定大小调整图片
void	BMPScale(CStringW strDstBmpFile, CStringW strSrcBmpFile, int nWidth, int nHeight);