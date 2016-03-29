#pragma once
#include <shlwapi.h>


///{{ 导入Excel COM组件
#define OFFICEXP 1
#define OFFICE2000 2

// Default Settings
#define OFFICE_VER OFFICEXP            //使用OFFICEXP的内核
#define USE_PROGID 1
#define USE_LIBID 0

#define _M2STR(x) #x
#define M2STR(x) _M2STR(x)

#pragma warning(disable: 4192)


// Ignore hard wired default paths if MSDLL_PATH is
// defined from the command line
#ifndef MSDLL_PATH

// Paths to required MS OFFICE files.
// Make sure these paths are correct for your  machine

#if OFFICE_VER == OFFICEXP
#define _MSDLL_PATH "C:\Program Files\Common Files\Microsoft Shared\Office11\MSO.DLL"

#elif  OFFICE_VER == OFFICE2000

#define   _MSDLL_PATH "C:\Program Files\Microsoft Office\Office\MSO9.dll"

#endif

#else

#define _MSDLL_PATH M2STR(MSDLL_PATH)

#endif

//
// Delete the *.tlh files when changing import qualifiers
#import _MSDLL_PATH rename("RGB", "NWRGB") rename("DocumentProperties","NWDocumentProperties")

#ifdef VBE6EXT_PATH
#import M2STR(VBE6EXT_PATH) 
#else
#import "C:\Program Files\Common Files\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"
#endif

#if USE_PROGID
#import "progid:Excel.Sheet" auto_search auto_rename rename_search_namespace("Office10") 

#elif USE_LIBID
#import "libid:{00020813-0000-0000-C000-000000000046}" auto_search auto_rename version(1.3) lcid(0) no_search_namespace
#else

#ifndef MSEXCEL_PATH

#if  OFFICE_VER == OFFICEXP
#define   _MSEXCEL_PATH "C:\Program Files\Microsoft Office\Office11\excel.exe"
//#define   _MSEXCEL_PATH "C:\Program Files (x86)\Microsoft Office\Office14\excel.exe"
#elif  OFFICE_VER == OFFICE2000
#define   _MSEXCEL_PATH "C:\Program Files\Microsoft Office\Office\excel.exe"
#endif

#else
#define _MSEXCEL_PATH  M2STR(MSEXCEL_PATH) 
#endif

#import _MSEXCEL_PATH auto_search auto_rename dual_interfaces
#endif // USE_LIBID

///}}

/*
*	CExcel的基本操作,创建工作薄，打开工作薄，保存工作薄，插入工作表，激活工作表
*	CExcelExport: 继承与CExcel导出数据到Excel,一张工作表数据插满会自动插入一张新的工作表
*      数据的插入是针对当前激活的工作表.
*   CExcelExport::AppendData();参数是字符串，\t表示换格，\n表示换行,如果字符串末尾没有\n
	  AppendData会自动添加换行
*
*Remark:所有的方法涉及到的参数, 列的取值范围为（ _A_~_IV_ ), 行的取值范围为 ( 1~65536), 索引号 >= 1
*       工作表和图表两种sheet
*Author:guoxuxing
*Date:2009-10-20
*
*copyright:guoxuxing
*/
#pragma warning(disable:4101)

using namespace Excel;
#include "gxx_base.h"
#include "queue.h"
#include "CriticalSection.h"

class CBitmap;

#ifdef _WINDOWS_
#undef _WINDOWS_
#endif

namespace NS_GXX
{
    //CExcel
    //Excel的打开，保存，关闭，插入工作表
    class CExcel:CComBase
    {
    public:
        CExcel(void);
        ~CExcel(void);
    protected:
        _ApplicationPtr m_ptrApp;                   //一个应用程序智能指针,对应一个工作薄集合
        _ApplicationPtr m_ptrApp_tmp;               //临时进程，供用户使用，防止用户干扰当前工作进程
        WorkbooksPtr m_ptrBooks;                    //工作薄集合
        _WorkbookPtr m_ptrBook;                     //一个工作薄
        SheetsPtr       m_ptrSheets;                //一个工作薄对应一个工作表集合
	
