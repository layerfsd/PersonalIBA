#include "stdafx.h"
#include ".\localcenter.h"
#include "..\IBA_DEBUG.h"
#include "..\IBALog.h"
#include "gxx_base.h"
#include "base64.h"
#include "G2XCommonTool.h"
#include "..\Encrypt.h"
#include "..\NetBarConfig.h"
#include "..\IBA.h"
#include "..\GlobalFunctions.h"
#include "..\Dal\IBADAL.h"
#include "GxxString.h"
#include "..\Socket\LocalServer.h"
#include <time.h>
#include "..\IBAHelpper.h"
#include "..\CashierHelper.h"
#include "..\ServletHellper.h"
#include "..\Servlet\CommonMethod.h"
#include "..\GlobalFunctions.h"
#include "..\CurrentNetBarUser.h"
#include "..\MainFrm.h"


using namespace G2XUtility;
using namespace IBAGlobal;
using namespace NS_DAL;

static const long DATA_CODE_NO_ERROR = 0;
static const long DATA_CODE_INVALID_USER = 1;  // 无效账户
static const long DATA_CODE_INVALID_PASSWORD = 2;  // 账号和密码无效
static const long DATA_CODE_LOCAL_CHECKOUT = 3; // 本地已结账，在未上报中心之前不能上机
static const long DATA_CODE_LOCAL_NO_ACTIVE = 4; // 本地未激活
static const long DATA_CODE_NO_USER_INFO = 5; // 本地缺少用户信息
static const long DATA_CODE_NO_USER_RATE = 6; // 无效用户，缺少扣率信息
static const long DATA_CODE_NO_BALANCE = 7; // 余额不足
static const long DATA_CODE_CHANGE_IN_LOCALMODE = 8; //本地模式不允许换机

static const long DATA_CODE_DATABASE_ERROR = 99; // 数据库异常

//控制一次只有一个线程进行上报的临界资源
static CCriticalSection g_csReportLocalCenterUserLock;

SINGLETON_IMPLEMENT(CLocalCenter)
/*************************************************
Function:       //构造函数
Description:    //定义端口7801,7802,
				//为每个端口new SocketServer,
				//往m_mapHandles中添加命令及命令处理函数对(queryLocalMode,LocalLogon,QueryBalance,ReportLogon,SqlQuery)
*************************************************/
CLocalCenter::CLocalCenter(void) 
{
	UINT nPortArray[SERVER_COUNT] = {7801,7802};

	for (UINT i = 0; i < SERVER_COUNT; i++) {
		m_nPortArray[i] = nPortArray[i];
		m_serverArray[i] = new CG2XSocketServer(this);
	}

	_addHandle("queryLocalMode", &CLocalCenter::HandleQueryLocalMode);
	_addHandle("localLogon", &CLocalCenter::HandleLocalLogon);
	_addHandle("queryBalance", &CLocalCenter::HandleQueryBalance);
	_addHandle("reportLogin", &CLocalCenter::HandleReportLogon);

	_addHandle("sqlQuery", &CLocalCenter::HandleSqlQuery);
}

CLocalCenter::~CLocalCenter(void)
{
	for (UINT i = 0; i < SERVER_COUNT; i++) {
		delete m_serverArray[i];
	}
}
/*************************************************
Function:       //InitLocalCenter
Description:    //为每个端口初始化及启动(创建IOCP,创建开启worker_thread,准备创建连接)
				//开启本地计费线程和监控网络线程. 
Return:         //成功返回TRUE
*************************************************/
BOOL CLocalCenter::InitLocalCenter()
{
	BOOL bRes = FALSE;
	for (UINT i = 0; i < SERVER_COUNT; i++) {
		CG2XSocketServer* pServer = m_serverArray[i];
		if (pServer->Initialize()){
			if (pServer->Start(m_nPortArray[i])){
				bRes = TRUE;
			}
		}
	}

	m_thread.SetCallBack(this, &CLocalCenter::ThreadEntry);
	m_thread.Start();

	//m_threadTestNetwork.SetCallBack(this, &CLocalCenter::ThreadEntryTestNetworking);
	//m_threadTestNetwork.Start();
	return bRes;
}

BOOL CLocalCenter::RecordRegisteredUser(const USERINFO& userInfo)
{	
	if (userInfo._strAccount.IsEmpty() ||
		userInfo._strMemberID.IsEmpty() ||
		userInfo._strClassID.IsEmpty())
	{
		IBA_LOG(_T("RecordRegisteredUser Error: account:%s, memberid:%s, classid:%s"),
			userInfo._strAccount, userInfo._strMemberID, userInfo._strClassID);
		return FALSE;
	}

	// 取得账号
	CGxxString<TCHAR> strAccount = userInfo._strAccount;
	strAccount.FillCharFromLeft(10, '0');

	// 取得memberId
	int nMemberId = _ttoi((LPCTSTR)userInfo._strMemberID);

	// 取得密码
	CString strPassword = userInfo._strPwd;
	strPassword.ReleaseBuffer();
	if (strPassword.GetLength() > 8)
		strPassword = strPassword.Left(8);
	if (!strPassword.IsEmpty())
	{
		int nLen = strPassword.GetLength();
		for (int i=nLen; i<8; ++i)
			strPassword.Append(_T("0"));
	}
	CString strPasswordMD5;
	CEncrypt::CalcMD5(strPassword, strPasswordMD5);

	// 创建上报时间
	CString strReportTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);

	// 会员级别
	int nClassId = _ttoi((LPCTSTR)userInfo._strClassID);

	// 是否本地注册
	//int nLocalRegister = static_cast<int>(userInfo._bLocalRegister);
	int nLocalRegister = 0;

	// 往上报表中插入数据
	CString sql;
	sql.Format(_T("select count(*) from localreport where memberId=%d"), nMemberId);
	if (CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0) 
	{
		sql.Format(_T("update localreport set password='%s',reportTime='%s',localRegister=%d where memberId=%d"), 
			strPasswordMD5, _T(""), nMemberId);
	} 
	else 
	{
		sql.Format(_T("insert into localreport (memberId,netId,password,reportTime,localRegister) values(%d,'%s','%s','%s',%d)"), 
			nMemberId, strAccount.GetData(), strPasswordMD5, _T(""), nLocalRegister);
	}
	if (CIBADAL::GetInstance()->ExecuteSQL(sql) != 1) 
	{
		IBA_LOG(_T("RecordRegisteredUser，插入LocalReport表失败！"));
		return FALSE;
	}
	return TRUE;
}

/**************************************************************************
*
* CG2XServerSocketDelegate
*
***************************************************************************/

