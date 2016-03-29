#pragma once

G2XUI_BEGIN

//-------------------------------------------------------------------------
//- CG2XDrawMask
//
//- 绘图蒙板, 所谓蒙板是指具有特殊规则透明度的位图。
//- 该位图和指定图像进行Alpha混合会产生特定的效果
//- 例如: Foxmail的标题栏的背景颜具有一定的渐变规则，实际上标题栏只有一种颜色,Foxmail
//-    改变主题时，只更换了颜色。而蒙板无需做任何改变，却可以产生一样渐变效果的背景。
//- 例如:
//     QQ电脑管家的工具栏，当鼠标滑过按钮时，出现的半透明圆角矩形效果。在这里都可认为是蒙板
//-------------------------------------------------------------------------
class G2X_CLASSES CG2XDrawMask
{
protected:
	// 不允许默认创建
	CG2XDrawMask();

public:
	virtual ~CG2XDrawMask();

public:

	//---------------------------------------------------------------------
	//- 创建一个增加亮度的矩形蒙板
	//-		说明: 与背景混合后，背景会变的更亮(更加趋于clrMask指定的颜色)
	//
	//- nWidth:  蒙板的宽度
	//- nHeight: 蒙板的高度
	//- nBrightness: 要增加的亮度, 取值范围0~100
	//- clrMask: 增加亮度后图像偏向的颜色，默认白色亮度最大
	//---------------------------------------------------------------------
	static CG2XDrawMask* CreateMaskIncreaseBrightness(
		INT nWidth,
		INT nHeight,
		BYTE nBrightness,
		COLORREF clrMask=RGB(255,255,255));

	//---------------------------------------------------------------------
	//- 创建一个降低亮度的矩形蒙板
	//-		说明: 与背景混合后，背景会变的更暗(更加趋于clrMask指定的颜色)
	//
	//- nWidth: 蒙板的宽度
	//- nHeight: 蒙板的高度
	//- nBrightness: 要降低的亮度, 取值范围0~100
	//- clrMask: 降低亮度后图像偏向的颜色
	//---------------------------------------------------------------------
	static CG2XDrawMask* CreateMaskDecreaseBrightness(
		INT nWidth,
		INT nHeight,
		BYTE nBrightness,
		COLORREF clrMask=RGB(0,0,0));

	//---------------------------------------------------------------------
	//- 创建渐变亮度的蒙板,图像按照clrMask方向变化
	//- style: 渐变风格,(G2XGradientStyle的定义，可知亮度参数的传入)
	//- nWidth: 蒙板的宽度
	//- nHeight: 蒙板的高度
	//- nBrightness1: 渐变的起始亮度
	//- nBrightness2: 渐变的中间亮度或者终止亮度
	//- nBrightness3: 渐变的中间亮度或者无效
	//- nBrightness4: 渐变的终止亮度或者无效
	//- clrMask: 蒙板的颜色
	//---------------------------------------------------------------------
	static CG2XDrawMask* CreateMaskGradientBrightness(
		INT nWidth,
		INT nHeight,
		G2XGradientStyle style,
		BYTE nBrightness1,
		BYTE nBrightness2,
		BYTE nBrightness3,
		BYTE nBrightness4,
		COLORREF clrMask=RGB(255,255,255)
		);

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	static CG2XDrawMask* CreateMaskRaisedBirghtness(
		INT nWidth,
		INT nHeight,
		BOOL bHorizontal,
		BYTE nBrightness,
		COLORREF clrMask=RGB(255,255,255)
		);

	static CG2XDrawMask* CreateMaskDepressedBirghtness(
		INT nWidth,
		INT nHeight,
		BOOL bHorizontal,
		BYTE nBrightness,
		COLORREF clrMask=RGB(255,255,255)
		);

	//---------------------------------------------------------------------
	//- 从一幅图像创建蒙板
	//---------------------------------------------------------------------
	static CG2XDrawMask* CreateMaskFromImage(
		Image* pImage);

public:
	//---------------------------------------------------------------------
	//- 获取蒙板图像
	//- 对返回的图像加工，蒙板也会发生相应的变化
	//---------------------------------------------------------------------
	Bitmap* GetBitmap();

	//- 获取蒙板的宽度
	INT GetWidth();

	//- 获取蒙板的高度
	INT GetHeight();

	//---------------------------------------------------------------------
	//- 将蒙板显示到Graphics对象
	//- 将蒙板完全显示在指定的位置
	//---------------------------------------------------------------------
	void ShowToGraphics(
		Graphics* pGraphics, 
		int nDesX, 
		int nDesY);
	
	//---------------------------------------------------------------------
	//- 将蒙板显示到Graphics对象
	//- 显示蒙板的特定区域到指定的位置
	//---------------------------------------------------------------------
	void ShowToGraphics(
		Graphics* pGraphics, 
		int nDesX, 
		int nDesY, 
		int nSrcX,
		int nSrcY, 
		int nSrcWidth, 
		int nSrcHeight);

	//---------------------------------------------------------------------
	//- 将蒙板显示到Graphics对象
	//- 将蒙板显示在指定的区域
	//---------------------------------------------------------------------
	void ShowToGraphics(
		Graphics* pGraphics, 
		LPCRECT lpDesRect);

	//---------------------------------------------------------------------
	//- 将蒙板显示到Graphics对象
	//- 将蒙板的特定区域显示在指定的区域
	//---------------------------------------------------------------------
	void ShowToGraphics(
		Graphics* pGraphics, 
		LPCRECT lpDesRect, 
		LPCRECT lpSrcRect);

private:
	Bitmap* m_pBitmap; // 位图对象指针 (蒙板也是位图)

public:
	

	
	
	
	
	
	

};

G2X_END