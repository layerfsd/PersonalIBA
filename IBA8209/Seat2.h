#pragma once

#include "ComputerInfo.h"
#include "Socket/LocalServer.h"
#include "IBACommDefine.h"
// CSeat2 命令目标
// CSeat是中心坐标为主
// CSeat2是左上角坐标为主
using namespace NS_IBACommDef;
class CSeat2 : public CObject
{
public:
	CSeat2(Seat2Type type);
	virtual ~CSeat2();

private:
	//位置信息
	CPoint		m_cpCenter;//座位中心坐标
	CPoint		m_cpLT;//左上角
	INT_PTR		m_nRadius;//座位外接圆半径
	CBitmap *	m_StatusBitmap;//座椅图
	BOOL		m_bIsBmpAttach;//图形是否已经绑定
	COLORREF	m_crBlack;
	COLORREF	m_crWhite;
	//CString		m_strSeatName;
	Seat2Type	m_Seat2Type;
	int			m_nBmpHeight;
	int			m_nBmpWidth;
public:
	BOOL GetBmpFlag() const { return m_bIsBmpAttach; }

	void GetRect(CRect& denRect);

	CPoint GetPosition() const;
	CPoint GetCenterPos() const;
	void  SetPosition(CPoint newPosition);
	void SetCenterPos(CPoint newPosition);
	
	INT_PTR GetRadius() const { return m_nRadius; }
	void SetRadius(INT_PTR newRadius) { m_nRadius = newRadius; }

	CComputerInfo& GetComputerInfo(); //为防止不一致，采用外部调用方法

	/************************************************************************/
	/* 绘制图形，如果没有载入图形，绘制矩形                                                                     */
	/************************************************************************/
	void Draw(CDC * pDC, double XRatio = 1.0, double YRatio = 1.0);
	void Draw(CDC * pDC, CPoint ptCenter, double XRatio = 1.0, double YRatio = 1.0);

	//绘制矩形，默认
	void DrawRect(CDC * pDC = NULL, double XRatio = 1.0, double YRatio = 1.0);
	void DrawRect(CDC * pDC, CPoint Centerpoint, double XRatio = 1.0, double YRatio = 1.0);
	//绘制图像，存在图形时
	void DrawBmp(CDC * pDC, CPoint ptCenter, double XRatio = 1.0, double YRatio = 1.0);
	void DrawBmp(CDC * pDC, double XRatio = 1.0, double YRatio = 1.0);
	
	Seat2Type GetSeat2Type() { return m_Seat2Type; }
private:
	// 绑定对应座椅图
	BOOL InitImage();
	void DrawSeatName(CDC* pDC, int x, int y);
	//取得对应计算机状态的图形标号
};