	public:
		//
		// EXCEL: 最多支持 A~IV 列,共计：256
		//
		enum emColumn
		{
			_A_=1,_B_,_C_,_D_,_E_,_F_,_G_,_H_,_I_,_J_,_K_,_L_,_M_,_N_,_O_,_P_,_Q_,_R_,_S_,_T_,_U_,_V_,_W_,_X_,_Y_,_Z_,
			_AA_,_AB_,_AC_,_AD_,_AE_,_AF_,_AG_,_AH_,_AI_,_AJ_,_AK_,_AL_,_AM_,_AN_,_AO_,_AP_,_AQ_,_AR_,_AS_,_AT_,_AU_,_AV_,_AW_,_AX_,_AY_,_AZ_,
			_BA_,_BB_,_BC_,_BD_,_BE_,_BF_,_BG_,_BH_,_BI_,_BJ_,_BK_,_BL_,_BM_,_BN_,_BO_,_BP_,_BQ_,_BR_,_BS_,_BT_,_BU_,_BV_,_BW_,_BX_,_BY_,_BZ_,
			_CA_,_CB_,_CC_,_CD_,_CE_,_CF_,_CG_,_CH_,_CI_,_CJ_,_CK_,_CL_,_CM_,_CN_,_CO_,_CP_,_CQ_,_CR_,_CS_,_CT_,_CU_,_CV_,_CW_,_CX_,_CY_,_CZ_,
			_DA_,_DB_,_DC_,_DD_,_DE_,_DF_,_DG_,_DH_,_DI_,_DJ_,_DK_,_DL_,_DM_,_DN_,_DO_,_DP_,_DQ_,_DR_,_DS_,_DT_,_DU_,_DV_,_DW_,_DX_,_DY_,_DZ_,
			_EA_,_EB_,_EC_,_ED_,_EE_,_EF_,_EG_,_EH_,_EI_,_EJ_,_EK_,_EL_,_EM_,_EN_,_EO_,_EP_,_EQ_,_ER_,_ES_,_ET_,_EU_,_EV_,_EW_,_EX_,_EY_,_EZ_,
			_FA_,_FB_,_FC_,_FD_,_FE_,_FF_,_FG_,_FH_,_FI_,_FJ_,_FK_,_FL_,_FM_,_FN_,_FO_,_FP_,_FQ_,_FR_,_FS_,_FT_,_FU_,_FV_,_FW_,_FX_,_FY_,_FZ_,
			_GA_,_GB_,_GC_,_GD_,_GE_,_GF_,_GG_,_GH_,_GI_,_GJ_,_GK_,_GL_,_GM_,_GN_,_GO_,_GP_,_GQ_,_GR_,_GS_,_GT_,_GU_,_GV_,_GW_,_GX_,_GY_,_GZ_,
			_HA_,_HB_,_HC_,_HD_,_HE_,_HF_,_HG_,_HH_,_HI_,_HJ_,_HK_,_HL_,_HM_,_HN_,_HO_,_HP_,_HQ_,_HR_,_HS_,_HT_,_HU_,_HV_,_HW_,_HX_,_HY_,_HZ_,
			_IA_,_IB_,_IC_,_ID_,_IE_,_IF_,_IG_,_IH_,_II_,_IJ_,_IK_,_IL_,_IM_,_IN_,_IO_,_IP_,_IQ_,_IR_,_IS_,_IT_,_IU_,_IV_
		};
    public:
        		//
        // 创建一个空白工作薄,Excel默认创建了一个Sheet
        //> bShowApp:是否显示Excel应用程序
        //> lpszSheetName:默认的第一张工作表的名称
		//
        BOOL CreateOneBlankBook(BOOL bShowApp = FALSE,const TCHAR *lpszSheetName=_T("Sheet1"));
        
		//
		// 打开一个已存在的工作薄
		//
        BOOL OpenBook(TCHAR *lpszBookPath,BOOL bShowApp = FALSE);

		//
        // 保存工作薄，OpenBook打开的
		//
        BOOL  Save();

		//
		// 保存工作薄,CreateOneBlankBook创建的
		//
		BOOL  Save(LPCTSTR lpszFullPath);

		//
		// 关闭工作薄
		// 
		void Close();

		//
		// 获取工作表的总个数
		// 
		int GetSheetCount();

		_WorkbookPtr GetWorkBook();

		//
		// 在工作表集合的最后面插入一张工作表
		// 
		virtual BOOL InsertSheet(const TCHAR * lpszSheetName);

		BOOL EmptyExcelClipboard();

		void SetMainWnd(HANDLE hWnd){ m_hMainWnd = hWnd; }

		//
		// 在指定位置插入工作表
		//> iIndex: 从1开始
		//> bAtBefore:TRUE:在iIndex的前面插入,FALSE:在iIndex的后面插入
		//
		virtual BOOL InsertSheet(int iIndex, const TCHAR * lpszSheetName,BOOL bAtBefore = TRUE);