/**
客户端连接，并且接收第一组数据
*/
bool CLocalCenter::didClientConnected(CG2XSocketServer* pServer,  PER_SOCKET_CONTEXT* pClient, const char* pData, const int nBufLen )
{
	IBA_LOG(_T("Start didClientConnected"));
	GxxDictionaryPtr reqData;
	int nError = this->_Decode(pData, nBufLen, reqData); //将数据解析到字典中
	if (nError != ERROR_CODE_NONE) {
		// 解码失败，返回错误信息给客户端
		this->OutputErrorInfo(pServer, pClient, nError);
		return false;
	}
	
	this->_handleCommand(reqData->valueForKey("cmd")->stringValue(), reqData, pServer, pClient);

	// 短连接服务，无需建立客户端长连接，永远返回false
	return false;
}
/*************************************************
Function:       //didDumpText
Description:    //输出日志，以CLocalCenter::dump开头
Input:          //const char* pText - 后面跟的文字
*************************************************/
void CLocalCenter::didDumpText( const char* pText, bool isErrorDump/* =false */ )
{
	GXX_TRACE("CLocalCenter::dump: %s", pText);
	CA2T ca(pText);
	IBA_LOG(_T("CLocalCenter::dump: %s"), (const TCHAR*)ca);
}

bool CLocalCenter::didHandleError(CG2XSocketServer* pServer,  PER_SOCKET_CONTEXT* pClient, ERROR_CODE code, DWORD dwLastErrorCode )
{
	return false;
}

/**************************************************************************
*
* 
*
***************************************************************************/

int CLocalCenter::_Decode( const char* data, int nLen, GxxDictionaryPtr& reqData)
{
	// 前两字节表示请求数据的长度
	if (nLen < 2) {
		return ERROR_CODE_ILLEGAL;
	}
	BYTE bt1 = data[0];
	BYTE bt2 = data[1];
	int nReqLen = (int)bt1;
	nReqLen <<= 8;
	nReqLen += (int)bt2;
	if (nReqLen + 2 != nLen) {
		return ERROR_CODE_ILLEGAL;
	}
	
	std::string strReq;
	strReq.append(data+2, nReqLen);

	// base64解密
	strReq = base64_decode(strReq);

	// 转换成json字典
	reqData = GxxDictionary::create();
	if (!reqData->initWithJsonText(strReq.c_str())) {
		// 无效的JSON数据
		return ERROR_CODE_ILLEGAL;
	}
	
	// 验证请求正确性
	if (!reqData->isKeyExist("cmd") || !reqData->isKeyExist("t") || !reqData->isKeyExist("checkcode")) {
		return ERROR_CODE_PARAM;
	}
	
	const char* key = "F3EA0F8D";

	time_t tm = time(NULL);
	long tmReq = reqData->valueForKey("t")->intValue();
	if (abs(tm - tmReq) > 5*60) {
		// 超过5分钟，则验证超时
		return ERROR_CODE_TIMEOUT; 
	}

	std::string codeString;
	codeString.append(reqData->valueForKey("cmd")->stringValue());
	codeString.append(reqData->valueForKey("t")->stringValue());
	codeString.append(key);
	CStringA strNetBarId; strNetBarId.Format("%d", CNetBarConfig::GetInstance()->GetNetBarId());
	codeString.append((const char*)strNetBarId);

	CStringA strMd5;

	CString strCodeString(codeString.c_str());
	//IBA_LOG(_T("CLocalCenter::_Decode  Checkcode before calc: %s"),strCodeString);

	CEncrypt::CalcMD5Bytes((void *)codeString.c_str(), codeString.length(), strMd5.GetBuffer(33));
	strMd5.ReleaseBuffer();

	char* lpsrc = strMd5.GetBuffer();
	int i = MultiByteToWideChar(CP_ACP, 0, lpsrc, -1, NULL, 0);
	wchar_t* strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_ACP, 0, lpsrc, -1, strSrc, i);
	CString strbefMd5(strSrc);
	delete []strSrc;

	//IBA_LOG(_T("CLocalCenter::_Decode  Checkcode after calc: %s"),strbefMd5);

	if (strMd5.CompareNoCase(reqData->valueForKey("checkcode")->stringValue()) != 0){
		// 校验失败
		return ERROR_CODE_CHECK;
	}
	return ERROR_CODE_NONE;
}

void CLocalCenter::OutputErrorInfo( CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, int nErrorCode )
{
	CString str;
	switch (nErrorCode) {
		case ERROR_CODE_ILLEGAL:
			str = _T("非法请求");
			break;
		case ERROR_CODE_CHECK:
			str = _T("校验失败");
			break;
		case ERROR_CODE_PARAM:
			str = _T("缺少参数");
			break;
		case ERROR_CODE_TIMEOUT:
			str = _T("请求过期");
			break;
		case ERROR_CODE_DATA_TOO_LARGE:
			str = _T("数据长度太大");
			break;
		default:
			str = _T("未知错误");
			break;
	}

	GxxDictionaryPtr responseData = GxxDictionary::create();

	CStringA tmp;
	tmp.Format("%d", nErrorCode);	
	responseData->setValueForKey(GxxValue::create(tmp.GetBuffer()), "errorcode");
	tmp.ReleaseBuffer();

	responseData->setValueForKey(ToUtf8Value(str.GetBuffer()), "message");
	str.ReleaseBuffer();

	this->OutputInfo(pServer, pClient, responseData);
}

void CLocalCenter::OutputInfo( CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient, GxxDictionaryPtr outputInfo )
{
	const char* key = "F3EA0F8D";

	if (!outputInfo->isKeyExist("errorcode")) {
		// 没有错误，则多错误码为0
		outputInfo->setValueForKey(GxxValue::create("0"), "errorcode");
	}

	// 添加时间戳
	time_t tm = time(NULL);
	CStringA strTmp;
	strTmp.Format("%lld", tm);
	outputInfo->setValueForKey(GxxValue::create((const char*)strTmp), "t");

	// 添加校验码
	strTmp.Append(key);
	strTmp.AppendFormat("%d", CNetBarConfig::GetInstance()->GetNetBarId());
	CStringA checkCode;
	CEncrypt::CalcMD5Bytes((void*)strTmp.GetString(), strTmp.GetLength(), checkCode.GetBuffer(33));
	checkCode.ReleaseBuffer(33);
	outputInfo->setValueForKey(GxxValue::create((const char*)checkCode), "checkcode");

	// base64编码
	std::string strInfo = outputInfo->describe_d(0, false);
	strInfo = base64_encode((unsigned char*)strInfo.c_str(), strInfo.length());
	int nLen = strInfo.length();

	// 填写长度
	CByteArray byteArray;
	byteArray.SetSize(nLen + 2);
	BYTE bt1,bt2;
	
	bt1 = (nLen & 0x0000FF00)>>8;
	bt2 = nLen & 0x000000FF;
	BYTE* pData = byteArray.GetData();
	pData[0] = bt1; pData[1] = bt2;
	memcpy(pData+2, strInfo.c_str(), nLen);

	if (nLen + 2 > MAX_BUFFER_LEN)
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_DATA_TOO_LARGE);
		return;
	}

	// 给客户端发送数据
	pServer->SendData(pClient, pData, nLen + 2);

	
}

