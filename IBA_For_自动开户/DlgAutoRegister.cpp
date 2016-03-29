// DlgAutoRegister.cpp : implementation file
//

#include "stdafx.h"
#include "IBA.h"
#include "DlgAutoRegister.h"
#include ".\dlgautoregister.h"
#include "IBAHelpper.h"
#include "Servlet\RegisterMethod.h"
#include "Cashier.h"
#include "Servlet\WinHttpRequest.h"

#define M_TASK_OVER 2000
#define M_INSERT_STRING 2001

#define S *1000

// CDlgAutoRegister dialog

IMPLEMENT_DYNAMIC(CDlgAutoRegister, CDialog)
CDlgAutoRegister::CDlgAutoRegister(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoRegister::IDD, pParent)
{
	m_hEventSetExit1 = CreateEvent(NULL, TRUE, FALSE, _T(""));
	m_hEventSetExit2 = CreateEvent(NULL, TRUE, FALSE, _T(""));
	m_hEventSetExit3 = CreateEvent(NULL, TRUE, FALSE, _T(""));
	m_hEventTheadNoRun1 = CreateEvent(NULL, TRUE, TRUE, _T(""));
	m_hEventTheadNoRun2 = CreateEvent(NULL, TRUE, TRUE, _T(""));
	m_hEventTheadNoRun3 = CreateEvent(NULL, TRUE, TRUE, _T(""));
}

CDlgAutoRegister::~CDlgAutoRegister()
{
}

void CDlgAutoRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT2, m_str24Amount);
	DDX_Text(pDX, IDC_EDIT3, m_strRandomAmount);
	DDX_Text(pDX, IDC_EDIT4, m_strStartMin);
	DDX_Text(pDX, IDC_EDIT5, m_strEndMin);
	DDX_Text(pDX, IDC_EDIT6, m_strFilePath);

	DDX_CBIndex(pDX, IDC_COMBO1, m_nStartHour);
	DDX_CBIndex(pDX, IDC_COMBO2, m_nEndHour);

	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_dateCtrlStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_dateCtrlEnd);

	DDX_Control(pDX, IDC_LIST1, m_listBox);

	DDX_Control(pDX, IDOK, m_btnStart);
	//DDX_Control(pDX, IDCANCEL, m_btnStop);
}


BEGIN_MESSAGE_MAP(CDlgAutoRegister, CDialog)
	ON_BN_CLICKED(IDC_BTN_SELECT, OnBnClickedBtnSelect)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(M_INSERT_STRING, OnInsertString)
	ON_MESSAGE(M_TASK_OVER, OnTaskOver)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_HIDE, OnBnClickedBtnHide)
END_MESSAGE_MAP()


// CDlgAutoRegister message handlers

