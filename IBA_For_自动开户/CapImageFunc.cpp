#include "stdafx.h"

#include <afxtempl.h>

#include <GdiPlus.h>
using namespace Gdiplus;

#include "CaptureVideo.h"
#include "IBALog.h"

// 不能用来做摄像头的视频源,不用的视频源以^^分隔,例如
// 鼎识扫描仪
const CString g_strUnuseCapture("IDR USB Device");

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCTSTR lpFileName )
{
	HDC hDC;					//设备描述表
	int iBits;					//当前显示分辨率下每个像素所占字节数
	WORD wBitCount;				//位图中每个像素所占字节数
	DWORD dwPaletteSize=0,		//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数
		dwBmBitsSize,
		dwDIBSize, dwWritten;
	BITMAP Bitmap;				//位图属性结构
	BITMAPFILEHEADER bmfHdr;	//位图文件头结构
	BITMAPINFOHEADER bi;		//位图信息头结构
	LPBITMAPINFOHEADER lpbi;	//指向位图信息头结构

	HANDLE fh, hDib, hPal,hOldPal=NULL; //定义文件，分配内存句柄，调色板句柄

	//计算位图文件每个像素所占字节数
	HDC hWndDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	hDC = ::CreateCompatibleDC( hWndDC ) ;
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);

	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 24 ;

	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32) * 4 * Bitmap.bmHeight ;

	//为位图内容分配内存
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	// 处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+dwPaletteSize,
		(LPBITMAPINFO )
		lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件
	fh = CreateFile(lpFileName, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	// 设置位图文件头
	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
		+ (DWORD)sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize;

	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize,
		&dwWritten, NULL);

	//清除
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return TRUE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;			// number of image encoders
	UINT size = 0;			// size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;			// Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;			// Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;   // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;   // Failure
}

void BMP2JPG(CString strBMPFile, CString strJPGFile)
{
	CLSID   encoderClsid;
	WCHAR   strGuid[39];
	if(GetEncoderClsid(L"image/jpeg", &encoderClsid) >= 0)
		StringFromGUID2(encoderClsid, strGuid, 39);

	Image image(strBMPFile.AllocSysString());
	image.Save(strJPGFile.AllocSysString(),&encoderClsid,NULL);
}

void BMPScale(CStringW strDstBmpFile, CStringW strSrcBmpFile, int nWidth, int nHeight)
{
	//LPWSTR pwcsSrcPic = (LPWSTR)malloc(sizeof(WCHAR) * (strSrcBmpFile.GetLength() + 1));
	//MultiByteToWideChar (CP_ACP, 0, strSrcBmpFile, -1, pwcsSrcPic, strSrcBmpFile.GetLength());
	////MultiByteToWideChar (CP_ACP, 0, strSrcBmpFile, -1, NULL, 0);
	//*(pwcsSrcPic + strSrcBmpFile.GetLength()) = 0;
	Bitmap image(strSrcBmpFile);

	Image* pScaledImage = NULL;

	pScaledImage = image.GetThumbnailImage(nWidth, nHeight, NULL, NULL);

	CLSID   encoderClsid;
	WCHAR   strGuid[39];
	if(GetEncoderClsid(L"image/bmp", &encoderClsid) >= 0)
		StringFromGUID2(encoderClsid, strGuid, 39);

	pScaledImage->Save(strDstBmpFile.AllocSysString(),&encoderClsid,NULL);

}