//从预先定义的命令处理map中,查找对应的处理函数并调用数
void CLocalCenter::_handleCommand( const char* command, GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	std::map<std::string, _HandleInfo>::iterator itFound = m_mapHandles.find(std::string(command));
	if (itFound != m_mapHandles.end()) {
		_HandleInfo& info = (*itFound).second;
		_HandleFunc func = info.handle;
		(this->*func)(reqData, pServer, pClient);
		return;
	}

	// 返回请求非法的消息给客户端
	OutputErrorInfo(pServer, pClient, ERROR_CODE_ILLEGAL);
}
/*************************************************
Function:       //_addHandle
Description:    //代表往m_mapHandles中添加pair. 参数1表示具体命令，参数2表示具体处理函数
Input:          //const char* command                  -- 命令字符串
                //CLocalCenter::_HandleFunc memberFunc -- 命令处理函数
*************************************************/
void CLocalCenter::_addHandle( const char* command, CLocalCenter::_HandleFunc memberFunc )
{
	_HandleInfo info;
	info.handle = memberFunc;
	this->m_mapHandles[std::string(command)] = info;
}

/*************************************************
Function:       // ThreadEntryTestNetworking
Description:    // 探测网络断开线程函数。程序启动时启动，每隔一段时间通过向中心请求时间来测试网络状态。如果网络不通，设置网络状态为断开。
				// 如果网络状态已变更到断开，则不测试。
Calls:          // CGetTimeMethod::Execute
*************************************************/
void CLocalCenter::ThreadEntryTestNetworking()
{
	IBA_LOG(_T("Start thread:Test network status"));
	//liyajun Temp
#ifdef _DEBUG
	const DWORD _timerTime = 1000*10;
#else
	const DWORD _timerTime = 1000*90;
#endif
	
	do {
	//try{

		while(TRUE)
		{
			if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal)
			{
				CGetTimeMethod time;
				time.Execute();

				if (time.GetStatusCode() < 0)
				{
					IBA_TRACE(_T("中心网络连接已断开"));
				}
			}		
			Sleep(_timerTime);
		}
		
	//}
	//catch(...)
	//{}
	} while(TRUE);
	
}

/*************************************************
Function:       // ThreadEntry
Description:    // 本地计费线程-程序运行时启动,程序结束时才退出.运行间隔30s
Table Accessed: // SystemConfig/LocalMember/ActiveMember
Table Updated:  // SystemConfig:localCounting,LocalMember
*************************************************/
void CLocalCenter::ThreadEntry()
{
	IBA_LOG(_T("Start thread:Local charging"));
	const DWORD _oneMin = 1000*60;
	const DWORD _timerTime = _oneMin / 2; // 半分钟. Note: 当其他收银台在计费时，该收银台线程每30s循环一次
	do 
	{
	//try
	//{
		while (TRUE) 
		{
			//IBA_LOG(_T("本地计费:New loop"));
			/********校验本地计费收银台*************/
			int nCashierID;
			int nLastUpdateTime;
			//是否存在localCounting记录
			int nRecordCount = CIBADAL::GetInstance()->GetLocalCounting(nCashierID,nLastUpdateTime);
			if(nRecordCount < 0) 
			{
				IBA_LOG(_T("Error.数据库查找本地计费记录失败."));
				Sleep(_timerTime);
				continue;
			}
			if(nRecordCount > 0)//存在则更新
			{
				//当前本地计费收银台是否就是当前收银台
				if(nCashierID == CIBAConfig::GetInstance()->GetCashRegisterID())//是则更新时间
				{
					if(FALSE == CIBADAL::GetInstance()->UpdateLastUpdateTime_LocalCounting()){Sleep(_timerTime);continue;}
				}
				else//不是则检查上次更新时间
				{
					//离上次更新是否已超过2分钟
					time_t now = time(NULL);
					if (now - nLastUpdateTime > 2*_oneMin)//超过则更新为当前收银台计费
					{
						if(FALSE == CIBADAL::GetInstance()->UpdateCashierandTime_LocalCounting()){Sleep(_timerTime);continue;}
					}
					else//未超过.表示其他收银台正在计费,则当前收银台不计费,退出循环.
					{
						IBA_LOG(_T("其他收银台在计费"));
						Sleep(_timerTime);
						continue;
					}
				}
			}
			else//不存在则插入
			{
				if(FALSE == CIBADAL::GetInstance()->AddLocalCounting()){Sleep(_timerTime);continue;}
			}		
			/********开始计费*************/
			//取出LocalMember表所有记录
			LocalMembers lMembers;
			if(FALSE == CIBADAL::GetInstance()->GetAllLocalMember(lMembers)){Sleep(_timerTime);continue;}//没有记录,跳出

			//遍历所有记录
			for(std::vector<CLocalMember>::iterator iter = lMembers.begin();iter != lMembers.end();++iter)
			{
				CLocalMember lmember = *iter;
				/**************************************************************************************
				note:计费流程是localmember先计费,传给UDO,UDO再updatedata给还IBA并同时更新ActiveMember.
				因此,此时的ActiveMember信息是滞后于localmember的,且有区别,所以要使用localmember.
				**************************************************************************************/

				//未激活的不计费
				CActiveMember ActiveMember;
				if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(lmember.nMemberId,ActiveMember)){continue;}
				//未在线的?有时候未上报,但又被踢出去了的.
				if(FALSE == ActiveMember.GetIsOnLine()){continue;}
				//已结账的不计费
				if(lmember.nIsReturned){continue;}
				//未登陆的不计费
				if(FALSE == CIBAHelpper::IsValidTime(lmember.checkinTime) || lmember.checkinTime < COleDateTime(2015,1,1,0,0,0)){continue;}
				//累计封顶的,如果余额为0,不计费
				if(1 == lmember.nPayType && 0 == lmember.nBalance){continue;}
				//足额预扣.包时未到不计费,包时已到转正常计费
				if(2 == lmember.nPayType)
				{
					//校验nextcheckinTime
					if(lmember.nNextCheckinTime < 0)//值非法,转正常计费,跳出
					{	
						CIBADAL::GetInstance()->UpdateLocalMember_PayTypeInfo(lmember.nMemberId,0,0);
						continue;
					}
					else
					{
						time_t now = time(NULL);
						if(now < lmember.nNextCheckinTime)//包时未到不计费
						{
							continue;;
						}
						else//包时到期,转正常计费,跳出
						{
							CIBADAL::GetInstance()->UpdateLocalMember_PayTypeInfo(lmember.nMemberId,0,0);
							continue;;
						}
					}
				}

				//检查间隔时间,<30s不予计费
				time_t lastT = lmember.nLastUpdateTime;
				time_t nowT = time(NULL);
				if (nowT - lastT < 30){continue;}// 小于30秒则不做计算    

				//检查扣率信息，如果扣率无效，不计费
				if(lmember.strRate.GetLength() != 96)
				{
					IBA_LOG(_T("本地计费:无效扣率信息(%s),无计费"),lmember.strRate);
					continue;
				}

				COleDateTime lt(lastT);
				COleDateTime nt(nowT);
				CString strlt = lt.Format(_T("%Y-%m-%d %H:%M:%S"));
				CString strnt = nt.Format(_T("%Y-%m-%d %H:%M:%S"));
				IBA_LOG(_T("本地计费 lastT=%s,nowT=%s"),strlt,strnt);
				IBA_LOG(_T("本地计费 lmember.nBalance=%d,lmember.strRate=%s"),lmember.nBalance,lmember.strRate);
				//计算费用,更新balance
				int nNewBalance = CCashierHelper::CalcExpense(lmember.strRate,lmember.nLastUpdateTime,lmember.nBalance);
				if(nNewBalance <= 0)
				{
					nNewBalance = 0;
				}
				if(0 > CIBADAL::GetInstance()->UpdateLocalMember_BalanceInfo(lmember.nMemberId,nNewBalance))//余额一直为0时，会不做更新
				{
					IBA_LOG(_T("Error.本地计费更新数据库Balance失败."));
					continue;;
				}
				IBA_LOG(_T("本地计费 localMember余额:%d ActiveMember余额:%d"),nNewBalance,ActiveMember.GetAvailavleBalance());

			}		
			//计费间隔30s
			Sleep(_timerTime);
		}
		//}
		//catch(...) 
		//{
		//}
	}while (TRUE);
}