		virtual BOOL DeleteSheet(int iIndex);
		virtual BOOL DeleteSheet(const TCHAR * lpszSheetName);

        // add yang
        BOOL InsertChartSheet(int iIndex, const TCHAR *lpszSheetName, BOOL bAtBefore = TRUE);
        // add yang

		//
		// 得到当前激活工作表
		//
		_WorksheetPtr GetActiveSheet();

		//
		// 得到指定的工作表,通过序号
		// iIndex >= 1
		//
		_WorksheetPtr GetSheet(const int iIndex);

		//
		// 得到指定的工作表，通过工作表名称
		// 
		_WorksheetPtr GetSheet(const WCHAR * lpszName);
		
		//
		//得到指定的图表
		// 没找到，返回NULL
		//
		_ChartPtr GetChart(const int iIndex);

		//
		// 得到指定的图表
		// 没找到，返回NULL
		//
		_ChartPtr GetChart(const WCHAR* lpszName);
		//
		// 移动工作表
		// lpszSrcSheetName:  要移动的工作表的名称
		// lpszDesSheetName:  移动到目的工作表的前面或者后面
		// bBefore: TRUE:在目的工作表的前面,FALSE:在目的工作表的后面
		//
		void MoveSheet(const WCHAR *lpszSrcSheetName,const WCHAR *lpszDesSheetName,BOOL bBefore = TRUE);
		//
		// 移动工作表
		// lpszSrcSheetName:  要移动的工作表的名称
		// nDesSheetItem:     移动到目的序号的工作表的前面或者后面
		// bBefore: TRUE:在目的工作表的前面,FALSE:在目的工作表的后面
		//
		void MoveSheet(const WCHAR *lpszSrcSheetName,const int nDesSheetItem,BOOL bBefore = TRUE);

		//
		// 激活指定工作表
		//> iIndex: 从1开始
		//
		virtual BOOL SetActiveSheet(int iIndex);

		//
		// 激活指定名称的工作表
		//> lpszSheetName: 工作表名称
		//
        virtual BOOL SetActiveSheet(TCHAR * lpszSheetName);

		//
        // 设置工作表名称
		// iIndex >= 1
		//
        BOOL SetSheetName(int iIndex,const TCHAR * lpszSheetName);

		//
        // 将列号转换成列标例如: 27 ="AA";
		// iCol >= 1
		//
        static _bstr_t ConvertCol(int iCol);

		//
		// 转换为单元格坐标："A1"
		//	iCol >= 1, iRow >=1
		//
		static _bstr_t ConvertColRow(int iCol,int iRow);

		//
		// 得到一个区域
		//
		static RangePtr GetRangePtr(_WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2);
		
		///{{{ 调整格式辅助函数

		//
		// 设置工作表的列宽
		//
		static void Format_ColsWidth(_WorksheetPtr ptrSheet,int iCol1,int iCol2,float fWidth = 8.50f);

		static void Format_RangeBkColor(_WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr=RGB(196,196,196));
		
		static void Format_RangeFontColor(_WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr=RGB(196,196,196) );

		static void Format_RangeFont(_WorksheetPtr ptrSheet,int iCol1,int iRow1,int iCol2,int iRow2,DWORD clr=RGB(0,0,0),const char* fontName = "宋体",int nFontSize = 10,BOOL bBold = FALSE,BOOL bItalic = FALSE );
		// Method:  Format_BorderRange   [ public ]
		// Description:  给指定区域添加 内外边框
		// Returns [void]: 
		// Parameters: 
		//    [ _WorksheetPtr ptrSheet ]:  指定工作表
		//    [ DWORD clr]: 边框颜色
		//    [ int col1 ]: 起始列 >=1
		//    [ int row1 ]: 起始行 >=1
		//    [ int col2 ]: 结束列 >=1
		//    [ int row2 ]: 结束行 >=1
		//    [ int nWeight ]: 宽度级别取值 1,2,3,4  //1～4从窄到宽
		//    [ int nType   ]: 边框类型 1:只有外边框,2:只有内边框,3:内外边框
		// Author:  Guoxuxing   
		// Modify-Date:2009/12/04
		//************************************
		static void Format_BorderRange(_WorksheetPtr ptrSheet,DWORD clr,int col1,int row1,int col2,int row2,int nWeight = 2,int nType = 3 );