void CDlgAutoRegister::OnBnClickedBtnSelect()
{
	CFileDialog    dlgFile(TRUE, m_strFilePath, m_strFilePath, OFN_HIDEREADONLY, _T("身份证列表文件 (*.txt)|*.txt|All Files (*.*)|*.*||"), NULL);


	if (IDOK == dlgFile.DoModal())
	{
		m_strFilePath = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}

int GetSeconds(COleDateTime& time1, COleDateTime& time2)
{
	SYSTEMTIME sm1, sm2;
	time1.GetAsSystemTime(sm1);
	time2.GetAsSystemTime(sm2);
	CTime tm1(sm1);
	CTime tm2(sm2);
	return (int)(tm1.GetTime() - tm2.GetTime());
}
int GetSeconds(COleDateTime& time1)
{
	SYSTEMTIME sm1;
	time1.GetAsSystemTime(sm1);
	CTime tm1(sm1);
	return (int)(tm1.GetTime());
}

void CDlgAutoRegister::SetRandomArray( int *pRandomAmount, int nFac, int dayIndex )
{
	if (nFac <= 0)
	{
		nFac = 1;
	}
	int nOneLen = (int)m_config.m_arrayRandomAmount.size();
	int nAm = m_config.m_array24Amount[dayIndex] / nFac;
	int nAm1,nAm2;
	if (nAm < 10) nAm1 = nAm2 = 10;
	else
	{
		nAm1 = nAm - 5;
		nAm2 = nAm + 5;
	}
	for (int i = 0; i < nOneLen; i++)
	{
		pRandomAmount[i] = m_config.m_arrayRandomAmount[i];
	}
	for (int i = nOneLen; i < nOneLen * 2; i++)
	{
		pRandomAmount[i] = nAm1;
	}
	for (int i = nOneLen*2; i < nOneLen * 3; i++)
	{
		pRandomAmount[i] = nAm2;
	}
}

// 一天最多可以充值次数就是金额,也即是2000元的话,则最多可以充值两千次

// 每充一次的单位间隔时间就是,时间总秒数除以总金额(用双浮点数存储,时间长了用整型存储误差很大)

// 下一次开户时间应该是当前时间加上开户金额乘以单位间隔时间

// 第一个参数充值总金额, 第二个参数返回开户数组
void GetRegisterMoneyArr(int nAmount, std::vector<int>& vecI)
{
	const int nArrCount = 3;
	int nMoneyArr[nArrCount] = {5, 10, 20};
	int nTotoalMoney = 0;
	while(nTotoalMoney < nAmount)
	{
		int iIndex = rand() % nArrCount;
		nTotoalMoney += nMoneyArr[iIndex];
		vecI.push_back(nMoneyArr[iIndex]);
	}
}

void CDlgAutoRegister::OnBnClickedOk()
{
	if (!SaveConfig())
		return;

	FILE *fp;
	if ((fp=fopen(CT2A(m_strFilePath), "r"))==NULL) {
		CString strErr;
		strErr.Format(_T("启动失败, 无法打开文件%s"), m_strFilePath);
		AfxMessageBox(strErr);
		return;
	}

	m_btnStart.EnableWindow(FALSE);

	// 读取文件，证件信息
	while(1) {
		char buf[200] = {0};

		if (!fgets(buf, 200, fp))
		{
			break;
		}

		CString str = CA2T(buf);
		str.Trim();
		int nSpace = str.Find(_T(' '));
		if (nSpace < 0) continue;
		int nSpace2 = str.ReverseFind(_T(' '));
		if (nSpace2 < 0) continue;

		IDInfo info;
		ZeroMemory(&info, sizeof(info));
		_tcscpy(info.idNumber, str.Mid(0, nSpace));
		_tcscpy(info.name, str.Mid(nSpace2 + 1));
		if (_tcslen(info.idNumber) != 18)
		{
			continue;
		}
		info.sex = (BYTE)(info.idNumber[16] - _T('0')) % 2;

		GXX_TRACE(_T("%s %s"), info.idNumber, info.name);

		m_arrayIDs.push_back(info);
	}

	COleDateTime begTime;
	COleDateTime curTime = COleDateTime::GetCurrentTime();
	if(m_config.m_startTime < curTime)
	{
		begTime = curTime;
	}
	else
	{
		begTime = m_config.m_startTime;
	}
	
	COleDateTime timeTodayEnd;
	// 结束时间不是今天
	if(m_config.m_startTime.GetDay() != m_config.m_endTime.GetDay())
	{
		timeTodayEnd.SetDateTime(begTime.GetYear(), begTime.GetMonth(), begTime.GetDay(), 23,59,59);
	}
	else
	{
		timeTodayEnd = m_config.m_endTime;
	}

	int dBeginDaySeconds = GetSeconds(timeTodayEnd, begTime);

	double dBeginDayInterval = (double)dBeginDaySeconds / (double)m_config.m_array24Amount[0];		// 第一天单位间隔开户时间
	if(0 == m_config.m_array24Amount.size())
		return ;

	m_arrayTasks.clear();
	int iIdIndex = 0;
	// 获取第一天的开户数组
	std::vector<int> vecRegMoneyBeginDay;
	GetRegisterMoneyArr(m_config.m_array24Amount[0], vecRegMoneyBeginDay);
	for(size_t i = 1; i < vecRegMoneyBeginDay.size(); i++)
	{
		TaskNode tn;
		tn.idInfo = m_arrayIDs[iIdIndex];
		tn.nDeltaSeconds = vecRegMoneyBeginDay[i] * dBeginDayInterval;
		tn.nAmount = vecRegMoneyBeginDay[i] * 100;

		m_arrayTasks.push_back(tn);
		iIdIndex++;
		if(iIdIndex >= m_arrayIDs.size())
		{
			iIdIndex = 0;
		}
	}

	if(2 <= m_config.m_array24Amount.size())
	{
		// 获取最后一天的开户数组
		COleDateTime timeEndDayEnd;
		timeEndDayEnd.SetDateTime(m_config.m_endTime.GetYear(), m_config.m_endTime.GetMonth(), m_config.m_endTime.GetDay(), 23,59,59);
		int dEndDaySeconds = GetSeconds(timeEndDayEnd, m_config.m_endTime);

		double dEndInterval = (double)dEndDaySeconds / (double)m_config.m_array24Amount[m_config.m_array24Amount.size() - 1];		// 最后一天单位间隔开户时间

		std::vector<int> vecRegMoneyEndDay;
		GetRegisterMoneyArr(m_config.m_array24Amount[m_config.m_array24Amount.size() - 1], vecRegMoneyEndDay);
		for(size_t i = 1; i < vecRegMoneyEndDay.size(); i++)
		{
			TaskNode tn;
			tn.idInfo = m_arrayIDs[iIdIndex];
			tn.nDeltaSeconds = vecRegMoneyEndDay[i] * dEndInterval;
			tn.nAmount = vecRegMoneyEndDay[i] * 100;

			m_arrayTasks.push_back(tn);
			iIdIndex++;
			if(iIdIndex >= m_arrayIDs.size())
			{
				iIdIndex = 0;
			}
		}

		// 获取中间天数
		int i = m_arrayTasks.size();
		std::vector<int> vecRegMoney24;
		for(size_t i = 1; i < m_config.m_array24Amount.size() - 1; i++)
		{
			GetRegisterMoneyArr(m_config.m_array24Amount[i], vecRegMoney24);
			double d24Interval = (double)(24 * 60 * 60) / (double)m_config.m_array24Amount[i];
			for(size_t i = 1; i < vecRegMoney24.size(); i++)
			{
				TaskNode tn;
				tn.idInfo = m_arrayIDs[iIdIndex];
				tn.nDeltaSeconds = vecRegMoney24[i] * d24Interval;
				tn.nAmount = vecRegMoney24[i] * 100;

				m_arrayTasks.push_back(tn);
				iIdIndex++;
				if(iIdIndex >= m_arrayIDs.size())
				{
					iIdIndex = 0;
				}
			}
			int i = m_arrayTasks.size();
			vecRegMoney24.clear();
		}

	}

	m_theThread1.SetCallBack(this, CDlgAutoRegister::Run);
	m_theThread1.Start();

	m_theThread2.SetCallBack(this, CDlgAutoRegister::Run2);
	m_theThread2.Start();

	m_theThread3.SetCallBack(this, CDlgAutoRegister::Run3);
	m_theThread3.Start();
}


//void CDlgAutoRegister::OnBnClickedOk()
//{
//	if (!SaveConfig())
//		return;
//
//	FILE *fp;
//	if ((fp=fopen(CT2A(m_strFilePath), "r"))==NULL) {
//		CString strErr;
//		strErr.Format(_T("启动失败, 无法打开文件%s"), m_strFilePath);
//		AfxMessageBox(strErr);
//		return;
//	}
//
//	m_btnStart.EnableWindow(FALSE);
//
//	// 读取文件，证件信息
//	while(1) {
//		char buf[200] = {0};
//		
//		if (!fgets(buf, 200, fp))
//		{
//			break;
//		}
//		
//		CString str = CA2T(buf);
//		str.Trim();
//		int nSpace = str.Find(_T(' '));
//		if (nSpace < 0) continue;
//		int nSpace2 = str.ReverseFind(_T(' '));
//		if (nSpace2 < 0) continue;
//
//		IDInfo info;
//		ZeroMemory(&info, sizeof(info));
//		_tcscpy(info.idNumber, str.Mid(0, nSpace));
//		_tcscpy(info.name, str.Mid(nSpace2 + 1));
//		if (_tcslen(info.idNumber) != 18)
//		{
//			continue;
//		}
//		info.sex = (BYTE)(info.idNumber[16] - _T('0')) % 2;
//
//		GXX_TRACE(_T("%s %s"), info.idNumber, info.name);
//
//		m_arrayIDs.push_back(info);
//	}
//
//	// 
//	int nTotalSeconds = GetSeconds(m_config.m_endTime, m_config.m_startTime);
//	int nDeltaSecodes = nTotalSeconds / m_arrayIDs.size();
//
//	int nStart = GetSeconds(m_config.m_startTime);
//	COleDateTime dtTmp;
//	dtTmp.SetDateTime(m_config.m_startTime.GetYear(), m_config.m_startTime.GetMonth(), m_config.m_startTime.GetDay(), 23,59,59);
//	int nEnd = GetSeconds(dtTmp);
//
//	int nLastEnd = GetSeconds(m_config.m_endTime);
//
//	BYTE dayIndex = 0;
//
//	int nSecs = nStart;
//
//	int nOneLen = (int)m_config.m_arrayRandomAmount.size();
//	int * pRandomAmount = new int[nOneLen * 3];
//	int nFac = (nEnd - nStart) / nDeltaSecodes;
//	SetRandomArray(pRandomAmount, nFac, dayIndex);
//
//	for (size_t i = 0; i < m_arrayIDs.size(); i++)
//	{
//		if (nSecs > nEnd)
//		{
//			dayIndex ++;
//			nStart = nEnd + 1;
//			if (nEnd > nLastEnd)
//			{
//				nEnd = nLastEnd;
//			}
//			else
//			{
//				nEnd = nStart + 24 * 3600 - 1;
//			}
//			nFac = (nEnd - nStart) / nDeltaSecodes;
//			SetRandomArray(pRandomAmount, nFac, dayIndex);
//		}
//		TaskNode tn;
//		tn.idInfo = m_arrayIDs[i];
//		tn.dayIndex = dayIndex;
//		tn.nDeltaSeconds = nDeltaSecodes;
//		tn.nAmount = 100 * pRandomAmount[rand() % (nOneLen * 3)];
//
//		m_arrayTasks.push_back(tn);
//
//		nSecs += nDeltaSecodes;
//	}
//
//	delete []pRandomAmount;
//
//	m_theThread1.SetCallBack(this, CDlgAutoRegister::Run);
//	m_theThread1.Start();
//
//	m_theThread2.SetCallBack(this, CDlgAutoRegister::Run2);
//	m_theThread2.Start();
//
//	m_theThread3.SetCallBack(this, CDlgAutoRegister::Run3);
//	m_theThread3.Start();
//}

void CDlgAutoRegister::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CDlgAutoRegister::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_config.ReadFrom();

	m_str24Amount = m_config.Get24Amount();
	m_strRandomAmount = m_config.GetRandomAmount();
	m_nStartHour = m_config.m_startTime.GetHour();
	m_nEndHour = m_config.m_endTime.GetHour();
	m_strStartMin.Format(_T("%d"), m_config.m_startTime.GetMinute());
	m_strEndMin.Format(_T("%d"), m_config.m_endTime.GetMinute());
	m_dateCtrlStart.SetTime(m_config.m_startTime);
	m_dateCtrlEnd.SetTime(m_config.m_endTime);

	m_strFilePath = m_config.m_strFilePath;

	UpdateData(FALSE);

	return TRUE;
}