/************************************************************************/
/* 查询本地状态                                                                     */
/************************************************************************/
void CLocalCenter::HandleQueryLocalMode( GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	IBA_LOG(_T("Start HandleQueryLocalMode"));
	// 判断参数是否充足
	if (!reqData->isKeyExist("termID"))
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}
	CString strTermID = CA2T(reqData->valueForKey("termID")->stringValue());// 终端号

	GxxDictionaryPtr outputInfo = GxxDictionary::create();
	if (!outputInfo->isKeyExist("errorcode")) 
	{
		outputInfo->setValueForKey(GxxValue::create("0"), "errorcode");
		outputInfo->setValueForKey(GxxValue::create("request successfully!"), "message");
	}	
	outputInfo->setValueForKey(GxxValue::create(GxxDictionary::create()), "data");

	//当网络为Normal时,localMember表中有该终端则认为还是Broken,否则就认为真的是Normal了
	long nMode(0);
	if(CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkBroken)
	{
		IBA_LOG(_T("HandleQueryLocalMode-网络断开.LocalMode=TRUE."));
		nMode = TRUE;
	}
	else
	{
		CLocalMember lMember;
		if(TRUE == CIBADAL::GetInstance()->GetLocalMemberByTermID(strTermID,lMember))
		{
			IBA_LOG(_T("HandleQueryLocalMode-网络正常.本地数据尚未恢复.LocalMode=TRUE."));
			nMode = TRUE;
		}
	}

	outputInfo->setValueForKeyPath(GxxValue::create(nMode), "data.mode");	
	this->OutputInfo(pServer, pClient, outputInfo);
}

/***************************************************************************************************
Function:       //HandleLocalLogon
Description:    //终端本地模式登陆(本地模式下,用户首次登陆 / 用户一直在线但进入本地模式,UDO自动进行本地登录)
Table Accessed: //LocalMember,LocalReport,CostRate
Table Updated:  //LocalMember
********************************************************************************************************/
void CLocalCenter::HandleLocalLogon( GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	IBA_LOG(_T("Start HandleLocalLogon"));
	// 判断参数是否充足
	if (
		!reqData->isKeyExist("termID") ||
		!reqData->isKeyExist("account") ||
		!reqData->isKeyExist("pwd")) 
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}

	//解析数据
	CGxxString<TCHAR> strAccount = CA2T(reqData->valueForKey("account")->stringValue());// 账号	
	strAccount.FillCharFromLeft(10, _T('0'));// 账号不足10位，左边补零	
	CString strTermID = CA2T(reqData->valueForKey("termID")->stringValue());// 终端号	
	CString strPwd = CA2T(reqData->valueForKey("pwd")->stringValue());// 密码
	
	IBA_LOG(_T("终端本地登录.TermID:%s ACcount:%s"),strTermID,strAccount.GetData());

	//准备返回的数据
	GxxDictionaryPtr outputInfo = GxxDictionary::create();
	outputInfo->setValueForKey(GxxValue::create(GxxDictionary::create()), "data");// 参照文档，增加data节点	

