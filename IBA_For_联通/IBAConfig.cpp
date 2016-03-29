#include "stdafx.h"
#include "ibaconfig.h"
#include "IBA.h"
#include "IBAHelpper.h"

SINGLETON_IMPLEMENT(CIBAConfig)

CString CIBAConfig::GetAppPath()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strPath(szPath);
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	return strPath;
}

CIBAConfig::CIBAConfig(void)
: m_LogLevel(255), m_CashRegisterID(1), m_nShiftCount(3)
, m_nDefUserClassID(1), m_nPasswordType(0), m_bPopMsgBox(TRUE)
, m_bDynamicReserve(FALSE), m_PrintFontSize(9), m_nOperationAfterSwipingCard(1)
, m_nDefIDTypeId(11), m_nScanDrvId(0), m_bHasCard(0), m_bEnmergencyVisible(FALSE)
, m_nShiftReportType(1), m_nStoreID(1)
, m_nRegisterAge(0), m_nRegisterMoney(0)
, m_nReturnAction(0), m_bShowShiftTotalInfo(TRUE)
, m_BundTimeColor(RGB(0, 0, 0)), m_bEditPayOut(TRUE), m_bCheckIDCardNumber(TRUE)
, m_bShowLastUserNetId(FALSE)
{
	m_strIniFile = theApp.GetWorkPath();
	m_strIniFile.Append(_T("\\IBAConfig\\IBA.ini"));

	CString strFile(theApp.GetWorkPath());
	strFile.Append(_T("\\IBABin\\ScanReco.ini"));
	m_nScanDrvId = GetPrivateProfileInt(_T("SCAN_LOAD"), _T("LOAD_ID"), 0, strFile);

	GPropertyInit(ShowLocalModeButton, FALSE);
	GPropertyInit(PrintTicketCreditTemp, FALSE);
	GPropertyInit(PrintTicketCreditMember, FALSE);
	GPropertyInit(PrintTicketReturnTemp, FALSE);
	GPropertyInit(PrintTicketReturnMember, FALSE);
	GPropertyInit(PrintTicketBTTemp, FALSE);
	GPropertyInit(PrintTicketBTMember, FALSE);
	GPropertyInit(PrintTicketOpenMember, FALSE);
	GPropertyInit(PrintTicketOpenTemp, FALSE);
	GPropertyInit(OperationAfterEnter, CIBAGlobal::emOperationAfterNull);
	GPropertyInit(OperationAfterDClick, CIBAGlobal::emOperationAfterRefund);
	GPropertyInit(MaxCreditMoney, 0);

	//LoadConfig();
}

CIBAConfig::~CIBAConfig(void)
{
	SaveConfig();
}

UINT CIBAConfig::GetLocalSerialNo()
{
	static const UINT MaxLocalSerialNo = 999999; //6个9

	m_LocalSerialNo = AfxGetApp()->GetProfileInt(_T("NetBarInfo"), _T("LocalSerialNo"), 0);

	if (m_LocalSerialNo > MaxLocalSerialNo)
	{
		m_LocalSerialNo = 0;
	}

	m_LocalSerialNo++;

	AfxGetApp()->WriteProfileInt(_T("NetBarInfo"), _T("LocalSerialNo"), m_LocalSerialNo);

	return m_LocalSerialNo;
}

