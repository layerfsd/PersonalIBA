#include "stdafx.h"
#include ".\idtypeinfo.h"
#include ".\DALJXC.h"
#include ".\DALUSER.h"
#include "..\IBA.h"
#include "..\Encrypt.h"

using namespace NS_DAL;



////////////////////////
CCommodity::CCommodity()
{
	GPropertyInit( UnitMultiple, 0 );
	GPropertyInit( ContractPrice, 0 );
	GPropertyInit( AdvicePrice, 0 );
	GPropertyInit( UnitCost, 0 );
	GPropertyInit( AccountType, 0 );
	GPropertyInit( RemainQuantity, 0 );
	GPropertyInit( Point, 0 );
	GPropertyInit( StoreInQuantity, 0 );
	GPropertyInit( StoreInAmount, 0 );
}

CReturnedInfo::CReturnedInfo()
{
	m_nNetBarId = 0;
	m_nRefNo = 0;
	m_nSerialNo = 0;
	m_nMemberId = 0;
	m_nCardId = 0;
	m_nDeposit = 0;
	m_nCashRegisterID = 0;
	m_nCostExpense = 0;
	m_nClassId = 0;
	m_nRoundAmount = 0;
	m_nReturnAmount = 0;
}

CCardCloseInfo::CCardCloseInfo()
{
	m_nNetBarId = 0;
	m_nRefNo = 0;
	m_nSerialNo = 0;
	m_nMemberId = 0;
	m_nCardId = 0;
	m_nDeposit = 0;
	m_nCashRegisterID = 0;
	m_nCostExpense = 0;
	m_nClassId = 0;
}

CCardOpenInfo::CCardOpenInfo()
{
	m_nNetBarId = 0;
	m_nRefNo = 0;
	m_nSerialNo = 0;
	m_nMemberId = 0;
	m_nCardId = 0;
	m_nDeposit = 0;
	m_nCostExpense = 0;
	m_nCashRegisterID = 0;
	m_nWeek = 0;
	m_nClassId = 0;
	m_nStatus = 0;
	m_nIdType = 0;
	m_nAccountNetBarId = 0;
}

CCreditInfoDB::CCreditInfoDB()
{
	m_nNetBarId = 0;
	m_nRefNo = 0;
	m_nSerialNo = 0;
	m_nMemberId = 0;
	m_nCreditAmount = 0;
	m_nCreditPresent = 0;
	m_nPoint = 0;
	m_nCashRegisterID = 0;
	m_nWeek = 0;
	m_nCassId = 0;
	m_nStatus = 0;
	m_strCategory = _T("ZS");
}

CRetailVoucher::CRetailVoucher()
{
	GPropertyInit( RefNo,          0 );
	GPropertyInit( SerialNo,       0 );
	GPropertyInit( MemberId,       0 );
	GPropertyInit( Week,           0 );
	GPropertyInit( ClassId,        0 );
	GPropertyInit( CashRegisterID, 0 );
	GPropertyInit( Point,          0 );
	GPropertyInit( StoreId,        0 );
	GPropertyInit( NetBarId,       0 );
	GPropertyInit( RetailType,     0 );
}

CRetailDetail::CRetailDetail()
{
	GPropertyInit( Quantity,       0 );
	GPropertyInit( Price,          0 );
	GPropertyInit( Amount,         0 );
	GPropertyInit( Cost,           0 );
	GPropertyInit( Point,          0 );
	GPropertyInit( ReturnQuantity, 0 );
}



NS_DAL::CLocalConsumeInfo::CLocalConsumeInfo()
{
	GPropertyInit(NetbarId, CNetBarConfig::GetInstance()->GetNetBarId());
	GPropertyInit(SerialNo, CIBAConfig::GetInstance()->GetLocalSerialNo());
	GPropertyInit3(RefNo, MemberId, DataStatus,
		           0,     0,        0);
	GPropertyInit4(CreditAmount, ConsumeAmount, ReturnAmount, TimeConsume,
		           0,     0,        0,          0);
	GPropertyInit3(ClassId, PayType, PCClass,
		           0,     0,        0);
}

void NS_DAL::CLocalConsumeInfo::MakeMD5()
{
	//校验串=MD5(上网账号、姓名、证件号码、充值金额、
	//          上网消费金额、退款金额、上网开始时间、
	//          上网结束时间、上机终端号、操作员、网吧PSK)
	/*BYTE* pBuf = (BYTE*)this;
	size_t nLen = sizeof(CLocalConsumeInfo) - */
	CString str;
	str += GetSerialNum();
	str += GetName();
	str += GetIdNumber();
	str.AppendFormat(L"%d", GetCreditAmount());
	str.AppendFormat(L"%d", GetConsumeAmount());
	str.AppendFormat(L"%d", GetReturnAmount());
	str += GetCheckinTime();
	str += GetCheckoutTime();
	str += GetTermId();
	str += GetOperator();
	str += CNetBarConfig::GetInstance()->GetNetBarKey();
	CEncrypt::CalcMD5(str, CheckData);
}