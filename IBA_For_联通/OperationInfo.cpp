

#include "StdAfx.h"
#include "OperationInfo.h"


COperationInfo::COperationInfo()
{
	m_nOperationType = 0;
	m_nOperationAmount = 0;
	m_nDeposit = 0;
	m_nAllAmount = 0;
	m_bIsBundTime = FALSE;
	m_nMemberId = 0;
	m_nBundTimeAmount = 0;
	m_nPresentAmount = 0;
	m_bIsMember = FALSE;
	GPropertyInit(IsResumeData, FALSE);
	GPropertyInit(ClassId, 0);
	GPropertyInit(ConsumeMoney, 0);
}

COperationInfo::~COperationInfo()
{

}