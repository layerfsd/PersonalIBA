#include "StdAfx.h"
#include "Excel.h"
#include "MyString.h"
#include "ClipBoardMgr.h"

#include <WinGDI.h>

#ifndef ASSERT
#define ASSERT(x) 
#endif

//#define //ExcelTrace GXX_AUTO_TRACE
////#define //ExcelTrace2 GXX_TRACE

namespace NS_GXX
{
	BOOL MyErrorMsg(_com_error& err)
	{
		_bstr_t br = err.Description();
		br += _T("  ");
		br += err.ErrorMessage();
		MessageBoxW(NULL,br,L"Excel error!",MB_OK|MB_ICONWARNING);
		return FALSE;
	}

    CExcel::CExcel(void)
    {
        m_bCreated = FALSE;
        m_bClosed = TRUE;
        memset(m_lpBookName,0,sizeof(TCHAR)*1024);

		m_hMainWnd = (HANDLE)0;
    }

    CExcel::~CExcel(void)
    {
        Close();
    }

    BOOL CExcel::CreateOneBlankBook(BOOL bShowApp /* = FALSE */,const TCHAR* lpszSheepName)
    {
		//ExcelTrace("创建新的工作薄");
        if( m_bCreated )
        {
             Close();
             m_bCreated = FALSE;
        }
        try
        {
             // add-s yang
             m_ptrApp_tmp.CreateInstance(L"Excel.Application");
             // add-e yang
             m_ptrApp.CreateInstance(L"Excel.Application");
             m_ptrApp->UserControl = VARIANT_FALSE;      //不允许其他用户控制
			 m_ptrApp->PutDisplayClipboardWindow(0,VARIANT_FALSE);
             if( bShowApp )
             {
                 m_ptrApp->Visible[0] = VARIANT_TRUE;
             }
             else
             {
                m_ptrApp->Visible[0] = VARIANT_FALSE;
             }
             m_ptrBooks = m_ptrApp->GetWorkbooks();
             m_ptrBook = m_ptrBooks->Add((long)xlWorkbook);
			// m_ptrBook->MultiUserEditing[0] = VARIANT_TRUE;
             m_ptrSheets = m_ptrBook->GetSheets();
             m_bCreated = TRUE;
             m_bClosed = FALSE;
             SetSheetName(1,lpszSheepName);
             SetActiveSheet(1);
			 m_ptrApp->DisplayAlerts[0] = VARIANT_FALSE;
             return TRUE;
        }
        catch(_com_error &err)
        {	
			//ExcelTrace2(L"创建新工作薄出现异常,%s",(const wchar_t*)err.Description());
            ASSERT(MyErrorMsg( err ));
            m_bCreated = FALSE;
            return FALSE;
        }  
    }
    BOOL CExcel::OpenBook(TCHAR *lpszBookPath,BOOL bShowApp /* = FALSE */)
    {
		//ExcelTrace(L"打开工作薄:%s",lpszBookPath);
        if( ! PathFileExists(lpszBookPath) )
        {
			//ExcelTrace2("打开工作薄失败,文件不存在");
            return FALSE;
        }
        if( m_bCreated )
        {
            Close();
            m_bCreated = FALSE;
        }
        try
        {
			// add-s yang
			m_ptrApp_tmp.CreateInstance(L"Excel.Application");

            m_ptrApp.CreateInstance(L"Excel.Application");
            if( bShowApp )
            {
                m_ptrApp->Visible[0] = VARIANT_TRUE;
            }
            else
            {
                m_ptrApp->Visible[0] = VARIANT_FALSE;
            }
            m_ptrBooks = m_ptrApp->GetWorkbooks();
            m_ptrBook = m_ptrBooks->Open((_bstr_t)lpszBookPath);
            m_ptrSheets = m_ptrBook->GetSheets();
            m_bCreated = TRUE;
            m_bClosed = FALSE;
            SetActiveSheet(1);  //默认激活工作表1
            memcpy(m_lpBookName,lpszBookPath,(wcslen(lpszBookPath)+1)*sizeof(TCHAR) );
			m_ptrApp->DisplayAlerts[0] = VARIANT_FALSE;
            return TRUE;
        }
        catch(_com_error &err)
        {
			//ExcelTrace2(L"打开工作薄出现异常,%s",(const wchar_t*)err.Description());
            ASSERT(MyErrorMsg(err));
            m_bCreated = FALSE;		
            return FALSE;
        }  
    }
    BOOL CExcel::Save(LPCTSTR lpszFullPath)
    {
        try
        {
			if( ::PathFileExists(lpszFullPath) )
			{
				//ExcelTrace2("源文件已存在..");
				if( MessageBoxW((HWND)m_hMainWnd,L"文件已存在，是否覆盖原文件?",L"提示",MB_YESNO|MB_ICONQUESTION) )
				{
					if( ::DeleteFileW(lpszFullPath) == FALSE)
					{
						NS_GXX::CMyStringW strErr;
						strErr.Format(L"%s    \r\n\r\n拒绝访问!\r\n文件在其他地方被使用",lpszFullPath);
						MessageBox((HWND)m_hMainWnd,strErr,L"Error",MB_OK|MB_ICONERROR);
						return FALSE;
					}
				}
			}
            BSTR bstr = _bstr_t(lpszFullPath);
            VARIANT v1;
            v1.vt = VT_BSTR;
            v1.bstrVal = bstr;
            HRESULT hR = m_ptrBook->SaveAs(v1,vtMissing,vtMissing,vtMissing,FALSE,FALSE,xlNoChange);
            return SUCCEEDED(hR);
        }
        catch (_com_error &err)
        {
			//ExcelTrace2("保存工作薄出现异常:%s",(const wchar_t*)err.Description());
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }
    }
    BOOL CExcel::Save()
    {
        //return Save(m_lpBookName);
		try
		{
			BOOL bRes = SUCCEEDED( m_ptrBook->Save() );
			return bRes;
		}
		catch (_com_error &err)
		{
			//ExcelTrace2("保存工作薄出现异常:%s",(const wchar_t*)err.Description());
			ASSERT(MyErrorMsg(err));
			return FALSE;
		}
        
    }
    void CExcel::Close()
    {
		//ExcelTrace("关闭工作薄，退出Excel应用程序");
        if( m_bClosed == FALSE)
        {
            try
            {
                m_ptrBook->Saved[0] = VARIANT_TRUE;
                m_ptrApp->Quit();
                m_ptrSheets.Release();
                m_ptrBook.Release();
                m_ptrBooks.Release();
                m_ptrApp.Release();
                m_bClosed = TRUE;

                // add-s yang
                //用户再使用则不关闭进程
                if (m_ptrApp_tmp->GetWorkbooks()->Count == 0)
                {
                    m_ptrApp_tmp->Quit();
                }
                // add-e yang
            }       
            catch(_com_error &err)
            {
                ASSERT(MyErrorMsg(err));
                m_bClosed= TRUE;
            }  
        }
    }
    int CExcel::GetSheetCount()
    {
        return m_ptrSheets->GetCount();
    }
	BOOL CExcel::DeleteSheet( int iIndex )
	{
		//ExcelTrace("删除工作表,%d",iIndex);
		try
		{
			if( iIndex < 1 || iIndex > GetSheetCount() )
			{
				ASSERT(0);
				return FALSE;
			}
			_WorksheetPtr ptrSheet = m_ptrSheets->GetItem(iIndex);
			if( ptrSheet == NULL )
			{
				//判断是不是图表sheet
				_ChartPtr ptrChart = m_ptrSheets->GetItem(iIndex);
				if( ptrChart != NULL)
				{
					ptrChart->Select();
					ptrChart->Delete();
				}
			}
			else //是工作表sheet
			{
				ptrSheet->Select();
				ptrSheet->Delete();
			}	

			return TRUE;
		}
		catch (_com_error &err)
		{
			//ExcelTrace2("删除工作表失败");
			ASSERT(MyErrorMsg(err));
			return FALSE;
		}  
		
	}