/*******检查各种情况********/

	//检查账号密码(LocalReport表)
	CLocalReport lr;
	if (FALSE == CIBADAL::GetInstance()->GetLocalReport(strAccount.GetData(),strPwd,lr))
	{
		IBA_LOG(_T("本地登录失败.LocalReport无对应记录.账号或密码错误.(NetId:%s Password:%s)"),strAccount.GetData(),strPwd);
		this->PutDataCode(outputInfo, DATA_CODE_INVALID_PASSWORD);
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}
	
	//检查是否激活以及是否有用户信息
	UINT nMemberId = lr.nMemberId;
	CActiveMember ActiveMember;
	if(FALSE == CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember))
	{
		CString sql;
		sql.Format(_T("select count(*) from member where serialNum = '%s'"),strAccount.GetData());
		if(NS_DAL::CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) < 1)
		{
			IBA_LOG(_T("本地登录失败.无用户信息.(NetId:%s)"),strAccount.GetData());
			this->PutDataCode(outputInfo, DATA_CODE_NO_USER_INFO);
			this->OutputInfo(pServer, pClient, outputInfo);
			return;
		}
		IBA_LOG(_T("本地登录失败.用户未激活.(NetId:%s )"),strAccount.GetData());
		this->PutDataCode(outputInfo, DATA_CODE_LOCAL_NO_ACTIVE);
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}

	//检查余额信息及包时到期
	if (ActiveMember.GetAvailavleBalance() == 0)
	{
		if(ActiveMember.GetPayType() == 0)//正常计费余额为0
		{
			IBA_LOG(_T("本地登录失败.余额为0.(NetId:%s)"),strAccount.GetData());
			this->PutDataCode(outputInfo, DATA_CODE_NO_BALANCE);
			this->OutputInfo(pServer, pClient, outputInfo);
			return ;
		}
		else 
		{
			if(ActiveMember.GetPayType() == 1 && ActiveMember.GetIsOnLine() == FALSE)//累计封顶,余额为0且不在线
			{
				IBA_LOG(_T("本地登录失败.累计封顶,余额为0,不允许重新登陆.(NetId:%s)"),strAccount.GetData());
				this->PutDataCode(outputInfo, DATA_CODE_NO_BALANCE);
				this->OutputInfo(pServer, pClient, outputInfo);
				return ;
			}
			else
			{
				if(ActiveMember.GetPayType() == 2 && (ActiveMember.GetNextCheckinTime() < time(NULL)))//足额预扣,且已经到期,余额为0
				{
					IBA_LOG(_T("本地登录失败.足额预扣已到时间,且余额为0.(NetId:%s)"),strAccount.GetData());
					this->PutDataCode(outputInfo, DATA_CODE_NO_BALANCE);
					this->OutputInfo(pServer, pClient, outputInfo);
					return ;
				}
			}
		}
	}

	//检查是否在其他终端上机(有checkin时间以及TermID与当前TermID不一致)
	if(CIBAHelpper::IsValidTime(ActiveMember.GetCheckInTime()) && ActiveMember.GetTerminalID().CompareNoCase(strTermID) != 0)
	{
		IBA_LOG(_T("本地登录失败.用户已在其他终端上机,且本地下不允许换机(NetId:%s Password:%s)"),strAccount.GetData(),strPwd);
		this->PutDataCode(outputInfo, DATA_CODE_CHANGE_IN_LOCALMODE);
		this->OutputInfo(pServer, pClient, outputInfo);
		return ;
	}

	//获取终端ID对应的PCClass
	strTermID.Trim();
	UINT CurPCClassId = CIBADAL::GetInstance()->GetPCClassIDWithTermID(strTermID);
	if(0 >= CurPCClassId)
	{
		IBA_LOG(_T("本地登录失败,未找到终端号(%s)对应的区域ID"),strTermID);
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}

	//从localMember中查找记录.没有则先创建并插入
	CLocalMember LocalMember;
	CString rate;
	if(FALSE == CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,LocalMember)) 
	{
		
		//创建
		GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->CreateNewLocalMemberWithMemberID(nMemberId,strTermID);
		if (!memberPtr)
		{
			this->PutDataCode(outputInfo, DATA_CODE_DATABASE_ERROR);
			this->OutputInfo(pServer, pClient, outputInfo);
			return;
		}
		//插入
		if (FALSE == CIBADAL::GetInstance()->InsertNewLocalMember(memberPtr))
		{
			this->PutDataCode(outputInfo, DATA_CODE_DATABASE_ERROR);
			this->OutputInfo(pServer, pClient, outputInfo);
			return;
		}
		//创建成功,重新读取一次localMember
		CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,LocalMember);
	}
	else//localMember中本就存在的记录,若下面操作失败,可不用删除localMember记录
	{
		if(LocalMember.nIsReturned > 0) //已存在的,已于本地模式下结账,不允许登陆.
		{
			IBA_LOG(_T("本地登录失败.用户已在于本地模式下结账,在上报中心前不得再次登陆.(NetId:%s)"),strAccount.GetData());
			this->PutDataCode(outputInfo, DATA_CODE_LOCAL_CHECKOUT);
			this->OutputInfo(pServer, pClient, outputInfo);
			return ;
		}
	}
	
	//更新终端区域ID(从sm_terminfo表中取)和扣率(从costrate表中取)到localMember表
	//查找扣率
	rate = CIBADAL::GetInstance()->GetCostRateFromCostRate(LocalMember.nClassId,CurPCClassId);
	if(rate.GetLength() != 96)
	{
		IBA_LOG(_T("本地登录失败,无法获取扣率信息(NetId:%s)"),strAccount.GetData());
		this->PutDataCode(outputInfo, DATA_CODE_NO_USER_RATE);
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}
	//更新扣率
	if(0 > CIBADAL::GetInstance()->UpdateLocalMember_RateInfo(nMemberId,rate))
	{
		IBA_LOG(_T("本地登录失败,无法更新扣率信息(NetId:%s)"),strAccount.GetData());
		this->PutDataCode(outputInfo, DATA_CODE_NO_USER_RATE);
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}
	//更新区域ID
	if(0 > CIBADAL::GetInstance()->UpdateLocalMember_PCClassId(nMemberId,CurPCClassId))
	{
		IBA_LOG(_T("本地登录失败,无法更新终端区域信息(NetId:%s)"),strAccount.GetData());
		this->PutDataCode(outputInfo, DATA_CODE_DATABASE_ERROR);
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}

	//更新终端ID,checkintime,localcheckintime,islocalcheckin,LastUpdateTime	
	if (CIBAHelpper::IsValidTime(LocalMember.checkinTime) && LocalMember.checkinTime > COleDateTime(2015,1,1,0,0,0)) //已经登陆,更新localCheckinTime为当前时间,终端ID为当前终端
	{
		if (FALSE == CIBAHelpper::IsValidTime(LocalMember.localCheckinTime) || LocalMember.localCheckinTime < COleDateTime(2015,1,1,0,0,0))
		{
			COleDateTime localCheckinTime = COleDateTime::GetCurrentTime();
			CIBADAL::GetInstance()->UpdateLocalMember_CheckInInfo(nMemberId,strTermID,localCheckinTime);
		}
	}
	else //从未登陆.初次本地登录, 更新checkintime, localCheckinTime,isLocalCheckIn.LastUpdateTime.同时需更新ActiveMemer.checkinTime和TermId
	{
		COleDateTime checkintime = COleDateTime::GetCurrentTime();
		CIBADAL::GetInstance()->UpdateLocalMember_CheckInInfo(nMemberId,strTermID,checkintime,checkintime,TRUE);

		//为了加速显示,更新ActiveMember.CheckinTime和TermId
		CString strtime;
		strtime = checkintime.Format(_T("%Y-%m-%d %H:%M:%S"));
		ActiveMember.SetCheckInTime(checkintime);		
		IBA_LOG(_T("首次本地登录,checkin时间为:%s,memberId为%d"),strtime,nMemberId);
		ActiveMember.SetTerminalID(strTermID);
		ActiveMember.SetPCClassID(CurPCClassId);
		CLocalServer::GetInstance()->ActiveMemberList.UpdateActiveMember(ActiveMember,TRUE);
		CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(FALSE);
	}

	//更新时间成功,重新读取一次localMember
	CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberId,LocalMember);