bool CDlgAutoRegister::SaveConfig()
{
	UpdateData(TRUE);

	m_config.m_strFilePath = m_strFilePath;
	m_config.Set24Amount(m_str24Amount);
	m_config.SetRandomAmount(m_strRandomAmount);
	if (!m_config.SetTimeRange(m_dateCtrlStart, m_dateCtrlEnd, m_strStartMin, m_strEndMin, m_nStartHour, m_nEndHour))
	{
		return false;
	}
	m_config.SaveTo();
	return true;
}

CString GetCardIdFromCenter(CString netId)
{
	CString strTmp = netId;
	//在账号前段插入0
	if (strTmp.GetLength() < 10)
	{
		strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
	}

	BOOL bRet = FALSE;

	CIDCheckCardMethod checkCard;
	checkCard.SetSerialNum(strTmp);

	::CoInitialize(NULL);
	theApp.GetCurCashier()->DoCheckCard(checkCard);
	::CoUninitialize();

	if (checkCard.GetStatusCode() == 0)
	{
		if (checkCard.GetMemberId() != 0) //已经开过的卡,充值退卡
		{
			return _T("");
		}
		else //新卡注册
		{
			return checkCard.GetCardIdAsString();
		}
	}
	else
	{
		return _T("");
	}

}

void CDlgAutoRegister::RegisterID( TaskNode& task )
{
	CRegisterMethod UserRegister;
	UserRegister.SetUserClass(1);
	UserRegister.SetIdType(12); // 二代证
	UserRegister.SetPersonalId(task.idInfo.idNumber);
	UserRegister.SetCountry(_T("CN"));
	UserRegister.SetUserName(task.idInfo.name);
	UserRegister.SetSex(task.idInfo.sex);
	UserRegister.SetCostExpense(0);//不受自带卡影响，因为设为0
	UserRegister.SetGuarantyMoney(0);

	CString strCardID = GetCardIdFromCenter(task.idInfo.idNumber);
	UserRegister.SetCardId(strCardID);
	UserRegister.SetNoCard(1);
	//UserRegister.SetNoCard(0); //固定收押金
	UserRegister.SetNoPresent(1);
	UserRegister.SetPassword("");
	UserRegister.SetCreditAmount(task.nAmount);
	UserRegister.SetAuditId(task.idInfo.idNumber);

	//深圳特殊的几个字段
	UserRegister.SetTel(_T(""));
	UserRegister.SetIdDepart(_T(""));
	UserRegister.SetJob(_T(""));
	UserRegister.SetAddress(_T(""));

	UserRegister.SetTimeId(0);

	if(!strCardID.IsEmpty())
	{
		theApp.GetCurCashier()->DoRegister(UserRegister);
		if (UserRegister.GetStatusCode() == 0)
		{
			CString str;
			str.Append(CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
			str.Append(_T(": "));
			str.AppendFormat(_T("%s %s %d元"), task.idInfo.idNumber, task.idInfo.name, task.nAmount / 100);

			ConsumeNode node;
			ZeroMemory(&node, sizeof(ConsumeNode));
			_tcscpy(node.idNumber, task.idInfo.idNumber);
			node.nMemberId = UserRegister.GetMemberId();

			COleDateTime cur = COleDateTime::GetCurrentTime();
			COleDateTime checkIn = cur + COleDateTimeSpan(0, 0, 1, 0);
			int n = task.nAmount / 100;
			int nH = n / 2;
			int nM = (n % 2 == 0 ? 0 : 30);
			COleDateTime checkOut = cur + COleDateTimeSpan(0, nH, nM, 0);
			_tcscpy(node.checkInTime,CIBAHelpper::FormatTime(checkIn));
			_tcscpy(node.checkOutTime,CIBAHelpper::FormatTime(checkOut));
			node.OleCheckOutTime = checkOut;
			node.OleCheckInTime = checkIn;
			node.nAmount = task.nAmount;
			_tcsncpy(node.name, task.idInfo.name, 10);
			COleDateTimeSpan timeSpan = checkOut - checkIn;
			node.nTimeConsume = timeSpan.GetTotalSeconds();

			node.bInvalid = 0;
			m_cs.Lock();
			m_arrConsumeNode.push_back(node);
			m_cs.Unlock();

			CString *pStr = new CString;
			*pStr = str;
			PostMessage(M_INSERT_STRING, (WPARAM)pStr);
		}
		else
		{
			CString str;
			str.Append(CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
			str.Append(_T(": 失败! "));
			str.AppendFormat(_T("%s %s %d元 %s"), task.idInfo.idNumber, task.idInfo.name, task.nAmount / 100, UserRegister.GetStatusMessage());

			CString *pStr = new CString;
			*pStr = str;
			PostMessage(M_INSERT_STRING, (WPARAM)pStr);
		}
	}
	else
	{
		CString str;
		str.Append(CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
		str.Append(_T(": 失败! "));
		str.AppendFormat(_T("%s %s %d元 %s"), task.idInfo.idNumber, task.idInfo.name, task.nAmount / 100, _T("卡号已经存在"));

		CString *pStr = new CString;
		*pStr = str;
		PostMessage(M_INSERT_STRING, (WPARAM)pStr);
	}
	
}

void CDlgAutoRegister::DeRegister( DeRegisterNode& node )
{
	CDeregisterMethod Deregister;

	Deregister.SetMemberId(node.nMemberId);
	Deregister.SetPersonalId(node.idNumber);
	Deregister.SetMemberName(node.name);
	Deregister.SetCardId(0);
	Deregister.SetPassword(_T(""));
	Deregister.SetEndUsingTime(_T(""));

	theApp.GetCurCashier()->DoDeregister(Deregister);

	if (Deregister.GetStatusCode() == 0)
	{	
		CString str;
		str.Append(CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
		str.Append(_T(": 退款成功 "));
		str.AppendFormat(_T("%s %s"), node.idNumber, node.name);

		CString *pStr = new CString;
		*pStr = str;
		PostMessage(M_INSERT_STRING, (WPARAM)pStr);
	}
	else
	{
		CString str;
		str.Append(CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
		str.Append(_T(": 退款失败! "));
		str.AppendFormat(_T("%s %s %s"), node.idNumber, node.name, Deregister.GetStatusMessage());

		CString *pStr = new CString;
		*pStr = str;
		PostMessage(M_INSERT_STRING, (WPARAM)pStr);
	}
}

int GetRandomClassID()
{
	const int nMinClassID = 1;
	const int nMaxClassID = 3;
	int nClassID = rand() % nMaxClassID;
	if(nClassID < nMinClassID)
		nClassID = nMinClassID;
	return nClassID;
}

int GetPCClassID()
{
	const int nMinPCClassID = 1;
	const int nMaxPCClassID = 3;
	int nClassID = rand() % nMaxPCClassID;
	if(nClassID < nMinPCClassID)
		nClassID = nMinPCClassID;
	return nClassID;
}

CString GetRandomTermID()
{
	const int nMinTermID = 1;
	const int nMaxTermID = 99;
	int nTermID = rand() % nMaxTermID;
	if(nTermID < nMinTermID)
		nTermID = nMinTermID;

	CString strTermID;
	strTermID.Format(_T("PC%03d"), nTermID);
	return strTermID;
}

int GetRandomRefNo()
{
	const int nMinRefNo = 1;
	const int nMaxRefNo = 999999; 
	int nRefNo = rand() % nMaxRefNo;
	if(nRefNo < nMinRefNo)
		nRefNo = nMinRefNo;
	return nRefNo;
}

// 发送扣费记录
void CDlgAutoRegister::SendConsumeRecord( ConsumeNode& node )
{
	static const LONG CodePage_GB2312 = 936; // 简体中文 (GB2312)

	static const LONG WinHttpRequestOption_URLCodePage = 2;

	CWinHttpRequest HttpRequest(FALSE); 

	try
	{
		HttpRequest.Create();
		HttpRequest.SetOption(WinHttpRequestOption_URLCodePage, COleVariant(CodePage_GB2312));

		HttpRequest.SetTimeouts(5000, 5000, 10000, 1000 * 15);

		CString strUrlMain = _T("http://");
		strUrlMain += CNetBarConfig::GetInstance()->GetMainCenterIP();
		strUrlMain.AppendFormat(_T(":%d"), CNetBarConfig::GetInstance()->GetMainCenterPort());
		strUrlMain += _T("/netbar/interface/Consume.jsp");
		strUrlMain.AppendFormat(_T("?pwd=%s"),_T("netbarneibushiyong"));
		strUrlMain.AppendFormat(_T("&submitTime=%s"), CIBAHelpper::FormatTime(COleDateTime::GetCurrentTime()));
		strUrlMain.AppendFormat(_T("&netBarId=%d"),CNetBarConfig::GetInstance()->GetNetBarId());
		strUrlMain.AppendFormat(_T("&refNo=%d"), GetRandomRefNo());
		strUrlMain.AppendFormat(_T("&serialNo=%d"), CIBAConfig::GetInstance()->GetLocalSerialNo());
		strUrlMain.AppendFormat(_T("&memberId=%d"), node.nMemberId);
		strUrlMain.AppendFormat(_T("&checkinTime=%s"), node.checkInTime);
		strUrlMain.AppendFormat(_T("&checkoutTime=%s"), node.checkOutTime);
		strUrlMain.AppendFormat(_T("&classId=%d"), GetRandomClassID());
		strUrlMain.AppendFormat(_T("&pcClass=%d"), GetPCClassID());
		strUrlMain.AppendFormat(_T("&termId=%s"), GetRandomTermID());
		strUrlMain.AppendFormat(_T("&amount=%d"), node.nAmount);
		strUrlMain.AppendFormat(_T("&timeConsume=%d"), node.nTimeConsume);

		strUrlMain.Replace(_T(" "), _T("%20"));

		GXX_TRACE(_T("扣消费记录的请求: %s"), strUrlMain.GetString());


		
		HttpRequest.Open(strUrlMain.GetString());
		HttpRequest.Send();

		DeRegisterNode deRegNode;
		deRegNode.OldDeRegisterTime = node.OleCheckOutTime + COleDateTimeSpan(0, 0, 5, 0);	// 下机后5分钟退款
		deRegNode.nMemberId = node.nMemberId;
		_tcscpy(deRegNode.idNumber, node.idNumber);
		_tcscpy(deRegNode.name, node.name);
		
		deRegNode.bInvalid = 0;
		m_cs.Lock();
		m_arrDeRegisterNode.push_back(deRegNode);
		m_cs.Unlock();

		GXX_TRACE(_T("Response: %s"), HttpRequest.GetResponseText());
	}
	catch(...)
	{

	}
}

LRESULT CDlgAutoRegister::OnInsertString( WPARAM wParam, LPARAM lParam )
{
	//LPCTSTR lpString = (LPCTSTR)wParam;
	//m_listBox.InsertString(0, lpString);
	CString* pStr = (CString*)wParam;
	m_listBox.InsertString(0, pStr->GetString());
	delete pStr;
	return 0;
}

LRESULT CDlgAutoRegister::OnTaskOver( WPARAM wParam, LPARAM lParam )
{
	m_btnStart.EnableWindow(TRUE);
	m_btnStart.EnableWindow(FALSE);
	return 0;
}

// 注册线程
void CDlgAutoRegister::Run()
{	
	ResetEvent(m_hEventTheadNoRun1);
	
	::CoInitialize(NULL);
	int nStart = GetSeconds(m_config.m_startTime);
	int nLast = GetSeconds(m_config.m_endTime);
	while(1)
	{
		int nCurrent = (int)time(NULL);
	//	if (nCurrent > nLast)
	//	{
	//		PostMessage(M_TASK_OVER);
	//		SetEvent(m_hEventTheadNoRun1);
	//		::CoUninitialize();
	//		return;
	//	}
		try
		{
			if (nCurrent >= nStart)
			{
				int n = m_arrayTasks.size();
				for (int i = 0; i < (int)m_arrayTasks.size(); i++)
				{
					TaskNode& task = m_arrayTasks[i];
					//Sleep(task.nDeltaSeconds S);
					SleepThread(task.nDeltaSeconds, 1);

					RegisterID(task);
				}
				::CoUninitialize();
				PostMessage(M_TASK_OVER);
				SetEvent(m_hEventTheadNoRun1);
				return ;
			}
		}
		catch(...)
		{
			IBA_LOG0(_T("出现异常1"));
		}
		

		//Sleep(1 S);
		SleepThread(1,1);
	}
	::CoUninitialize();
	PostMessage(M_TASK_OVER);
	SetEvent(m_hEventTheadNoRun1);
}

// 消费线程
void CDlgAutoRegister::Run2()
{
	ResetEvent(m_hEventTheadNoRun2);
	::CoInitialize(NULL);

	while(1)
	{
		//Sleep(1 * 60 S); // 5分钟
		SleepThread(5 * 60, 2);
		try
		{
			int nCount = 0;
			m_cs.Lock();
			nCount = (int)m_arrConsumeNode.size();
			m_cs.Unlock();

			if (nCount > 0)
			{
				for (int i = 0; i < nCount; i++)
				{
					m_cs.Lock();
					ConsumeNode& node = m_arrConsumeNode[i];
					m_cs.Unlock();

					if(0 == node.bInvalid)
					{
						::CoInitialize(NULL);
						SendConsumeRecord(node);
						::CoUninitialize();
						m_cs.Lock();
						node.bInvalid = 1;
						m_cs.Unlock();
					}
					SleepThread(1, 2);
				}
			}
		}
		catch(...)
		{
			IBA_LOG0(_T("出现异常2"));
		}
		
	}

	::CoUninitialize();
	SetEvent(m_hEventTheadNoRun2);
}

void CDlgAutoRegister::Run3()
{	
	ResetEvent(m_hEventTheadNoRun3);

	::CoInitialize(NULL);
	while(1)
	{
		try
		{
			int nCount = 0;
			m_cs.Lock();
			nCount = (int)m_arrDeRegisterNode.size();
			m_cs.Unlock();

			if (nCount > 0)
			{
				for (int i = 0; i < nCount; i++)
				{
					m_cs.Lock();
					DeRegisterNode& node = m_arrDeRegisterNode[i];
					m_cs.Unlock();

					if(0 == node.bInvalid)
					{
						if(node.OldDeRegisterTime < COleDateTime::GetCurrentTime())
						{
							DeRegister(node);
							m_cs.Lock();
							node.bInvalid = 1;
							m_cs.Unlock();
						}
					}
					SleepThread(1, 3);
				}
			}
		}
		catch(...)
		{
			IBA_LOG0(_T("出现异常3"));
		}
		SleepThread(1 * 60, 3);	// 一分钟检测一次
	}
	SetEvent(m_hEventTheadNoRun3);
}

void CDlgAutoRegister::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	m_arrayIDs.clear();
	m_arrayTasks.clear();
	m_arrConsumeNode.clear();
	m_arrDeRegisterNode.clear();
	WaitThreadExit();
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
}

void CDlgAutoRegister::OnBnClickedBtnHide()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
}

void CDlgAutoRegister::WaitThreadExit()
{
	SetEvent(m_hEventSetExit1);
	SetEvent(m_hEventSetExit2);
	SetEvent(m_hEventSetExit3);
	WaitForSingleObject(m_hEventTheadNoRun1, 20 * 1000);
	WaitForSingleObject(m_hEventTheadNoRun2, 20 * 1000);
	WaitForSingleObject(m_hEventTheadNoRun3, 20 * 1000);
}

void CDlgAutoRegister::SleepThread( int nSeconds, int nThreadTag )
{
	//nSeconds = 5;
	if(1 == nThreadTag)
	{
		for(int i = 0; i < nSeconds; i++)
		{
			if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEventSetExit1, 0))
			{
				::CoUninitialize();
				ResetEvent(m_hEventSetExit1);
				SetEvent(m_hEventTheadNoRun1);
			}
			Sleep(1000);
		}
	}
	else if(2 == nThreadTag)
	{
		for (int i = 0; i < nSeconds; i++)
		{
			if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEventSetExit2, 0))
			{
				::CoUninitialize();
				ResetEvent(m_hEventSetExit2);
				SetEvent(m_hEventTheadNoRun2);
			}
			Sleep(1000);
		}
	}
	else if(3 == nThreadTag)
	{
		for (int i = 0; i < nSeconds; i++)
		{
			if(WAIT_OBJECT_0 == WaitForSingleObject(m_hEventSetExit3, 0))
			{
				::CoUninitialize();
				ResetEvent(m_hEventSetExit3);
				SetEvent(m_hEventTheadNoRun3);
			}
			Sleep(1000);
		}
	}
}