	BOOL CExcel::DeleteSheet( const TCHAR * lpszSheetName )
	{
		//ExcelTrace(L"删除工作表,%s",lpszSheetName);
		try
		{
			if( lpszSheetName == NULL || lpszSheetName[0] == L'\0')
			{
				ASSERT(0);
				return FALSE;
			}
			_bstr_t bs = lpszSheetName;
			_WorksheetPtr ptrSheet = m_ptrSheets->GetItem(bs);
			if( ptrSheet == NULL )
			{
				//判断是不是图表sheet
				_ChartPtr ptrChart = m_ptrSheets->GetItem(bs);
				if( ptrChart != NULL)
				{
					ptrChart->Select();
					ptrChart->Delete();
				}
			}
			else
			{
				ptrSheet->Select();
				ptrSheet->Delete();
			}
			
			
			return TRUE;
		}
		catch (_com_error &err)
		{
			//ExcelTrace2("删除工作表失败");
			ASSERT(MyErrorMsg(err));
			return FALSE;
		}

	}
    BOOL CExcel::InsertSheet(const TCHAR * lpszSheetName)
    {
		//ExcelTrace(L"插入工作表:%s",lpszSheetName);
        try
        {
            long lNum = m_ptrSheets->GetCount();
			NS_GXX::CMyStringW strSheetName;
            if(lpszSheetName == NULL || lpszSheetName[0] == L'\0')
            {		
				strSheetName.Format(L"%s%d",L"sheet",lNum);
            }
			else
			{
				strSheetName = lpszSheetName;
			}

			VARIANT var;
			var.vt = VT_DISPATCH;

            _WorksheetPtr pSheet = m_ptrSheets->GetItem(lNum);
            if( pSheet == NULL )
			{
				_ChartPtr pChart = m_ptrSheets->GetItem(lNum);
				if( pChart == NULL )
				{
					return FALSE;
				}
				var.pdispVal = pChart;
			}
			else
			{
				var.pdispVal = pSheet;
			}
            
            //在最后一个工作表后面插入一个工作表
            pSheet = m_ptrSheets->Add(vtMissing,var);
            pSheet->PutName(strSheetName.GetData());
            pSheet->Activate();
            return TRUE;
        }
        catch (_com_error &err)
        {
            ASSERT( MyErrorMsg(err) );
            return FALSE;
        }  
    }
    BOOL CExcel::InsertSheet(int iIndex, const TCHAR * lpszSheetName,BOOL bAtBefore)
    {
		//ExcelTrace(L"插入工作表:%s",lpszSheetName);
        if( iIndex <= 0 || iIndex > m_ptrSheets->GetCount() )
        {
            return InsertSheet(lpszSheetName);
        }
        try
        {
            long lNum = m_ptrSheets->GetCount();
			NS_GXX::CMyStringW strSheetName;

            if(lpszSheetName == NULL || lpszSheetName[0] == L'\0')
            {		
                strSheetName.Format(L"%s%d",L"sheet",lNum);
            }
			else
			{
				strSheetName = lpszSheetName;
			}

			VARIANT var;
			var.vt = VT_DISPATCH;

			_WorksheetPtr pSheet = m_ptrSheets->GetItem(lNum);
			if( pSheet == NULL )
			{
				_ChartPtr pChart = m_ptrSheets->GetItem(lNum);
				if( pChart == NULL )
				{
					return FALSE;
				}
				var.pdispVal = pChart;
			}
			else
			{
				var.pdispVal = pSheet;
			}

            //在iIndex工作表前面插入一个工作表
            if( bAtBefore)
            {
                  pSheet = m_ptrSheets->Add(var,vtMissing);
            }
            else
            {
                 pSheet = m_ptrSheets->Add(vtMissing,var);
            }
            pSheet->PutName(strSheetName.GetData());
            pSheet->Activate();
            return TRUE;
        }
        catch (_com_error &err)
        {
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }  
    }

    // add yang
    BOOL CExcel::InsertChartSheet(int iIndex, const TCHAR *lpszSheetName, BOOL bAtBefore)
    {
        //ExcelTrace(L"插入graph表:%s",lpszSheetName);
        if( iIndex <= 0 || iIndex > m_ptrSheets->GetCount() )
        {
            // return InsertSheet(lpszSheetName);
            return FALSE;
        }
        try
        {
            long lNum = m_ptrSheets->GetCount();
            NS_GXX::CMyStringW strSheetName;

            if(lpszSheetName == NULL || lpszSheetName[0] == L'\0')
            {		
                strSheetName.Format(L"%s%d",L"graph",lNum);
            }
            else
            {
                strSheetName = lpszSheetName;
            }

           
            _WorksheetPtr pSheet = GetSheet(1);
            _ChartPtr pChart = m_ptrBook->GetCharts()->Add();

            RangePtr ptrRange = pSheet->GetRange(L"A1",L"B3");

            pChart->ChartType = xlLineMarkers;
            pChart->SetSourceData(ptrRange);
            pChart->PutName(strSheetName.GetData());
            pChart->Activate();
            return TRUE;
        }
        catch (_com_error &err)
        {
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }  
    }
    // add yang

    BOOL CExcel::SetActiveSheet(int iIndex)
    {
		//ExcelTrace("激活工作表：%d",iIndex);
        if( iIndex <= 0 || iIndex > m_ptrSheets->GetCount() )
        {
            return FALSE;
        }
        try
        {
            _WorksheetPtr ptrSheet = m_ptrSheets->GetItem(iIndex);
			if( ptrSheet == NULL)
			{
				return FALSE;
			}
            HRESULT hr = ptrSheet->Activate();
            return SUCCEEDED(hr);
        }
        catch (_com_error& err)
        {
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }      
    }
    BOOL CExcel::SetActiveSheet(TCHAR * lpszSheetName)
    {
		//ExcelTrace(L"激活工作表：%d",lpszSheetName);
        if( lpszSheetName == NULL )
        {
            return FALSE;
        }
        try
        {
            _bstr_t bstr = lpszSheetName;
            _WorksheetPtr ptrSheet = m_ptrSheets->GetItem(bstr);
			if( ptrSheet == NULL )
			{
				return FALSE;
			}
            HRESULT hr = ptrSheet->Activate();
            return SUCCEEDED(hr);
        }
        catch (_com_error& err)
        {
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }

    }
    BOOL CExcel::SetSheetName(int iIndex,const TCHAR * lpszSheetName)
    {
        if( lpszSheetName == NULL || iIndex <= 0 || iIndex> m_ptrSheets->GetCount() )
        {
            return FALSE;
        }
        try
        {
            _WorksheetPtr ptrSheet = m_ptrSheets->GetItem(iIndex);
			if( ptrSheet == NULL )
			{
				_ChartPtr ptrChart = m_ptrSheets->GetItem(iIndex);
				if( ptrChart == NULL)
				{
					return FALSE;
				}
				ptrChart->PutName(lpszSheetName);
			}
			else
			{
				ptrSheet->PutName(lpszSheetName);
			}
            return TRUE;
        }
        catch (_com_error& err)
        {
            ASSERT(MyErrorMsg(err));
            return FALSE;
        }
        
    }

