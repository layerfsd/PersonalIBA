#pragma once

#include "ComputerInfo.h"
#include "Socket/LocalServer.h"
// CSeat 命令目标
// CSeat是中心坐标为主
// CSeat2是左上角坐标为主
class CSeat : public CObject
{
public:
	CSeat(LPCTSTR szSeatPath = NULL);
	virtual ~CSeat();
private:
	//位置信息
	CPoint		m_cpCenter;//座位中心坐标
	INT_PTR		m_nRadius;//座位外接圆半径
	CString		m_strTerminalID;//终端代号
	CString		m_strTerminalIP;//终端IP
	INT_PTR		m_nDirection;//座椅方向，分为8方向
	INT_PTR		m_ComputerStatus;//计算机状态
	static CBitmap		*m_StatusBitmap;//座椅图				// 2014-8-19 - qsc 改为static
	static BOOL			m_bIsBmpAttach;//图形是否已经绑定		// 2014-8-19 - qsc 改为static
	COLORREF	m_crBlack;
	COLORREF	m_crWhite;
	BOOL		m_bStatusChange;
	INT_PTR		m_PreComputerStatus;//计算机状态
public:
	BOOL GetBmpFlag() const { return m_bIsBmpAttach; }

	void GetRect(CRect& denRect);

	INT_PTR GetComputerStatus() const { return m_ComputerStatus; }

	CPoint GetPosition() const { return m_cpCenter; }
	void  SetPosition(CPoint newPosition) { m_cpCenter = newPosition; }

	INT_PTR GetRadius() const { return m_nRadius; }
	void SetRadius(INT_PTR newRadius) { m_nRadius = newRadius; }

	INT_PTR GetDirection() const { return m_nDirection; }
	void SetDirection(INT_PTR newDirection) { m_nDirection = newDirection; }

	CString GetTerminalID() const { return m_strTerminalID; }
	void SetTerminalID(CString newValue) { m_strTerminalID = newValue; }

	CString GetTerminalIP() const { return m_strTerminalIP; }
	void SetTerminalIP(CString newValue) { m_strTerminalIP = newValue; }
 
	CComputerInfo& GetComputerInfo(); //为防止不一致，采用外部调用方法

	/************************************************************************/
	/* 绘制图形，如果没有载入图形，绘制矩形                                                                     */
	/************************************************************************/
	void Draw(CDC * pDC, double XRatio = 1.0, double YRatio = 1.0);
	void Draw(CDC * pDC, CPoint ptCenter, double XRatio = 1.0, double YRatio = 1.0);

	//绘制矩形，默认
	void DrawRect(CDC * pDC = NULL, double XRatio = 1.0, double YRatio = 1.0, BOOL bDrawTermID = TRUE);
	void DrawRect(CDC * pDC, CPoint Centerpoint, double XRatio = 1.0, double YRatio = 1.0, BOOL bDrawTermID = TRUE);
	//绘制图像，存在图形时
	void DrawBmp(CDC * pDC, CPoint ptCenter, double XRatio = 1.0, double YRatio = 1.0);
	void DrawBmp(CDC * pDC, double XRatio = 1.0, double YRatio = 1.0);

	BOOL InvalidateIfStatusChange(HWND hWnd);
	BOOL IsStatusChange();
private:
	// 绑定对应座椅图
	BOOL InitImage(LPCTSTR szSeatPath = NULL);
	//取得对应计算机状态的图形标号
	BOOL UpdateComputerStatus();
	void DrawTermID(CDC* pDC, int x, int y);
	BOOL UpdateComputerStatus2();
	
};