void CIBAConfig::LoadConfig()
{
	ReadValue(_T("Log"), _T("LogLevel"), m_LogLevel, 255);

	ReadValue(_T("IBA"), _T("PopMsgBox"), m_bPopMsgBox, TRUE);
	ReadValue(_T("IBA"), _T("UseOurReader"), m_bUseOurReader, FALSE);
	ReadValue(_T("IBA"), _T("OperationAfterSwipingCard"), m_nOperationAfterSwipingCard, 1);
	ReadValue(_T("IBA"), _T("EnmergencyVisible"), m_bEnmergencyVisible, FALSE);
	ReadValue(_T("IBA"), _T("ShowLastUserNetId"), m_bShowLastUserNetId, TRUE);
	ReadValue(_T("IBA"), _T("MsgLeftBottom"), m_bMsgLeftBottom, FALSE);
	ReadValue(_T("IBA"), _T("MsgAutoHide"), m_bMsgAutoHide, TRUE);

	ReadValue(_T("Register"), _T("PasswordType"), m_nPasswordType, 0);
	ReadValue(_T("Register"), _T("DefPassword"), m_strDefPassword);

	ReadValue(_T("Register"), _T("TempPasswordType"), m_nTempPasswordType, 0);
	ReadValue(_T("Register"), _T("TempDefPassword"), m_strTempDefPassword);

	ReadValue(_T("Register"), _T("DefUserClassID"), m_nDefUserClassID);
	ReadValue(_T("Register"), _T("DefIDTypeId"), m_nDefIDTypeId, 11);

	//用户自带卡
	ReadValue(_T("Register"), _T("MemberHasCard"), m_bHasCard, 0);

	ReadValue(_T("Register"), _T("CheckIDCardNumber"), m_bCheckIDCardNumber, TRUE);

	ReadValue(_T("CashRegister"), _T("CashRegisterID"), m_CashRegisterID, 1);
	ReadValue(_T("CashRegister"), _T("StoreID"), m_nStoreID, 1);
	
	//动态备用金
	ReadValue(_T("Duty"), _T("DynamicReserve"), m_bDynamicReserve, 0);
	ReadValue(_T("Duty"), _T("ShiftReportType"), m_nShiftReportType, 0);

	ReadValue(_T("ShiftSet"), _T("Shift1"), m_strShift1, _T("0-8"));
	ReadValue(_T("ShiftSet"), _T("Shift2"), m_strShift2, _T("8-16"));
	ReadValue(_T("ShiftSet"), _T("Shift3"), m_strShift3, _T("16-24"));

	ReadValue(_T("ShiftSet"), _T("ShiftName1"), m_strShiftName1, _T("早班"));
	ReadValue(_T("ShiftSet"), _T("ShiftName2"), m_strShiftName2, _T("中班"));
	ReadValue(_T("ShiftSet"), _T("ShiftName3"), m_strShiftName3, _T("晚班"));

	ReadValue(_T("ShiftSet"), _T("ShiftCount"), m_nShiftCount, 3); //默认为3个班次

	ReadValue(_T("TTS"), _T("EnableTTS"), m_bEnableTTS, 0);
	ReadValue(_T("TTS"), _T("TTSVol"), m_nTTSVol, 100);
	ReadValue(_T("TTS"), _T("TTSVoice"), m_nTTSVoice, 0);

	/////////////////////////////自动退款段//////////////////////////////////
	ReadValue(_T("Refundment"), _T("RefundmentTime"), RefundmentTime, 30);
	///////////////////////////////////////////////////////////

	ReadValue(_T("Monitor"), _T("OpenAfterSave"), m_bOpenAfterSave, TRUE);
	ReadValue(_T("Monitor"), _T("LittleMoney"), m_nMinMoney, 50);
	ReadValue(_T("Monitor"), _T("PauseRevMsg"), m_bPauseRevMsg, 0);
	ReadValue(_T("Monitor"), _T("ShowAllComputer"), m_bShowAllComputer, TRUE);
	ReadValue(_T("Monitor"), _T("OpenAfterEnter"), m_bOpenAfterEnter, TRUE);
	ReadValue(_T("Monitor"), _T("FontType"), m_nFontType, 0);

	ReadValue(_T("Color"), _T("OnLine"), m_OnLineColor, RGB(0, 0, 255));
	ReadValue(_T("Color"), _T("OffLine"), m_OffLineColor, RGB(255, 0, 0));
	ReadValue(_T("Color"), _T("LockScreen"), m_LockScreenColor, RGB(0, 255, 0));
	ReadValue(_T("Color"), _T("BirthDay"), m_BirthDayColor);
	ReadValue(_T("Color"), _T("Black"), m_BlackColor);
	ReadValue(_T("Color"), _T("VIP"), m_VIPColor);
	ReadValue(_T("Color"), _T("Conflict"), m_ConflictColor);
	ReadValue(_T("Color"), _T("NoRecord"), m_NoRecordColor);
	ReadValue(_T("Color"), _T("LittleMoney"), m_LittleMoneyColor);
	ReadValue(_T("Color"), _T("Thief"), m_ThiefColor, RGB(255, 0, 0));
	ReadValue(_T("Color"), _T("Suspend"), m_SuspendColor);
	ReadValue(_T("Color"), _T("UnLock"), m_UnLockColor);
	//包时颜色
	ReadValue(_T("Color"), _T("BundTime"), m_BundTimeColor, RGB(128, 0, 255));

	ReadValue(_T("Print"), _T("FontSize"), m_PrintFontSize, 9);
	
	ReadValue(_T("UploadImage"), _T("UpLoadIDImage"), m_bUploadIDImage, 1);
	ReadValue(_T("UploadImage"), _T("UploadPersonImage"), m_bUploadPersonImage, 1);
	ReadValue(_T("UploadImage"), _T("IDImageDrvId"), m_nIDImageDrvId, 0);
	ReadValue(_T("UploadImage"), _T("PersonImageDrvId"), m_nPersonImageDrvId, 1);

	/////////////////////////////////座位图路径/////////////////////
	ReadValue(_T("SeatPlan"), _T("SeatBitmapPath"), m_BitmapPath, 0);
	ReadValue(_T("SeatPlan"), _T("DisplayType"), m_nDisplayType, 0);
	/////////////////////////////////交班设置//////////////////////////////
	ReadValue(_T("ShiftSet"), _T("PrintShiftReport"), m_bPrintShiftReport, 0);
	/////////////////////////////是否启用上交金额//////////////////////////
	ReadValue(_T("ShiftSet"), _T("ShowShiftTotalInfo"), m_bShowShiftTotalInfo, 1);
	/////////////允许编辑“其他支出“///////////
	ReadValue(_T("ShiftSet"), _T("EditPayOut"), m_bEditPayOut, 1);

	// 2011/05/19-gxx: 
	ReadValue(_T("ShiftSet"),_T("AllowNextDutyMoney"), SetAllowNextDutyMoney(),TRUE);

	////////////////限制开户的年龄///////////////
	ReadValue(_T("Register"), _T("RegisterAge"), m_nRegisterAge, 0);
	//////////////开户金额///////////
	ReadValue(_T("Register"), _T("RegisterMoney"), m_nRegisterMoney, 0);

	////////////充值退款回车动作////////////
	ReadValue(_T("Credit"), _T("ReturnAction"), m_nReturnAction, 0);

	//if (CNetBarConfig::GetInstance()->GetMustAutoReturn()==1)
	//{
	//	RefundmentTime = 0;  // 强制退款时间为0
	//}

	// 2011/05/27-gxx: 商品入库是否现金支付
	ReadValue(_T("JXC"),_T("JXCPayCash"), SET(JXCPayCash),TRUE);

	// 2011/06/24-gxx: 
	ReadValue(_T("IBA"),_T("ShowLocalModeButton"), SET(ShowLocalModeButton), FALSE);

	// 2011/09/07-8201-gxx: 
	ReadValue(_T("Print"),_T("PrintTicketCreditMember"), SET(PrintTicketCreditMember), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketCreditTemp"), SET(PrintTicketCreditTemp), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketOpenTemp"), SET(PrintTicketOpenTemp), FALSE);  // 临时用户开户默认打印
	ReadValue(_T("Print"),_T("PrintTicketOpenMember"), SET(PrintTicketOpenMember), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketBTMember"), SET(PrintTicketBTMember), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketBTTemp"), SET(PrintTicketBTTemp), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketReturnMember"), SET(PrintTicketReturnMember), FALSE);
	ReadValue(_T("Print"),_T("PrintTicketReturnTemp"), SET(PrintTicketReturnTemp), FALSE);

	// 2011/10/20-8206-gxx: 
	ReadValue(_T("IBA"),_T("OperationAfterEnter"), SET(OperationAfterEnter), CIBAGlobal::emOperationAfterNull);
	ReadValue(_T("IBA"),_T("OperationAfterDClick"), SET(OperationAfterDClick), CIBAGlobal::emOperationAfterRefund);
	
	// 2012/03/05-8238-gxx: 
	ReadValue(_T("IBA"), _T("MaxCreditMoney"), SET(MaxCreditMoney), 0);
}