    _bstr_t CExcel::ConvertCol(int iCol)
    {
        _bstr_t br="";
		CMyStringW colR;
		colR.MakeExcelColumn(iCol);
		br = colR.GetData();
        return br;
	}
	_bstr_t CExcel::ConvertColRow( int iCol,int iRow )
	{
		_bstr_t br="";
		CMyStringW colR;
		colR.MakeExcelColumn(iCol);
		colR.AppendFormat(L"%d",iRow);
		br = colR.GetData();
		return br;
	}

	Excel::_WorksheetPtr CExcel::GetActiveSheet()
	{
		return m_ptrBook->GetActiveSheet();
	}

	Excel::_WorksheetPtr CExcel::GetSheet(const int iIndex)
	{
		return m_ptrSheets->GetItem(iIndex);
	}

	Excel::_WorksheetPtr CExcel::GetSheet( const WCHAR * lpszName )
	{
		_bstr_t bstr = lpszName;
		return m_ptrSheets->GetItem(bstr);
	}
	Excel::_ChartPtr CExcel::GetChart( const int iIndex )
	{
		if( iIndex < 1 || iIndex > GetSheetCount() )
		{
			ASSERT(0);
			return NULL;
		}
		return m_ptrSheets->GetItem(iIndex);
	}

	Excel::_ChartPtr CExcel::GetChart( const WCHAR* lpszName )
	{
		if( lpszName == NULL || lpszName[0] == L'\0')
		{
			ASSERT(0);
			return NULL;
		}
		return m_ptrSheets->GetItem(lpszName);
	}

	void CExcel::Format_ColsWidth( _WorksheetPtr ptrSheet,int iCol1,int iCol2,float fWidth /*= 8.50f*/ )
	{
		_bstr_t bstart = ConvertColRow(iCol1,1);
		_bstr_t bend = ConvertColRow(iCol2,1);
		RangePtr ptrRange = ptrSheet->Range[bstart][bend];
		ptrRange->Select();
		ptrRange->PutColumnWidth(fWidth)	;
	}

	void CExcel::Format_RangeBkColor( _WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr/*=RGB(196,196,196)*/ )
	{
		ptrSheet->Activate();
		_bstr_t bstart = ConvertColRow(iCol1,iRow1);
		_bstr_t bend = ConvertColRow(iCol2,iRow2);
		RangePtr ptrRange = ptrSheet->Range[bstart][bend];
		ptrRange->Select();
		ptrRange->Interior->Color = clr;
	}

	void CExcel::Format_RangeFontColor( _WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr/*=RGB(196,196,196) */ )
	{
		_bstr_t bstart = ConvertColRow(iCol1,iRow1);
		_bstr_t bend = ConvertColRow(iCol2,iRow2);
		RangePtr ptrRange = ptrSheet->Range[bstart][bend];
		ptrRange->Select();
		ptrRange->Font->Color = clr;
	}
	void CExcel::Format_RangeFont( _WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr/*=RGB(0,0,0)*/,const char* fontName /*= "宋体"*/,int nFontSize /*= 10 */,BOOL bBold ,BOOL bItalic  )
	{
		_bstr_t bstart = ConvertColRow(iCol1,iRow1);
		_bstr_t bend = ConvertColRow(iCol2,iRow2);
		RangePtr ptrRange = ptrSheet->Range[bstart][bend];
		ptrRange->Select();
		ptrRange->Font->Color = clr;
		ptrRange->Font->Name = fontName;
		ptrRange->Font->Size = nFontSize;
		ptrRange->Font->Bold = bBold;
		ptrRange->Font->Italic = bItalic;
	}
	void CExcel::Format_BorderRange( _WorksheetPtr ptrSheet,DWORD clr,int col1,int row1,int col2,int row2,int nWeight /*= 2*/ ,int nType )
	{
	/*	try
		{*/

		
		_bstr_t bstart = ConvertColRow(col1,row1);
		_bstr_t bend = ConvertColRow(col2,row2);
		RangePtr range = ptrSheet->Range[bstart][bend];
		range->Select();
		//range->BorderAround(xlContinuous,xlThin,xlColorIndexAutomatic,RGB(255,0,0));
		XlBorderWeight borWg;
		switch( nWeight )
		{
		case 1:
			borWg = xlHairline;
			break;
		case 2:
			borWg = xlThin;
			break;
		case 3:
			borWg = xlMedium;
			break;
		case 4:
			borWg = xlThick;
			break;
		default:
			borWg = xlThin;
			break;
		}

		int nTy = nType;
		if( nTy < 1)
		{
			nTy = 1;
		}
		if( nType > 3)
		{
			nTy = 3;
		}
		BordersPtr pBorders = range->GetBorders();
		pBorders->GetItem(xlDiagonalDown)->LineStyle = xlNone;  //左上和右下对角线
		pBorders->GetItem(xlDiagonalUp)->LineStyle = xlNone;    //左下和右上对角线

		if( nTy == 1 || nTy == 3)
		{

			pBorders->GetItem(xlEdgeLeft)->LineStyle = xlContinuous;
			pBorders->GetItem(xlEdgeLeft)->Weight = borWg;
			pBorders->GetItem(xlEdgeLeft)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlEdgeLeft)->Color = clr;

			pBorders->GetItem(xlEdgeRight)->LineStyle = xlContinuous;
			pBorders->GetItem(xlEdgeRight)->Weight = borWg;
			pBorders->GetItem(xlEdgeRight)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlEdgeRight)->Color = clr;