		//
		// 设备某个范围内行的高度
		//
		static void Format_RowsHeight(_WorksheetPtr ptrSheet,int row1,int row2,float fHeight = 19.0f);
		
		//
		//
		//
		static void Format_RangeMerge(_WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2);

		//
		//	设置范围内文本的对齐方式
		//
		//	nHoriAlign:取值为1,2,3.  1表示左对齐,2表示居中，3表示右对齐
		//  nVertAlign:取值为1,2,3.  1表示顶部对齐，2表示居中，3表示底部对齐
		//
		static void Format_RangeAlignment(_WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2,int nHoriAlign = 2,int nVertAlign = 2);

		//
		// Method:  Format_MoveShapeToCell   [ public static ]
		// Description:   将指定 形状对象 移动到指定单元格
		// Returns [void]: 
		// Parameters: 
		//    [ _WorksheetPtr ptrSheet ]:   
		//    [ int nShapeIndex ]:      形状索引
		//    [ int col1 ]:  指定列
		//    [ int row1 ]:  指定行
		//    [ OUT float * pfWidth ]:   如果 pfWidht不为空，得到偏移的宽度
		//    [ OUT float * pfHeight ]:  如果 pfHeight不为空，得到偏移的高度
		// Author:  Guoxuxing   
		// Modify-Date:2009/12/11
		//************************************
		static ShapeRangePtr Format_MoveShapeToCell(_WorksheetPtr ptrSheet,int nShapeIndex,int col1,int row1,OUT float *pfWidth = NULL , OUT float *pfHeight = NULL); 
		
		//
		// 将指定 图形移动到 相对左上角的偏移宽度和高度
		//
		static ShapeRangePtr Format_MoveShape(_WorksheetPtr ptrSheet,int nShapeIndex,IN float fWidth,IN float fHeight);
		
		//
		//	设置指定范围的单元的数字的格式，例如 "0.00" , "0.00%"...
		//
		static void Format_PutNumberFormat(_WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2,const char* nFormat);
		
		//
		//	设置边框
		//
		void Format_PutBorderAround( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2, short sLineStyle = 1, 
			long lWeight =2 , long lCorIndex = 1, DWORD clr=RGB(0,0,0));

		//
		//	设置指定范围的单元的宽度自适应
		//
		static void Format_PutAutoFit( _WorksheetPtr ptrSheet,int col1,int row1,int col2,int row2 , BOOL bCol = TRUE);
		//
		//	设置指定单元格的文本
		//
		static void Format_PutCellText(_WorksheetPtr ptrSheet,int col,int row,const WCHAR * wszText);

		//
		//	冻结指定单元格的左上角区域
		//
		static void Format_Frozen(_WorksheetPtr ptrSheet,int col,int row);

		///}}}
	private:
        TCHAR m_lpBookName[1024];
        BOOL m_bCreated ;          //标识是否已经创建Excel实例
        BOOL m_bClosed ;           //表示是否已经关闭Excel实例

		HANDLE m_hMainWnd;

        static const DWORD m_dwMaxLine = 65536;			//Excel的默认最大行数为 65536,最大列数为"VI",但几乎不可能用到这么多列
    };

////------------------------------------------------------------
//// CExcelExport
///------------------------------------------------------

    class CExcelExport:public CExcel
    {
    public:
        CExcelExport();
        virtual ~CExcelExport();
		///{{{ 重载CExcel的以下四个方法，实现了对各个工作表的操作

		//
        //在指定位置插入工作表
        //>iIndex: 从1开始
        //>bAtBefore:是否在iIndex的前面插入
		//
        virtual BOOL InsertSheet(int iIndex, const TCHAR * lpszSheetName,BOOL bAtBefore  = TRUE );
        
		//
		//在末尾添加一个工作表
		//
		virtual BOOL InsertSheet(const TCHAR *lpszSheetName);

		//
        //激活指定工作表
        //>iIndex: 从1开始
		//
        virtual BOOL SetActiveSheet(int iIndex);

		//
		// 激活指定工作表
		//
        virtual BOOL SetActiveSheet(TCHAR * lpszSheetName);

		///}}}
    public:
		//
        // Method:  AppendData   [ public ]
		// Description:     总是往当前活动的工作表插入数据,以追加的方式
		//					插入数据到内存,1行或者多行
        // Returns [void]: 
        // Parameters: 
        //    [ char * lpszData ]:  =NULL:插入空白行
		//                如果末端没有'\n',InsertOneLine自动添加'\n'.'\t'表示下一单元格
        // Author:  Guoxuxing   
        // Modify-Date:2009/10/23
        //************************************
        void AppendData(char *lpszData = NULL);