void CDlgAutoRegister::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	OnBnClickedBtnStop();
	DestroyWindow();
	//CDialog::OnCancel();
}


//------------------------------------------------------------------------------------
bool CAutoRegisterConfig::SetTimeRange( 
	CDateTimeCtrl& start, CDateTimeCtrl& end, 
	CString& strStartMin, CString& strEndMin, 
	int nStartHour, int nEndHour )
{
	SYSTEMTIME smStart, smEnd;
	start.GetTime(&smStart);
	end.GetTime(&smEnd);
	
	m_startTime.SetDateTime(smStart.wYear, smStart.wMonth, smStart.wDay,nStartHour,
		_ttoi(strStartMin), 0);

	m_endTime.SetDateTime(smEnd.wYear, smEnd.wMonth, smEnd.wDay,nEndHour,
		_ttoi(strEndMin), 0);

	COleDateTimeSpan dtSpan = m_endTime - m_startTime;

	if (dtSpan.GetTotalSeconds() <= 0)
	{
		AfxMessageBox(_T("结束时间不得小于起始时间"));
		return false;
	}
	COleDateTime dtEnd, dtStart;
	dtStart.SetDate(m_startTime.GetYear(), m_startTime.GetMonth(), m_startTime.GetDay());
	dtEnd.SetDate(m_endTime.GetYear(), m_endTime.GetMonth(), m_endTime.GetDay());
	COleDateTimeSpan dtSpan2;
	if(dtSpan2.GetTotalDays() > m_array24Amount.size())
	{
		AfxMessageBox(_T("开始时间和结束时间之间的天数不能大于每天开户金额的天数"));
		return false;
	}
	
	return true;
}

