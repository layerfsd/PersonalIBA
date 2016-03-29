#pragma once


G2XUI_BEGIN

enum G2XBorderStyle
{
	G2XBorderNone,				// 没有边框
	G2XBorderInner,				// 内边框, (边框在控件区域内)
	G2XBorderOuter				// 外边框, (边框在控件区域外)
};
// 控件的显示状态, 控件画图时需要绘制这4种状态
enum G2XControlState
{
	G2XControlStateNormal = 0,  // 控件正常显示状态
	G2XControlStateHover = 1,   // 控件鼠标位于上方滑动的状态
	G2XControlStateSinkage = 2, // 控件下沉状态,即鼠标按下左键没有释放时的状态
	G2XControlStateDisable = 3  // 控件不可用状态
};

#define G2XControlStateCount 4  // 控件的状态数量为4种

// 文本水平对齐方式
enum G2XTextHAlign
{
	G2XTextHAlignLeft = 0,
	G2XTextHAlignCenter = 1,
	G2XTextHAlignRight = 2
};

// 文本垂直方向对齐方式
enum G2XTextVAlign
{
	G2XTextVAlignTop = 0,
	G2XTextVAlignMiddle = 1,
	G2XTextVAlignBottom = 2
};

// 滚动文字控件的风格
enum G2XRollTextStyle
{
	G2XRollTextStyleTop = 0,    // 文字从下往上滚动
	G2XRollTextStyleBottom =1,  // 文字从上往下滚动
	G2XRollTextStyleLeft =2,    // 文字从右往左滚动
	G2XRollTextStyleRight = 3   // 文字从左往右滚动
};

// 按钮风格
enum G2XButtonStyle
{
	G2XButtonStylePushButton,         // 通用的按钮样式,背景和文字
	G2XButtonStyleLink,               // 链接样式的按钮
	G2XButtonStyleAlphaButton,        // 带Alpha通道的button,有文本,默认背景色是透明的
	G2XButtonStyleImageButton,        // 图像按钮,没有文字,图像拉伸填充到按钮的整个区域
	G2XButtonStyleImageButtonEx       // 扩展的图像按钮，按钮的右边有文本,按钮背景是透明的
};

// 进度条风格
enum G2XProgressStyle
{
	G2XProgressStyleGeneral,          // 一般普通样式
	G2XProgressStyleXP,               // XP样式的进度条
	G2XProgressStyleWin7,             // WIN7样式的进度条
	G2XProgressStyleBalls,            // 环形多个球状动态进度球
	G2XProgressStyleCustum            // 自定义
};

// 系统按钮条风格
enum G2XSysBtnBarStyle
{
	G2XSysBtnBarStyleCustom,          // 自定义风格，需要重载CG2XSysBtnBar::DrawCustomButton方法
	G2XSysBtnBarStyleQQSoftMgr,       // 像QQ电脑管家的软件管理一样的按钮风格
	G2XSysBtnBarStyleQQ2012           // 像QQ2012一样的按钮风格
};

// 渐变风格
enum G2XGradientStyle
{
	G2XGradientStyleNull,             // 没有任何渐变效果
	G2XGradientStyleVLiner,           // 垂直方向线性渐变,需指定2个颜色值
	G2XGradientStyleVBlock,           // 垂直方向分成上下两个块, 两个块分别渐变,需指定4个颜色值
	G2XGradientStyleHLiner,           // 水平方向线性渐变,需指定2个颜色值
	G2XGradientStyleHBlock,           // 水平方向分成左右两个块,两个块分别渐变,需指定4个颜色值
	G2XGradientStyleEllipseInner,     // 内部椭圆渐变,需指定两个颜色值,边界颜色和中心颜色
	G2XGradientStyleEllipseOuter      // 外部椭圆渐变,需指定两个颜色值,边界颜色和中心颜色
};

// 面板中控件的放置样式
enum G2XPanelPlace
{
	G2XPanlePalceNormal,       // 控件保持不变

	G2XPanelPlaceFollowRight,  // 控件位置跟随面板右边界
	G2XPanelPlaceFollowBottom, // 控件位置跟随面板下边界
	G2XPanelPlaceFollowRB,     // 控件位置跟随面板右下角

	G2XPanelPlaceAutoWidth,    // 控件位置不变化，宽度自动变化，高度保持不变
	G2XPanelPlaceAutoHeight,   // 控件位置不变化，宽度不变化，高度自动变化
	G2XPanelPlaceAutoSize,     // 控件位置不变化，控件大小自动变化

	G2XPanelPlaceFollowAutoWidth, // 控件位置跟随面板下边界并且控件宽度自动变化,高度不变化
	G2XPanelPlaceFollowAutoHeight// 控件位置跟随面板右边界并且控件高度自动变化，宽度不变化
};

// 标题栏风格
enum G2XCaptionBarStyle
{
	G2XCaptionBarStyleMinBox = 0x0001,
	G2XCaptionBarStyleMaxBox = 0x0002,
	G2XCaptionBarStyleCloseBox = 0x0004,
	G2XCaptionBarStyleSysButton = (G2XCaptionBarStyleMinBox|G2XCaptionBarStyleMaxBox|G2XCaptionBarStyleCloseBox)
};

// 可分割面板的面板分割样式
enum G2XSplitPanelStyle
{
	G2XSplitPanelStyleHorizontal, // 水平方向两个块
	G2XSplitPanelStyleVertical    // 垂直方向两个块
};

enum G2XSplitPanelSizeStyle
{
	G2XSplitPanelSizeStylePercentage,// 分隔面板的大小改变时，2个面板的宽度按比例变化
	G2XSplitPanelSizeStyleFirst,     // 分隔面板的大小改变时，第一个面板的宽度跟随变化，但第二个面板的宽度不变
	G2XSplitPanelSizeStyleSecond     // 分隔面板的大小改变时，第二个面板的宽度跟随变化，但第一个面板的宽度不变
};

// 排序风格
enum G2XSortStyle
{
	G2XSortStyleNormal=0,   // 不排序
	G2XSortStyleDesc=-1,    // 降序
	G2XSortStyleAsce=1      // 升序
};

// 节点状态
enum G2XNodeState
{
	G2XNodeStateNone,     // 没有子节点
	G2XNodeStateExpanded, // 子节点列表是展开的
	G2XNodeStateFolded    // 子节点列表是折叠起来的
};

G2X_END