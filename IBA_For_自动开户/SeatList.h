#pragma once


#include "Seat.h"

// CSeatList 命令目标

class CSeatList : public CObject
{
public:
	CSeatList();
	virtual ~CSeatList();

private:
	CTypedPtrArray <CObArray, CSeat *> m_SeatList;

public:
	INT Add(CSeat * Seat);
	void InsertAt(INT nIndex, CSeat * newSeat,INT nCount = 1);
	void RemoveAt(INT nIndex, INT nCount = 1);
	void RemoveAll();
	void DeleteAt(INT nIndex, INT nCount = 1);
	void DeleteAll();
	INT GetSize();
	CSeat* GetAt(INT nIndex);
	INT Append(const CSeatList &src);
	void Copy(const CSeatList &src);

public:
	BOOL SaveList();
	BOOL ReadList();

	void DrawAllRect(CDC * pDC = NULL, double XRatio = 1.0, double YRatio = 1.0);
	//点是否进入元素范围，进入则返回序号，反之返回-1
	INT InRect(CPoint MousePoint);
	//CString GetComputerIP(CString strTreminID) const;


};