/*******本地登录成功,返回消息************/
	int nConsumeTime = 0;
	if(CIBAHelpper::IsValidTime(LocalMember.checkinTime))
	{
		time_t lastUpdateTime = time(NULL);
		COleDateTimeSpan span = COleDateTime(lastUpdateTime) - LocalMember.checkinTime;;
		int nSecs = (int)span.GetTotalSeconds();
		if (nSecs > 0) 
		{
			nConsumeTime = nSecs;
		}
	}	
	int nConsumeType = LocalMember.nPayType;
	int nClassId = LocalMember.nClassId;
	int nIsVIP = CNetBarConfig::GetInstance()->GetIsMember(nClassId) ? 1 : 0;
	int nPCClass = LocalMember.nPcClass;
	int nBalance = LocalMember.nBalance;
	nBalance -= nBalance % 50;
	if (nBalance < 0) 
	{
		nBalance = 0;
	}
	int nUsedMoney = LocalMember.ntotalMoney - nBalance; 

	outputInfo->setValueForKeyPath(GxxValue::create((long)nMemberId), "data.memberID");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nBalance), "data.balance");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nConsumeTime), "data.consumeTime");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nConsumeType), "data.consumeType");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nClassId), "data.classID");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nIsVIP), "data.isVIP");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nPCClass), "data.pcClass");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nUsedMoney), "data.usedMoney");
	char sCheckinTime[128];//登陆时间
	memcpy(sCheckinTime,CT2A(LocalMember.checkinTime.Format(_T("%Y-%m-%d %H:%M:%S")).GetBuffer()),sizeof(sCheckinTime));
	outputInfo->setValueForKeyPath(GxxValue::create(sCheckinTime), "data.checkinTime");
	//char sMemberName[128];//用户名
	//memcpy(sMemberName,CT2A(LocalMember.strMemberName.GetBuffer()),sizeof(sMemberName));
	//outputInfo->setValueForKeyPath(GxxValue::create(sMemberName), "data.memberName");
	//IBA_LOG(_T("OnLocalLogon SendToUDO:UserName:%s"),LocalMember.strMemberName);
	CString sql;//用户名
	sql.Format(_T("select * from LocalMember where  memberId=%d"), nMemberId);
	GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
	outputInfo->setValueForKeyPath(GxxValuePtr(memberPtr->valueForKey("memberName")), "data.memberName");
	char sIdNumber[128];//用户证件ID号
	memcpy(sIdNumber,CT2A(LocalMember.strIdNumber.GetBuffer()),sizeof(sIdNumber));
	outputInfo->setValueForKeyPath(GxxValue::create(sIdNumber), "data.idNumber");
	outputInfo->setValueForKeyPath(GxxValue::create((long)LocalMember.nNextCheckinTime), "data.nextCheckinTime");//包时到期时间
	char sRate[128];//扣率
	memcpy(sRate,CT2A(LocalMember.strRate.GetBuffer()),sizeof(sRate));
	IBA_LOG(_T("HandleLocalLogon: SendTo UDO Rate = %s"),LocalMember.strRate);
	outputInfo->setValueForKeyPath(GxxValue::create(sRate), "data.costRate");
	
	this->PutDataCode(outputInfo, DATA_CODE_NO_ERROR);;
	this->OutputInfo(pServer, pClient, outputInfo);
}

void TestBalance(int nTag, CString strNetId, int nMemberID)
{
	CIDCheckCardMethod IDCheckCard;
	IDCheckCard.SetSerialNum(strNetId);
	IDCheckCard.SetSerialNumType(1);

	theApp.GetCurCashier()->DoCheckCard(IDCheckCard);

	int nCardId = 0;
	if (IDCheckCard.GetStatusCode() == 0)
	{
		nCardId = IDCheckCard.GetCardId();

		CBalanceMethod& Balance = CCurrentNetBarUser::GetInstance()->Balance;
		Balance.SetMemberId(nMemberID);
		Balance.SetCardId(nCardId);
		Balance.SetRound(0);

		theApp.GetCurCashier()->DoBalance(Balance);

		int nStatus = Balance.GetStatusCode();
		if (nStatus == 0)
		{
			// 总金额
			int a = Balance.GetTotalBalacne();

			// 充值账户可用余额
			int b = Balance.GetBalance();

			// 上次消费金额(分)，在线值为0
			int c = Balance.GetLastConsumeAmount();

			// 应退余额
			int d = Balance.GetReturnAmount();

			// 押金
			int e = Balance.GetGuarantyMoney();

			// 用户预扣金额
			int f = Balance.GetOnlineAmount();

			IBA_TRACE(_T("%d_____总金额:%d, 充值账户可用余额:%d, 上次消费金额:%d, 应退余额:%d, 押金:%d, 用户预扣金额:%d"),
				nTag, a, b, c, d, e, f);
		}
	}
}

