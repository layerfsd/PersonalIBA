#pragma once

G2XUI_BEGIN

class G2X_CLASSES CG2XImageList
{
public:
	CG2XImageList();
	virtual ~CG2XImageList();

	//- 清除所有图像
	void Delete();

	//---------------------------------------------------------------------
	//- 创建
	//- nIDRes: 资源ID
	//- nImgWidth: 子图像的宽度, 图像列表的子图像个数=图像的宽度/nImgWidth, 向上取整
	//- lpszResType: 位图指定为RT_BITMAP,自定义的需要指定名称,例如:"PNG"
	//---------------------------------------------------------------------
	BOOL Create(UINT nIDRes, INT nImgWidth, LPCTSTR lpszResType=RT_BITMAP, BOOL bHorizontal=TRUE);

	//---------------------------------------------------------------------
	//- 创建
	//- lpstrImgPath: 图像的路径
	//- nImgWidth: 子图像的宽度, 图像列表的子图像个数=图像的宽度/nImgWidth, 向上取整
	//---------------------------------------------------------------------
	BOOL Create(LPCTSTR lpstrImgPath, INT nImgWidth, BOOL bHorizontal=TRUE);

	//---------------------------------------------------------------------
	//- 创建
	//- img: 图像对象
	//- nImgWidth: 子图像的宽度, 图像列表的子图像个数=图像的宽度/nImgWidth, 向上取整
	//---------------------------------------------------------------------
	BOOL Create(Image& img, INT nImgWidth,  BOOL bHorizontal=TRUE);
	
	//- 从其他图像列表克隆
	BOOL CreateFrom(const CG2XImageList& imgList);
	
	//- 追加一个子图像
	BOOL AddImage(LPCTSTR lpwstrPath);

	//- 追加一个子图像
	BOOL AddImage(Image& img);

	//- 获取第一个图像的大小
	SIZE GetImgSize() const;

	//- 获取某个图像的大小
	SIZE GetImgSize(INT iImage) const;

	//- 得到子图像的个数
	INT GetImgCount() const;

	//- 是否有子图像
	BOOL HasImage();

	//- 绘制子图像
	void DrawImg(Gdiplus::Graphics* pGraphics, INT nIndex, INT nDesX, INT nDesY);

	//- 绘制子图像
	void DrawImg(Gdiplus::Graphics* pGraphics, INT nIndex, LPRECT lpRectDes, BOOL bGray=FALSE);

	//- 设置图像的透明色, 是的绘制图像时，不绘制透明色指定的颜色点
	void SetTranparentColor(ARGB clr);


	std::vector<Gdiplus::Bitmap*> m_Images;
};

G2X_END