void CAutoRegisterConfig::SetRandomAmount( CString& strRandomAmount )
{
	m_arrayRandomAmount.clear();
	CStringArray strArray;
	CIBAHelpper::SplitLine(strRandomAmount, strArray, _T(','));
	for (int i = 0; i < strArray.GetCount(); i++)
	{
		m_arrayRandomAmount.push_back((UINT)_ttoi(strArray.GetAt(i)));
	}
}

void CAutoRegisterConfig::SaveTo()
{
	CString iniFile = theApp.GetWorkPath();
	iniFile.Append(_T("\\IBAConfig\\register.ini"));

	CString str;
	str = Get24Amount();
	::WritePrivateProfileString(_T("Base"), _T("Amount24"), str, iniFile);

	str = GetRandomAmount();
	::WritePrivateProfileString(_T("Base"), _T("RandomAmount"), str, iniFile);

	str = CIBAHelpper::FormatTime(m_startTime);
	::WritePrivateProfileString(_T("Base"), _T("StartTime"), str, iniFile);

	str = CIBAHelpper::FormatTime(m_endTime);
	::WritePrivateProfileString(_T("Base"), _T("EndTime"), str, iniFile);

	str = m_strFilePath;
	::WritePrivateProfileString(_T("Base"), _T("IDFile"), str, iniFile);
}