			pBorders->GetItem(xlEdgeBottom)->LineStyle = xlContinuous;
			pBorders->GetItem(xlEdgeBottom)->Weight = borWg;
			pBorders->GetItem(xlEdgeBottom)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlEdgeBottom)->Color = clr;

			pBorders->GetItem(xlEdgeTop)->LineStyle = xlContinuous;
			pBorders->GetItem(xlEdgeTop)->Weight = borWg;
			pBorders->GetItem(xlEdgeTop)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlEdgeTop)->Color = clr;

		}
		if( nTy == 2 || nTy == 3)
		{
			pBorders->GetItem(xlInsideHorizontal)->LineStyle = xlContinuous;
			pBorders->GetItem(xlInsideHorizontal)->Weight = borWg;
			pBorders->GetItem(xlInsideHorizontal)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlInsideHorizontal)->Color = clr;

			pBorders->GetItem(xlInsideVertical)->LineStyle = xlContinuous;
			pBorders->GetItem(xlInsideVertical)->Weight = borWg;
			pBorders->GetItem(xlInsideVertical)->ColorIndex = xlColorIndexAutomatic;
			pBorders->GetItem(xlInsideVertical)->Color = clr;
		}

		//}
		//catch(...)
		//{

		//}
	}

	void CExcel::Format_RowsHeight( _WorksheetPtr ptrSheet,int row1,int row2,float fHeight )
	{
		CMyStringW str1(L"A"),str2(L"A");
		str1 += row1;
		str2 += row2;
		RangePtr ptrRange = ptrSheet->Range[str1.GetData()][str2.GetData()];
		ptrRange->Select();
		ptrRange->PutRowHeight(fHeight);		
	}

	Excel::RangePtr CExcel::GetRangePtr( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2 )
	{
		_bstr_t bstart = ConvertColRow(col1,row1);
		_bstr_t bend = ConvertColRow(col2,row2);
		RangePtr ptrRange = ptrSheet->Range[bstart][bend];
		return ptrRange;
	}

	void CExcel::Format_RangeAlignment( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2,int nHoriAlign /*= 2*/,int nVertAlign /*= 2*/ )
	{
		RangePtr range = GetRangePtr(ptrSheet,col1,row1,col2,row2);
		range->Select();
		_variant_t hAlign = xlCenter;
		_variant_t vAlign = xlCenter;

		switch( nHoriAlign )
		{
		case 1:
			hAlign = xlLeft;
			break;
		case 3:
			hAlign = xlRight;
			break;
		default:
			break;
		}

		switch( nVertAlign )
		{
		case 1:
			vAlign = xlTop;
			break;
		case 3:
			vAlign = xlBottom;
			break;
		default:
			break;
		}

		range->HorizontalAlignment = hAlign;
		range->VerticalAlignment = vAlign;
	}

	void CExcel::Format_PutNumberFormat( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2,const char* nFormat )
	{
		RangePtr ptrRange = GetRangePtr( ptrSheet ,col1,row1,col2,row2);
		ptrRange->Select();
		ptrRange->NumberFormatLocal = nFormat;
	}

	void CExcel::Format_PutBorderAround( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2, 
		short sLineStyle, long lWeight, long lCorIndex, DWORD clr)
	{
		// 找不到设置内部边框的办法，通过一个个设置外部边框来达到内部边框画线的目的
		for(int row = row1; row <= row2; row++)
			for(int col = col1; col <= col2; col++)
			{
				RangePtr ptrRange = GetRangePtr( ptrSheet ,col,row,col,row);
				ptrRange->BorderAround(COleVariant(sLineStyle), Excel::XlBorderWeight(lWeight),Excel::XlColorIndex(lCorIndex), clr);
			}	
	}
	
	void CExcel::Format_PutAutoFit( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2, BOOL bCol/* = TRUE*/)
	{
		RangePtr ptrRange = GetRangePtr( ptrSheet ,col1,row1,col2,row2);
		ptrRange->Select();
		if(bCol)
		{
			ptrRange->GetColumns()->AutoFit();
		}
		else
		{
			ptrRange->GetRows()->AutoFit();
		}
		
	}

	void CExcel::Format_PutCellText( _WorksheetPtr ptrSheet,int col,int row,const WCHAR * wszText )
	{
		ptrSheet->Activate();
		RangePtr ptrRange = GetRangePtr( ptrSheet,col,row,col,row);
		ptrRange->Select();
		_bstr_t br = wszText;
		ptrRange->Value2 = br;   //设置单元格文本	
	}

	void CExcel::Format_Frozen( _WorksheetPtr ptrSheet,int col,int row )
	{
		GetRangePtr(ptrSheet,col,row,col,row)->Select();
		ptrSheet->GetApplication()->GetActiveWindow()->FreezePanes = TRUE;
	}

	Excel::_WorkbookPtr CExcel::GetWorkBook()
	{
		return m_ptrBook;
	}

	void CExcel::MoveSheet( const WCHAR *lpszSrcSheetName,const WCHAR *lpszDesSheetName,BOOL bBefore /*= TRUE*/ )
	{
		_WorksheetPtr ptrSrcSheet = m_ptrSheets->GetItem(lpszSrcSheetName);
		_ChartPtr ptrSrcChart = m_ptrSheets->GetItem( lpszSrcSheetName );
		_WorksheetPtr ptrDesSheet = m_ptrSheets->GetItem( lpszDesSheetName );
		_ChartPtr ptrDesChart = m_ptrSheets->GetItem( lpszDesSheetName );
		VARIANT var;
		var.vt = VT_DISPATCH;
		if( ptrDesSheet != NULL )
		{
			var.pdispVal = ptrDesSheet;
		}
		else if( ptrDesChart != NULL )
		{
			var.pdispVal = ptrDesChart;
		}
		else
		{
			return ;
		}

		if( ptrSrcSheet != NULL )
		{
			ptrSrcSheet->Select();
			if( bBefore )
			{
				ptrSrcSheet->Move( var);
			}
			else
			{
				ptrSrcSheet->Move( vtMissing,var );
			}
		}
		else if( ptrSrcChart != NULL )
		{
			ptrSrcChart->Select();
			if( bBefore )
			{
				ptrSrcChart->Move( var );
			}
			else
			{
				ptrSrcChart->Move( vtMissing,var );
			}
		}
	}

	void CExcel::MoveSheet( const WCHAR *lpszSrcSheetName,const int nDesSheetItem,BOOL bBefore /*= TRUE*/ )
	{
		_WorksheetPtr ptrSrcSheet = m_ptrSheets->GetItem(lpszSrcSheetName);
		_ChartPtr ptrSrcChart = m_ptrSheets->GetItem( lpszSrcSheetName );
		_WorksheetPtr ptrDesSheet = m_ptrSheets->GetItem( nDesSheetItem );
		_ChartPtr ptrDesChart = m_ptrSheets->GetItem( nDesSheetItem );
		VARIANT var;
		var.vt = VT_DISPATCH;
		if( ptrDesSheet != NULL )
		{
			var.pdispVal = ptrDesSheet;
		}
		else if( ptrDesChart != NULL )
		{
			var.pdispVal = ptrDesChart;
		}
		else
		{
			return ;
		}
		
		if( ptrSrcSheet != NULL )
		{
			ptrSrcSheet->Select();
			if( bBefore )
			{
				ptrSrcSheet->Move( var);
			}
			else
			{
				ptrSrcSheet->Move( vtMissing,var );
			}
		}
		else if( ptrSrcChart != NULL )
		{
			ptrSrcChart->Select();
			if( bBefore )
			{
				ptrSrcChart->Move( var );
			}
			else
			{
				ptrSrcChart->Move( vtMissing,var );
			}
		}
	}

	BOOL NS_GXX::CExcel::EmptyExcelClipboard()
	{
		//获取Excel主窗口句柄
		long nExcel = m_ptrApp->GetHwnd();

		//打开任务窗格
		/*PostMessage((HWND)nExcel, WM_KEYDOWN, VK_CONTROL, 0);
		PostMessage((HWND)nExcel, WM_KEYDOWN, VK_F1, 0);
		PostMessage((HWND)nExcel, WM_KEYUP, VK_F1, 0);
		PostMessage((HWND)nExcel, WM_KEYUP, VK_CONTROL, 0);*/

		try
		{
			Office::CommandBarPtr pCmdBar = NULL;
			pCmdBar = m_ptrApp->GetCommandBars()->GetItem(1);
			if (pCmdBar == NULL)
			{
				//ExcelTrace("EmptyExcelClipboard Fails!");
				return FALSE;
			}
			//找到剪贴板
			Office::CommandBarControlPtr pCmdBarCtl = NULL;
			pCmdBarCtl = pCmdBar->FindControl(vtMissing, 809, 
				vtMissing, vtMissing, TRUE);
			if (pCmdBarCtl == NULL)
			{
				//ExcelTrace("EmptyExcelClipboard Fails!");
				return FALSE;
			}
			//打开剪贴板
			pCmdBarCtl->Execute();

			_bstr_t btName = m_ptrApp->GetCommandBars()
				->GetItem("Task Pane")->GetNameLocal();
			m_ptrApp->GetCommandBars()->GetItem("Task Pane")
				->PutVisible(VARIANT_FALSE);

			//剪贴板句柄
			HWND hClipborad = NULL;
			//临时窗口句柄
			HWND hWindow = NULL;
			hWindow = FindWindowEx((HWND)nExcel, NULL, 
				L"MsoWorkPane", L"MsoWorkPane");
			hClipborad = FindWindowEx(hWindow, NULL, 
				L"bosa_sdm_XL9", L"Collect and Paste 2.0");

			if (hClipborad == NULL)
			{
				//ExcelTrace("Get Clipboard HWnd Error!");
				return FALSE;
			}
			//发送模拟鼠标消息  (x,y)=(100,10),取(92,6)~(168,27)均可
			int nXY = 100 + 10 * 65536;  
			PostMessage(hClipborad, WM_LBUTTONDOWN, 0, nXY);
			PostMessage(hClipborad, WM_LBUTTONUP, 0, nXY);
		}
		catch (_com_error& e)
		{
			//ExcelTrace((char*)e.Description());
			return FALSE;
		}

		//ExcelTrace("EmptyClipboard success!");
		return TRUE;
	}


	ShapeRangePtr NS_GXX::CExcel::Format_MoveShapeToCell( _WorksheetPtr ptrSheet,int nShapeIndex,int col1,int row1 ,OUT float *pfWidth /*= NULL*/ , OUT float *pfHeight /*= NULL*/)
	{
		try
		{
			ShapeRangePtr ptrShape = ptrSheet->Shapes->GetRange(nShapeIndex);

			ptrShape->Select();
			ptrShape->IncrementLeft( -3000.0);
			ptrShape->IncrementTop( -3000.0);

			float nWidth = 0.0;
			float nHeight = 0.0;

			if( col1 > 1 )
			{
				RangePtr pRange = GetRangePtr( ptrSheet , 1,1,col1-1,1 );
				nWidth = pRange->GetWidth();
			}
			if( row1 > 1)
			{
				RangePtr pRange = GetRangePtr( ptrSheet , 1,1,1,row1-1 );
				nHeight = pRange->GetHeight();
			}
			ptrShape->Select();
			ptrShape->IncrementLeft( nWidth );
			ptrShape->IncrementTop( nHeight );

			if( pfWidth != NULL )
			{
				*pfWidth = nWidth;
			}
			if( pfHeight != NULL )
			{
				*pfHeight = nHeight;
			}

			return ptrShape;
		}
		catch (_com_error &e)
		{
			ASSERT(0);
			e.ErrorMessage();
			return NULL;
		}  
	}
	

	Excel::ShapeRangePtr NS_GXX::CExcel::Format_MoveShape( _WorksheetPtr ptrSheet,int nShapeIndex,IN float fWidth,IN float fHeight )
	{
		try
		{
			ShapeRangePtr ptrShape = ptrSheet->Shapes->GetRange(nShapeIndex);

			ptrShape->Select();
			ptrShape->IncrementLeft( -3000.0);
			ptrShape->IncrementTop( -3000.0);
			
			ptrShape->IncrementLeft( fWidth );
			ptrShape->IncrementTop( fHeight );


			return ptrShape;
		}
		catch (_com_error &e)
		{
			ASSERT(0);
			e.ErrorMessage();
			return NULL;
		}  
	}


	void NS_GXX::CExcel::Format_RangeMerge( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2 )
	{
		RangePtr ptrRange = GetRangePtr(ptrSheet,col1,row1,col2,row2);
		ptrRange->Select();
		ptrRange->Merge();
	}