void CIBAConfig::SaveConfig()
{
	SaveValue(_T("IBA"), _T("PopMsgBox"), m_bPopMsgBox);
	SaveValue(_T("IBA"), _T("UseOurReader"), m_bUseOurReader);
	SaveValue(_T("IBA"), _T("OperationAfterSwipingCard"), m_nOperationAfterSwipingCard);
	SaveValue(_T("IBA"), _T("EnmergencyVisible"), m_bEnmergencyVisible);
	SaveValue(_T("IBA"), _T("ShowLastUserNetId"), m_bShowLastUserNetId);
	SaveValue(_T("IBA"), _T("MsgLeftBottom"), m_bMsgLeftBottom);
	SaveValue(_T("IBA"), _T("MsgAutoHide"), m_bMsgAutoHide);

	SaveValue(_T("CashRegister"), _T("CashRegisterID"), m_CashRegisterID);
	SaveValue(_T("CashRegister"), _T("StoreID"), m_nStoreID);

	SaveValue(_T("Register"), _T("PasswordType"), m_nPasswordType);
	SaveValue(_T("Register"), _T("DefPassword"), m_strDefPassword);

	SaveValue(_T("Register"), _T("TempPasswordType"), m_nTempPasswordType);
	SaveValue(_T("Register"), _T("TempDefPassword"), m_strTempDefPassword);

	SaveValue(_T("Register"), _T("DefUserClassID"), m_nDefUserClassID);
	SaveValue(_T("Register"), _T("DefIDTypeId"), m_nDefIDTypeId);

	SaveValue(_T("Register"), _T("CheckIDCardNumber"), m_bCheckIDCardNumber);
	//用户自带卡
	SaveValue(_T("Register"), _T("MemberHasCard"), m_bHasCard);

	////////////////限制开户的年龄///////////////
	SaveValue(_T("Register"), _T("RegisterAge"), m_nRegisterAge);
	////////////开户金额//////////////
	SaveValue(_T("Register"), _T("RegisterMoney"), m_nRegisterMoney);

	SaveValue(_T("Duty"), _T("ShiftReportType"), m_nShiftReportType);
	//动态保留金额功能
	SaveValue(_T("Duty"), _T("DynamicReserve"), m_bDynamicReserve);

	SaveValue(_T("TTS"), _T("EnableTTS"), m_bEnableTTS);
	SaveValue(_T("TTS"), _T("TTSVol"), m_nTTSVol);
	SaveValue(_T("TTS"), _T("TTSVoice"), m_nTTSVoice);

	/////////////////////////////自动退款段//////////////////////////////////
	SaveValue(_T("Refundment"), _T("RefundmentTime"), RefundmentTime);
	/////////////////////////////////////////////////////////////

	SaveValue(_T("Monitor"), _T("OpenAfterSave"), m_bOpenAfterSave);
	SaveValue(_T("Monitor"), _T("LittleMoney"), m_nMinMoney);
	SaveValue(_T("Monitor"), _T("PauseRevMsg"), m_bPauseRevMsg);
	SaveValue(_T("Monitor"), _T("ShowAllComputer"), m_bShowAllComputer);
	SaveValue(_T("Monitor"), _T("OpenAfterEnter"), m_bOpenAfterEnter);
	SaveValue(_T("Monitor"), _T("FontType"), m_nFontType);

	SaveValue(_T("Color"), _T("OnLine"), m_OnLineColor);
	SaveValue(_T("Color"), _T("OffLine"), m_OffLineColor);
	SaveValue(_T("Color"), _T("LockScreen"), m_LockScreenColor);
	SaveValue(_T("Color"), _T("BirthDay"), m_BirthDayColor);
	SaveValue(_T("Color"), _T("Black"), m_BlackColor);
	SaveValue(_T("Color"), _T("VIP"), m_VIPColor);
	SaveValue(_T("Color"), _T("Conflict"), m_ConflictColor);
	SaveValue(_T("Color"), _T("NoRecord"), m_NoRecordColor);
	SaveValue(_T("Color"), _T("LittleMoney"), m_LittleMoneyColor);
	SaveValue(_T("Color"), _T("Thief"), m_ThiefColor);
	SaveValue(_T("Color"), _T("Suspend"), m_SuspendColor);
	SaveValue(_T("Color"), _T("UnLock"), m_UnLockColor);
	//监控颜色
	SaveValue(_T("Color"), _T("BundTime"), m_BundTimeColor);

	SaveValue(_T("Print"), _T("FontSize"), m_PrintFontSize);

	SaveValue(_T("UploadImage"), _T("UpLoadIDImage"), m_bUploadIDImage);
	SaveValue(_T("UploadImage"), _T("UploadPersonImage"), m_bUploadPersonImage);
	SaveValue(_T("UploadImage"), _T("IDImageDrvId"), m_nIDImageDrvId);
	SaveValue(_T("UploadImage"), _T("PersonImageDrvId"), m_nPersonImageDrvId);

	/////////////////////////////////座位图路径/////////////////////
	SaveValue(_T("SeatPlan"), _T("SeatBitmapPath"), m_BitmapPath);
	SaveValue(_T("SeatPlan"), _T("DisplayType"), m_nDisplayType);
	/////////////////////////////////交班设置//////////////////////////////
	SaveValue(_T("ShiftSet"), _T("PrintShiftReport"), m_bPrintShiftReport);
	//////////////////////////////是否使能上交金额////////////////////////
	SaveValue(_T("ShiftSet"), _T("ShowShiftTotalInfo"), m_bShowShiftTotalInfo);
	/////////////允许编辑“其他支出“///////////
	SaveValue(_T("ShiftSet"), _T("EditPayOut"), m_bEditPayOut);

	SaveValue(_T("ShiftSet"), _T("ShiftCount"), m_nShiftCount);

	SaveValue(_T("ShiftSet"), _T("Shift1"), m_strShift1);
	SaveValue(_T("ShiftSet"), _T("Shift2"), m_strShift2);
	SaveValue(_T("ShiftSet"), _T("Shift3"), m_strShift3);

	SaveValue(_T("ShiftSet"), _T("ShiftName1"), m_strShiftName1);
	SaveValue(_T("ShiftSet"), _T("ShiftName2"), m_strShiftName2);
	SaveValue(_T("ShiftSet"), _T("ShiftName3"), m_strShiftName3);

	SaveValue(_T("ShiftSet"), _T("AllowNextDutyMoney"), GetAllowNextDutyMoney());

	////////////充值退款回车动作////////////
	SaveValue(_T("Credit"), _T("ReturnAction"), m_nReturnAction);

	// 2011/05/27-gxx: 商品入库是否现金支付
	SaveValue(_T("JXC"),_T("JXCPayCash"), GET(JXCPayCash));

	// 2011/06/24-gxx: 
	SaveValue(_T("IBA"),_T("ShowLocalModeButton"), GET(ShowLocalModeButton));

	// 2011/09/07-8201-gxx: 
	SaveValue(_T("Print"),_T("PrintTicketCreditMember"), GET(PrintTicketCreditMember));
	SaveValue(_T("Print"),_T("PrintTicketCreditTemp"), GET(PrintTicketCreditTemp));
	
	SaveValue(_T("Print"),_T("PrintTicketOpenTemp"), GET(PrintTicketOpenTemp));
	SaveValue(_T("Print"),_T("PrintTicketOpenMember"), GET(PrintTicketOpenMember));

	SaveValue(_T("Print"),_T("PrintTicketBTMember"), GET(PrintTicketBTMember));
	SaveValue(_T("Print"),_T("PrintTicketBTTemp"), GET(PrintTicketBTTemp));

	SaveValue(_T("Print"),_T("PrintTicketReturnMember"), GET(PrintTicketReturnMember));
	SaveValue(_T("Print"),_T("PrintTicketReturnTemp"), GET(PrintTicketReturnTemp));

	SaveValue(_T("IBA"),_T("OperationAfterEnter"), GET(OperationAfterEnter));
	SaveValue(_T("IBA"),_T("OperationAfterDClick"), GET(OperationAfterDClick));

	// 2012/03/05-8238-gxx: 
	SaveValue(_T("IBA"), _T("MaxCreditMoney"), GET(MaxCreditMoney));
}