void CAutoRegisterConfig::ReadFrom()
{
	CString iniFile = theApp.GetWorkPath();
	iniFile.Append(_T("\\IBAConfig\\register.ini"));

	{
		CString str;
		::GetPrivateProfileString(_T("Base"), _T("Amount24"), _T(""), str.GetBuffer(200), 200, iniFile);
		str.ReleaseBuffer();
		Set24Amount(str);
	}

	{
		m_arrayRandomAmount.clear();
		CString str;
		::GetPrivateProfileString(_T("Base"), _T("RandomAmount"), _T("5,10,20"), str.GetBuffer(200), 200, iniFile);
		str.ReleaseBuffer();
		CStringArray strArray;
		CIBAHelpper::SplitLine(str, strArray, _T(','));
		for (int i = 0; i < strArray.GetCount(); i++)
		{
			m_arrayRandomAmount.push_back(_ttoi(strArray.GetAt(i)));
		}
	}
	{
		CString str;
		::GetPrivateProfileString(_T("Base"), _T("StartTime"), _T(""), str.GetBuffer(200), 200, iniFile);
		str.ReleaseBuffer();
		str.Remove(_T(' '));
		str.Remove(_T('-'));
		str.Remove(_T(':'));
		m_startTime = CIBAHelpper::CenterTimeToOleDateTime(str);
	}
	{
		CString str;
		::GetPrivateProfileString(_T("Base"), _T("EndTime"), _T(""), str.GetBuffer(200), 200, iniFile);
		str.ReleaseBuffer();
		str.Remove(_T(' '));
		str.Remove(_T('-'));
		str.Remove(_T(':'));
		m_endTime = CIBAHelpper::CenterTimeToOleDateTime(str);
	}
	{
		CString str;
		::GetPrivateProfileString(_T("Base"), _T("IDFile"), _T(""), str.GetBuffer(200), 200, iniFile);
		str.ReleaseBuffer();
		m_strFilePath = str;
	}
}

CString CAutoRegisterConfig::GetRandomAmount()
{
	CString str;
	for (size_t i = 0; i < m_arrayRandomAmount.size(); i++)
	{
		str.AppendFormat(_T("%d"), m_arrayRandomAmount[i]);
		if (i < m_arrayRandomAmount.size() - 1)
		{
			str.AppendChar(_T(','));
		}
	}
	return str;
}

void CAutoRegisterConfig::Set24Amount( CString& str24Amount )
{
	m_array24Amount.clear();
	CStringArray strArray;
	CIBAHelpper::SplitLine(str24Amount, strArray, _T(','));
	for (int i = 0; i < strArray.GetCount(); i++)
	{
		m_array24Amount.push_back((UINT)_ttoi(strArray.GetAt(i)));
	}
}

CString CAutoRegisterConfig::Get24Amount()
{
	CString str;
	for (size_t i = 0; i < m_array24Amount.size(); i++)
	{
		str.AppendFormat(_T("%d"), m_array24Amount[i]);
		if (i < m_array24Amount.size() - 1)
		{
			str.AppendChar(_T(','));
		}
	}
	return str;
}