////--------------------------------------------------------------------------------
//// CExcelExport
////--------------------------------------------------------------------------------
    //
    CExcelExport::CExcelExport()
    {
        m_hGlobalMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,MEM_SIZE+1); //+1是为了 *m_pMem = '\0';不会发生意外
        m_pMem = (char*)GlobalLock(m_hGlobalMem);
        memset(m_pMem,0,MEM_SIZE);
        m_nUsedMem = 0;
        m_iUsedLines = 0;
        m_iMemLines = 0;
        m_iMaxColumns = 1;
        m_bFirstInsertData = TRUE;
    }
    CExcelExport::~CExcelExport()
    {
		//输出最后剩余没有导出的数据
        Flush();
        if(m_hGlobalMem )
        {
            GlobalUnlock(m_hGlobalMem);
            GlobalFree(m_hGlobalMem);
        }
    }
    BOOL CExcelExport::InsertSheet(int iIndex, const TCHAR * lpszSheetName,BOOL bAtBefore /* = TRUE */ )
    {
        BOOL bRel = CExcel::InsertSheet(iIndex,lpszSheetName,bAtBefore);
        if( bRel )
        {
            UpdateLines();
        }
        return bRel;
    }
    BOOL CExcelExport::InsertSheet(const TCHAR *lpszSheetName)
    {
        BOOL bRel = CExcel::InsertSheet(lpszSheetName);
        if( bRel )
        {
            UpdateLines();
        }
        return bRel;
    }
    BOOL CExcelExport::SetActiveSheet(TCHAR * lpszSheetName)
    {
        BOOL bRel = CExcel::SetActiveSheet(lpszSheetName);
        if( bRel )
        {
            UpdateLines();
        }
        return bRel;
    }
    BOOL CExcelExport::SetActiveSheet(int iIndex)
    {
        BOOL bRel = CExcel::SetActiveSheet(iIndex);
        if( bRel )
        {
            UpdateLines();
        }
        return bRel;
    }
    void CExcelExport::AppendData(char *lpszData)
    {
        if( m_bFirstInsertData )
        {
            m_bFirstInsertData = FALSE;
            if( m_iUsedLines == MAX_LINE )   //首次插入时判断插入位置是否为65536
            {
                //是，该工作表已经插满了，需要另起一张新的工作表
                InsertAppendSheet();
            }
        }

		static char * lpEmpty = "\n";
        int nMaxColumn = 1;
        char *pWriteBuf = NULL;     
        size_t nBufLen = 0;
        int nCurLine = 0;
		bool bN = false;
        if( lpszData == NULL || lpszData[0] == '\0')
        {
            pWriteBuf = lpEmpty;
            nBufLen = 1;
			bN = true;
        }
        else
        {
            size_t nLen = strlen(lpszData);
			pWriteBuf = lpszData;
			if( pWriteBuf[nLen-1] == '\n' )
			{
				bN = true;
			}
			nBufLen = nLen;

        }
        for(int i = 0 ; i < nBufLen ; i ++)
        {
            if( pWriteBuf[i] == '\t' )
            {
                nMaxColumn++;
            }
            if( pWriteBuf[i] == '\n')
            {
                if( nMaxColumn > m_iMaxColumns)
                {
                    m_iMaxColumns = nMaxColumn;
                    nMaxColumn = 1;
                }
                nCurLine ++;
            }
        }//end for
		if( !bN )
		{
			nBufLen += 1;
			nMaxColumn ++;
			if( nMaxColumn > m_iMaxColumns)
			{
				m_iMaxColumns = nMaxColumn;
			}
			nCurLine ++;
		}

        //写入之前还要判断当前工作表行数是否要超过 65536
        if( nCurLine + m_iUsedLines + m_iMemLines > MAX_LINE )
        {
            //超过
            Flush();         //把内存数据写入Excel，并且创建一个新的工作表
            InsertAppendSheet();
        }
        //写入内存之前判断内存空间够不够
        if( m_nUsedMem + nBufLen > MEM_SIZE )
        {
            //不够
            Flush();     //把内存数据写入Excel
        }
        //可以写入内存了
        m_nUsedMem += nBufLen;
        m_iMemLines += nCurLine;
        memcpy(m_pMem,pWriteBuf,nBufLen-1);
		m_pMem[nBufLen-1] = '\n';
        m_pMem += nBufLen;
    }
	void CExcelExport::AppendData(char *lpszData,int nLineCount,int nColumnCount)
	{
		if( m_bFirstInsertData )
		{
			m_bFirstInsertData = FALSE;
			if( m_iUsedLines == MAX_LINE )   //首次插入时判断插入位置是否为65536
			{
				//是，该工作表已经插满了，需要另起一张新的工作表
				InsertAppendSheet();
			}
		}
		static char * lpEmpty = "\n";
		int nMaxColumn = 1;
		char *pWriteBuf = NULL;     
		size_t nBufLen = 0;
		int nCurLine = 0;
		bool bN = false;
		if( lpszData == NULL || lpszData[0] == '\0')
		{
			pWriteBuf = lpEmpty;
			nBufLen = 1;
			bN = true;
		}
		else
		{
			size_t nLen = strlen(lpszData);
			pWriteBuf = lpszData;
			if( pWriteBuf[nLen-1] == '\n' )
			{
				bN = true;
			}
			nBufLen = nLen;

		}
		if( !bN )
		{
			nBufLen += 1;
		}

		if( nColumnCount > m_iMaxColumns )
		{
			m_iMaxColumns = nColumnCount;
		}
		nCurLine = nLineCount;

		//写入之前还要判断当前工作表行数是否要超过 65536
		if( nCurLine + m_iUsedLines + m_iMemLines > MAX_LINE )
		{
			//超过
			Flush();         //把内存数据写入Excel，并且创建一个新的工作表
			InsertAppendSheet();
		}
		//写入内存之前判断内存空间够不够
		if( m_nUsedMem + nBufLen > MEM_SIZE )
		{
			//不够
			Flush();     //把内存数据写入Excel
		}
		//可以写入内存了
		m_nUsedMem += nBufLen;
		m_iMemLines += nCurLine;
		memcpy(m_pMem,pWriteBuf,nBufLen-1);
		m_pMem[nBufLen-1] = '\n';
		m_pMem += nBufLen;
	}
    void CExcelExport::InsertAppendSheet(TCHAR * lpszNewName /*= NULL*/)
    {
		//ExcelTrace(L"自动插入工作表:%s",lpszNewName);
        try
        {
			_WorksheetPtr pSheet = m_ptrBook->GetActiveSheet();
			m_iUsedLines = 0;  //把新加的工作作为活动工作表，使用的行需要重置
			m_iMaxColumns = 1;
			_bstr_t bName ;
			int nMaxIndex = pSheet->GetIndex();  //当前最大的相似名称工作表的索引值
			if( lpszNewName != NULL && lpszNewName[0] != L'\0' )
			{
				bName = lpszNewName;
			}
			else
			{
				


				bName = pSheet->GetName();
				CMyStringW strOrgName((WCHAR*)bName);

				//得到名称前缀相同的个数
				CMyStringW strNumCount = strOrgName.Clip( strOrgName.FindFromRight(L"__",strOrgName.GetLength() - 1) +2, strOrgName.GetLength() );

				int nCount = strNumCount.ParseInt();

				if( nCount == 0 )
				{   
					//表明该表 为 同名前缀 表的第二个表，把第一个表重命名
					CMyStringW strNewName = strOrgName;
					strNewName += L"__1";
					this->SetSheetName( this->GetSheet(strOrgName)->GetIndex(),strNewName);
					nCount ++;
				}
				strOrgName.CutAway( strOrgName.FindFromRight(L"__",strOrgName.GetLength()-1 ));
				
				//生成新表的名称
				CMyStringW strNewName = strOrgName;
				strNewName.AppendFormat(L"__%d",nCount + 1);


				//得到原始名称
				
				int nSheetCount = m_ptrSheets->GetCount();

				//计算新表应该放在哪里？
				for(int i = 1 ; i <= nSheetCount ; i ++)
				{
					_WorksheetPtr ptrSheet = m_ptrSheets->GetItem(i);
					bName = ptrSheet->GetName();
					CMyStringW strTemp((WCHAR*)bName);
					strTemp.CutAway( strTemp.FindFromRight(L"__",strTemp.GetLength()-1 ));
					if( strTemp == strOrgName )
					{
						nMaxIndex = i;  //循环结束后，得到的就是最大索引值
					}
				}
				
				bName = strNewName.GetData();
			}
			InsertSheet(nMaxIndex,(wchar_t*)bName,FALSE);
        }
         catch(_com_error &err)
         {
            ASSERT(MyErrorMsg(err));
         }  
    }