/***************************************************************************************************
Function:       //HandleQueryBalance
Description:    //终端查询本地计费余额
Table Accessed: //LocalMember,LocalReport,CostRate
Table Updated:  //LocalMember
********************************************************************************************************/
void CLocalCenter::HandleQueryBalance( GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	if (!reqData->isKeyExist("memberID")) 
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}
	UINT nMemberID = reqData->valueForKey("memberID")->intValue();

	IBA_LOG(_T("Start HandleQueryBalance-MemberId:%d"),nMemberID);
	
	//从localMember表中查找
	CLocalMember lMember;
	if(FALSE == CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberID,lMember))//无对应记录,返回无效用户
	{
		GxxDictionaryPtr outputInfo = GxxDictionary::create();
		this->PutDataCode(outputInfo, DATA_CODE_INVALID_USER);		// 无效用户
		this->OutputInfo(pServer, pClient, outputInfo);
		return;
	}

	//计算ConsumeTime
	UINT nConsumeTime = 0;
	if (CIBAHelpper::IsValidTime(lMember.checkinTime)) 
	{
		COleDateTimeSpan span = COleDateTime::GetCurrentTime() - lMember.checkinTime;
		int nSecs = (int)span.GetTotalSeconds();
		if (nSecs > 0) 
		{
			nConsumeTime = nSecs;
		}
	}
	
	//计算剩余时间
	UINT nRemainTime = CCashierHelper::CalcRemainTime(lMember.nBalance,lMember.nLastUpdateTime,lMember.strRate);
	IBA_LOG(_T("CLocalCenter::HandleQueryBalance RemainTime = %d"),nRemainTime);

	// 五毛取整
	UINT nBalance = lMember.nBalance - lMember.nBalance % 50;
	if (nBalance < 0)
	{
		nBalance = 0;
	}

	//返回数据
	GxxDictionaryPtr outputInfo = GxxDictionary::create();

	outputInfo->setValueForKey(GxxValue::create(GxxDictionary::create()), "data");
	outputInfo->setValueForKeyPath(GxxValue::create((long)0), "data.code");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nRemainTime), "data.remainTime");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nBalance), "data.balance");
	outputInfo->setValueForKeyPath(GxxValue::create((long)nConsumeTime), "data.consumeTime");
	outputInfo->setValueForKeyPath(GxxValue::create((long)lMember.nPayType), "data.consumeType");

	char sNetCheckinTime[128];
	COleDateTime NextCheckinTime((time_t)lMember.nNextCheckinTime);
	memcpy(sNetCheckinTime,CT2A(NextCheckinTime.Format(_T("%Y-%m-%d %H:%M:%S")).GetBuffer()),sizeof(sNetCheckinTime));
	outputInfo->setValueForKeyPath(GxxValue::create(sNetCheckinTime), "data.nextCheckinTime");

	int nUsedMoney = (lMember.ntotalMoney - nBalance);
	outputInfo->setValueForKeyPath(GxxValue::create((long)nUsedMoney), "data.usedMoney");

	IBA_LOG(_T("Temp. CLocalCenter::HandleQueryBalance Send to UDO :\r\n \
				balance = %d \r\n \
				ConsumeTime = %d \r\n \
				ConsumeType = %d \r\n \
				usedMoney = %d \r\n \
				for MemberId = %d"),nBalance,nConsumeTime,lMember.nPayType,nUsedMoney,lMember.nMemberId);

	//如果网络恢复了,开始上报本地模式数据
	if (CIBAGlobal::NetworkStatus == CIBAGlobal::emNetworkNormal) 
	{
		//本地库查找用户记录.无记录就不用上报.
		CLocalMember lMember;
		if(FALSE == CIBADAL::GetInstance()->GetLocalMemberByMemberID(nMemberID,lMember))
		{
			this->PutDataCode(outputInfo, DATA_CODE_NO_ERROR);
			this->OutputInfo(pServer, pClient, outputInfo);
			return;
		}

		//上报本地充值
		CString sql;
		sql.Format(_T("select * from localMember where memberId=%d and isReturned=0"), nMemberID);
		GxxDictionaryPtr memberPtr = CIBADAL::GetInstance()->ReadRecordFromDB(sql);
		CCashierHelper::ReportLocalCredit(memberPtr);

		//本地模式登陆或中心登陆
		if(lMember.nIsLocalCheckin)//本地模式登录
		{
			//上报本地消息
			CCashierHelper::ReportLocalConsume(lMember);
			//激活用户
			CActivationMethod Activation;

			Activation.SetMemberId(lMember.nMemberId);
			Activation.SetMode( CActivationMethod::ACTIVATION_YES);
			//Activation.SetDotType(nActivationType);
			if(FALSE == theApp.GetCurCashier()->DoActivation(Activation))
			{
				IBA_LOG(_T("Error.上报在线用户.激活失败(本地模式登陆).MemberId:%d,ErrorCode:%d,ErrorMsg:%s"),lMember.nMemberId,Activation.GetStatusCode(),Activation.GetStatusMessage());
			}
		}
		else//中心模式登陆
		{
			if(CNetBarConfig::GetInstance()->GetIsMember(lMember.nClassId))//会员-人工登出,然后激活
			{
				CCheckout2Method Checkout2;
				Checkout2.SetNetBarId(CNetBarConfig::GetInstance()->GetNetBarId());
				Checkout2.SetSafeLogout(0);
				Checkout2.SetMemberId(lMember.nMemberId);	
				Checkout2.SetOperator(theApp.GetCurCashier()->GetName());

				if(FALSE == Checkout2.ExecuteCheckout2())
				{
					IBA_LOG(_T("Error.上报在线用户.为在线会员人工登出失败.MemberId:%d,ErrorCode:%d,ErrorMsg:%s"),lMember.nMemberId,Checkout2.GetStatusCode(),Checkout2.GetStatusMessage());
				}				

				//激活用户
				CActivationMethod Activation;

				Activation.SetMemberId(lMember.nMemberId);
				Activation.SetMode( CActivationMethod::ACTIVATION_YES);
				//Activation.SetDotType(nActivationType);
				if(FALSE == theApp.GetCurCashier()->DoActivation(Activation))
				{
					IBA_LOG(_T("Error.上报在线用户.激活失败(中心模式登陆).MemberId:%d,ErrorCode:%d,ErrorMsg:%s"),lMember.nMemberId,Checkout2.GetStatusCode(),Checkout2.GetStatusMessage());
				}
			}
		}

		//查询余额
		CIDCheckCardMethod CheckCard;
		CString strTmp = lMember.strNetId;
		IBA_LOG(_T("strTmp = %s"),strTmp);
		if (strTmp.GetLength() < 10)
		{
			strTmp.Insert(0, CString(_T("0000000000")).Left(10 - strTmp.GetLength()));
		}
		CheckCard.SetSerialNum(strTmp);
		theApp.GetCurCashier()->DoCheckCard(CheckCard);

		if(CheckCard.GetStatusCode() == 0)
		{
			int nCardId = CheckCard.GetCardId();
			CBalanceMethod Balance;
			Balance.SetMemberId(lMember.nMemberId);
			Balance.SetCardId(nCardId);
			//Balance.SetRound(0);

			theApp.GetCurCashier()->DoBalance(Balance);
		}	

		//删除用户记录
		if(FALSE == CBrokenNetwork::RemoveLocalMember(lMember.nMemberId))
		{
			IBA_LOG(_T("Serious Error!删除本地库记录失败!程序将退出."));
			TerminateProcess(GetCurrentProcess(),0);
		}

		//告知客户端已经上传
		outputInfo->setValueForKeyPath(GxxValue::create((long)1), "data.centerResumed");	

		////检查在线用户是否都上报完毕.完成则设置bHasResumedForOnlineUser,以便让UDO进入正常模式
		//int nCount = CIBADAL::GetInstance()->GetCountOfOnlineUser();
		//if(0 == nCount)
		//{
		//	bHasResumedForOnlineUser = TRUE;
		//}
		
		//检查本地库,都没有记录了,就弹出上报完成日志
		GxxArrayPtr localArrayPtr = CIBADAL::GetInstance()->ReadRecordsFromDB(_T("select * from localMember"));
		if(0 == localArrayPtr->count())
		{
			COperationInfo OperationInfo ;
			OperationInfo.SetDescription (_T("上报本地模式数据完成!"));
			SendMessage(CMainFrame ::m_hOperationLog, WM_OPERATIONLOG, 0, (LPARAM )(&OperationInfo));	
		}
	}
	//无论如何，都是发送无错误
	this->PutDataCode(outputInfo, DATA_CODE_NO_ERROR);
	this->OutputInfo(pServer, pClient, outputInfo);
	OutputDebugString(_T("\n\n================end\n"));
}

