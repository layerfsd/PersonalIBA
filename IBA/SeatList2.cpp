// SeatList.cpp : 实现文件
//

#include "stdafx.h"
#include "IBA.h"
#include "SeatList2.h"
#include "IBAHelpper.h"
#include "SeatLayoutView.h"

// CSeatList2

CSeatList2::CSeatList2()
{
}

CSeatList2::~CSeatList2()
{
	DeleteAll();
}

///////////////////////////////////////////////////////
BOOL CSeatList2::SaveList()
{
 	CString szPath, strKey, strPos, strAppName;
	szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\SeatConfig.ini");

	CFile tFile;
	if(!PathFileExists(szPath))
	{
		if (tFile.Open(szPath, CFile::modeCreate|CFile::modeReadWrite))
		{
			tFile.Close();
		}
	}

	int nIBACount = 0, nWCCount = 0, nEntrance = 0, nRefrigerator = 0, nTmp = 0;
	for(int i = 0; i < m_SeatList.GetCount(); i++)
	{
		CSeat2 *pSeat2 = m_SeatList.GetAt(i);
		strAppName.Format(_T("Seat2Config %d"), pSeat2->GetSeat2Type());
		strKey.Empty();
		strPos.Empty();
		switch(pSeat2->GetSeat2Type())
		{
		case MainIBA:
			nTmp = nIBACount;
			++nIBACount;
			break;

		case WC:
			nTmp = nWCCount;
			++nWCCount;
			break;

		case Entrance:
			nTmp = nEntrance;
			++nEntrance;
			break;

		case Refrigerator:
			nTmp = nRefrigerator;
			++nRefrigerator;
			break;

		default:
			ASSERT(FALSE);
			break;
		}
		strKey.Format(_T("SeatPosX %d"), nTmp);
		strPos.Format(_T(" %d"),pSeat2->GetPosition().x);
		CPoint ptTmp(0,0);
		ptTmp.x = WritePrivateProfileString(strAppName, strKey, strPos, szPath);

		strKey.Empty();
		strPos.Empty();
		strKey.Format(_T("SeatPosY %d"), nTmp);
		strPos.Format(_T(" %d"),pSeat2->GetPosition().y);
		ptTmp.y = WritePrivateProfileString(strAppName, strKey, strPos, szPath);
	}
	WritePrivateProfileString(_T("Seat2ConfigFlag"), _T("FirstRun"), _T("0"), szPath);		// 不是第一次启动了

	return TRUE;
}

//BOOL CSeatList2::ReadList()
//{
//	CString szPath, strKey;
//	szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\SeatConfig.ini");
//
//	for(int i = 0; i < SeatListSize; i ++)
//	{
//		strKey.Empty();
//		strKey.Format(_T("SeatPosX %d"), i);
//		CPoint ptTmp(0,0);
//		ptTmp.x = GetPrivateProfileInt(_T("SeatConfig2"), strKey, 10 + i * 100, szPath);
//
//		strKey.Empty();
//		strKey.Format(_T("SeatPosY %d"), i);
//		ptTmp.y = GetPrivateProfileInt(_T("SeatConfig2"), strKey, 10, szPath);
//
//		m_SeatList[i] = new CSeat2((CSeat2::Seat2Type)i);
//		m_SeatList[i]->SetPosition(ptTmp);
//		m_SeatList[i]->SetRadius(SizeOfBMPHalf);		// 图片大小的一半
//	}
//	return TRUE;
//}

BOOL CSeatList2::ReadList()
{
	if (GetSize() > 0)
	{
		DeleteAll();
	}

	CString szPath, strKey, strAppName;
	szPath = theApp.GetWorkPath() + _T("\\IBAConfig\\SeatConfig.ini");

	INT ii = 0;

	for(int i = 0; i < Seat2Count; i ++)
	{
		strAppName.Format(_T("Seat2Config %d"), i);
		int ii = 0;
		do 
		{
			strKey.Empty();
			strKey.Format(_T("SeatPosX %d"), ii);
			CPoint ptTmp(0,0);
			//ptTmp.x = GetPrivateProfileInt(strAppName, strKey, 10 + i * 100, szPath);
			ptTmp.x = GetPrivateProfileInt(strAppName, strKey, 0, szPath);

			strKey.Empty();
			strKey.Format(_T("SeatPosY %d"), ii);
			//ptTmp.y = GetPrivateProfileInt(strAppName, strKey, 10, szPath);
			ptTmp.y = GetPrivateProfileInt(strAppName, strKey, 0, szPath);
			if (ptTmp.x == 0 || ptTmp.y == 0 )
			{
				break;
			}
			CSeat2 * tSeat = new CSeat2((Seat2Type)i);
			tSeat->SetPosition(ptTmp);
			tSeat->SetRadius(SizeOfBMPHalf);		// 图片大小的一半
			Add(tSeat);
			ii++;

		} while (1);
	}
	
	return TRUE;
}


void CSeatList2::DrawAllRect(CDC * pDC, double XRatio, double YRatio)
{
	if (!pDC)
	{
		return;
	}
	/////////////////////////////////////////
	for (INT ii = 0; ii < GetSize(); ii++)
	{
		GetAt(ii)->Draw(pDC, XRatio, YRatio);
	}

}

INT CSeatList2::InRect(CPoint MousePoint)
{
	INT ii = 0;
	for (ii = 0; ii < GetSize(); ii++)
	{
		CRect rect(0, 0, 0, 0);
		m_SeatList[ii]->GetRect(rect);
		if (rect.PtInRect(MousePoint))
		{
			break;
		}
	}
	if (ii < GetSize())
	{
		return ii;
	}
	else
	{
		return -1;
	}
}

CSeat2* CSeatList2::GetAt( int nIndex )
{
	if(nIndex < 0 && nIndex > GetSize())
	{
		return NULL;
	}
	else
	{
		return m_SeatList.GetAt(nIndex);
	}
}

INT CSeatList2::Add( CSeat2 * Seat )
{
	// 计算最右和最下坐标，以当前终端的坐标，加上5个半径计算（5个半径可以改变，根据实际效果来）
	CSeatLayoutView::SetMaxRightCmp(Seat->GetPosition().x + Seat->GetRadius() * 5);
	CSeatLayoutView::SetMaxBottomCmp(Seat->GetPosition().y + Seat->GetRadius() * 5);

	return m_SeatList.Add(Seat);
}

//清除整个数组
void CSeatList2::DeleteAll()
{
	for (int ii = 0 ; ii < m_SeatList.GetSize(); ii++)
	{
		CSeat2* tp = m_SeatList.GetAt(ii);
		if(tp)
		{
			delete tp;
			tp = NULL;
		}
	}
	m_SeatList.RemoveAll();
}

//清除nCount个nIndex后的指针，对象内存也清除
void CSeatList2::DeleteAt(INT nIndex, INT nCount)
{
	for (int ii = nIndex; ii < (nIndex + nCount); ii++)
	{
		CSeat2* tp = m_SeatList.GetAt(ii);
		if(tp)
		{
			delete tp;
			tp = NULL;
		}
	}
	m_SeatList.RemoveAt(nIndex, nCount);
}