#ifndef _WINDOWS_
	void NS_GXX::CExcelExport::InsertOneBmp( IN CBitmap& bmp,int iCol /*= 0*/, int iRow /*= 0*/ )
	{
		AppendData();  //先插入一个空白行
		Flush();
		
		BITMAP  bm ;
		bmp.GetBitmap(&bm);								

		if( iRow == 0 && iCol == 0 )
		{ //做默认处理
			
			int nLineWidth = (int)GetRangePtr(GetActiveSheet(),1,m_iUsedLines,1,m_iUsedLines)->GetRowHeight();
			nLineWidth = int( ((float)nLineWidth)*19.0/14.0);
			//计算行数

			int nRows = bm.bmHeight / nLineWidth + 1;
			
			
			if( nRows + m_iUsedLines > MAX_LINE )
			{
				_bstr_t brSheet = GetActiveSheet()->GetName();
				InsertSheet( (WCHAR*)brSheet );
			}
			for(int i = 0 ; i < nRows ; i ++ )
			{
				AppendData();
			}
			Flush();

			OpenClipboard(NULL);								// 打开剪帖板
			EmptyClipboard();									// 清空前帖板
			SetClipboardData(CF_BITMAP,bmp.GetSafeHandle());	// 将位图设置到剪帖板上	
			CloseClipboard();                                   // 关闭前帖板	

			RangePtr ptrRange;
			ptrRange = GetRangePtr( GetActiveSheet(),1,m_iUsedLines - nRows,1,m_iUsedLines - nRows);
			VARIANT var;
			var.vt = VT_DISPATCH;
			var.pdispVal = ptrRange;
			ptrRange->Select();
			GetActiveSheet()->Paste(var); 
			
		}
		else
		{
			_WorksheetPtr ptrSheet = GetActiveSheet();
			RangePtr ptrRange = GetRangePtr( ptrSheet,iCol,iRow,iCol,iRow);

			OpenClipboard(NULL);								// 打开剪帖板
			EmptyClipboard();									// 清空前帖板
			SetClipboardData(CF_BITMAP,bmp.GetSafeHandle());	// 将位图设置到剪帖板上	
			CloseClipboard(); 

			VARIANT var;
			var.vt = VT_DISPATCH;
			var.pdispVal = ptrRange;
			ptrRange->Select();
			ptrSheet->Paste(var); 
		}

		//wchar_t wcsRow[16];
		//_bstr_t bstrFirst,bstrTemp1;
		//wsprintf(wcsRow,L"%d",m_dwUsedLine + 1);
		//bstrTemp1 = SysAllocString(wcsRow);
		//bstrFirst = L"A" + bstrTemp1;

		//RangePtr range = pSheet->Range[bstrFirst][vtMissing];
		//VARIANT var;
		//var.vt = VT_DISPATCH;
		//var.pdispVal = range;		
		//range->Select();
		//pSheet->Paste(var);


		//GetRangePtr( GetActiveSheet(), )
	}