/************************************************************************/
/* 上报登录状态                                                                     */
/************************************************************************/
void CLocalCenter::HandleReportLogon( GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	IBA_LOG(_T("Start HandleReportLogon"));
	// 判断参数是否充足
	if (
		!reqData->isKeyExist("account") || 
		!reqData->isKeyExist("memberID") || 
		!reqData->isKeyExist("p") ||
		!reqData->isKeyExist("costRate") ||
		!reqData->isKeyExist("classID") ||
		!reqData->isKeyExist("termID") ||
		!reqData->isKeyExist("pcClass")) 
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}

	//账号
	CGxxString<TCHAR> strAccount = CA2T(reqData->valueForKey("account")->stringValue());
	strAccount.FillCharFromLeft(10, '0');	
	//memberId
	int nMemberId = reqData->valueForKey("memberID")->intValue();
	IBA_LOG(_T("HandleReportLogon: Get memberId = %d"),nMemberId);
	//密码
	CString strPassword = CA2T(reqData->valueForKey("p")->stringValue());
	//上报时间
	CString strReportTime = CIBAHelpper::GetCurrentTimeAsString(TRUE);
	//扣率
	CString strCostRate = CA2T(reqData->valueForKey("costRate")->stringValue());
	IBA_LOG(_T("HandleReportLogon: Get Costrate = %s"),strCostRate);
	//会员级别
	int nClassId = reqData->valueForKey("classID")->intValue();
	IBA_LOG(_T("HandleReportLogon: Get nClassId = %d"),nClassId);
	//机器区域
	int nPcClass = reqData->valueForKey("pcClass")->intValue();
	IBA_LOG(_T("HandleReportLogon: Get nPcClass = %d"),nPcClass);
	//终端号
	CString strTermID = CA2T(reqData->valueForKey("termID")->stringValue());

	BOOL bResult = TRUE;
	
	// 往上报表中更新或插入数据
	CString sql;
	sql.Format(_T("select count(*) from localreport where memberId=%d"), nMemberId);
	if (CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0) 
	{
		sql.Format(_T("update localreport set password='%s',reportTime='%s' where memberId=%d"), 
			strPassword, strReportTime, nMemberId);
	} 
	else 
	{
		sql.Format(_T("insert into localreport (memberId,netId,password,reportTime) values(%d,'%s','%s','%s')"), 
			nMemberId, strAccount.GetData(), strPassword, strReportTime);
	}
	if (CIBADAL::GetInstance()->ExecuteSQL(sql) != 1) 
	{
		CIBALog::GetInstance()->WriteFormat(_T("Error.上报登陆信息HandleReportLogon失败.%s"),sql);
		bResult = FALSE;
	}

	// 往扣率表中更新数据
	/*if (!strCostRate.IsEmpty())
	{
		sql.Empty();
		sql.Format(_T("select count(*) from CostRate where  pcClass=%d and classId=%d"), nPcClass, nClassId);
		if (CIBADAL::GetInstance()->CountOfRecordsWithSQL(sql) > 0)
		{
			sql.Empty();
			sql.Format(_T("update CostRate set rate='%s' where  pcClass=%d and classId=%d"), strCostRate, nPcClass, nClassId);
			IBA_LOG(_T("HandleReportLogon Update costRate:%s"),sql);
			if (CIBADAL::GetInstance()->ExecuteSQLEx(sql) < 0) 
			{
				IBA_LOG(_T("上报登陆信息HandleReportLogon更新CostRate失败,%s"),sql);
				bResult = FALSE;
			}
		} 
		else 
		{
			sql.Empty();
			sql.Format(_T("insert into CostRate (pcClass, classId, rate) values(%d,%d,'%s')"), nPcClass, nClassId, strCostRate);
			IBA_LOG(_T("HandleReportLogon Insert costRate:%s"),sql);
			if (CIBADAL::GetInstance()->ExecuteSQLEx(sql) != 1) 
			{				
				IBA_LOG(_T("上报登陆信息HandleReportLogon失败新增CostRate失败.%s"),sql);
				bResult = FALSE;
			}
		}

		CActiveMember ActiveMember;
		if(!CLocalServer::GetInstance()->ActiveMemberList.GetActiveMember(nMemberId,ActiveMember))
		{
			IBA_LOG(_T("Abnormal! Cant find ActiveMember in ActiveMemberList for MemberId = %d"),nMemberId);
		}
		else
		{
			ActiveMember.SetPolicyInfo(strCostRate); 
			CLocalServer::GetInstance()->GetActiveMemberView()->Refresh(false);
		}
	}

	if (!bResult)
	{
		IBA_LOG(_T("Abnormal.HandleReportLogon失败!"));
	}*/

	GxxDictionaryPtr outputInfo = GxxDictionary::create();

	// 永远告诉UDO上报成功, UDO不对上报的结果做处理
	this->PutDataCode(outputInfo, DATA_CODE_NO_ERROR);

	// 返回数据给客户端
	this->OutputInfo(pServer, pClient, outputInfo);
}

/************************************************************************/
/* sql语句查询数据                                                                    */
/************************************************************************/
void CLocalCenter::HandleSqlQuery( GxxDictionaryPtr reqData, CG2XSocketServer* pServer, PER_SOCKET_CONTEXT* pClient )
{
	IBA_LOG(_T("Start HandleSqlQuery"));
	// 判断参数是否充足
	if (!reqData->isKeyExist("sql")) 
	{
		this->OutputErrorInfo(pServer, pClient, ERROR_CODE_PARAM);
		return;
	}

	GxxDictionaryPtr dataPtr = GxxDictionary::create();

	GxxArrayPtr recordsPtr = CIBADAL::GetInstance()->ReadRecordsFromDB(CA2T(reqData->valueForKey("sql")->stringValue()));

	dataPtr->setValueForKey(GxxValue::create((long)recordsPtr->count()), "count");
	dataPtr->setValueForKey(GxxValue::create(recordsPtr), "records");

	GxxDictionaryPtr outputInfo = GxxDictionary::create();
	outputInfo->setValueForKey(ToUtf8Value(GetDataCodeErrorMessage(DATA_CODE_NO_ERROR).c_str()), "message");
	outputInfo->setValueForKey(GxxValue::create(dataPtr), "data");

	this->OutputInfo(pServer, pClient, outputInfo);

}


GxxValuePtr CLocalCenter::ToUtf8Value( LPCWSTR lpText )
{
	return GxxValue::create(G2XUtility::CG2XCommonTool::WideChar_to_Utf8(std::wstring(lpText)).c_str());
}

std::wstring CLocalCenter::GetDataCodeErrorMessage( long nDataCodeError )
{
	std::wstring msg;
	switch(nDataCodeError){

		case DATA_CODE_NO_ERROR:
			msg = L"请求成功";
			break;

		case DATA_CODE_INVALID_USER:
			msg = L"无效用户";
			break;

		case DATA_CODE_INVALID_PASSWORD:
			msg = L"账号和密码无效";
			break;

		case DATA_CODE_LOCAL_CHECKOUT:
			msg = L"本地已结账，在未上报中心之前不能上机";
			break;

		case DATA_CODE_LOCAL_NO_ACTIVE:
			msg = L"本地未激活";
			break;

		case DATA_CODE_NO_USER_INFO:
			msg = L"本地缺少用户信息";
			break;

		case DATA_CODE_NO_USER_RATE:
			msg = L"无效用户，缺少扣率信息";
			break;

		case DATA_CODE_DATABASE_ERROR:
			msg = L"数据库异常";
			break;

		case DATA_CODE_NO_BALANCE:
			msg = L"余额不足";
			break;

		case DATA_CODE_CHANGE_IN_LOCALMODE:
			msg = L"本地模式下不允许换机";
			break;

		default:
			msg = L"未知错误";
			break;
	}

	return msg;
}

void CLocalCenter::PutDataCode( GxxDictionaryPtr& outputInfo, long nDataCodeError )
{
	outputInfo->setValueForKey(ToUtf8Value(GetDataCodeErrorMessage(nDataCodeError).c_str() ), "message");

	if (!outputInfo->isKeyExist("data")) 
	{
		outputInfo->setValueForKey(GxxValue::create(GxxDictionary::create()), "data");
	}
	outputInfo->setValueForKeyPath(GxxValue::create(nDataCodeError), "data.code");
}

BOOL CLocalCenter::IsBundleTimeEnd( GxxDictionaryPtr& memberPtr )
{
	if (memberPtr->intValueForKey("payType") == 0) 
	{
		// 非包时，肯定是包时已结束
		return TRUE;
	}

	time_t nextCheckinTime = memberPtr->intValueForKey("nextCheckinTime");
	time_t now = time(NULL);

	return now > nextCheckinTime;
}