		//
		// 同AppendData，但处理大批量的数据导出，使用此方法效率高
		//
		void AppendData(char *lpszData,int nLineCount,int nColumnCount);

		//
		// 该函数是自动调用的，如果有人为的调用，将会出错.
		// 插入的新表，为当前激活的工作表.重载此方法实现特殊的功能
		//
        // 数据插满65536，有必要在当前活动工作表的后面追加一个新的工作表，新工作表
        // 的名称是该活动工作表名称+工作表的数量,如果当前活动工作表的名称为abc,那么新工作表的名称为abc__2,再者为abc__3
		//
        virtual void InsertAppendSheet(TCHAR * lpszNewName = NULL);

        // Method:  InsertOneBmp   [ public ]
        // Description:    插入一张位图
        // Returns [void]: 
        // Parameters: 
        //    [ CBitmap & bmp ]: 位图对象
        //    [ int iRow ]: 起始行号(>=1),0表示默认处理
        //    [ int iCol ]: 起始列号(>=1),0表示默认处理
        // Author:  Guoxuxing   
        // Modify-Date:2009/10/23
		// Remark: 位图立即插入工作表
        //************************************
#ifndef _WINDOWS_
        void InsertOneBmp(IN CBitmap& bmp,int iCol = 0, int iRow = 0);
#endif

		// 往激活sheet中粘贴文本
		void PasteText(const char*lpszData,int iCol1,int iRow1,int iCol2,int iRow2);
    public:
		//
        //将剪切板的内容写入Excel中活动的工作表
        //bNewSheet:>写入之后要不要插入一张新的工作表,如果插入新的工作表，就把新的工作表
        //          作为活动的工作表
        void Flush(BOOL bNewSheet = FALSE);
	public:
		
	protected:
		//
		//工作激活时必须调用此函数
		// 派生类如果调用此方法，需要非常注意
		//
		void UpdateLines();       
    protected:
        static const DWORD MAX_LINE    = 65536 ;   //Excel支持的最到行数
        static const DWORD MEM_SIZE = 0x80000; //500K内存大小
        HANDLE m_hGlobalMem;            //内存句柄，GloableAllocate获得
        CHAR *m_pMem;                //内存指针
        DWORD m_nUsedMem;       //已使用的内存字节数
        int m_iUsedLines ;          //在某个工作表中，已经使用的行数
        int m_iMemLines;           //在内存中已经写入的行数
        int m_iMaxColumns;       //最大列数，用来控制粘贴区域的范围
        
        BOOL m_bFirstInsertData;  //表示激活某个工作后，是否为首次插入数据
    };

	class CExcelExportEx:public CExcelExport
	{
	public:
		CExcelExportEx();
		~CExcelExportEx();

	protected:
		enum emStatus{
			S_CanRead,
			S_Write,
			S_Idle,			
		};
		static const DWORD BLOCK_COUNT = 5;
		static const DWORD BLOCK_SIZE = MEM_SIZE;

		CHAR *m_blocks[BLOCK_COUNT];  //指向块的指针
		int m_status[BLOCK_COUNT];    //各块的使用状态
		int m_usedLength[BLOCK_COUNT]; //各块已使用的字节长度
		int m_iCurrentBlock;  //当前块
		int m_blockMaxCols[BLOCK_COUNT];

		NS_GXX::CQueue<int> m_queueCanRead;

		CCriticalSection m_cs;

		HANDLE m_hEventHaveIdleBlock;
		HANDLE m_hEventCanRead;

		HANDLE m_hEventQueueEmpty;

		//------ 线程控制

		void HoldCanRead();   //阻止可读
		void WaitCanRead();   //等待可读
		void AllowCanRead();  //允许可读

		void WaitIdleBlock(); //等待有空闲块出现
		void HoldIdleBlock(); //没有有空闲块出现
		void AllowIdleBlock(); //有空闲块出现

		//------

		static UINT _ThreadEntry(LPVOID);

		CWinThread *m_pWinThread;
		HANDLE m_hEventThreadFinish; //控制在对象析构时，先等待线程退出

		bool m_bExit;
	public:
		void Flush();
		void AppendData(char *lpszData = NULL);
		void AppendData(char *lpszData,int nLineCount,int nColumnCount);
	};


}