#endif
	void CExcelExport::Flush(BOOL bNewSheet)
    {
		//ExcelTrace("数据写入工作表");
        if(m_pMem == NULL || m_iMemLines == 0)
        {
            return ;
        }

        //在字符串的结尾添加结束标记
        *m_pMem = '\0';

        //打开剪切板
        if(!OpenClipboard(NULL))
        {
            return ;
        }
        //清空剪切板
        EmptyClipboard();

        //解除对堆栈的锁定
        GlobalUnlock(m_hGlobalMem);

        //设置剪切板
        SetClipboardData(CF_TEXT,m_hGlobalMem);

        //释放堆栈空间
        //	GlobalFree(m_hGlobalMem);			//剪切板会自动释放此空间

        //关闭剪切板
        CloseClipboard();

        //粘贴数据到EXCEL
        _bstr_t first,last;
      
        char buf[6];
        wsprintfA(buf,"%d",m_iUsedLines+1);
        first = "A";
        first += buf;
        wsprintfA(buf,"%d",m_iUsedLines+m_iMemLines);
        _bstr_t col = ConvertCol(m_iMaxColumns+1);
        last = col;
        last += buf;
        //选择写入数据的工作表
        
		try
		{_WorksheetPtr pSheet;
		pSheet = m_ptrBook->GetActiveSheet();
		RangePtr range = pSheet->Range[first][last];
		VARIANT var;
		var.vt = VT_DISPATCH;
		var.pdispVal = range;		
		range->Select();
			pSheet->Paste(var);
		}
		catch (_com_error &err)
		{
			ASSERT(MyErrorMsg(err));
		}
        

        //还原到初始状态

        //分配堆栈
        m_hGlobalMem  = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,MEM_SIZE+1);

        //设置当前写入数据的大小
        m_nUsedMem = 0;

        //设置写入内存数据的行数
        m_iUsedLines += m_iMemLines;
        m_iMemLines = 0;

        //获得堆栈的指针
        m_pMem  = (char*)GlobalLock(m_hGlobalMem);    
        memset(m_pMem,0,MEM_SIZE+1);   
    }
    void CExcelExport::UpdateLines()
    {
        Flush();
        m_iUsedLines = 0 ;
        _WorksheetPtr ptrSheet = m_ptrBook->GetActiveSheet();
        int Gap = 100;
        RangePtr ran = ptrSheet->UsedRange;
        int count = ran->Rows->Count;                    //得到使用了的行数
        if( count == 1)  //空白的工作表 或者 只有一行数据的工作表
        {
            _bstr_t first = "A1";
            _bstr_t last = "IV1";
            RangePtr ran= ptrSheet->Range[first][last];
            ran->Select();
            ran->Copy();
            if( OpenClipboard(NULL) )
            {
                HANDLE hMem = GetClipboardData(CF_TEXT);
                char *pMem = (char*)GlobalLock(hMem);
                size_t nLen = strlen(pMem);
                if( strlen(pMem) == 2 )  //空白工作表
                {
                    count =0;
                }
                CloseClipboard();
            }
        }
        m_iUsedLines = count;
        m_pMem =(char*) GlobalLock(m_hGlobalMem);
        m_iMemLines = 0;
        m_iMaxColumns = 0;
        m_nUsedMem = 0;
        m_bFirstInsertData = TRUE;            //设置为首次插入
    }	

	void CExcelExport::PasteText( const char *lpszData,int iCol1,int iRow1,int iCol2,int iRow2 )
	{
		try
		{
			CClipBoardTextBuffer clipB;
			if( !clipB.Lock() )
			{
				return;			
			}
			clipB.AppendData(lpszData);
			clipB.UnLock();

			RangePtr ptrRange;
			ptrRange = GetRangePtr( GetActiveSheet(),iCol1,iRow1,iCol2,iRow2);
			VARIANT var;
			var.vt = VT_DISPATCH;
			var.pdispVal = ptrRange;
			ptrRange->Select();
			GetActiveSheet()->Paste(var); 
		}
		catch (_com_error &err)
		{
			ASSERT(MyErrorMsg(err));
		}
		//catch (...)
		//{
		//	// GXX_TRACE("粘贴文本失败");
		//}
	}

