#pragma once

enum E_WNDMODE
{
	e_GeneralMode	= 1, //普通类型
	e_QQMode = 2	//	QQ类型

};

typedef struct Tips_ShowParam
{
	size_t length; //自身结构体的大小

	INT    nPublicID;
	
	SYSTEMTIME startTime; //
	SYSTEMTIME endTime; //
	
	INT		nShowWay;			//弹出还是闪烁
	BOOL    bFullScreenShow;	//全屏是否弹出+

	INT     nDelayDisplayTime;      //登录以后延迟显示时间
	INT     nDelayDisappearTime;	//渐隐的时间 

                                //下面是没传递的数据
	INT     nContentType;       //内容类型，URL连接还是文件内容
	RECT    DistanceRect; //保存与边界距离的数据

	E_WNDMODE eWndMode;     
}TIPS_SHOWPARAM, *PTIPS_SHOWPARAM;


typedef struct Tips_ManagerParam
{
	size_t length; //自身结构体的大小


}TIPS_MANAGERPARAM, *PTIPS_MANAGERPARAM;