UINT CIBAConfig::GetLogLevel() const
{
	return m_LogLevel;
}

COLORREF CIBAConfig::GetUserTypeColor(UINT nClassID)
{ 
	COLORREF UserTypeColor;//用户级别颜色

	CString strTemp;
	strTemp.Format(_T("UserType%d"), nClassID);

	ReadValue(_T("Color"), strTemp, UserTypeColor, RGB(0, 0, 0));

	return UserTypeColor; 
}

void CIBAConfig::SetUserTypeColor(UINT nClassID, COLORREF newVal)
{ 
	COLORREF UserTypeColor;//用户级别颜色
	UserTypeColor = newVal;

	CString strTemp;
	strTemp.Format(_T("UserType%d"), nClassID);

	SaveValue(_T("Color"), strTemp, UserTypeColor);
}

UINT CIBAConfig::GetShowPCClassType(UINT nClassID)
{ 
	UINT ShowPCClassType = 0;//显示隐藏计算机区域

	CString strTemp;
	strTemp.Format(_T("PCClassType%d"), nClassID);

	ReadValue(_T("Monitor"), strTemp, ShowPCClassType, 1);

	return ShowPCClassType; 
}

void CIBAConfig::SetShowPCClassType(UINT nClassID, UINT newVal)
{ 
	UINT  ShowPCClassType = newVal;//显示隐藏计算机区域

	CString strTemp;
	strTemp.Format(_T("PCClassType%d"), nClassID);

	SaveValue(_T("Monitor"), strTemp, ShowPCClassType);
}

UINT CIBAConfig::GetDynamicReserveMoney()
{
	UINT nRet = 0;
	ReadValue(_T("Duty"), _T("DynamicReserveMoney"), nRet, 0);

	return nRet;
}

void CIBAConfig::SetDynamicReserveMoney(UINT newVal)
{
	SaveValue(_T("Duty"), _T("DynamicReserveMoney"), newVal);
}

BOOL CIBAConfig::GetPrintCreditTicket(BOOL bIsMember)
{
	if (bIsMember)
	{
		return GetPrintTicketCreditMember();
	}
	else
	{
		return GetPrintTicketCreditTemp();
	}
}

BOOL CIBAConfig::GetPrintReturnTicket(BOOL bIsMember)
{
	if (bIsMember)
	{
		return GetPrintTicketReturnMember();
	}
	else
	{
		return GetPrintTicketReturnTemp();
	}
}

BOOL CIBAConfig::GetPrintTicketBT(BOOL bIsMember)
{
	if (bIsMember)
	{
		return GetPrintTicketBTMember();
	}
	else
	{
		return GetPrintTicketBTTemp();
	}
}