////-----------------------------------------------------------------
//// CExcelExportEx
////-----------------------------------------------------------------

	CExcelExportEx::CExcelExportEx()
	{
		for(DWORD i=0; i <BLOCK_COUNT; i++)
		{
			m_blocks[i] = new CHAR[BLOCK_SIZE+1];
			if( m_blocks[i] == NULL )
			{
				//ExcelTrace2("内存不足");
				ASSERT(0);
			}
			m_status[i] = S_Idle;
			m_usedLength[i] = 0;
			m_blockMaxCols[i] = 0;
		}
		m_iCurrentBlock = 0;
		m_status[m_iCurrentBlock] = S_Write;

		m_hEventCanRead = ::CreateEvent(NULL,TRUE,FALSE,NULL);
		m_hEventHaveIdleBlock = ::CreateEvent(NULL,TRUE,FALSE,NULL);
		m_hEventQueueEmpty = ::CreateEvent(NULL,TRUE,FALSE,NULL);
		m_hEventThreadFinish = ::CreateEvent(NULL,TRUE,FALSE,NULL);

		ResetEvent(m_hEventThreadFinish);
		m_bExit = false;
		SetEvent(m_hEventQueueEmpty);

		AllowIdleBlock();
		HoldCanRead();

		m_pWinThread = AfxBeginThread(_ThreadEntry ,this);

	}

	CExcelExportEx::~CExcelExportEx()
	{

		
		m_bExit = true;
		AllowCanRead();
		WaitForSingleObject(m_hEventThreadFinish,INFINITE);

		if( m_hEventCanRead != INVALID_HANDLE_VALUE )
		{
			CloseHandle(m_hEventCanRead);
		}
		if( m_hEventHaveIdleBlock != INVALID_HANDLE_VALUE )
		{
			CloseHandle(m_hEventHaveIdleBlock);
		}
		if( m_hEventThreadFinish != INVALID_HANDLE_VALUE )
		{
			CloseHandle(m_hEventThreadFinish);
		}
		
		if( m_hEventQueueEmpty != INVALID_HANDLE_VALUE )
		{
			CloseHandle(m_hEventQueueEmpty);
		}

		for(DWORD i=0; i < BLOCK_COUNT ; i ++)
		{
			if(m_blocks[i])
			{

				delete m_blocks[i];
			}
		}
	}

	void CExcelExportEx::AppendData( char *lpszData /*= NULL*/ )
	{
		static char *emptyBuf = "\n";

		int nUsedLen = m_usedLength[m_iCurrentBlock];

		ASSERT(m_status[m_iCurrentBlock]==S_Write);
		
		char *pWirteBuf = NULL;
		bool bHaveN = false;
		size_t nLen =0;
		if(lpszData == NULL || lpszData[0] == '\0')
		{
			pWirteBuf = emptyBuf;
			bHaveN = true;
			nLen = 1;
		}
		else
		{
			nLen = strlen(lpszData);
			pWirteBuf = lpszData;
			if(lpszData[nLen-1] == '\n')
				bHaveN = true;
		}
		
		int nWriteLen = nLen + (bHaveN?0:1);
		if( nUsedLen + nWriteLen > BLOCK_SIZE)
		{
			// 当前块已经写满,标识为可读
			m_cs.Lock();
			m_status[m_iCurrentBlock] = S_CanRead;
			m_queueCanRead.enqueue(m_iCurrentBlock); //放进"能读队列"
			m_cs.Unlock();

			AllowCanRead();

			int iIdle = -1;
			//找到一个空闲的块
			m_cs.Lock();
			for(DWORD iBlock =0 ; iBlock <BLOCK_COUNT; iBlock ++)
			{
				if( iBlock != m_iCurrentBlock && m_status[iBlock] == S_Idle )
				{
					iIdle = iBlock;
					break;
				}
			}
			m_cs.Unlock();

			if( iIdle == -1 )
			{	
				HoldIdleBlock();
				//等到有空闲块
				WaitIdleBlock();

				for(DWORD iBlock =0 ; iBlock <BLOCK_COUNT; iBlock ++)
				{
					if( iBlock != m_iCurrentBlock && m_status[iBlock] == S_Idle )
					{
						iIdle = iBlock;
					}
				}
				m_cs.Unlock();
			}

			ASSERT(iIdle!=-1);

			if( iIdle == -1 )
			{
				return;
			}

			//找到空闲块
			m_cs.Lock();
			m_iCurrentBlock = iIdle;  //设置为当前块
			m_usedLength[ iIdle ] = 0;  //使用长度为0
			m_status[ iIdle ] = S_Write; //标识状态为写入
			m_cs.Unlock();	

			CExcelExportEx::AppendData(pWirteBuf);
		}
		else
		{	//当前块有足够的空间写数据
			char *pBlock = m_blocks[m_iCurrentBlock];
			memcpy(&pBlock[nUsedLen],pWirteBuf,nLen);
			nUsedLen += nWriteLen;
			pBlock[nUsedLen-1] = '\n';

			m_cs.Lock();
			m_usedLength[m_iCurrentBlock] = nUsedLen;
			m_cs.Unlock();
		}
	}

	void CExcelExportEx::AppendData( char *lpszData,int nLineCount,int nColumnCount )
	{

	}

	void CExcelExportEx::WaitCanRead()
	{
		WaitForSingleObject(m_hEventCanRead,INFINITE);
	}

	void CExcelExportEx::HoldCanRead()
	{
		ResetEvent(m_hEventCanRead);
	}

	void CExcelExportEx::AllowCanRead()
	{
		SetEvent(m_hEventCanRead);
	}
	void CExcelExportEx::HoldIdleBlock()
	{
		ResetEvent(m_hEventHaveIdleBlock);
	}

	void CExcelExportEx::AllowIdleBlock()
	{
		SetEvent(m_hEventHaveIdleBlock);
	}

	void CExcelExportEx::WaitIdleBlock()
	{
		WaitForSingleObject(m_hEventHaveIdleBlock,INFINITE);
	}

	UINT CExcelExportEx::_ThreadEntry( LPVOID pParam)
	{
		::CoInitialize(NULL);

		CExcelExportEx *pEE = (CExcelExportEx *)pParam;
		
		while(1)
		{
			pEE->WaitCanRead();

			if( pEE->m_bExit )
			{
				SetEvent( pEE->m_hEventThreadFinish );
				::CoUninitialize();
				return 0;
			}

			pEE->m_cs.Lock();
			if( pEE->m_queueCanRead.size() == 0 )
			{
				pEE->HoldCanRead(); // 让线程等待
				SetEvent(pEE->m_hEventQueueEmpty);  //队列为空
				pEE->m_cs.Unlock(); 
				continue;
			}
			pEE->m_cs.Unlock();

			pEE->m_cs.Lock();
			int iBlock = pEE->m_queueCanRead.dequeue();
			ASSERT( pEE->m_status[iBlock] == S_CanRead);
			CHAR* pBlock = pEE->m_blocks[iBlock];
			int nLen = pEE->m_usedLength[iBlock];
			pEE->m_cs.Unlock();

			CHAR* pAt = pBlock;
			CHAR* pIt = pBlock;
			
			CHAR tempChar;
			for(int i = 0 ; i < nLen ; i ++)
			{
				if( (pIt[i]) == '\n')
				{
					tempChar = pIt[i+1];
					pIt[i+1] = '\0';
					((CExcelExport*)pEE)->AppendData(pAt);
					pIt[i+1] = tempChar;
					pAt = &pIt[i+1];
				}
			}
			
			pEE->m_cs.Lock();
			pEE->m_status[iBlock] = S_Idle;
			pEE->m_usedLength[iBlock] = 0;
			pEE->m_cs.Unlock();
			pEE->AllowIdleBlock();
			
		}
		::CoUninitialize();
		return 0;
	}

	void CExcelExportEx::Flush()
	{
		//
		m_cs.Lock();
		bool bWrite = (m_status[m_iCurrentBlock] == S_Write);
		m_cs.Unlock();
		
		if( bWrite )
		{
			m_cs.Lock();
			m_status[m_iCurrentBlock] = S_CanRead;
			m_queueCanRead.enqueue(m_iCurrentBlock);
			m_cs.Unlock();
			
			m_cs.Lock();
			ResetEvent(m_hEventQueueEmpty); // 队列为非空
			m_cs.Unlock();
		}

		AllowCanRead();  // 激活线程读取数据
		WaitForSingleObject(m_hEventQueueEmpty,INFINITE); //等待队列为空

		for(DWORD iBlock =0 ; iBlock <BLOCK_COUNT; iBlock ++)
		{
			m_status[iBlock] = S_Idle;
			m_usedLength[iBlock] = 0;
		}

		m_iCurrentBlock = 0;
		m_status[m_iCurrentBlock] = S_Write;
		m_usedLength[m_iCurrentBlock] = 0;

		CExcelExport::Flush();
		
	}
}
